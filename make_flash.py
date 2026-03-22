#!/usr/bin/env python3
"""
make_flash.py — Build + convert Arduino sketch for picotiny
Uses the real picotiny crt.S and sections.lds

Usage:  py make_flash.py sketch_blink.c
Output: sketch_blink_flash.v  (ready for pico-programmer.py)

Required files in same folder:
  crt.S        (from picotiny fw-flash)
  sections.lds (from picotiny fw-flash)
"""
import sys, subprocess, os

if len(sys.argv) < 2:
    print("Usage: py make_flash.py sketch_blink.c")
    sys.exit(1)

src      = sys.argv[1]
base     = src.replace('.c', '')
elf      = base + '.elf'
bin_file = base + '.bin'
out_file = base + '_flash.v'
CC       = 'riscv-none-elf-gcc'
OBJCOPY  = 'riscv-none-elf-objcopy'

# Step 1: compile
print(f"Compiling {src}...")
ret = subprocess.run([
    CC,
    '-march=rv32i', '-mabi=ilp32',
    '-Os', '-ffreestanding', '-nostdlib', '-static',
    '-Wall', '-Wno-unused-function',
    '-I.',
    '-T', 'sections.lds',
    'crt.S', src,
    '-o', elf
], capture_output=True, text=True)

if ret.returncode != 0:
    print("COMPILE ERROR:\n" + ret.stderr)
    sys.exit(1)

# Step 2: size report
ret2 = subprocess.run([CC.replace('gcc','size'), elf],
    capture_output=True, text=True)
print(ret2.stdout.strip())

# Step 3: binary extraction (code sections only)
ret3 = subprocess.run([
    OBJCOPY, '-O', 'binary',
    '--only-section=.vector',
    '--only-section=.text',
    '--only-section=.rodata',
    '--only-section=.ctors',
    '--only-section=.data',
    elf, bin_file
], capture_output=True, text=True)

if ret3.returncode != 0:
    print("OBJCOPY ERROR:\n" + ret3.stderr)
    sys.exit(1)

with open(bin_file, 'rb') as f:
    data = f.read()

print(f"Binary: {len(data)} bytes")

# Step 4: pad + write pico-programmer format
pad  = (16 - len(data) % 16) % 16
data = data + b'\xff' * pad

with open(out_file, 'wb') as f:
    f.write(b'@00000000\n')
    for i in range(0, len(data), 16):
        chunk = data[i:i+16]
        f.write((' '.join(f'{b:02X}' for b in chunk) + ' \n').encode())

print(f"Written:  {out_file}  ({len(data)//16} lines)")
print(f"\nFlash with:")
print(f"  py ..\\sw\\pico-programmer.py {out_file} COM4")

os.remove(bin_file)
