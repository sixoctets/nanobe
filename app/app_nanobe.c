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

static void *nanobe_main_sp;
static void *nanobe_0_sp;
static void *nanobe_1_sp;

static uint8_t __noinit isr_stack[256];
static uint8_t __noinit main_stack[512];
static uint8_t __noinit nanobe_0_stack[256];
static uint8_t __noinit nanobe_1_stack[256];
static uint8_t __noinit nanobe_2_stack[256];
static uint8_t __noinit nanobe_3_stack[256];

void * const isr_stack_top = isr_stack + sizeof(isr_stack);
void * const main_stack_top = main_stack + sizeof(main_stack);

void nanobe_0(void)
{
	PRINT("nanobe_0 init.\n");

	while(1) {
		PRINT("nanobe_0.\n");
		DEBUG_PIN_OFF(17);
		_nanobe_switch(nanobe_1_sp, &nanobe_0_sp);
		DEBUG_PIN_ON(17);
	}
}

void nanobe_1(void)
{
	PRINT("nanobe_1 init.\n");

	while(1) {
		PRINT("nanobe_1.\n");
		DEBUG_PIN_OFF(18);
		_nanobe_switch(nanobe_main_sp, &nanobe_1_sp);
		DEBUG_PIN_ON(18);
	}
}

void nanobe_2(void)
{
	PRINT("nanobe_2 init.\n");

	while(1) {
		PRINT("nanobe_2.\n");
		DEBUG_PIN_OFF(19);
		nanobe_sched_yield();
		DEBUG_PIN_ON(19);
	}
}

void nanobe_3(void)
{
	PRINT("nanobe_3 init.\n");

	while(1) {
		PRINT("nanobe_3.\n");
		DEBUG_PIN_OFF(20);
		nanobe_sched_yield();
		DEBUG_PIN_ON(20);
	}
}

void nanobe_injection(void)
{
	DEBUG_PIN_ON(22);
	PRINT("injection enter.\n");

	DEBUG_PIN_ON(22);
	nanobe_sched_yield();
	DEBUG_PIN_OFF(22);

	PRINT("injection exit.\n");
	DEBUG_PIN_OFF(22);
	return;
}

static uint32_t volatile ticks;

void isr_timer2(void *param)
{
	(void)param;

	DEBUG_PIN_ON(21);

	if (NRF_TIMER2->EVENTS_COMPARE[0]) {
		NRF_TIMER2->EVENTS_COMPARE[0] = 0;

		ticks++;

		DEBUG_PIN_OFF(21);
		return;
	}

	ASSERT(0);
}

int main(void)
{
	extern _isr_table_entry_t _isr_table[];
	void *nanobe_sp;

	DEBUG_PIN_INIT(16);
	DEBUG_PIN_INIT(17);
	DEBUG_PIN_INIT(18);
	DEBUG_PIN_INIT(19);
	DEBUG_PIN_INIT(20);
	DEBUG_PIN_INIT(21);
	DEBUG_PIN_INIT(22);

	/* Dongle Blue LED */
	NRF_GPIO->DIRSET = (1 << 23);
	NRF_GPIO->OUTSET = (1 << 23);

	#if UART
	extern void isr_uart0(void *);

	uart_init(UART, 1);
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

	nanobe_0_sp = _nanobe_init(nanobe_0,
				   nanobe_0_stack + sizeof(nanobe_0_stack));
	nanobe_1_sp = _nanobe_init(nanobe_1,
				   nanobe_1_stack + sizeof(nanobe_1_stack));

	nanobe_sp = _nanobe_init(nanobe_2,
				 nanobe_2_stack + sizeof(nanobe_2_stack));
	nanobe_sched_enqueue(nanobe_sp);

	nanobe_sp= _nanobe_init(nanobe_3,
				nanobe_3_stack + sizeof(nanobe_3_stack));
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

	irq_priority_set(PendSV_IRQn, 0xFF);

	while (1) {
		PRINT("nanobe main.\n");
		DEBUG_PIN_OFF(16);
		_nanobe_switch(nanobe_0_sp, &nanobe_main_sp);
		DEBUG_PIN_ON(16);

		PRINT("pendsv.\n");
		DEBUG_PIN_OFF(16);
		SCB->ICSR = SCB_ICSR_PENDSVSET_Msk;
		DEBUG_PIN_ON(16);

		if ((ticks % 100) == 0) {
			NRF_GPIO->OUTCLR = (1 << 23);
		} else if ((ticks % 20) == 0) {
			NRF_GPIO->OUTSET = (1 << 23);
		}
	}
}
