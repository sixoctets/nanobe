	.section .text.entry
	.globl _start
_start:
	/* I might know somethings down below */
	/* Initialise global pointer */
	la gp, __gp

	/* Initialise stack pointer */
	la sp, _sp

	/* Setup an early trap routine */
	la t0, early_trap_vector
	csrw mtvec, t0

	/* Jump to main */
	j main

	.align 2
early_trap_vector:
	/* I dont know whats down here! */
	csrr t0, mcause
	csrr t1, mepc
	csrr t2, mtval
	j early_trap_vector

	.section .noinit
	.align 4
	.fill 64, 1, 0
_sp:
 
	.end
