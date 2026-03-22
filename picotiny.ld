/* picotiny.ld — linker script for picotiny SoC user firmware
 * Code lives in SPI Flash XIP region starting at 0x00100000
 * Data/stack lives in 8KB SRAM at 0x40000000
 */
MEMORY {
    flash (rx)  : ORIGIN = 0x00100000, LENGTH = 0x700000  /* 7MB user flash */
    sram  (rwx) : ORIGIN = 0x40000000, LENGTH = 0x2000    /* 8KB SRAM */
}

SECTIONS {
    .text : {
        *(.text.start)
        *(.text*)
        *(.rodata*)
    } > flash

    .data : {
        _data_lma = LOADADDR(.data);
        _data_start = .;
        *(.data*)
        _data_end = .;
    } > sram AT > flash

    .bss : {
        _bss_start = .;
        *(.bss*)
        *(COMMON)
        _bss_end = .;
    } > sram

    _stack_top = ORIGIN(sram) + LENGTH(sram);
}
