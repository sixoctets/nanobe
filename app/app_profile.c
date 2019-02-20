/*
Copyright (c) 2016, Vinayak Kariappa Chettimada
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "soc.h"
#include "irq.h"

#include "util/misc.h"

#include "nanobe.h"
#include "nanobe_sched.h"

#include "hal/debug.h"

static uint8_t __noinit isr_stack[256];
static uint8_t __noinit main_stack[512];
static uint8_t __noinit nanobe_0_stack[256];

void * const isr_stack_top = isr_stack + sizeof(isr_stack);
void * const main_stack_top = main_stack + sizeof(main_stack);

void nanobe_0(void)
{
	while(1) {
		DEBUG_PIN_OFF(13);
		DEBUG_PIN_ON(13);
	}
}

void nanobe_injection(void)
{
	DEBUG_PIN_ON(14);
	nanobe_sched_yield();
	DEBUG_PIN_OFF(14);
	return;
}

void isr_timer2(void *param)
{
	(void)param;

	DEBUG_PIN_ON(15);
	if (NRF_TIMER2->EVENTS_COMPARE[0]) {
		NRF_TIMER2->EVENTS_COMPARE[0] = 0;
		DEBUG_PIN_OFF(15);
		return;
	}

	ASSERT(0);
}

int main(void)
{
	extern _isr_table_entry_t _isr_table[];
	void *nanobe_sp;

	DEBUG_PIN_INIT(12);
	DEBUG_PIN_INIT(13);
	DEBUG_PIN_INIT(14);
	DEBUG_PIN_INIT(15);

	nanobe_sp= _nanobe_init(nanobe_0,
				nanobe_0_stack + sizeof(nanobe_0_stack));
	nanobe_sched_enqueue(nanobe_sp);

	NRF_TIMER2->MODE = 0;
	NRF_TIMER2->BITMODE = 0;
	NRF_TIMER2->PRESCALER = 4;
	NRF_TIMER2->SHORTS = TIMER_SHORTS_COMPARE0_CLEAR_Msk;
	NRF_TIMER2->CC[0] = 10000;
	NRF_TIMER2->EVENTS_COMPARE[0] = 0;
	NRF_TIMER2->INTENSET = TIMER_INTENSET_COMPARE0_Msk;

	_isr_table[TIMER2_IRQn].isr = isr_timer2;
	irq_priority_set(TIMER2_IRQn, 0xFF);
	irq_enable(TIMER2_IRQn);

	NRF_TIMER2->TASKS_START = 1;

	while (1) {
		DEBUG_PIN_OFF(12);
		DEBUG_PIN_ON(12);
	}

	return 0;
}
