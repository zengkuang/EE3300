#ifndef _MPU6500_SPI_H_
#define _MPU6500_SPI_H_
#include "stm32f4xx.h"

#define I2C_MPU6500  I2C2
#define I2C_MPU6500_CLK RCC_APB1Periph_I2C2
#define I2C_MPU6500_GPIO GPIOB
#define I2C_MPU6500_GPIO_CLK RCC_AHB1Periph_GPIOB
#define I2C_MPU6500_SCL GPIO_Pin_10
#define I2C_MPU6500_SDA GPIO_Pin_11
#define I2C_SCL_PINSOURCE          GPIO_PinSource10
#define I2C_SDA_PINSOURCE          GPIO_PinSource11
#define MPU6500_SLAVE_ADDRESS 0x68
#define whoami 0x70

#define	SMPLRT_DIV		0x19
#define	CONFIG			0x1A	
#define	GYRO_CONFIG		0x1B	
#define	ACCEL_CONFIG	0x1C	
#define	ACCEL_CONFIG2	0x1D	
#define SIGNAL_PATH_RESET 0x68
#define	ACCEL_XOUT_H	0x3B
#define	ACCEL_XOUT_L	0x3C
#define	ACCEL_YOUT_H	0x3D
#define	ACCEL_YOUT_L	0x3E
#define	ACCEL_ZOUT_H	0x3F
#define	ACCEL_ZOUT_L	0x40

#define	TEMP_OUT_H		0x41
#define	TEMP_OUT_L		0x42

#define	GYRO_XOUT_H		0x43
#define	GYRO_XOUT_L		0x44	
#define	GYRO_YOUT_H		0x45
#define	GYRO_YOUT_L		0x46
#define	GYRO_ZOUT_H		0x47
#define	GYRO_ZOUT_L		0x48           

#define	PWR_MGMT_1		0x6B	
#define	WHO_AM_I		  0x75	
#define GRAV          9.80665f
typedef struct{
	float gyroData[3];
	float accelData[3];
	float _accel_psc;
  float _gyro_psc;
	float gyro_offset[3];
	int calibrated;
}IMUInfo;


void IMUGetDataRaw(IMUInfo* );
void SPI_MPU6500_init(void);
void mySPI_SendData(uint8_t address, uint8_t data);
void calibrateGyro(IMUInfo* );
uint8_t mySPI_GetData(uint8_t );
void DataConverge(IMUInfo* );
#endif