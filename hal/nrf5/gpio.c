#include <stdint.h>

#include "soc.h"

#include "hal/gpio.h"

#include "util/misc.h"

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
	} else if (pin < 64) {
		uint32_t bitmask = BIT(pin - 32);

		if (level) {
			NRF_P1->OUTSET = bitmask;
		} else {
			NRF_P1->OUTCLR = bitmask;
		}
	}
}

void gpio_pin_out_config(uint32_t pin, uint8_t level)
{
	if (pin < 32) {
		uint32_t bitmask = BIT(pin);

		gpio_bitmask_out(bitmask, level);
		NRF_GPIO->DIRSET = bitmask;
	} else if (pin < 64) {
		uint32_t bitmask = BIT(pin - 32);

		if (level) {
			NRF_P1->OUTSET = bitmask;
		} else {
			NRF_P1->OUTCLR = bitmask;
		}
		NRF_P1->DIRSET = bitmask;
	}
}
