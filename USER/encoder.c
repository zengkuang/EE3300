#include "encoder.h"

#if ENCODER_USES_INTERRUPT_MODE
	static volatile s16 loopCountEnc2 = 0;
#else
	static volatile s32 enc2Count = 0;
#endif
static volatile s32 enc1Count = 0; 
static volatile s32 enc3Count = 0; 
static volatile s32 enc4Count = 0; 
encoder encoder_count;
void encoder_4_init(void){
	encoder_count.encoder4 = &enc4Count;
	ENCODER4_TIMER_RCC_INIT;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	GPIO_PinAFConfig(GPIOA, ENCODER4_GPIO_PINSOURCE1, ENCODER4_AF);
	GPIO_PinAFConfig(GPIOB, ENCODER4_GPIO_PINSOURCE2, ENCODER4_AF);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStructure.GPIO_Speed = GPIO_High_Speed;
	GPIO_InitStructure.GPIO_Pin = ENCODER4_PORT1;
	GPIO_InitStructure.GPIO_PuPd = ENCODER4_MODE;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStructure.GPIO_Speed = GPIO_High_Speed;
	GPIO_InitStructure.GPIO_Pin = ENCODER4_PORT2;
	GPIO_InitStructure.GPIO_PuPd = ENCODER4_MODE;	
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	//Timer init
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_TimeBaseStructure.TIM_Prescaler = 0;
	TIM_TimeBaseStructure.TIM_Period = 0xFFFF;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(ENCODER4_TIMER, &TIM_TimeBaseStructure);
		
	//Encoder interface init for encoder 4
	TIM_EncoderInterfaceConfig(ENCODER4_TIMER, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
	
	//Let's go!
	TIM_SetCounter(ENCODER4_TIMER, 0);
	TIM_Cmd(ENCODER4_TIMER, ENABLE);
	
	//Let's go!
	enc4Count = 0;
}

//Init encoder
void encoder_1_init(void){
	encoder_count.encoder1 = &enc1Count;
	ENCODER1_TIMER_RCC_INIT;
	ENCODER1_GPIO_RCC_INIT;
	
	GPIO_PinAFConfig(ENCODER1_GPIOx, ENCODER1_GPIO_PINSOURCE1, ENCODER1_AF);
	GPIO_PinAFConfig(ENCODER1_GPIOx, ENCODER1_GPIO_PINSOURCE2, ENCODER1_AF);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStructure.GPIO_Speed = GPIO_High_Speed;
	
	//Init encoder 1
	GPIO_InitStructure.GPIO_Pin = ENCODER1_PORT1 | ENCODER1_PORT2;
	GPIO_InitStructure.GPIO_PuPd = ENCODER1_MODE;
	GPIO_Init(ENCODER1_GPIOx, &GPIO_InitStructure);
	
	//Timer init
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_TimeBaseStructure.TIM_Prescaler = 0;
	TIM_TimeBaseStructure.TIM_Period = 0xFFFF;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(ENCODER1_TIMER, &TIM_TimeBaseStructure);
		
	//Encoder interface init for encoder 2
	TIM_EncoderInterfaceConfig(ENCODER1_TIMER, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
	
	//Let's go!
	TIM_SetCounter(ENCODER1_TIMER, 0);
	TIM_Cmd(ENCODER1_TIMER, ENABLE);
	
	//Let's go!
	enc1Count = 0;
}

void encoder_2_init(void){
	encoder_count.encoder2 = &enc2Count;
	ENCODER2_TIMER_RCC_INIT;
	ENCODER2_GPIO_RCC_INIT;
	
	GPIO_PinAFConfig(ENCODER2_GPIOx, ENCODER2_GPIO_PINSOURCE1, ENCODER2_AF);
	GPIO_PinAFConfig(ENCODER2_GPIOx, ENCODER2_GPIO_PINSOURCE2, ENCODER2_AF);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStructure.GPIO_Speed = GPIO_High_Speed;
	
	//Init encoder 2
	GPIO_InitStructure.GPIO_Pin = ENCODER2_PORT1 | ENCODER2_PORT2;
	GPIO_InitStructure.GPIO_PuPd = ENCODER2_MODE;
	GPIO_Init(ENCODER2_GPIOx, &GPIO_InitStructure);
	
	//Timer init
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_TimeBaseStructure.TIM_Prescaler = 0;
	TIM_TimeBaseStructure.TIM_Period = 0xFFFF;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(ENCODER2_TIMER, &TIM_TimeBaseStructure);
	
	#if ENCODER_USES_INTERRUPT_MODE
		//Interrupt init
		TIM_ITConfig(ENCODER2_TIMER, TIM_IT_Update, ENABLE);
		
		NVIC_InitTypeDef NVIC_InitStructure;

		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_InitStructure.NVIC_IRQChannel = ENCODER2_IRQ;
		NVIC_Init(&NVIC_InitStructure);
		
		loopCountEnc2 = 0;
	#else
		enc2Count = 0;
	#endif
	
	//Encoder interface init for encoder 2
	TIM_EncoderInterfaceConfig(ENCODER2_TIMER, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
	
	//Let's go!
	TIM_SetCounter(ENCODER2_TIMER, 0);
	TIM_Cmd(ENCODER2_TIMER, ENABLE);
}



void encoder_3_init(void){
	encoder_count.encoder3 = &enc3Count;
	ENCODER3_TIMER_RCC_INIT;
	ENCODER3_GPIO_RCC_INIT;
	
	GPIO_PinAFConfig(ENCODER3_GPIOx, ENCODER3_GPIO_PINSOURCE1, ENCODER3_AF);
	GPIO_PinAFConfig(ENCODER3_GPIOx, ENCODER3_GPIO_PINSOURCE2, ENCODER3_AF);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStructure.GPIO_Speed = GPIO_High_Speed;
	
	//Init encoder 3
	GPIO_InitStructure.GPIO_Pin = ENCODER3_PORT1 | ENCODER3_PORT2;
	GPIO_InitStructure.GPIO_PuPd = ENCODER3_MODE;
	GPIO_Init(ENCODER3_GPIOx, &GPIO_InitStructure);
	
	//Timer init
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_TimeBaseStructure.TIM_Prescaler = 0;
	TIM_TimeBaseStructure.TIM_Period = 0xFFFF;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(ENCODER3_TIMER, &TIM_TimeBaseStructure);
		
	//Encoder interface init for encoder 3f
	TIM_EncoderInterfaceConfig(ENCODER3_TIMER, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
	
	//Let's go!
	TIM_SetCounter(ENCODER3_TIMER, 0);
	TIM_Cmd(ENCODER3_TIMER, ENABLE);
	
	//Let's go!
	enc3Count = 0;
}

//Handler for encoder 1
void ENCODER1_IRQ_HANDLER(){
	static u8 lastAReading = 0, lastBReading = 0;
	
	if (EXTI_GetITStatus(EXTI_Line8) != RESET) {
		u8 aReading = GPIO_ReadInputDataBit(ENCODER1_GPIOx, ENCODER1_PORT1);
		u8 bReading = GPIO_ReadInputDataBit(ENCODER1_GPIOx, ENCODER1_PORT2);
		
		if (bReading ^ lastAReading){
			enc1Count++;
		}
		
		if (aReading ^ lastBReading){
			enc1Count--;
		}
		
		lastAReading = aReading;
		lastBReading = bReading;
		EXTI_ClearITPendingBit(EXTI_Line8);
		
	}else if(EXTI_GetITStatus(EXTI_Line9) != RESET){
		u8 aReading = GPIO_ReadInputDataBit(ENCODER1_GPIOx, ENCODER1_PORT1);
		u8 bReading = GPIO_ReadInputDataBit(ENCODER1_GPIOx, ENCODER1_PORT2);
		
		if (bReading ^ lastAReading){
			enc1Count++;
		}
		
		if (aReading ^ lastBReading){
			enc1Count--;
		}
		
		lastAReading = aReading;
		lastBReading = bReading;
		EXTI_ClearITPendingBit(EXTI_Line9);
	}
}

#if ENCODER_USES_INTERRUPT_MODE
	//Handler for encoder 2
	void ENCODER2_IRQ_HANDLER(){
		if (TIM_GetITStatus(ENCODER2_TIMER, TIM_IT_Update) != RESET){
			TIM_ClearITPendingBit(ENCODER2_TIMER, TIM_IT_Update);
			if (TIM_GetCounter(ENCODER2_TIMER) < 32768){
				//Overflow
				loopCountEnc2++;
			}else{
				//Underflow
				loopCountEnc2--;
			}
		}
	}
#else
	//Manually update the encoder count without using interrupt
	void encoder_2_update(){
		s16 cnt = (s16) TIM_GetCounter(ENCODER2_TIMER);
		enc2Count = cnt;
		TIM_SetCounter(ENCODER2_TIMER, 0);
	}
#endif

void encoder_1_update(){
		s16 cnt = (s16) TIM_GetCounter(ENCODER1_TIMER);
		enc1Count = cnt;
		TIM_SetCounter(ENCODER1_TIMER, 0);
	}	

	
void encoder_3_update(){
		s16 cnt = (s16) TIM_GetCounter(ENCODER3_TIMER);
		enc3Count = cnt;
		TIM_SetCounter(ENCODER3_TIMER, 0);
	}	

	
void encoder_4_update(){
		s16 cnt = (s16) TIM_GetCounter(ENCODER4_TIMER);
		enc4Count = cnt;
		TIM_SetCounter(ENCODER4_TIMER, 0);
	}	
	
	
int32_t get_encoder_count(EncoderID id){
	if (id == ENCODER_1){
		return enc1Count;
	}
	else if(id == ENCODER_2){
		return enc2Count;
	}
	else if(id == ENCODER_3){
		return enc3Count;
	}
	else{
		return enc4Count;
	}
	
}

void reset_encoder(EncoderID id){
	if (id == ENCODER_1){
		enc1Count = 0;
	}else{
		#if ENCODER_USES_INTERRUPT_MODE
			loopCountEnc2 = 0;
		#else
			enc2Count = 0;
		#endif
		TIM_SetCounter(ENCODER2_TIMER, 0);
	}
}

