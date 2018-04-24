#include "stm32f4xx.h"
#include "LED.H"
#include "timer.h"
#include "motor.h"
#include "lcd_main.h"
#include "encoder.h"
#include "ticks.h"
#include "chassis.h"
#include "usart.h"
#include "MPU6500_SPI.h"
#include "math.h"
#define M_PI 3.14159265358979323846
#define KEY0 GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_4)   //PE4 PRESS - 0
//#define KEY2 GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0)	//PA0  WK_UP PRESS - 1
#define LED1_OFF GPIO_SetBits(GPIOF,GPIO_Pin_9);    //PF9
#define LED1_ON GPIO_ResetBits(GPIOF,GPIO_Pin_9);   //PF9
#define LED2_OFF GPIO_SetBits(GPIOF,GPIO_Pin_10);    //PF10
#define LED2_ON GPIO_ResetBits(GPIOF,GPIO_Pin_10);   //PF10
void Delayus(int duration);






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
	pid_controller_t pid1;
	pid_controller_t pid2;
	pid_controller_t pid3;
	pid_controller_t pid4;
	IMUInfo mpu6500;
	
	/*
	PID Configuration
	*/
	pid1.error_P = 30;
	pid1.error_I = 4;
	pid1.I = 0;
	pid2.error_P = 30;
	pid2.error_I = 4;
	pid2.I = 0;
	pid3.error_P = 20;
	pid3.error_I = 4;
	pid3.I = 0;
	pid4.error_P = 30;
	pid4.error_I = 4;
	pid4.I = 0;
	
	/*IMU scalar Configuration
	
	*/
	mpu6500._accel_psc = (GRAV /  4096.0f);
	mpu6500._gyro_psc = (1.0f /  65.5f) * M_PI/180.0f;;
	
	
	
	chassis_init();
	LED_GPIO_Config();	
	LED1_OFF;
	LED2_OFF;
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
	USART1_INIT();
	DMA_config();
	SPI_MPU6500_init();
	/*
	I2C_MPU6500_init();
	MPU6500_Init();
	sprintf(str, "%d", I2C_ByteRead(MPU6500_SLAVE_ADDRESS,GYRO_CONFIG));
	tft_prints(5,5,str);
	*/
	/*
	if(I2C_ByteRead(MPU6500_SLAVE_ADDRESS,0x75) == whoami){
		tft_prints(0,0,"Connected");
		tft_update();
	}
	else{
		tft_prints(0,0,"not Connected");
		tft_update();
	}*/
	 sprintf(str, "%d",mySPI_GetData(WHO_AM_I));
	 tft_prints(0,0,str);
	 tft_update();
while(1)
{
	
	if(time != get_ticks()){
		time = get_ticks();
	}
	if(time - time2 > 100){
		IMUGetDataRaw(&mpu6500);

		/*
		BUF[0] = I2C_ByteRead(MPU6500_SLAVE_ADDRESS,GYRO_XOUT_H);
		BUF[1] = I2C_ByteRead(MPU6500_SLAVE_ADDRESS,GYRO_XOUT_L);
		Gyro_x = (BUF[0] << 8) | BUF[1];
		
		
		BUF[2] = I2C_ByteRead(MPU6500_SLAVE_ADDRESS,GYRO_YOUT_H);
		BUF[3] = I2C_ByteRead(MPU6500_SLAVE_ADDRESS,GYRO_YOUT_L);
		Gyro_y = (BUF[2] << 8) | BUF[3];
		
		
		BUF[4] = I2C_ByteRead(MPU6500_SLAVE_ADDRESS,GYRO_ZOUT_H);
		BUF[5] = I2C_ByteRead(MPU6500_SLAVE_ADDRESS,GYRO_ZOUT_L);
		Gyro_z = (BUF[4] << 8) | BUF[5];
		
		BUF[6] = I2C_ByteRead(MPU6500_SLAVE_ADDRESS,ACCEL_XOUT_H);
		BUF[7] = I2C_ByteRead(MPU6500_SLAVE_ADDRESS,ACCEL_XOUT_H);
		A_x = (BUF[6] << 8) | BUF[7];
		
		BUF[8] = I2C_ByteRead(MPU6500_SLAVE_ADDRESS,ACCEL_YOUT_H);
		BUF[9] = I2C_ByteRead(MPU6500_SLAVE_ADDRESS,ACCEL_YOUT_H);
		A_y = (BUF[8] << 8) | BUF[9];
		
		BUF[10] = I2C_ByteRead(MPU6500_SLAVE_ADDRESS,ACCEL_ZOUT_H);
		BUF[11] = I2C_ByteRead(MPU6500_SLAVE_ADDRESS,ACCEL_ZOUT_H);
		A_z = (BUF[10] << 8) | BUF[11];
		
		
		*/
		
		
		
		time2 = time;
		chassis_operation(&pid1,&pid2,&pid3,&pid4,channel);
	
		sprintf(str, "%.4f", mpu6500.accelData[0]);
		tft_prints(1,1,"      ");
		tft_prints(1,1,str);
		sprintf(str, "%.4f",  mpu6500.accelData[1]);
		tft_prints(1,2,"      ");
		tft_prints(1,2,str);
		sprintf(str, "%.4f",  mpu6500.accelData[2]);
		tft_prints(1,3,"      ");
		tft_prints(1,3,str);
		sprintf(str, "%.4f", mpu6500.gyroData[0]);
		tft_prints(1,4,"      ");
		tft_prints(1,4,str);
		sprintf(str, "%.4f", mpu6500.gyroData[1]);
		tft_prints(1,5,"      ");
		tft_prints(1,5,str);
		sprintf(str, "%.4f", mpu6500.gyroData[2]);
		tft_prints(1,6,"      ");
		tft_prints(1,6,str);
		sprintf(str, "%d", get_seconds());
		tft_prints(0,0,str);
		
		/*
		sprintf(str, "%d", get_encoder_count(ENCODER_4));
		tft_prints(4,4,str);		
		sprintf(str, "%d", get_seconds());
		tft_prints(0,0,str);
		*/
		tft_update();
		
		
		}
	//_delay_ms(10);

}
}



void Delayus(int duration)
{
		while(duration--) 
		{
			int i=0x02;				
			while(i--)
			__asm("nop");
		}
}




