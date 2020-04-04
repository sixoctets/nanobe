	.section .text.entry
	.globl _start
_start:
	/* Initialise global pointer */
	la gp, __global_pointer$

	/* Initialise stack pointer */
	la sp, _sp

	/* Setup an early trap routine */
	la t0, early_trap_vector
	csrw mtvec, t0

	/* TODO: Jump to _soc_init */
soc_init_return:

	/* TODO: Load data memory */

	/* TODO: Zero bss memory */

	/* Branch to main */
	jal main

loop:
	wfi
	j loop

	.section .text
	.align 2
	.type early_trap_vector, %function
	.weak early_trap_vector
early_trap_vector:
	csrr t0, mcause /* Read the machine cause register */
	csrr t1, mepc   /* Read the machine exception current PC */
	csrr t2, mtval  /* Read the machine trap value */
	j early_trap_vector

	.section .noinit
	.align 4
	.fill 256, 1, 0
_sp:
	.end
