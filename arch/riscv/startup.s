	.section .text.entry
	.globl _start
_start:
	/* Initialise global pointer */
	la gp, __global_pointer$

	/* Initialise stack pointer */
	la sp, _sp

	/* Setup an early trap routine */
	la t0, trap
	csrw mtvec, t0

	/* Jump to main */
	jal main

loop:
	wfi
	j loop

	.section .text
	.align 2
	.type trap, %function
	.weak trap
trap:
	csrr t0, mcause /* Read the machine cause register */
	csrr t1, mepc   /* Read the machine exception current PC */
	csrr t2, mtval  /* Read the machine trap value */
	j trap

	.section .noinit
	.align 4
	.fill 256, 1, 0
_sp:
	.end
