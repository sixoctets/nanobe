/*
Copyright (c) 2016, Vinayak Kariappa Chettimada
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

	.thumb

	.section .text
	.thumb_func
	.global _nanobe_init
	.type _nanobe_init, %function
_nanobe_init:
	push {r2-r7, lr}
	mov lr, r0
	mrs r0, psp
	msr psp, r1
	push {r2-r7, lr}
	mov r2, r8
	mov r3, r9
	mov r4, r10
	mov r5, r11
	mov r6, r12
	mrs r7, xpsr
	push {r2-r7}
	mov r2, #0
	push {r2}
	mrs r1, psp
	msr psp, r0
	mov r0, r1
	pop {r2-r7, pc}

	.section .text
	.thumb_func
	.global _nanobe_switch
	.type _nanobe_switch, %function
_nanobe_switch:
	push {r2-r7, lr}
	mov r2, r8
	mov r3, r9
	mov r4, r10
	mov r5, r11
	mov r6, r12
	mrs r7, xpsr
	push {r2-r7}
	ldr r3, =__igrd
	ldrb r2, [r3]
	push {r2}
	mov r2, #1
	str r2, [r3]
	mrs r2, psp
	str r2, [r1]
	msr psp, r0
	pop {r2}
	strb r2, [r3]
	ldr r3, =_sgrd
	mov r2, #0
	strb r2, [r3]
	pop {r0-r5}
	msr xpsr_nzcvq, r5
	mov r12, r4
	mov r11, r3
	mov r10, r2
	mov r9, r1
	mov r8, r0
	pop {r2-r7, pc}

	.section .text
	.thumb_func
	.global _nanobe_isr_inject
	.type _nanobe_isr_inject, %function
_nanobe_isr_inject:
	mov r2, #4
	mov r3, lr
	tst r2, r3
	beq __not_nanobe
	/* Interruptible continuable instruction return unhandled as ICI/IT
	 * cannot be restored other than by exception return.
	 * maybe we use pendSV to perform injection using exception stack
	 * frame manipulation?
	 */
	mrs r0, psp
	ldr r3, [r0, #28]
	ldr r2, =0xF000
	and r2, r3
	bne __ici_skip
	ldr r3, = _sgrd
	ldrb r2, [r3]
	cmp r2, #0
	bne __slocked
	ldr r3, = __igrd
	ldrb r2, [r3]
	cmp r2, #0
	bne __ilocked
	mov r2, #1
	strb r2, [r3]
	mrs r0, psp
	ldr r2, [r0, #24]
	ldr r3, =__syringe
	str r3, [r0, #24]
	mov r3, #1
	add r3, r2

.ifdef NANOBE_USE_STACK_STORE
	/* Stores return and callee in stack, this needs irq lock! */
	sub r0, #4
	str r3, [r0]
	sub r0, #4
	str r1, [r0]
.else
	/* store return and callee in RAM variable */
	ldr r0, =__iret
	str r3, [r0]
	ldr r0, =__ical
	str r1, [r0]
.endif /* NANOBE_USE_STACK_STORE */

__not_nanobe:
__ici_skip:
	bx lr

__ilocked:
	ldr r3, =__itrg
	mov r2, #1
	strb r2, [r3]
	bx lr

__slocked:
	ldr r3, =_strg
	mov r2, #1
	strb r2, [r3]
	bx lr

__syringe:
.ifdef NANOBE_USE_STACK_STORE
	/* Retrieve stored return and callee in stack, this needs irq lock! */
	cpsid i
	push {r0}
	push {r0, r1}
	mrs r0, xpsr
	push {r0}
	mov r0, sp
	add r0, #0x13
	ldr r1, =0xFFFFFFF8
	and r0, r1
	sub r0, #36
	ldr r1, [r0]
	str r1, [sp, #12]
	sub r0, #4
	ldr r1, [r0]
	cpsie i
.else
	push {r0}
	push {r0, r1}
	mrs r0, xpsr
	push {r0}
	ldr r1, =__iret
	ldr r1, [r1]
	str r1, [sp, #12]
	ldr r1, =__ical
	ldr r1, [r1]
.endif /* NANOBE_USE_STACK_STORE */

	push {r2, r3}
__loop:
	mov r2, r12
	push {r1-r2, lr}
	blx r1
	pop {r1-r3}
	mov lr, r3
	mov r12, r2
	ldr r3, =__igrd
	mov r0, #0
	strb r0, [r3]
	ldr r3, =__itrg
	ldrb r0, [r3]
	cmp r0, #0
	bne __again
	pop {r2, r3}
	pop {r0}
	msr xpsr_nzcvq, r0
	pop {r0, r1}
	pop {pc}

__again:
	mov r0, #0
	strb r0, [r3]
	ldr r3, =__igrd
	mov r0, #1
	strb r0, [r3]
	b __loop

	.section .text
	.thumb_func
	.weak _isr_wrapper
	.type _isr_wrapper, %function
_isr_wrapper:
	mrs r0, IPSR
	ldr r2, =16
	sub r0, r2
	lsl r0, #3 /* *param, *fp_isr, options (inject) */
	ldr r2, =_isr_table
	add r2, r2, r0
	ldm r2!, {r0, r1}
	push {lr}
	blx r1
	pop {r0}
	mov lr, r0
	ldr r1, =nanobe_injection
	b _nanobe_isr_inject

	.section .bss.__nanobe
.ifndef NANOBE_USE_STACK_STORE
__iret:
	.word 0
__ical:
	.word 0
.endif /* !NANOBE_USE_STACK_STORE */
__igrd:
	.byte 0
__itrg:
	.byte 0

	.section .bss._nanobe_sched
	.global _sgrd
	.global _strg
_sgrd:
	.byte 0
_strg:
	.byte 0

	.end
