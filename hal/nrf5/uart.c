/*
Copyright (c) 2012, Vinayak Kariappa Chettimada
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#include "soc.h"
#include "hal/uart.h"
#include "util/util.h"

#include "hal/debug.h"

#define UART_TX_BUFFER_MAX  (1)
#define UART_RX_BUFFER_MAX  (1)

#define UART_TX_BUFFER_SIZE (UART_TX_BUFFER_MAX + 1)
#define UART_RX_BUFFER_SIZE (UART_RX_BUFFER_MAX + 1)

static uint8_t tx[UART_TX_BUFFER_SIZE];
static uint8_t volatile tx_first;
static uint8_t volatile tx_last;
static uint8_t rx[UART_RX_BUFFER_SIZE];
static uint8_t volatile rx_first;
static uint8_t volatile rx_last;

void uart_init(uint8_t pin, uint8_t hwfc)
{
	NRF_UART0->ENABLE = UART_ENABLE_ENABLE_Enabled;
	NRF_UART0->BAUDRATE = UART_BAUDRATE_BAUDRATE_Baud115200;
	NRF_UART0->CONFIG = (hwfc) ? UART_CONFIG_HWFC_Msk: 0;
	NRF_UART0->ERRORSRC = 0x0F;
	NRF_UART0->PSELRTS = pin++;
	NRF_UART0->PSELTXD = pin++;
	NRF_UART0->PSELCTS = pin++;
	NRF_UART0->PSELRXD = pin++;
	NRF_UART0->EVENTS_TXDRDY = 0;
	NRF_UART0->EVENTS_RXDRDY = 0;
	NRF_UART0->EVENTS_ERROR = 0;
	NRF_UART0->INTENSET = (UART_INTENSET_RXDRDY_Msk |
			       UART_INTENSET_TXDRDY_Msk |
			       UART_INTENSET_ERROR_Msk);

	NRF_UART0->TASKS_STARTTX = 1;
	NRF_UART0->TASKS_STARTRX = 1;
}

void uart_tx(uint8_t x)
{
	uint8_t prev_last;
	uint8_t last;

	last = tx_last + 1;
	if (last == UART_TX_BUFFER_SIZE) {
		last = 0;
	}

	while (last == tx_first) {
		__WFE();
		__SEV();
		__WFE();
	}

	tx[tx_last] = x;
	prev_last = tx_last;
	tx_last = last;

	if (tx_first == prev_last) {
		NRF_UART0->TXD = tx[tx_first];
	}
}

uint32_t uart_tx_done(void)
{
	return (tx_first == tx_last);
}

void uart_tx_str(char *s)
{
	while (*s) {
		uart_tx(*s++);
	}
}

void uart_tx_hex(uint8_t *p, uint16_t size, uint16_t warp_size, uint8_t *p_warp)
{
	uint16_t index;

	index = 0;
	while (size--) {
		char buf[3];

		util_itoa(buf, 'x', *p++);

		uart_tx_str(buf);
		uart_tx(' ');

		if ((0 != warp_size) && (0 != p_warp) && (0 != size) &&
		    (0 == (++index % warp_size))) {
			uart_tx_str((char *) p_warp);
		}
	}
}

uint32_t uart_rx(uint8_t *p_x)
{
	uint8_t first;

	if (rx_first == rx_last) {
		return(0);
	}

	*p_x = rx[rx_first];
	first = rx_first + 1;
	if (first == UART_RX_BUFFER_SIZE) {
		first = 0;
	}
	rx_first = first;

	if (NRF_UART0->EVENTS_RXDRDY) {
		NRF_UART0->INTENSET = UART_INTENSET_RXDRDY_Msk;
	}

	return(1);
}

void uart_echo(void)
{
	uint8_t x;

	if (uart_rx(&x)) {
		uart_tx(x);
	}
}

void isr_uart0(void *param)
{
	/* TODO: use param as s/w instance of h/w */
	(void)param;

	if (NRF_UART0->EVENTS_TXDRDY) {
		uint8_t first;

		NRF_UART0->EVENTS_TXDRDY = 0;

		first = tx_first + 1;
		if (first == UART_TX_BUFFER_SIZE) {
			first = 0;
		}
		tx_first = first;

		if (tx_first != tx_last) {
			NRF_UART0->TXD = tx[tx_first];
		}
	}

	while ((NRF_UART0->INTENSET & UART_INTENSET_RXDRDY_Msk) &&
	       NRF_UART0->EVENTS_RXDRDY) {
		uint8_t last;

		last = rx_last + 1;
		if (last == UART_RX_BUFFER_SIZE) {
			last = 0;
		}

		if (last == rx_first) {
			NRF_UART0->INTENCLR = UART_INTENSET_RXDRDY_Msk;

			break;
		}

		NRF_UART0->EVENTS_RXDRDY = 0;
		rx[rx_last] = NRF_UART0->RXD;
		rx_last = last;
	}

	ASSERT(NRF_UART0->EVENTS_ERROR == 0);
	if (NRF_UART0->EVENTS_ERROR) {
		NRF_UART0->EVENTS_ERROR = 0;
	}
}
