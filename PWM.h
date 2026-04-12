#ifndef PWM_H
#define PWM_H

#include <stdint.h>

void PWM_init(void);
void PWM1_duty(uint16_t duty);
void PWM2_duty(uint16_t duty);

#endif