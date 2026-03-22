# Makefile — picotiny user firmware builder
# Drop sketch_*.c files alongside this Makefile and run:
#   make SKETCH=sketch_blink
#   make flash SKETCH=sketch_blink PORT=/dev/ttyUSB1

SKETCH  ?= sketch_blink
PORT    ?= /dev/ttyUSB1

CC      = riscv64-linux-gnu-gcc
OBJCOPY = riscv64-linux-gnu-objcopy
OBJDUMP = riscv64-linux-gnu-objdump
SIZE    = riscv64-linux-gnu-size

CFLAGS  = -march=rv32i -mabi=ilp32 \
          -Os -ffreestanding -nostdlib -static \
          -Wall -Wno-unused-function \
          -I.
LDFLAGS = -T picotiny.ld

all: $(SKETCH).v

$(SKETCH).elf: $(SKETCH).c crt.S picotiny_hal.h picotiny.ld
	$(CC) $(CFLAGS) $(LDFLAGS) crt.S $< -o $@
	@$(SIZE) $@

$(SKETCH).v: $(SKETCH).elf
	$(OBJCOPY) -O verilog $< $@
	@echo ">>> Built $(SKETCH).v  ($(shell wc -l < $(SKETCH).v) lines)"

disasm: $(SKETCH).elf
	$(OBJDUMP) -d -M no-aliases $<

flash: $(SKETCH).v
	python3 ../../picotiny/sw/pico-programmer.py $(SKETCH).v $(PORT)

clean:
	rm -f *.elf *.v

.PHONY: all disasm flash clean
