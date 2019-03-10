#ifndef _PWM_H_
#define _PWM_H_

void pwm_init(uint8_t instance, uint32_t period_us, uint8_t duty, uint8_t pin);
void pwm_start(void);
void pwm_stop(void);

#endif /* _PWM_H_ */
