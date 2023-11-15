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
#include "board.h"
#include "cpu.h"
#include "irq.h"

#include "util/misc.h"

#include "hal/isr.h"
#include "hal/timer.h"
#include "hal/debug.h"

#include "nanobe.h"
#include "nanobe_sched.h"

static uint8_t __noinit isr_stack[256];
static uint8_t __noinit main_stack[512];
static uint8_t __noinit nanobe_0_stack[256];

void * const isr_stack_top = isr_stack + sizeof(isr_stack);
void * const main_stack_top = main_stack + sizeof(main_stack);

static uint32_t volatile ticks;
static uint32_t volatile seconds;

static void isr_timer_cb(void *param)
{
	ARG_UNUSED(param);

	DEBUG_PIN_ON(LED3);
	ticks++;
	if ((ticks % 100) == 0) {
		seconds++;
	}
	DEBUG_PIN_OFF(LED3);
}

static void nanobe_0(void)
{
	while(1) {
		if (seconds & 1) {
			DEBUG_PIN_CLR(LED2);
		} else {
			DEBUG_PIN_SET(LED2);
		}

		cpu_sleep();
	}
}

void nanobe_injection(void)
{
	DEBUG_PIN_ON(LED4);
	nanobe_sched_yield();
	DEBUG_PIN_OFF(LED4);
	return;
}

int main(void)
{
	void *nanobe_sp;
	uint32_t irq;
	isr_t isr;
	void *isr_param[] = {isr_timer_cb, 0};

	DEBUG_PIN_INIT(LED1);
	DEBUG_PIN_INIT(LED2);
	DEBUG_PIN_INIT(LED3);
	DEBUG_PIN_INIT(LED4);

	nanobe_sp = _nanobe_init(nanobe_0,
				 nanobe_0_stack + sizeof(nanobe_0_stack));
	nanobe_sched_enqueue(nanobe_sp);

	(void)timer_init();
	(void)timer_irq_init(&irq, &isr);
	_isr_table[irq].isr = isr;
	_isr_table[irq].param = isr_param;

	(void)timer_irq_on();
	(void)timer_on();

	while (1) {
		if (seconds & 1) {
			DEBUG_PIN_SET(LED1);
		} else {
			DEBUG_PIN_CLR(LED1);
		}

		cpu_sleep();
	}

	return 0;
}
