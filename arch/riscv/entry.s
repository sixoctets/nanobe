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

.ifdef TEST_IRQ
	/* Disable Software, Timer and External interrupt */
	li t0, 0x888
	csrrc zero, mie, t0

	/* Machine Interrupt Enable */
	li t0, 0x8
	csrrs zero, mstatus, t0
.endif /* TEST_IRQ */

.ifdef TEST_SWI
	/* Software Interrupt Enable */
	li t0, 0x8
	csrrs zero, mie, t0

	/* Software Interrupt Pend */
	li t0, 0x02000000
	li t1, 0x1
	sw t1, 0(t0)
.endif /* TEST_SWI */

.ifdef TEST_TIMER
	/* Timer Interrupt Enable */
	li t0, 0x80
	csrrs zero, mie, t0

	li a0, 0x0200bffc
	lw a1, 0(a0)
	li a0, 0x02004004
	sw a1, 0(a0)

	li a0, 0x0200bff8
	lw a1, 0(a0)

	li a0, 0x02004000
	li t0, 0x10000
	add a1, a1, t0
	sw a1, 0(a0)
.endif /* TEST_TIMER */

	/* Jump to main */
	jal main

loop:
	wfi
	j loop

	.align 2
early_trap_vector:
	csrr t0, mcause /* Read the machine cause register */
	csrr t1, mepc   /* Read the machine exception current PC */
	csrr t2, mtval  /* Read the machine trap value */

.ifdef TEST_SWI
	/* Software Interrupt Enable */
	li t0, 0x8
	csrrs zero, mie, t0

	/* Software Interrupt Clear */
	li t0, 0x02000000
	li t1, 0x0
	sw t1, 0(t0)
.endif /* TEST_SWI */

.ifdef TEST_TIMER
	/* Timer Interrupt Disable */
	li t0, 0x80
	csrrc zero, mie, t0
.endif /* TEST_TIMER */

	jal trap
	mret

	.section .noinit
	.align 4
	.fill 256, 1, 0
_sp:
 
	.end
