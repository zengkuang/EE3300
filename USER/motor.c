#include "motor.h"
void GPIO_Config(void){
	GPIO_InitTypeDef  GPIO_InitStructure;
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
}

void TIM3_PWM_Config(void){

	/*
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	TIM_TimeBaseStructure.TIM_Period = 239; //<-TIMx_ARR register 
	TIM_TimeBaseStructure.TIM_Prescaler = 0;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 23;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OC1Init(TIM3, &TIM_OCInitStructure);
	
	TIM_Cmd(TIM3,ENABLE);
	
	RCC_APB2PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
		*/
	GPIO_InitTypeDef GPIO_InitStructure;     
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);   //Open GPIOA Clock
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE);   //Open GPIOC Clock
   
//config PA8 MCO Output
  GPIO_PinAFConfig(GPIOA,GPIO_PinSource8,GPIO_AF_MCO); 
  GPIO_InitStructure.GPIO_Pin=GPIO_Pin_8;
  GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

//Connect TIM3 Pin to AF2
  GPIO_PinAFConfig(GPIOC,GPIO_PinSource6,GPIO_AF_TIM3);  //TIM3_Ch1
  GPIO_PinAFConfig(GPIOC,GPIO_PinSource7,GPIO_AF_TIM3);  //TIM3_Ch2
  GPIO_PinAFConfig(GPIOC,GPIO_PinSource8,GPIO_AF_TIM3);  //TIM3_Ch3
  GPIO_PinAFConfig(GPIOC,GPIO_PinSource9,GPIO_AF_TIM3);  //TIM3_Ch4
// Config TIM3 Pin
	GPIO_InitStructure.GPIO_Pin=(GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9);
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);   //Open TIM3  Clock
/*******************************************************************************
TIM3 Config:generate 4 PWM Signals
TIM3CLK(CK_PSC)=2*PCLK1=2*SYSCLK/4=SYSCLK/2=84MHZ

To get TIM3 counter clock at 21 MHz, the prescaler is computed as follows:
TIM3CLK(CK_CNT)=Fclk_psc/(psc[15:0]+1);
psc[15:0]=(Fclk_psc/Fclk_cnt)-1;
prescaler=psc[15:0]=(Fclk_psc/Fclk_cnt)-1=(84/21)-1=3;

To get TIM3 output clock at 1K Hz, the period (ARR)) is computed as follows:
Fout_clk=Fclk_cnt/(ARR+1);
ARR=(Fclk_cnt/Fout_clk)-1=(21000000/1000)-1=20999;

TIM3 Channelx duty cycle=(TIM3_CCRx/ TIM3_ARR)*100;
TIM3 Channel1 duty cycle=(TIM3_CCR1/ TIM3_ARR)*100=5000/20999=50%
TIM3 Channel2 duty cycle=(TIM3_CCR2/ TIM3_ARR)*100=5000/20999=28.6%
TIM3 Channel3 duty cycle=(TIM3_CCR3/ TIM3_ARR)*100=5000/20999=14.3%
TIM3 Channel4 duty cycle=(TIM3_CCR4/ TIM3_ARR)*100=5000/20999=71.5%
********************************************************************************/
	TIM_TimeBaseStructure.TIM_Prescaler=3;          //clk_cnt prescale
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;   //TIM3 Count mode
	TIM_TimeBaseStructure.TIM_Period=20009;         //Fout_clk=Fclk_cnt/(ARR+1)=21000000/700=30KHZ
	TIM_TimeBaseStructure.TIM_ClockDivision=0;   

	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

/* PWM1 Mode configuration: TIM3_Ch1 */
	TIM_OCInitStructure.TIM_OCMode=TIM_OCMode_PWM1;               //select PWM1 mode
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //config oc1 as output 
	TIM_OCInitStructure.TIM_Pulse=5000;                            //config TIM3_CCR1 vaule
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;    //config oc1 high level avaliable
	TIM_OC1Init(TIM3, &TIM_OCInitStructure);
	TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);         // turn on oc1 preload 

/* PWM1 Mode configuration: TIM3_Ch2 */
	TIM_OCInitStructure.TIM_OCMode=TIM_OCMode_PWM1;               //select PWM1 mode
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //config oc2 as output 
	TIM_OCInitStructure.TIM_Pulse=5000;                            //config TIM3_CCR2 vaule
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;    //config oc2 high level avaliable
	TIM_OC2Init(TIM3, &TIM_OCInitStructure);

	TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);         // turn on oc2 preload 

/* PWM1 Mode configuration: TIM3_CH3 */
	TIM_OCInitStructure.TIM_OCMode=TIM_OCMode_PWM1;               //select PWM1 mode
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //config oc3 as output 
	TIM_OCInitStructure.TIM_Pulse=5000;                            //config TIM3_CCR1 vaule
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;    //config oc3 high level avaliable
	TIM_OC3Init(TIM3, &TIM_OCInitStructure);

	TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable);         // turn on oc3 preload 

/* PWM1 Mode configuration: TIM3_CH4 */
	TIM_OCInitStructure.TIM_OCMode=TIM_OCMode_PWM1;               //select PWM1 mode
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //config oc4 as output 
	TIM_OCInitStructure.TIM_Pulse=5000;                            //config TIM3_CCR1 vaule
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;    //config oc4 high level avaliable
	TIM_OC4Init(TIM3, &TIM_OCInitStructure);

	TIM_OC4PreloadConfig(TIM3, TIM_OCPreload_Enable);         // turn on oc4 preload 

	TIM_ARRPreloadConfig(TIM3, ENABLE);
  /* TIM3 enable counter */
  TIM_Cmd(TIM3, ENABLE);
}

