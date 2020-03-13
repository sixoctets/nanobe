/*
Copyright (c) 2012, Vinayak Kariappa Chettimada
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef _IRQ_H_
#define _IRQ_H_

static inline uint32_t irq_lock(void)
{
	uint32_t mask;

	__asm__ volatile ("MRS %0, PRIMASK;"
			  "CPSID i"
			  :"=r" (mask)
			  :
			  : "memory");

	return (mask & 0x01);
}

static inline void irq_unlock(uint32_t mask)
{
	if (mask) {
		return;
	}
	__asm__ volatile ("CPSIE i"
			  :
			  :
			  : "memory");
}

static inline void irq_enable(uint32_t irq)
{
	NVIC_EnableIRQ(irq);
}

static inline void irq_disable(uint32_t irq)
{
	NVIC_DisableIRQ(irq);
}

static inline uint32_t irq_is_enabled(uint32_t irq)
{
	return NVIC->ISER[(irq >> 5)] & (1 << (irq & 0x1F));
}

static inline void irq_pending_set(uint32_t irq)
{
	NVIC_SetPendingIRQ((IRQn_Type) irq);
}

static inline void irq_pending_clear(uint32_t irq)
{
	NVIC_ClearPendingIRQ((IRQn_Type) irq);
}

static inline void irq_priority_set(uint32_t irq, int priority)
{
	NVIC_SetPriority(irq, priority);
}

static inline int irq_is_priority_equal(uint32_t irq)
{
	uint32_t curr_ctx;
	uint32_t curr_prio;

	curr_ctx = SCB->ICSR & SCB_ICSR_VECTACTIVE_Msk;
	if (curr_ctx > 15) {
		/* Interrupts */
		curr_prio = NVIC_GetPriority(curr_ctx - 16);
	} else if (curr_ctx > 3) {
		/* Execeptions */
		/* TODO */;
	} else if (curr_ctx > 0) {
		/* Fixed Priority Exceptions: -3, -2, -1 priority */
		curr_prio = curr_ctx - 4;
	} else {
		/* Thread mode */
		curr_prio = 256;
	}

	return (NVIC_GetPriority(irq) == curr_prio);
}

#endif /* _IRQ_H_ */
