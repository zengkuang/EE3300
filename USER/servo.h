#ifndef _SERVO_H_
#define _SERVO_H_

#include "stm32f4xx.h"
/*
Use timer9 to generate PWM for servo
*/
void servo_change(uint16_t, uint16_t);
void servo_timer_config(void);
#endif