

/******************** ��ѧ�Ƽ� **************************
 * ʵ��ƽ̨��������STM32������
 * ��汾  ��ST3.5.0
 * ����    ����ѧ�Ƽ��Ŷ� 
 * �Ա�    ��http://shop102218275.taobao.com/
 * ������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
 *��Ȩ���У�����ؾ���
**********************************************************************************/


#include "stm32f4xx.h"
#include "LED.H"
#include "timer.h"
#include "motor.h"
int main(void)
{

		/* led �˿����� */ 
	LED_GPIO_Config();	
	/* TIM2 ��ʱ���� */
	TIME_NVIC_Configuration();
  //TIME_Configuration();
	TIM3_PWM_Config();
	GPIO_Config();
	GPIO_ResetBits(GPIOB,GPIO_Pin_5); 
	GPIO_SetBits(GPIOB,GPIO_Pin_6); 
	GPIO_SetBits(GPIOB,GPIO_Pin_7);
while(1)
{


}
}






