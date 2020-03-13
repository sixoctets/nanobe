/*
Copyright (c) 2019, Vinayak Kariappa Chettimada
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
#include "util/util.h"

#include "hal/isr.h"
#include "hal/uart.h"
#include "hal/debug.h"

#include "nanobe.h"
#include "nanobe_sched.h"

#if UART
#define PRINT(x) do { \
			uint8_t lock; \
			\
			lock = nanobe_sched_lock(); \
			uart_tx_str(x); \
			while (!uart_tx_done()) { \
			} \
			nanobe_sched_unlock(lock); \
		} while (0)
#else
#define PRINT(x)
#endif

static uint8_t __noinit isr_stack[256];
static uint8_t __noinit main_stack[512];
static uint8_t __noinit nanobe_0_stack[256];

void * const isr_stack_top = isr_stack + sizeof(isr_stack);
void * const main_stack_top = main_stack + sizeof(main_stack);

static void nanobe_0(void)
{
	PRINT("nanobe_0 init.\n");

	while(1) {
		PRINT("nanobe_0.\n");
		nanobe_sched_yield();
	}
}

void nanobe_injection(void)
{
	PRINT("injection enter.\n");

	nanobe_sched_yield();

	PRINT("injection exit.\n");
	return;
}

static void isr_timer2(void *param)
{
	(void)param;

	/* not in use, assert for now */
	ASSERT(0);
}

static void delay(uint32_t d)
{
	uint32_t p, n;

	NRF_TIMER2->TASKS_CAPTURE[1] = 1;
	p = NRF_TIMER2->CC[1];
	do {
		NRF_TIMER2->TASKS_CAPTURE[1] = 1;
		n = NRF_TIMER2->CC[1];
	} while (((n - p) & 0x7FFFFFFF) < d);
}

int main(void)
{
	extern _isr_table_entry_t _isr_table[];
	void *nanobe_sp;

	DEBUG_PIN_INIT(13);

#if 0
	NRF_P1->DIRSET = (1 << 7); /* output */
	NRF_P1->DIRCLR = (1 << 8); /* input */
	NRF_P1->PIN_CNF[8] = 0; /* input no pull down, no pull up */
#endif

	#if UART
	extern void isr_uart0(void *);

	uart_init(UART, 0);
	_isr_table[UART0_IRQn].isr = isr_uart0;
	irq_priority_set(UART0_IRQn, 0xFF);
	irq_enable(UART0_IRQn);

	PRINT("\n\n\nNanobe.\n");

	{
		extern void assert_print(void);
		uint8_t lock;

		lock = nanobe_sched_lock();
		assert_print();
		nanobe_sched_unlock(lock);
	}
	#endif

	/* setup interrupt vector table entry for timer2 */
	_isr_table[TIMER2_IRQn].isr = isr_timer2;
	irq_priority_set(TIMER2_IRQn, 0xFF);
	irq_enable(TIMER2_IRQn);

	/* setup timer */
	NRF_TIMER2->MODE = 0;
	NRF_TIMER2->BITMODE = 3;
	NRF_TIMER2->PRESCALER = 4;
	/* we are not interested in interrupts */
	//NRF_TIMER2->CC[0] = 100000;
	//NRF_TIMER2->EVENTS_COMPARE[0] = 0;
	//NRF_TIMER2->SHORTS = TIMER_SHORTS_COMPARE0_CLEAR_Msk;
	//NRF_TIMER2->INTENSET = TIMER_INTENSET_COMPARE0_Msk;
	/* start the timer */
	NRF_TIMER2->TASKS_START = 1;

	/* Add a dummy nanobe */
	nanobe_sp = _nanobe_init(nanobe_0,
				 nanobe_0_stack + sizeof(nanobe_0_stack));
	nanobe_sched_enqueue(nanobe_sp);

	while (1) {
		uint32_t s, e, t, d;
		char buf[64];

		PRINT("nanobe main.\n");

		/* trigger ultrasound */
#if 0
		NRF_P1->OUTSET = (1 << 7);
		delay(10);
		NRF_P1->OUTCLR = (1 << 7);

		/* measure echo */
		while ((NRF_P1->IN & (1 << 8)) == 0);
		NRF_TIMER2->TASKS_CAPTURE[1] = 1;
		s = NRF_TIMER2->CC[1];
		while ((NRF_P1->IN & (1 << 8)) != 0);
		NRF_TIMER2->TASKS_CAPTURE[1] = 1;
		e = NRF_TIMER2->CC[1];
#endif

		/* calculate distance */
		t = (e - s) & 0x7FFFFFFF;
		d = t * 17/1000;

		util_sprintf(buf, "D = %u\n", d);
		PRINT(buf);

		delay(60000);
	}
}
