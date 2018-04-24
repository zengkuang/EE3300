#include "MPU6500_SPI.h"
#include "ticks.h"

int16_t Gyro_x = 0;
int16_t Gyro_y = 0;
int16_t Gyro_z = 0;
int16_t A_x = 0;
int16_t A_y = 0;
int16_t A_z = 0;
uint8_t BUF[2];
void SPI_GPIO_Configuration(void);
void SPI_GPIO_Configuration(){
	GPIO_InitTypeDef GPIO_InitStruct;
	/*-------- Configuring SCK, MISO, MOSI --------*/
	 
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOC, &GPIO_InitStruct);
	 
	 
	/*-------- Configuring ChipSelect-Pin PD0--------*/ 
	 
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOD, &GPIO_InitStruct);
	
	
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource10, GPIO_AF_SPI3);
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource11, GPIO_AF_SPI3);
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource12, GPIO_AF_SPI3);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOD, ENABLE);
	
}
void SPI_MPU6500_init(void){
	SPI_GPIO_Configuration();
	GPIO_SetBits(GPIOD,GPIO_Pin_0);
	SPI_InitTypeDef SPI_InitTypeDefStruct;
	SPI_InitTypeDefStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitTypeDefStruct.SPI_Mode = SPI_Mode_Master;
	SPI_InitTypeDefStruct.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitTypeDefStruct.SPI_CPOL = SPI_CPOL_High;
	SPI_InitTypeDefStruct.SPI_CPHA = SPI_CPHA_2Edge;
	SPI_InitTypeDefStruct.SPI_NSS = SPI_NSS_Soft;
	SPI_InitTypeDefStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
	SPI_InitTypeDefStruct.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_Init(SPI3, &SPI_InitTypeDefStruct);
	SPI_Cmd(SPI3, ENABLE);
	
	
	mySPI_SendData(PWR_MGMT_1, 0x80|0x40);
	waitms(100);
	mySPI_SendData(SIGNAL_PATH_RESET, 0x07);
	waitms(100);	
	mySPI_SendData(PWR_MGMT_1, 0x01);
	waitms(100);
	mySPI_SendData(CONFIG, 0x03);
	waitms(100);
	mySPI_SendData(GYRO_CONFIG, 1<<3); // +-500dps
	waitms(100);
	mySPI_SendData(ACCEL_CONFIG, 2<<3);// +-8g
	waitms(100);
	mySPI_SendData(ACCEL_CONFIG2, 3);//bandwith41
	
}
void mySPI_SendData(uint8_t address, uint8_t data){
	GPIO_ResetBits(GPIOD, GPIO_Pin_0);
	 
	while(!SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_TXE)); 
	SPI_I2S_SendData(SPI3, address);
	while(!SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_RXNE));
	SPI_I2S_ReceiveData(SPI3);
	 
	while(!SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_TXE)); 
	SPI_I2S_SendData(SPI3, data);
	while(!SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_RXNE));
	SPI_I2S_ReceiveData(SPI3);
	 
	GPIO_SetBits(GPIOD, GPIO_Pin_0);
}
uint8_t mySPI_GetData(uint8_t adress){
 
	GPIO_ResetBits(GPIOD, GPIO_Pin_0); 
	 
	adress = 0x80 | adress;
	 
	while(!SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_TXE)); 
	SPI_I2S_SendData(SPI3, adress);
	while(!SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_RXNE));
	SPI_I2S_ReceiveData(SPI3); //Clear RXNE bit
	 
	while(!SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_TXE)); 
	SPI_I2S_SendData(SPI3, 0x00); //Dummy byte to generate clock
	while(!SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_RXNE));
	 
	GPIO_SetBits(GPIOD, GPIO_Pin_0);
	_delay_us(10);
	return  SPI_I2S_ReceiveData(SPI3);
}

void IMUGetDataRaw(IMUInfo* imu){
	
		BUF[0] = mySPI_GetData(GYRO_XOUT_H);
		BUF[1] = mySPI_GetData(GYRO_XOUT_L);
		Gyro_x = (BUF[0] << 8) | BUF[1];
		
		BUF[0] = mySPI_GetData(GYRO_YOUT_H);
		BUF[1] = mySPI_GetData(GYRO_YOUT_L);
		Gyro_y = (BUF[0] << 8) | BUF[1];
		
		BUF[0] = mySPI_GetData(GYRO_ZOUT_H);
		BUF[1] = mySPI_GetData(GYRO_ZOUT_L);
		Gyro_z = (BUF[0] << 8) | BUF[1];
		
		BUF[0] = mySPI_GetData(ACCEL_XOUT_H);
		BUF[1] = mySPI_GetData(ACCEL_XOUT_L);
		A_x = (BUF[0] << 8) | BUF[1];
		
		BUF[0] = mySPI_GetData(ACCEL_YOUT_H);
		BUF[1] = mySPI_GetData(ACCEL_YOUT_L);
		A_y = (BUF[0] << 8) | BUF[1];
		
		BUF[0] = mySPI_GetData(ACCEL_ZOUT_H);
		BUF[1] = mySPI_GetData(ACCEL_ZOUT_L);
		A_z = (BUF[0] << 8) | BUF[1]; 
		/* Accel X*/
		imu->accelData[0] = (float)A_x * imu->_accel_psc;
		/* Accel Y*/
		imu->accelData[1] = (float)A_y * imu->_accel_psc;
		/* Accel Z*/
		imu->accelData[2] = (float)A_z * imu->_accel_psc;
		/* Gyro X*/
		imu->gyroData[0] = (float)Gyro_x * imu->_gyro_psc;
		/* Gyro Y*/
		imu->gyroData[1] = (float)Gyro_y * imu->_gyro_psc;
		/* Gyro Z*/
		imu->gyroData[2] = (float)Gyro_z * imu->_gyro_psc;
		
		
}




