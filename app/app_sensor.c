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

#include "hal/uart.h"

#include "util/misc.h"
#include "util/util.h"

#include "nanobe.h"
#include "nanobe_sched.h"

#include "hal/debug.h"

static uint8_t __noinit isr_stack[256];
static uint8_t __noinit main_stack[512];
static uint8_t __noinit nanobe_0_stack[256];
static uint8_t __noinit nanobe_1_stack[512];

void * const isr_stack_top = isr_stack + sizeof(isr_stack);
void * const main_stack_top = main_stack + sizeof(main_stack);

int n_print(char *s)
{
	uint8_t lock;

	lock = nanobe_sched_lock();
	uart_tx_str(s);
	while(!uart_tx_done()){}
	nanobe_sched_unlock(lock);

	return 0;
}

int n_assert_print(void)
{
	extern void assert_print(void);
	uint8_t lock;

	lock = nanobe_sched_lock();
	assert_print();
	while(!uart_tx_done()){}
	nanobe_sched_unlock(lock);

	return 0;
}

void mem_set(void *mem, uint8_t val, uint16_t size)
{
	uint8_t *p = mem;

	while (size--) {
		*p++ = val;
	}
}

uint16_t callstack_used(void *stack, uint16_t size)
{
	uint8_t *p = stack;
	uint16_t s = size;

	while (s-- && (*p++ == 0xA5)) {}

	return s;
}

static char _buf[64];

void nanobe_0(void)
{
	n_print("nanobe_0\n");

	while(1) {
		util_sprintf(_buf, "nanobe 0: stack usage, isr     : %u\n",
			     callstack_used(isr_stack, sizeof(isr_stack)));
		n_print(_buf);
		util_sprintf(_buf, "nanobe 0: stack usage, main    : %u\n",
			     callstack_used(main_stack, sizeof(main_stack)));
		n_print(_buf);
		util_sprintf(_buf, "nanobe 0: stack usage, nanobe 0: %u\n",
			     callstack_used(nanobe_0_stack,
					    sizeof(nanobe_0_stack)));
		n_print(_buf);
		util_sprintf(_buf, "nanobe 0: stack usage, nanobe 1: %u\n",
			     callstack_used(nanobe_1_stack,
					    sizeof(nanobe_1_stack)));
		n_print(_buf);
		DEBUG_PIN_ON(13);
		DEBUG_PIN_OFF(13);
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

int i2c_init(void *dev)
{
	ARG_UNUSED(dev);

	NRF_TWI0->PSELSCL = 27;
	NRF_TWI0->PSELSDA = 26;
	NRF_TWI0->FREQUENCY = (TWI_FREQUENCY_FREQUENCY_K400 <<
			       TWI_FREQUENCY_FREQUENCY_Pos) &
			      TWI_FREQUENCY_FREQUENCY_Msk;
	return 0;
}

int i2c_enable(void *dev)
{
	ARG_UNUSED(dev);

	NRF_TWI0->ENABLE = (TWI_ENABLE_ENABLE_Enabled <<
			    TWI_ENABLE_ENABLE_Pos) & TWI_ENABLE_ENABLE_Msk;

	return 0;
}

int i2c_disable(void *dev)
{
	ARG_UNUSED(dev);

	NRF_TWI0->ENABLE = 0;

	return 0;
}

int i2c_addr_set(void *dev, uint8_t addr)
{
	ARG_UNUSED(dev);

	NRF_TWI0->ADDRESS = addr;

	return 0;
}

int i2c_write(void *dev, uint8_t *msg, uint16_t msg_len, uint8_t flag)
{
	ARG_UNUSED(dev);

	NRF_TWI0->EVENTS_ERROR = 0;
	NRF_TWI0->EVENTS_TXDSENT = 0;
	NRF_TWI0->SHORTS = 0;
	NRF_TWI0->TASKS_STARTTX = 1;
	while(msg_len) {
		NRF_TWI0->TXD = *msg++;
		while(NRF_TWI0->EVENTS_TXDSENT == 0) {
			if (NRF_TWI0->EVENTS_ERROR != 0) {
				goto i2c_write_error;
			}
		}
		NRF_TWI0->EVENTS_TXDSENT = 0;
		msg_len--;
	}

	if (!flag) {
i2c_write_error:
		NRF_TWI0->TASKS_STOP = 1;
		while(NRF_TWI0->EVENTS_STOPPED == 0) {}
		NRF_TWI0->EVENTS_STOPPED = 0;
	}

	return msg_len;
}

int i2c_read(void *dev, uint8_t *msg, uint16_t msg_len, uint8_t flag)
{
	ARG_UNUSED(dev);

	NRF_TWI0->EVENTS_ERROR = 0;
	NRF_TWI0->EVENTS_SUSPENDED = 0;
	NRF_TWI0->EVENTS_RXDREADY = 0;

	if (msg_len - 1) {
		NRF_TWI0->SHORTS = (TWI_SHORTS_BB_SUSPEND_Enabled <<
				    TWI_SHORTS_BB_SUSPEND_Pos) &
				   TWI_SHORTS_BB_SUSPEND_Msk;
	} else {
		NRF_TWI0->SHORTS = (TWI_SHORTS_BB_STOP_Enabled <<
				    TWI_SHORTS_BB_STOP_Pos) &
				   TWI_SHORTS_BB_STOP_Msk;
	}

	NRF_TWI0->TASKS_STARTRX = 1;
	while (msg_len) {
		uint16_t len;

		while(NRF_TWI0->EVENTS_RXDREADY == 0) {
			if (NRF_TWI0->EVENTS_ERROR != 0) {
				goto i2c_read_error;
			}
		}
		NRF_TWI0->EVENTS_RXDREADY = 0;
		NRF_TWI0->EVENTS_SUSPENDED = 0;
		*msg++ = NRF_TWI0->RXD;

		len = --msg_len;
		if (len) {
			if (!--len) {
				NRF_TWI0->SHORTS =
					(TWI_SHORTS_BB_STOP_Enabled <<
					 TWI_SHORTS_BB_STOP_Pos) &
					TWI_SHORTS_BB_STOP_Msk;
			}

			NRF_TWI0->TASKS_RESUME = 1;
		}
	}

	if (!flag) {
i2c_read_error:
		NRF_TWI0->TASKS_STOP = 1;
		while(NRF_TWI0->EVENTS_STOPPED == 0) {}
		NRF_TWI0->EVENTS_STOPPED = 0;
	}

	return msg_len;
}

void i2c_test(void)
{
	int err;
	char buf[64];
	uint8_t msg[4];
	uint32_t id, temp, humd;

	/* Configure DRDYn connected to P0.22 */
	NRF_GPIO->PIN_CNF[22] = ((GPIO_PIN_CNF_DIR_Input <<
				  GPIO_PIN_CNF_DIR_Pos) &
				 GPIO_PIN_CNF_DIR_Msk) |
				((GPIO_PIN_CNF_INPUT_Connect <<
				  GPIO_PIN_CNF_INPUT_Pos) &
				 GPIO_PIN_CNF_INPUT_Msk) |
				((GPIO_PIN_CNF_PULL_Pullup <<
				  GPIO_PIN_CNF_PULL_Pos) &
				 GPIO_PIN_CNF_PULL_Msk) |
				((GPIO_PIN_CNF_DRIVE_S0D1 <<
				  GPIO_PIN_CNF_DRIVE_Pos) &
				 GPIO_PIN_CNF_DRIVE_Msk) |
				((GPIO_PIN_CNF_SENSE_Disabled <<
				  GPIO_PIN_CNF_SENSE_Pos) &
				 GPIO_PIN_CNF_SENSE_Msk);

	i2c_init(0);
	i2c_enable(0);
	i2c_addr_set(0, 0x43);

	/* Software reset */
	msg[0] = 0x02;
	msg[1] = BIT(7);
	msg[2] = 0x00;
	err = i2c_write(0, msg, 3, 0);
	util_sprintf(buf, "write (%x).\n", err);
	n_print(buf);

	/* Read Manufacturer Id */
	msg[0] = 0xFE;
	err = i2c_write(0, msg, 1, 1);
	util_sprintf(buf, "write (%x).\n", err);
	n_print(buf);
	err = i2c_read(0, msg, 2, 0);
	util_sprintf(buf, "read (%x).\n", err);
	n_print(buf);

	id = (msg[0] << 8) | msg[1];

	util_sprintf(buf, "id %x\n", id);
	n_print(buf);

	n_print("Waiting high...\n");
	/* Wait for DRDYn */
	while (!(NRF_GPIO->IN & BIT(22))) {}

	/* Configure Temperature and Humidity reading */
	msg[0] = 0x02;
	msg[1] = BIT(4); /* Temp and Humidity */
	msg[2] = 0x00;
	err = i2c_write(0, msg, 3, 0);
	util_sprintf(buf, "write (%x).\n", err);
	n_print(buf);

	/* Read Config register */
	msg[0] = 0x02;
	err = i2c_write(0, msg, 1, 1);
	util_sprintf(buf, "write (%x).\n", err);
	n_print(buf);

	err = i2c_read(0, msg, 2, 0);
	util_sprintf(buf, "read (%x).\n", err);
	n_print(buf);

	id = (msg[0] << 8) | msg[1];

	util_sprintf(buf, "conf %x\n", id);
	n_print(buf);

	/* Trigger measurement */
	msg[0] = 0x00;
	err = i2c_write(0, msg, 1, 0);
	util_sprintf(buf, "write (%x).\n", err);
	n_print(buf);

	n_print("Waiting low...\n");
	/* Wait for DRDYn */
	while (NRF_GPIO->IN & BIT(22)) {}

	n_print("Reading...\n");
	/* Read temperature and humidity */
	err = i2c_read(0, msg, 4, 0);
	util_sprintf(buf, "read (%x).\n", err);
	n_print(buf);

	util_sprintf(buf, "err (%x).\n", NRF_TWI0->ERRORSRC);
	n_print(buf);

	temp = ((((uint32_t)msg[0] << 8) | msg[1]) * 165 / 65536) - 40;
	humd = (((uint32_t)msg[2] << 8) | msg[3]) * 100 / 65536;

	util_sprintf(buf, "Temp: %u, Humd: %u\n", temp, humd);
	n_print(buf);
}

void nanobe_1(void)
{
	n_print("nanobe_1\n");

	while (1) {
		i2c_test();
	}
}

int main(void)
{
	extern _isr_table_entry_t _isr_table[];
	extern void isr_uart0(void *);
	void *nanobe_sp;

	DEBUG_PIN_INIT(12);
	DEBUG_PIN_INIT(13);
	DEBUG_PIN_INIT(14);
	DEBUG_PIN_INIT(15);

	mem_set(isr_stack, 0xA5, sizeof(isr_stack));
	mem_set(main_stack, 0xA5, sizeof(main_stack) - 4);
	mem_set(nanobe_0_stack, 0xA5, sizeof(nanobe_0_stack));
	mem_set(nanobe_1_stack, 0xA5, sizeof(nanobe_1_stack));

	uart_init(UART, 0);
	_isr_table[UART0_IRQn].isr = isr_uart0;
	irq_priority_set(UART0_IRQn, 0xFF);
	irq_enable(UART0_IRQn);

	n_print("\n\nNanobe Weather.\n");

	n_assert_print();

	nanobe_sp = _nanobe_init(nanobe_0,
				 nanobe_0_stack + sizeof(nanobe_0_stack));
	nanobe_sched_enqueue(nanobe_sp);
	nanobe_sp = _nanobe_init(nanobe_1,
				 nanobe_1_stack + sizeof(nanobe_1_stack));
	nanobe_sched_enqueue(nanobe_sp);

	NRF_TIMER2->MODE = 0;
	NRF_TIMER2->BITMODE = 0;
	NRF_TIMER2->PRESCALER = 4;
	NRF_TIMER2->SHORTS = TIMER_SHORTS_COMPARE0_CLEAR_Msk;
	NRF_TIMER2->CC[0] = 100;
	NRF_TIMER2->EVENTS_COMPARE[0] = 0;
	NRF_TIMER2->INTENSET = TIMER_INTENSET_COMPARE0_Msk;

	_isr_table[TIMER2_IRQn].isr = isr_timer2;
	irq_priority_set(TIMER2_IRQn, 0xFF);
	irq_enable(TIMER2_IRQn);

	NRF_TIMER2->TASKS_START = 1;

	while (1) {
		n_print("main\n");
		DEBUG_PIN_OFF(12);
		DEBUG_PIN_ON(12);
	}

	return 0;
}
