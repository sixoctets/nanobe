OUTPUT_ARCH("riscv")

ENTRY(_start)

MEMORY {
	FLASH (rxl) : ORIGIN = FLASH_ORIGIN, LENGTH = FLASH_LENGTH
	RAM (rwx) : ORIGIN = RAM_ORIGIN, LENGTH = RAM_LENGTH
}

SECTIONS {

	.text : {
		KEEP(*(.text.entry))
		*(.text*)
	} > FLASH
	__text_end = .;

	. = ALIGN(4);
	.data : AT (__text_end) {
		__data_start = .;
		PROVIDE(__global_pointer$ = . + 0x800);
		*(.data*)
		__data_end = .;
	} > RAM

	. = ALIGN(4);
	.bss (NOLOAD) : {
		__bss_start = .;
		*(.bss*)
		__bss_end = .;
	} > RAM

	. = ALIGN(8);
	.noinit (NOLOAD) : {
		__noinit_start = .;
		*(.noinit*)
		__noinit_end = .;
	} > RAM
}
