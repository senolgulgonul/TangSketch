#!/usr/bin/env python3
"""
TangSketch upload tool for Arduino IDE
Converts .elf to picotiny flash format and uploads via UART
"""
import sys, os, serial, time, glob, subprocess

if len(sys.argv) < 3:
    print("Usage: upload.py <sketch.bin> <COMport>")
    sys.exit(1)

bin_path = sys.argv[1]
port     = sys.argv[2]

# ── Find the actual ELF/BIN in build folder ───────────────────
build_dir = os.path.dirname(bin_path)
print(f"Build dir: {build_dir}")
print(f"Files in build dir:")
for f in os.listdir(build_dir):
    print(f"  {f}")

# Try to find .elf file — Arduino IDE always produces this
elf_candidates = glob.glob(os.path.join(build_dir, "*.elf"))
print(f"ELF files found: {elf_candidates}")

if not elf_candidates:
    print("ERROR: No .elf file found in build directory!")
    sys.exit(1)

elf_path = elf_candidates[0]
print(f"Using ELF: {elf_path}")

# ── Convert ELF to binary using objcopy ──────────────────────
bin_path = elf_path.replace('.elf', '_flash.bin')
objcopy = 'riscv-none-elf-objcopy'

ret = subprocess.run([
    objcopy, '-O', 'binary',
    '--only-section=.vector',
    '--only-section=.text',
    '--only-section=.rodata',
    '--only-section=.ctors',
    '--only-section=.data',
    elf_path, bin_path
], capture_output=True, text=True)

if ret.returncode != 0:
    # Try without section filtering (simpler)
    ret = subprocess.run([
        objcopy, '-O', 'binary',
        elf_path, bin_path
    ], capture_output=True, text=True)

if ret.returncode != 0:
    print(f"ERROR: objcopy failed: {ret.stderr}")
    sys.exit(1)

with open(bin_path, 'rb') as f:
    data = f.read()

print(f"Binary: {len(data)} bytes")

# ── Pad to 16-byte boundary ───────────────────────────────────
pad = (16 - len(data) % 16) % 16
data = data + b'\xff' * pad
prog = list(data)
nproglen = len(prog)
print(f"Read program with {nproglen} bytes")

# ── Open serial port ──────────────────────────────────────────
try:
    ser = serial.Serial(port, 115200, timeout=0.01)
except Exception as e:
    print(f"ERROR opening {port}: {e}")
    sys.exit(1)

print("  - Waiting for reset -")
print("    Press S1 button on Tang Nano 9K now!")
print('    ', end='', flush=True)

res = b''
for i in range(100):
    ser.reset_input_buffer()
    ser.write(bytes([0x55, 0x55]))
    ser.flush()
    time.sleep(0.1)
    res = ser.read()
    if i % 10 == 0:
        print('.', end='', flush=True)
    if res and res[0] == 0x56:
        break

print()

if not res or res[0] != 0x56:
    print("ERROR: Board not detected. Check port and press S1.")
    ser.close()
    sys.exit(1)

time.sleep(0.1)
ser.read()

# ── Flash ─────────────────────────────────────────────────────
def isp_wait_byte(ser, expected):
    resp = b''
    while not resp:
        resp = ser.read()
    return resp[0] == expected, resp[0]

def isp_exec_esec(ser, addr):
    saddr = bytes([(addr//65535)&0xFF, (addr//256)&0xF0, 0x00])
    ser.write(bytes([0x30])); isp_wait_byte(ser, 0x31)
    ser.write(saddr);         isp_wait_byte(ser, 0x32)

def isp_exec_wbuf(ser, data):
    wrbyte = bytes([len(data)-1] + list(data))
    chksum = sum(data) & 0xFF
    ser.write(bytes([0x10])); isp_wait_byte(ser, 0x11)
    ser.write(wrbyte)
    ok, _ = isp_wait_byte(ser, chksum)
    return ok

def isp_exec_wpag(ser, addr):
    pgbuf = bytes([(addr//65535)&0xFF, (addr//256)&0xFF, addr&0xFF])
    ser.write(bytes([0x40])); isp_wait_byte(ser, 0x41)
    ser.write(pgbuf);         isp_wait_byte(ser, 0x42)

def isp_exec_rst(ser):
    ser.write(bytes([0xF0])); ser.read()

pagestep = 256
rembyte  = len(prog)
sectreq  = ((rembyte-1) // 4096) + 1
pagereq  = ((rembyte-1) // pagestep) + 1
curraddr = 0

print(f"Total sectors {sectreq}")
print(f"Total pages   {pagereq}")

for i in range(sectreq):
    print(f"Flashing {i+1} / {sectreq}", flush=True)
    isp_exec_esec(ser, curraddr)
    for j in range(min(16, pagereq - i*16)):
        wlen  = min(pagestep, rembyte - curraddr)
        wrdat = prog[curraddr:curraddr+wlen]
        for _ in range(3):
            if isp_exec_wbuf(ser, wrdat): break
        isp_exec_wpag(ser, curraddr)
        curraddr += pagestep

isp_exec_rst(ser)
ser.close()
print("\nFlashing completed OK")
