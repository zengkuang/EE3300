#include "servo.h"
static 	TIM_OCInitTypeDef TIM_OCInitStructure1;
static 	TIM_OCInitTypeDef TIM_OCInitStructure2;
void servo_timer_config(void){	
	GPIO_InitTypeDef GPIO_InitStructure;     
	//Open GPIOC Clock
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE); 
//Connect TIM9 Pin to AF
  GPIO_PinAFConfig(GPIOA,GPIO_PinSource2,GPIO_AF_TIM9);  //TIM9_Ch1
  GPIO_PinAFConfig(GPIOA,GPIO_PinSource3,GPIO_AF_TIM9);  //TIM9_Ch2
	GPIO_InitStructure.GPIO_Pin=(GPIO_Pin_2|GPIO_Pin_3);
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM9, ENABLE);   //Open TIM9  Clock
	
	TIM_TimeBaseStructure.TIM_Prescaler=41;          //clk_cnt prescale
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;   //TIM9 Count mode
	TIM_TimeBaseStructure.TIM_Period=39999;         // 100HZ
	TIM_TimeBaseStructure.TIM_ClockDivision=0;   
	TIM_TimeBaseInit(TIM9, &TIM_TimeBaseStructure);
// Continue here
/* PWM1 Mode configuration: TIM9_Ch1 */
	TIM_OCInitStructure1.TIM_OCMode=TIM_OCMode_PWM1;               //select PWM1 mode
	TIM_OCInitStructure1.TIM_OutputState = TIM_OutputState_Enable; //config oc1 as output 
	TIM_OCInitStructure1.TIM_Pulse=6200;                            //config TIM9_CCR1 vaule
	TIM_OCInitStructure1.TIM_OCPolarity = TIM_OCPolarity_High;    //config oc1 high level avaliable
	TIM_OC1Init(TIM9, &TIM_OCInitStructure1);
	TIM_OC1PreloadConfig(TIM9, TIM_OCPreload_Enable);         // turn on oc1 preload 
	
	/* PWM1 Mode configuration: TIM9_Ch2 */
	TIM_OCInitStructure2.TIM_OCMode=TIM_OCMode_PWM1;               //select PWM1 mode
	TIM_OCInitStructure2.TIM_OutputState = TIM_OutputState_Enable; //config oc2 as output 
	TIM_OCInitStructure2.TIM_Pulse=6200;                            //config TIM9_CCR2 vaule
	TIM_OCInitStructure2.TIM_OCPolarity = TIM_OCPolarity_High;    //config oc2 high level avaliable
	TIM_OC2Init(TIM9, &TIM_OCInitStructure2);
	TIM_OC2PreloadConfig(TIM9, TIM_OCPreload_Enable);         // turn on oc2 preload 
	
	TIM_ARRPreloadConfig(TIM9, ENABLE);
  /* TIM9 enable counter */
  TIM_Cmd(TIM9, ENABLE);
}
/*pwm range:  4000~8000
*/
void servo_change(uint16_t pwm1, uint16_t pwm2)
{
	
		if(pwm1 < 4500){
			pwm1 = 4500;
		}
		if(pwm1 > 7500){
			pwm1 = 7500;
		}
		if(pwm2 < 4500){
			pwm2 = 4500;
		}
		if(pwm2 > 7500){
			pwm2 = 7500;
		}
		TIM_OCInitStructure1.TIM_Pulse= pwm1;
		TIM_OC1Init(TIM9, &TIM_OCInitStructure1);
	
		TIM_OCInitStructure2.TIM_Pulse= pwm2;
		TIM_OC2Init(TIM9, &TIM_OCInitStructure2);
	
}