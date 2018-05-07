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
#include "servo.h"
#include "AHRS.h"
#define KEY0 GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_4)   //PE4 PRESS - 0
//#define KEY2 GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0)	//PA0  WK_UP PRESS - 1
#define LED1_OFF GPIO_SetBits(GPIOF,GPIO_Pin_9);    //PF9
#define LED1_ON GPIO_ResetBits(GPIOF,GPIO_Pin_9);   //PF9
#define LED2_OFF GPIO_SetBits(GPIOF,GPIO_Pin_10);    //PF10
#define LED2_ON GPIO_ResetBits(GPIOF,GPIO_Pin_10);   //PF10
void Key_GPIO_Config(void)
{ 
	GPIO_InitTypeDef  GPIO_InitStructure;
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOE, &GPIO_InitStructure);

}

int main(void)
{	
	pid_controller_t pid1;
	pid_controller_t pid2;
	pid_controller_t pid3;
	pid_controller_t pid4;
	pid_controller_t heading_pid;
	IMUInfo mpu6500;
	int roll_value = 6200;
	int pitch_value = 6200;
	/*
	PID Configuration
	*/
	pid1.error_P = 20;
	pid1.error_I = 10;
	pid1.I = 0;
	pid2.error_P = 20;
	pid2.error_I = 10;
	pid2.I = 0;
	pid3.error_P = 20;
	pid3.error_I = 10;
	pid3.I = 0;
	pid4.error_P = 20;
	pid4.error_I = 10;
	pid4.I = 0;
	heading_pid.error_P = 5;
	heading_pid.error_D = 1;	
	/*IMU scalar Configuration
	*/
	mpu6500._accel_psc = (GRAV /  4096.0f);
	mpu6500._gyro_psc = (1.0f /  65.5f) * M_PI/180.0f;
	chassis_init();
	LED_GPIO_Config();	
	LED1_OFF;
	LED2_OFF;
	int32_t time = 0;
	int32_t time2 = 0;
	TIM3_PWM_Config();
	motor_GPIO_Config();
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
	calibrateGyro(&mpu6500);
	servo_timer_config();
	DataConverge(&mpu6500);
	getRollPitchYaw();
	chassis.heading_sp = rpy[2];
while(1)
{
	
	if(time != get_ticks()){
		time = get_ticks();
	}
	if(time - time2 > 20){
		int start =  get_ticks();
		time2 = time;
		chassis_operation(&pid1,&pid2,&pid3,&pid4,&heading_pid,channel);

		/*
		left y-axis larger to right  right x-axis larger to front
		*/
		/*
		rpy[3] 0-roll 1-pitch 2-yaw
		*/
		
		IMUGetDataRaw(&mpu6500);
		MadgwickAHRSupdateIMU(mpu6500.gyroData[0],mpu6500.gyroData[1],mpu6500.gyroData[2],\
		mpu6500.accelData[0],mpu6500.accelData[1],mpu6500.accelData[2]);
		getRollPitchYaw();
		if(channel[2] >10 || channel[2] <-10 ){
			chassis.heading_sp = rpy[2];
		}
		roll_value += (rpy[0] * 8);
		pitch_value -= (rpy[1] * 8);
		servo_change(roll_value ,pitch_value);
		
		/*
		servo_change(6200,6200);
		waitms(1000);
		servo_change(6200,6400);
		waitms(1000);
		servo_change(6200,6600);
		waitms(1000);
		servo_change(6200,6400);
		waitms(1000);
		servo_change(6200,6200);
		waitms(1000);
		servo_change(6200,6000);
		waitms(1000);
		servo_change(6200,5800);
		waitms(1000);
		servo_change(6200,6000);
		waitms(1000);
		*/
		
    /*
		sprintf(str, " %.4f",rpy[0]);
		tft_prints(1,1,"      ");
		tft_prints(1,1,str);
		
		sprintf(str, " %.4f",  rpy[1]);
		tft_prints(1,2,"      ");
		tft_prints(1,2,str);
		*/
		
		sprintf(str, "%.4f",  rpy[2]);
		tft_prints(1,3,"      ");
		tft_prints(1,3,str);
		
		/*
		sprintf(str, "roll : %.4f", mpu6500.accelData[0]);
		tft_prints(1,1,"      ");
		tft_prints(1,1,str);
		sprintf(str, "pitch : %.4f",  mpu6500.accelData[1]);
		tft_prints(1,2,"      ");
		tft_prints(1,2,str);
		sprintf(str, "yaw: %.4f",  mpu6500.accelData[2]);
		tft_prints(1,3,"      ");
		tft_prints(1,3,str);
		*/
/*
		sprintf(str, "%.2f", mpu6500.gyroData[0]);
		tft_prints(1,4,"      ");
		tft_prints(1,4,str);
		sprintf(str, "%.2f", mpu6500.gyroData[1]);
		tft_prints(1,5,"      ");
		tft_prints(1,5,str);
		sprintf(str, "%.2f", mpu6500.gyroData[2]);
		tft_prints(1,6,"      ");
		tft_prints(1,6,str);
	*/	
		/*
		sprintf(str, "%d", get_ticks() -start);
		tft_prints(0,0,str);
		*/
		tft_update();
		
		}

}
}






