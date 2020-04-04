#ifndef _GPIO_H_
#define _GPIO_H_

void gpio_bitmask_out(uint32_t bitmask, uint8_t level);
void gpio_pin_out(uint32_t pin, uint8_t level);
void gpio_pin_out_config(uint32_t pin, uint8_t level);

#endif
