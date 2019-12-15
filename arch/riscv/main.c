#include <stdint.h>

#define GPIO_BASE          0x10012000UL
#define GPIO_REG_INPUT_EN  0x00000004UL
#define GPIO_REG_OUTPUT_EN 0x00000008UL
#define GPIO_REG_PORT_VAL  0x0000000cUL

#define GPIO_PIN_LED_GREEN 19
#define GPIO_PIN_LED_BLUE  21
#define GPIO_PIN_LED_RED   22
#define GPIO_PIN_LED       GPIO_PIN_LED_RED

#define BIT(x) (1<<(x))

static inline void mmio_clr_u32(uint32_t base, uint32_t offset, uint32_t val)
{
	(*(volatile uint32_t *) ((base) + (offset))) &= ~val;
}

static inline void mmio_set_u32(uint32_t base, uint32_t offset, uint32_t val)
{
	(*(volatile uint32_t *) ((base) + (offset))) |= val;
}

//__attribute__((section(".text.entry")))
//void _start(void)
void main(void)
{
	mmio_set_u32(GPIO_BASE, GPIO_REG_PORT_VAL, BIT(GPIO_PIN_LED));

	mmio_clr_u32(GPIO_BASE, GPIO_REG_INPUT_EN, BIT(GPIO_PIN_LED));
	mmio_set_u32(GPIO_BASE, GPIO_REG_OUTPUT_EN, BIT(GPIO_PIN_LED));

	mmio_clr_u32(GPIO_BASE, GPIO_REG_PORT_VAL, BIT(GPIO_PIN_LED));

	while (1);
}
