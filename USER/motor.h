#ifndef __MOTOR_H
#define	__MOTOR_H

#include "stm32f4xx.h"
/*
Use Timer3  channel 1~4 to generate PWM signal
*/
void TIM3_PWM_Config(void);   /* PWM configuration */
void GPIO_Config(void);
void drive_motor(int,int,int,int);
#endif /* __MOTOR_H */