#include <stdint.h>

#define GPIO_BASE          0x10012000UL
#define GPIO_REG_INPUT_EN  0x00000004UL
#define GPIO_REG_OUTPUT_EN 0x00000008UL
#define GPIO_REG_PORT_VAL  0x0000000cUL

#define GPIO_PIN_LED_GREEN 19
#define GPIO_PIN_LED_BLUE  21
#define GPIO_PIN_LED_RED   22

#define BIT(x) (1<<(x))

static void mmio_clr_u32(uint32_t base, uint32_t offset, uint32_t val)
{
	(*(volatile uint32_t *) ((base) + (offset))) &= ~val;
}

static void mmio_set_u32(uint32_t base, uint32_t offset, uint32_t val)
{
	(*(volatile uint32_t *) ((base) + (offset))) |= val;
}

static void gpio_out(uint32_t pinmask, uint8_t level)
{
	if (level) {
		mmio_set_u32(GPIO_BASE, GPIO_REG_PORT_VAL, pinmask);
	} else {
		mmio_clr_u32(GPIO_BASE, GPIO_REG_PORT_VAL, pinmask);
	}
}

static void gpio_pin_out(uint32_t pin, uint8_t level)
{
	gpio_out(BIT(pin), level);
}

static void gpio_pin_out_config(uint32_t pin, uint8_t level)
{
	uint32_t pinmask = BIT(pin);

	gpio_out(pinmask, level);
	mmio_clr_u32(GPIO_BASE, GPIO_REG_INPUT_EN, pinmask);
	mmio_set_u32(GPIO_BASE, GPIO_REG_OUTPUT_EN, pinmask);
}

void main(void)
{
	gpio_pin_out_config(GPIO_PIN_LED_GREEN, 0);
	__asm__ volatile ("wfi");
	gpio_pin_out_config(GPIO_PIN_LED_BLUE, 0);
}

void trap(void)
{
	gpio_pin_out_config(GPIO_PIN_LED_RED, 0);
}
