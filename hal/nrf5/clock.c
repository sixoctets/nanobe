/*
Copyright (c) 2020, Vinayak Kariappa Chettimada
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "soc.h"
#include "irq.h"

#include "hal/isr.h"
#include "hal/debug.h"

#if !defined(NRF_TIMER2)
#if defined(NRF_TIMER2_S)
#define NRF_TIMER2 NRF_TIMER2_S
#else
#define NRF_TIMER2 NRF_TIMER2_NS
#endif
#endif

#define NRF_TIMER  NRF_TIMER2
#define TIMER_IRQn TIMER2_IRQn

static void isr_timer(void *param);

int clock_init(void)
{
	NRF_TIMER->MODE = 0;
	NRF_TIMER->BITMODE = 0;
	NRF_TIMER->PRESCALER = 4;
	NRF_TIMER->SHORTS = TIMER_SHORTS_COMPARE0_CLEAR_Msk;
	NRF_TIMER->CC[0] = 10000;
	NRF_TIMER->EVENTS_COMPARE[0] = 0;
	NRF_TIMER->INTENSET = TIMER_INTENSET_COMPARE0_Msk;

	return 0;
}

int clock_on(void)
{
	NRF_TIMER->TASKS_START = 1;

	return 0;
}

int clock_off(void)
{
	/* TODO */

	return 0;
}

int clock_irq_init(uint32_t *irq, isr_t *isr)
{
	irq_priority_set(TIMER_IRQn, 0xFF);

	*irq = TIMER_IRQn;
	*isr = isr_timer;

	return 0;
}

int clock_irq_on(void)
{
	irq_enable(TIMER_IRQn);

	return 0;
}

int clock_irq_off(void)
{
	/* TODO */

	return 0;
}

static void isr_timer(void *param)
{
	DEBUG_PIN_ON(15);
	if (NRF_TIMER->EVENTS_COMPARE[0]) {
		NRF_TIMER->EVENTS_COMPARE[0] = 0;

		if (param) {
			isr_t func;
			void **p;

			p = param;
			func = *p++;
			func(*p);
		}

		DEBUG_PIN_OFF(15);
		return;
	}

	ASSERT(0);
}
