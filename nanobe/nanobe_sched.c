/*
Copyright (c) 2016, Vinayak Kariappa Chettimada
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <stdint.h>
#include "nanobe.h"

static struct {
	void *head;
	void *tail;
	void *prev;
} volatile sched;

void nanobe_sched_enqueue(void *nanobe_sp)
{
	*((void **)nanobe_sp - 1) = 0;
	if (sched.tail) {
		*((void **)sched.tail - 1) = nanobe_sp;
	} else {
		sched.head = nanobe_sp;
	}
	sched.tail = nanobe_sp;
}

static void *_sched_dequeue(void)
{
	void *nanobe;

	nanobe = sched.head;
	if (nanobe) {
		sched.head = *((void **)sched.head - 1);
		if (!sched.head) {
			sched.tail = 0;
		}
	}

	return nanobe;
}

void nanobe_sched_yield(void)
{
	void *ready;

	if (_sgrd) {
		return;
	}
	_sgrd = 1;

	if (sched.prev) {
		nanobe_sched_enqueue(sched.prev);
		sched.prev = 0;
	}

	ready = _sched_dequeue();

	if (!ready) {
		_sgrd = 0;
		return;
	}

	_nanobe_switch(ready, (void **)&sched.prev);
}

uint8_t nanobe_sched_lock(void)
{
	uint8_t lock;

	lock = _sgrd;
	_sgrd = 1;

	return lock;
}

void nanobe_sched_unlock(uint8_t lock)
{
	if (lock) {
		return;
	}

	_sgrd = 0;

	if (_strg) {
		_strg = 0;

		nanobe_sched_yield();
	}
}
