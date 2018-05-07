typedef struct{
	int I;
	float error_P;
  float error_I;
	float error_D;
	float error[3];
}pid_controller_t;

typedef enum{
	stop = 0,
	forward = 1,
	backward = 2,
	left = 3,
	right =4,
	rotate_left = 5,
	rotate_right = 6,
}chassis_mode;

typedef struct{
	float vx;
	float vy;
	float vw;
	
	float heading_sp;
	float speed_sp[4];
	int pwm_signal[4];
	chassis_mode mode;
} chassisStruct;
extern chassisStruct chassis;
void go_forward(void);
void go_backward(void);
void go_left(void);
void go_right(void);
void chassis_init(void);
void chassis_operation(pid_controller_t*,pid_controller_t*,pid_controller_t*,pid_controller_t*,pid_controller_t*,int*);



