#ifndef __LED_H
#define	__LED_H

#include "stm32f4xx.h"

#define LED1_OFF GPIO_SetBits(GPIOF,GPIO_Pin_9);    //PF9����ߵ�ƽ
#define LED1_ON GPIO_ResetBits(GPIOF,GPIO_Pin_9);   //PF9����͵�ƽ
#define LED2_OFF GPIO_SetBits(GPIOF,GPIO_Pin_10);    //PF10����ߵ�ƽ
#define LED2_ON GPIO_ResetBits(GPIOF,GPIO_Pin_10);   //PF10����͵�ƽ


void LED_GPIO_Config(void);   /* LED �˿ڳ�ʼ�� */

#endif /* __LED_H */