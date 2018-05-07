#ifndef _ENCODER_H_
#define _ENCODER_H_

#include "stm32f4xx.h"
#include "stm32f4xx_tim.h"

#define ENCODER_COUNT 4
//ENCODER 1 configuration
#define ENCODER1_TIMER										TIM4
#define ENCODER1_AF                       GPIO_AF_TIM4
#define ENCODER1_TIMER_RCC_INIT						RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
#define ENCODER1_IRQ											TIM4_IRQn
#define ENCODER1_IRQ_HANDLER							TIM4_IRQHandler

#define ENCODER1_GPIO_RCC_INIT			    	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
#define ENCODER1_PORT1										GPIO_Pin_12
#define ENCODER1_PORT2										GPIO_Pin_13
#define ENCODER1_GPIOx										GPIOD
#define ENCODER1_GPIO_PINSOURCE1          GPIO_PinSource12
#define ENCODER1_GPIO_PINSOURCE2          GPIO_PinSource13
#define ENCODER1_MODE											GPIO_PuPd_UP

//ENCODER 2 configuration
#define ENCODER2_TIMER										TIM5
#define ENCODER2_AF                       GPIO_AF_TIM5
#define ENCODER2_TIMER_RCC_INIT						RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);
#define ENCODER2_IRQ											TIM5_IRQn
#define ENCODER2_IRQ_HANDLER							TIM5_IRQHandler

#define ENCODER2_GPIO_RCC_INIT			    	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
#define ENCODER2_PORT1										GPIO_Pin_0
#define ENCODER2_PORT2										GPIO_Pin_1
#define ENCODER2_GPIOx										GPIOA
#define ENCODER2_GPIO_PINSOURCE1          GPIO_PinSource0
#define ENCODER2_GPIO_PINSOURCE2          GPIO_PinSource1
#define ENCODER2_MODE											GPIO_PuPd_UP


//ENCODER 3 configuration
#define ENCODER3_TIMER										TIM1
#define ENCODER3_AF                       GPIO_AF_TIM1
#define ENCODER3_TIMER_RCC_INIT						RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
#define ENCODER3_IRQ											TIM1_IRQn
#define ENCODER3_IRQ_HANDLER							TIM1_IRQHandler

#define ENCODER3_GPIO_RCC_INIT			    	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
#define ENCODER3_PORT1										GPIO_Pin_9
#define ENCODER3_PORT2										GPIO_Pin_11
#define ENCODER3_GPIOx										GPIOE
#define ENCODER3_GPIO_PINSOURCE1          GPIO_PinSource9
#define ENCODER3_GPIO_PINSOURCE2          GPIO_PinSource11
#define ENCODER3_MODE											GPIO_PuPd_UP



//ENCODER 4 configuration
#define ENCODER4_TIMER										TIM2
#define ENCODER4_AF                       GPIO_AF_TIM2
#define ENCODER4_TIMER_RCC_INIT						RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
#define ENCODER4_IRQ											TIM2_IRQn
#define ENCODER4_IRQ_HANDLER							TIM2_IRQHandler

#define ENCODER4_PORT1										GPIO_Pin_15
#define ENCODER4_PORT2										GPIO_Pin_3
#define ENCODER4_GPIO_PINSOURCE1          GPIO_PinSource15
#define ENCODER4_GPIO_PINSOURCE2          GPIO_PinSource3
#define ENCODER4_MODE											GPIO_PuPd_UP



typedef enum{
	ENCODER_1 = 0, 
	ENCODER_2,
	ENCODER_3,
	ENCODER_4
}EncoderID;

typedef struct{
	s32* encoder1;
	s32* encoder2;
	s32* encoder3;
	s32* encoder4;
}encoder;

extern encoder encoder_count;
void encoder_1_init(void);
void encoder_2_init(void);
void encoder_3_init(void);
void encoder_4_init(void);

/** Get the encoder reading. Overflow after 65535.
	@return encoder count
*/
int32_t get_encoder_count(EncoderID id);

// Reset the encoder count
void reset_encoder(EncoderID id);
void encoder_1_update(void);
void encoder_2_update(void);
void encoder_3_update(void);
void encoder_4_update(void);

#endif
