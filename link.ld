ENTRY(_start)

MEMORY
{
  ROM (rx)  : ORIGIN = 0x80000000, LENGTH = 512K
  RAM (rwx) : ORIGIN = 0x00000000, LENGTH = 128K
}

SECTIONS
{
  /* Program code (in flash / ROM) */
  .text : {
    KEEP(*(.init))
    KEEP(*(.init.*))
    *(.text*)
    *(.rodata*)
  } > ROM

  /* Initialized data (copied from flash to RAM at startup) */
  .data : AT (ADDR(.text) + SIZEOF(.text)) {
    _data_start = .;
    *(.data*)
    _data_end = .;
  } > RAM

  /* Zero-initialized data (in RAM) */
  .bss : {
    _bss_start = .;
    *(.bss*)
    *(COMMON)
    _bss_end = .;
  } > RAM

  /* Stack at the top of RAM */
  _stack_top = ORIGIN(RAM) + LENGTH(RAM);
}
