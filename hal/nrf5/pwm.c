#include "soc.h"

#include "hal/pwm.h"

#define PWM_TIMER NRF_TIMER1

void pwm_init(uint8_t instance, uint32_t period_us, uint8_t duty, uint8_t pin)
{
	uint8_t ppi_index;

	NRF_GPIOTE->CONFIG[instance] = 0;

	NRF_GPIO->DIRSET = (1 << pin);
	if (!duty) {
		NRF_GPIO->OUTSET = (1 << pin);

		return;
	}

	PWM_TIMER->MODE = 0;
	PWM_TIMER->PRESCALER = 3;
	PWM_TIMER->BITMODE = 2; /* 24 - bit */
	PWM_TIMER->EVENTS_COMPARE[instance] = 0;
	PWM_TIMER->EVENTS_COMPARE[3] = 0;
	PWM_TIMER->SHORTS = TIMER_SHORTS_COMPARE3_CLEAR_Msk;
	PWM_TIMER->CC[instance] = (period_us << 1) * duty / 100;
	PWM_TIMER->CC[3] = (period_us << 1);
	PWM_TIMER->TASKS_CLEAR = 1;

	NRF_GPIOTE->CONFIG[instance] = 0x00030003 | (pin << 8);

	ppi_index = 8 + (instance << 1);
	NRF_PPI->CH[ppi_index].EEP =
		(uint32_t)&(PWM_TIMER->EVENTS_COMPARE[instance]);
	NRF_PPI->CH[ppi_index].TEP =
		(uint32_t)&(NRF_GPIOTE->TASKS_OUT[instance]);
	NRF_PPI->CH[ppi_index + 1].EEP =
		(uint32_t)&(PWM_TIMER->EVENTS_COMPARE[3]);
	NRF_PPI->CH[ppi_index + 1].TEP =
		(uint32_t)&(NRF_GPIOTE->TASKS_OUT[instance]);
	NRF_PPI->CHENSET = (1 << ppi_index) | (1 << (ppi_index + 1));
}

void pwm_start(void)
{
	PWM_TIMER->TASKS_START = 1;
}

void pwm_stop(void)
{
	uint8_t pin;

	PWM_TIMER->TASKS_STOP = 1;

	pin = (NRF_GPIOTE->CONFIG[0] >> 8) & 0xFF;
	NRF_GPIO->OUTSET = (1 << pin);
	pin = (NRF_GPIOTE->CONFIG[1] >> 8) & 0xFF;
	NRF_GPIO->OUTSET = (1 << pin);
	pin = (NRF_GPIOTE->CONFIG[2] >> 8) & 0xFF;
	NRF_GPIO->OUTSET = (1 << pin);
}
