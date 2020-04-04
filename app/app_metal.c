#include <stdint.h>

#include "board.h"

#include "hal/gpio.h"

#include "util/misc.h"

static uint8_t __noinit isr_stack[256];
static uint8_t __noinit main_stack[512];

void * const isr_stack_top = isr_stack + sizeof(isr_stack);
void * const main_stack_top = main_stack + sizeof(main_stack);

int main(void)
{
	gpio_pin_out_config(LED_BLINK, 0);
}
