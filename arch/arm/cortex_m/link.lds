MEMORY {
	FLASH (rx) : ORIGIN = 0x00001000, LENGTH = 0x1f000
	RAM (rw) : ORIGIN = 0x20000008, LENGTH = 0x3ff8
}

SECTIONS {

	.text : {
		KEEP(*(.exc_vector))
		KEEP(*(.irq_vector))
		*(.text*)
		*(.rodata*)
	} > FLASH

	. = ALIGN(4);

	__text_end = .;

	.data : AT (__text_end) {
		__data_start = .;
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
