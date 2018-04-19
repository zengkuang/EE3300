#include "chassis.h"
#include "encoder.h"
#include "motor.h"
#include "lcd_main.h"


chassisStruct chassis;
static inline float boundOutput(const float input, const float max)
{
  float output;
  if(input <= max && input >= -max)
    output = input;
  else if(input > max)
    output = max;
  else
    output = -max;

  return output;
}
#define OUTPUT_MAX 20000
static int16_t chassis_controlSpeed(int speed_sp, int speed, pid_controller_t* controller) 
{ 
  float error = speed_sp - speed; 
  controller->I += error;  //KI
  controller->I = boundOutput(controller->I, 40000);  
  float output = error* controller->error_P + controller->I * controller->error_I; 
	//float output = 0;
  return (int16_t)(boundOutput(output,OUTPUT_MAX)); 
} 

void chassis_init(){

	chassis.pwm_signal[0] = 0;
	chassis.pwm_signal[1] = 0;
	chassis.pwm_signal[2] = 0;
	chassis.pwm_signal[3] = 0;
}
void chassis_operation(pid_controller_t* pid1,pid_controller_t* pid2,pid_controller_t* pid3, pid_controller_t* pid4){
	switch(chassis.mode){
		case stop:{
			chassis.vy = 0;
			chassis.vx = 0;
			chassis.vw = 0;
		}break;
		case forward:{
			
			chassis.vy = 100;
			chassis.vx = 0;
			chassis.vw = 0;
		}break;
		case backward:{
			chassis.vy = -100;
			chassis.vx = 0;
			chassis.vw = 0;
		}break;
		case left:{
		}break;
		case right:{
		}break;
		case rotate_left:{
		}break;
		case rotate_right:{
		}break;
		default:{
		}break;
	}
		chassis.speed_sp[0] = chassis.vy - chassis.vx + chassis.vw;
		chassis.speed_sp[1] = chassis.vy + chassis.vx - chassis.vw;
		chassis.speed_sp[2] = chassis.vy - chassis.vx - chassis.vw;
		chassis.speed_sp[3] = chassis.vy + chassis.vx + chassis.vw;
		encoder_1_update();
		encoder_2_update();
		encoder_3_update();
		encoder_4_update();
		char str[20];
		sprintf(str, "%d", pid1->I);
		tft_prints(8,7,str);
		sprintf(str, "%.1f", pid1->error_P);
		tft_prints(8,5,str);
		sprintf(str, "%.1f", pid1->error_I);
		tft_prints(8,6,str);
		chassis.pwm_signal[0] = chassis_controlSpeed(chassis.speed_sp[0],get_encoder_count(ENCODER_1),pid1);
		chassis.pwm_signal[1] = chassis_controlSpeed(chassis.speed_sp[1],get_encoder_count(ENCODER_2),pid2);
		chassis.pwm_signal[2] = chassis_controlSpeed(chassis.speed_sp[2],get_encoder_count(ENCODER_3),pid3);
		chassis.pwm_signal[3] = chassis_controlSpeed(chassis.speed_sp[3],get_encoder_count(ENCODER_4),pid4);

	
	/*	
		sprintf(str, "%.1f", chassis.speed_sp[0]);
		tft_prints(8,5,str);
		sprintf(str, "%.1f", chassis.speed_sp[1]);
		tft_prints(8,6,str);
		sprintf(str, "%.1f", chassis.speed_sp[2]);
		tft_prints(8,7,str);
		sprintf(str, "%.1f",chassis.speed_sp[3]);
		tft_prints(8,8,str);
		*/
		sprintf(str, "%d", chassis.pwm_signal[0]);
		tft_prints(1,5,str);
		sprintf(str, "%d", chassis.pwm_signal[1]);
		tft_prints(1,6,str);
		sprintf(str, "%d", chassis.pwm_signal[2]);
		tft_prints(1,7,str);
		sprintf(str, "%d",chassis.pwm_signal[3]);
		tft_prints(1,8,str);
	
		drive_motor(chassis.pwm_signal[0],chassis.pwm_signal[1],chassis.pwm_signal[2],chassis.pwm_signal[3]);
}

