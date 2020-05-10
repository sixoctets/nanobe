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
#include "util/misc.h"

/****************************************************************************
System
****************************************************************************/
void soc_init(void)
{
#if defined(NRF51_SERIES)
	/* Power on peripherals */
	*(uint32_t *)0x40000504 = 0xC002FFC7;
#endif

#if defined(NRF52_SERIES)
	/* Disconnect all GPIOs */
	do {
		uint8_t i;

		NRF_GPIO->DIRCLR = 0xFFFFFFFF;

		for (i = 0; i < 32; i++) {
			NRF_GPIO->PIN_CNF[i] = 0x00000002;
		}
	} while (0);
#endif

	/* SEVONPEND */
	SCB->SCR |= SCB_SCR_SEVONPEND_Msk;
}

/****************************************************************************
Assert handler
****************************************************************************/
#if UART
#include "util/util.h"
#include "hal/uart.h"
#endif

#define ASSERT_STACK_FRAME (0x0003FC00)

void exc_hardfault(uint32_t sp)
{
#if defined(NRF51_SERIES) || defined(NRF52_SERIES)
	uint32_t *p_flash = (uint32_t *) ASSERT_STACK_FRAME;
	uint32_t count = 9; /* Cortex-M0 stack frame size = 8 32-bit words, 
			     * plus SP itself to store.
			     */

	/* turn LEDs on */
	NRF_GPIO->DIRSET = 0xFFFFFFFF;
	NRF_GPIO->OUTCLR = 0xFFFFFFFF;

	/* Store SP before the stack frame */
	*((uint32_t *) (sp - 4)) = sp;
	sp -= 4;

	/* erase flash page */
	NRF_NVMC->CONFIG = NVMC_CONFIG_WEN_Een;
	NRF_NVMC->ERASEPAGE = (uint32_t) p_flash;
	while (NRF_NVMC->READY == 0) {
	}

	/* write to flash */
	NRF_NVMC->CONFIG = NVMC_CONFIG_WEN_Wen;
	while (count--)	{
		*p_flash++ = *((uint32_t *)sp);
		sp += 4;
		while (NRF_NVMC->READY == 0) {
		}
	}

	*p_flash = 0;
	while (NRF_NVMC->READY == 0) {
	}
	NRF_NVMC->CONFIG = NVMC_CONFIG_WEN_Ren;
#else
	ARG_UNUSED(sp);
#endif

	/* low power hang! */
	while(1)
	{
		__WFE();
	}
}

#if UART
void assert_print(void)
{
	uint32_t sp = ASSERT_STACK_FRAME;
	char buf[0xFF];
	char *p_buf = buf;

	if (*((uint32_t *) sp) == 0xFFFFFFFF) {
		return;
	}

	sp += 4;

	/* prepare string to write in flash */
	util_sprintf(buf, "\nAssert @ 0x%x\nStack Frame @ 0x%x\nR0: 0x%x\nR1: 0x%x\nR2: 0x%x\nR3: 0x%x\n"
		, *((uint32_t *)(sp + 0x18))
		, *((uint32_t *)(sp - 0x04))
		, *((uint32_t *)(sp + 0x00))
		, *((uint32_t *)(sp + 0x04))
		, *((uint32_t *)(sp + 0x08))
		, *((uint32_t *)(sp + 0x0C))
		);

	while (*p_buf) {
		uart_tx(*p_buf++);
	}
}
#endif
