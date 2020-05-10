/*
Copyright (c) 2016, Vinayak Kariappa Chettimada
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef _DEBUG_H_
#define _DEBUG_H_

#define ASSERT(x) do { \
			if (!(x)) { \
				__asm__ volatile (".inst 0xde00\n"); \
			} \
		} while (0)

#if defined(DEBUG)

#ifndef NRF_P0
#define NRF_P0 NRF_P0_S
#endif

#ifndef NRF_P1
#define NRF_P1 NRF_P1_S
#endif

#ifndef NRF_GPIO
#define NRF_GPIO NRF_P0
#endif

#define DEBUG_PIN_INIT(x) do { \
				NRF_GPIO->DIRSET = (1 << x); \
				NRF_GPIO->OUTCLR = (1 << x); \
			  } while (0)
#define DEBUG_PIN_SET(x)  do { \
				NRF_GPIO->OUTSET = (1 << x); \
			  } while (0)
#define DEBUG_PIN_CLR(x)  do { \
				NRF_GPIO->OUTCLR = (1 << x); \
			  } while (0)
#define DEBUG_PIN_ON(x)   do { \
				NRF_GPIO->OUTCLR = (1 << x); \
				NRF_GPIO->OUTSET = (1 << x); \
			  } while (0)
#define DEBUG_PIN_OFF(x)  do { \
				NRF_GPIO->OUTSET = (1 << x); \
				NRF_GPIO->OUTCLR = (1 << x); \
			  } while (0)
#else
#define DEBUG_PIN_INIT(x)
#define DEBUG_PIN_SET(x)
#define DEBUG_PIN_CLR(x)
#define DEBUG_PIN_ON(x)
#define DEBUG_PIN_OFF(x)
#endif

#endif /* _DEBUG_H_ */
