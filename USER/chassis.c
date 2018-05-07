#include "chassis.h"
#include "encoder.h"
#include "motor.h"
#include "lcd_main.h"
#include "AHRS.h"

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
  return (int16_t)(boundOutput(output,OUTPUT_MAX)); 
} 

static int16_t chassis_heading_control(pid_controller_t* controller,float get, float set){ 
  controller->error[0] = set - get; 
  float output = controller->error[0]*controller->error_P+controller->error_D*(controller->error[0]-2*controller->error[1]+controller->error[2]); 
  controller->error[1] = controller->error[0]; 
  controller->error[2] = controller->error[1]; 
 
  return output; 
 
} 





void chassis_init(){

	chassis.pwm_signal[0] = 0;
	chassis.pwm_signal[1] = 0;
	chassis.pwm_signal[2] = 0;
	chassis.pwm_signal[3] = 0;
}
void chassis_operation(pid_controller_t* pid1,pid_controller_t* pid2,pid_controller_t* pid3, pid_controller_t* pid4, pid_controller_t* heading,int* channel){
		chassis.vx = channel[0];
		chassis.vy = channel[1];
		if(channel[2] >10 || channel[2] <-10){
			chassis.vw = channel[2];
		}
		else{
			chassis.vw = chassis_heading_control(heading,rpy[2], chassis.heading_sp);
		}
		chassis.speed_sp[0] = chassis.vy - chassis.vx + chassis.vw;
		chassis.speed_sp[1] = chassis.vy + chassis.vx - chassis.vw;
		chassis.speed_sp[2] = chassis.vy - chassis.vx - chassis.vw;
		chassis.speed_sp[3] = chassis.vy + chassis.vx + chassis.vw;
		encoder_1_update();
		encoder_2_update();
		encoder_3_update();
		encoder_4_update();
		chassis.pwm_signal[0] = chassis_controlSpeed(chassis.speed_sp[0],get_encoder_count(ENCODER_1),pid1);
		chassis.pwm_signal[1] = chassis_controlSpeed(chassis.speed_sp[1],get_encoder_count(ENCODER_2),pid2);
		chassis.pwm_signal[2] = chassis_controlSpeed(chassis.speed_sp[2],get_encoder_count(ENCODER_3),pid3);
		chassis.pwm_signal[3] = chassis_controlSpeed(chassis.speed_sp[3],get_encoder_count(ENCODER_4),pid4);

		char str[20];
		drive_motor(chassis.pwm_signal[0],chassis.pwm_signal[1],chassis.pwm_signal[2],chassis.pwm_signal[3]);
}

