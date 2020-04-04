#include <stdint.h>

#include "hal/mmio.h"
#include "hal/gpio.h"

#include "util/misc.h"

#define GPIO_BASE          0x10012000UL
#define GPIO_REG_INPUT_EN  0x00000004UL
#define GPIO_REG_OUTPUT_EN 0x00000008UL
#define GPIO_REG_PORT_VAL  0x0000000cUL

inline void gpio_bitmask_out(uint32_t bitmask, uint8_t level)
{
	if (level) {
		mmio_set_u32(GPIO_BASE, GPIO_REG_PORT_VAL, bitmask);
	} else {
		mmio_clr_u32(GPIO_BASE, GPIO_REG_PORT_VAL, bitmask);
	}
}

void gpio_pin_out(uint32_t pin, uint8_t level)
{
	gpio_bitmask_out(BIT(pin), level);
}

void gpio_pin_out_config(uint32_t pin, uint8_t level)
{
	uint32_t bitmask = BIT(pin);

	gpio_bitmask_out(bitmask, level);
	mmio_clr_u32(GPIO_BASE, GPIO_REG_INPUT_EN, bitmask);
	mmio_set_u32(GPIO_BASE, GPIO_REG_OUTPUT_EN, bitmask);
}
