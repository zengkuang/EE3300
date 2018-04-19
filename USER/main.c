#include "stm32f4xx.h"
#include "LED.H"
#include "timer.h"
#include "motor.h"
#include "lcd_main.h"
#include "encoder.h"
#include "ticks.h"
#include "chassis.h"
#include "usart.h"


#define KEY0 GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_4)   //PE4 PRESS - 0
//#define KEY2 GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0)	//PA0  WK_UP PRESS - 1
#define LED1_OFF GPIO_SetBits(GPIOF,GPIO_Pin_9);    //PF9
#define LED1_ON GPIO_ResetBits(GPIOF,GPIO_Pin_9);   //PF9
#define LED2_OFF GPIO_SetBits(GPIOF,GPIO_Pin_10);    //PF10
#define LED2_ON GPIO_ResetBits(GPIOF,GPIO_Pin_10);   //PF10


void Key_GPIO_Config(void)
{ 
	GPIO_InitTypeDef  GPIO_InitStructure;
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);//使能GPIOA,GPIOE时钟
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4; //KEY0 对应引脚
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//普通输入模式
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100M
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
  GPIO_Init(GPIOE, &GPIO_InitStructure);//初始化GPIOE4
	/*
	 RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;//WK_UP对应引脚PA0
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN ;//下拉
  GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA0
		 */

}
/*
int8_t  ReceiveBuff[RECEIVE_BUF_SIZE];   
int16_t UART1_ReceiveSize = 0; 
*/
int main(void)
{	
	USART1_INIT();
	DMA_config();
	pid_controller_t pid1;
	pid_controller_t pid2;
	pid_controller_t pid3;
	pid_controller_t pid4;
	pid1.error_P = 100;
	pid1.error_I = 0.5;
	pid1.I = 0;
	pid2.error_P = 100;
	pid2.error_I = 0.5;
	pid2.I = 0;
	pid3.error_P = 100;
	pid3.error_I = 0.5;
	pid3.I = 0;
	pid4.error_P = 100;
	pid4.error_I = 0.5;
	pid4.I = 0;
	chassis_init();
	LED_GPIO_Config();	
	//TIME_NVIC_Configuration();
  //TIME_Configuration();
	int32_t time = 0;
	int32_t time2 = 0;
	TIM3_PWM_Config();
	GPIO_Config();
	Key_GPIO_Config();
	encoder_1_init();
	encoder_2_init();
	encoder_3_init();
	encoder_4_init();
	ticks_init();
	char str[20];
	tft_init(0, WHITE, BLACK, WHITE);
	sprintf(str, "%d", get_encoder_count(ENCODER_2));
	tft_prints(0,0,str);
	tft_prints(2,2,str);
	tft_update();
	uint16_t rxdata = 0;
while(1)
{
	
	if(time != get_ticks()){
		time = get_ticks();
	}
	if(time - time2 > 100){
		if (USART_GetFlagStatus(USART1,USART_FLAG_RXNE) == SET) {
			rxdata = USART_ReceiveData(USART1);
		  if (rxdata == 0x30) {
          LED1_OFF;
					LED2_OFF;
			}	
			else if (rxdata == 'R') {
		     LED1_ON;
				 LED2_ON;
			}	
	
		}
		time2 = time;
		if(KEY0 == 0){
			chassis.mode = forward;
			/*
			drive_motor(5000, 5000, 5000,5000);
		*/
			}
		else{
			chassis.mode = stop;
			/*
			drive_motor(0, 0, 0, 0);
		*/
			}
		chassis_operation(&pid1,&pid2,&pid3,&pid4);
		sprintf(str, "%d", get_encoder_count(ENCODER_1));
		tft_prints(1,1,str);
		sprintf(str, "%d", get_encoder_count(ENCODER_2));
		tft_prints(2,2,str);
		sprintf(str, "%d", get_encoder_count(ENCODER_3));
		tft_prints(3,3,str);
		sprintf(str, "%d", get_encoder_count(ENCODER_4));
		tft_prints(4,4,str);		
		sprintf(str, "%d", get_seconds());
		tft_prints(0,0,str);
		tft_update();
		
		}
	//_delay_ms(10);

}
}






