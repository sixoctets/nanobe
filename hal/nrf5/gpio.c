#include <stdint.h>

#include "soc.h"

#include "hal/gpio.h"

#include "util/misc.h"

#ifndef NRF_P0
#define NRF_P0 NRF_P0_S
#endif

#ifndef NRF_P1
#define NRF_P1 NRF_P1_S
#endif

#ifndef NRF_GPIO
#define NRF_GPIO NRF_P0
#endif

inline void gpio_bitmask_out(uint32_t bitmask, uint8_t level)
{
	if (level) {
		NRF_GPIO->OUTSET = bitmask;
	} else {
		NRF_GPIO->OUTCLR = bitmask;
	}
}

void gpio_pin_out(uint32_t pin, uint8_t level)
{
	if (pin < 32) {
		uint32_t bitmask = BIT(pin);

		gpio_bitmask_out(bitmask, level);

#if (GPIO_COUNT > 1)
	} else if (pin < 64) {
		uint32_t bitmask = BIT(pin - 32);

		if (level) {
			NRF_P1->OUTSET = bitmask;
		} else {
			NRF_P1->OUTCLR = bitmask;
		}
#endif
	}
}

void gpio_pin_out_config(uint32_t pin, uint8_t level)
{
	if (pin < 32) {
		uint32_t bitmask = BIT(pin);

		gpio_bitmask_out(bitmask, level);
		NRF_GPIO->DIRSET = bitmask;

#if (GPIO_COUNT > 1)
	} else if (pin < 64) {
		uint32_t bitmask = BIT(pin - 32);

		if (level) {
			NRF_P1->OUTSET = bitmask;
		} else {
			NRF_P1->OUTCLR = bitmask;
		}
		NRF_P1->DIRSET = bitmask;
#endif
	}
}
