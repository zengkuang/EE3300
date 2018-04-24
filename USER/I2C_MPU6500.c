#include "I2C_MPU6500.h"
#include "lcd_main.h"
#define I2C_Speed 100000
#define I2C_SLAVE_ADDRESS7 0x35

void GPIO_Configuration(void);
void I2C_Configuration(void);
void LongDelay(u32 nCount);

void GPIO_Configuration(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure; 
  
  /* Configure I2C_GY80 pins: SCL and SDA */
  GPIO_InitStructure.GPIO_Pin =  I2C_MPU6500_SCL | I2C_MPU6500_SDA; 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_Init(I2C_MPU6500_GPIO, &GPIO_InitStructure);
	GPIO_PinAFConfig(I2C_MPU6500_GPIO,I2C_SCL_PINSOURCE,GPIO_AF_I2C2);
	GPIO_PinAFConfig(I2C_MPU6500_GPIO,I2C_SDA_PINSOURCE,GPIO_AF_I2C2);

}

void I2C_Configuration(void)
{
  I2C_InitTypeDef  I2C_InitStructure; 
  
  /* I2C configuration */
  I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
  I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
  I2C_InitStructure.I2C_OwnAddress1 = I2C_SLAVE_ADDRESS7;
  I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
  I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
  I2C_InitStructure.I2C_ClockSpeed = I2C_Speed;
  I2C_SendData(I2C_MPU6500,0x6B);
  /* I2C Peripheral Enable */
  I2C_Cmd(I2C_MPU6500, ENABLE);
  /* Apply I2C configuration after enabling it */
  I2C_Init(I2C_MPU6500, &I2C_InitStructure);
}

void MPU6500_Init(void)
{
	//tft_prints(0,0,"Initing");
	//tft_update();
  I2C_ByteWrite(MPU6500_SLAVE_ADDRESS, PWR_MGMT_1, 0x00);
	//tft_prints(0,0,"Initing1");
	//tft_update();
 // I2C_ByteWrite(MPU6500_SLAVE_ADDRESS, SMPLRT_DIV, 0x07);
	//tft_prints(0,0,"Initing2");
	//tft_update();
	//I2C_ByteWrite(MPU6500_SLAVE_ADDRESS, CONFIG, 0x06);
	//tft_prints(0,0,"Initing3");
	//tft_update();
	I2C_ByteWrite(MPU6500_SLAVE_ADDRESS, GYRO_CONFIG, 0x00);
	//tft_prints(0,0,"Initing4");
	//tft_update();
	I2C_ByteWrite(MPU6500_SLAVE_ADDRESS, ACCEL_CONFIG, 0x00);
	//tft_prints(0,0,"Initing5");
	//tft_update();
	
	/*
	I2C_ByteWrite(0xD0, PWR_MGMT_1, 0x00);
  I2C_ByteWrite(0xD0, SMPLRT_DIV, 0x07);
	I2C_ByteWrite(0xD0, CONFIG, 0x06);
	I2C_ByteWrite(0xD0, GYRO_CONFIG, 0x18);
	I2C_ByteWrite(0xD0, ACCEL_CONFIG, 0x01);
	*/
	}
void I2C_MPU6500_init()
{
  /* I2C Periph clock enable */
  RCC_APB1PeriphClockCmd(I2C_MPU6500_CLK, ENABLE);   
  
  /* GPIO Periph clock enable */
  RCC_AHB1PeriphClockCmd(I2C_MPU6500_GPIO_CLK, ENABLE);    
     
  /* GPIO configuration */
  GPIO_Configuration();

  /* I2C configuration */
  I2C_Configuration();

}


void I2C_ByteWrite(uint8_t SlaveAddr, uint8_t WriteAddr, uint8_t WriteByte)
{

  /* Send STRAT condition */
  I2C_GenerateSTART(I2C_MPU6500, ENABLE);
	//tft_prints(0,0,"1");
	//tft_update();
  /* Test on EV5 and clear it */
  //while(!I2C_CheckEvent(I2C_MPU6500, I2C_EVENT_MASTER_MODE_SELECT)); 
	while(!((u16)(I2C2->SR1) & (u16)(0x0001)));
	while(!((u16)(I2C2->SR2) & (u16)(0x0011)) == 0x0011);

  /* Send EEPROM address for write */
  I2C_Send7bitAddress(I2C_MPU6500, SlaveAddr, I2C_Direction_Transmitter);
  	//tft_prints(0,0,"2");
	//tft_update();
  /* Test on EV6 and clear it */
  //while(!I2C_CheckEvent(I2C_MPU6500, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
	while(!((u16)(I2C2->SR1) & (u16)(0x0082)) == 0x0082);
	while(!((u16)(I2C2->SR2) & (u16)(0x0007)) == 0x0007);
  I2C_SendData(I2C_MPU6500, WriteAddr);
  //tft_prints(0,0,"3");
	//tft_update();
  /* Test on EV8 and clear it */
 // while(!I2C_CheckEvent(I2C_MPU6500, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	while(!((u16)(I2C2->SR1) & (u16)(0x0084)) == 0x0084);
	while(!((u16)(I2C2->SR2) & (u16)(0x0007)) == 0x0007);
  /* Send the byte to be written */
  I2C_SendData(I2C_MPU6500, WriteByte); 
   	//tft_prints(0,0,"4");
	//tft_update();
  /* Test on EV8 and clear it */
  //while(!I2C_CheckEvent(I2C_MPU6500, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
  while(!((u16)(I2C2->SR1) & (u16)(0x0084)) == 0x0084);
	while(!((u16)(I2C2->SR2) & (u16)(0x0007)) == 0x0007);
//tft_prints(0,0,"5");
	//tft_update();
  /* Send STOP condition */
  I2C_GenerateSTOP(I2C_MPU6500, ENABLE);

  /* Delay */
  LongDelay(0x50);
}
uint8_t I2C_ByteRead(uint8_t I2C_Addr, uint8_t Reg_addr)
{ 
uint8_t readout;
    u32 tmr;

    //tmr = ulTimeOut_Time;
    while(I2C_GetFlagStatus(I2C_MPU6500, I2C_FLAG_BUSY));
    

    I2C_GenerateSTART(I2C_MPU6500, ENABLE);
   
  //  tmr = ulTimeOut_Time;
    //while((!I2C_CheckEvent(I2C_MPU6500, I2C_EVENT_MASTER_MODE_SELECT)));
	  while(!((u16)(I2C2->SR1) & (u16)(0x0001)));
	  while(!((u16)(I2C2->SR2) & (u16)(0x0011)) == 0x0011);
    //if(tmr==0) I2C_Err = 1;

    I2C_Send7bitAddress(I2C_MPU6500,I2C_Addr,I2C_Direction_Transmitter);
   // tmr = ulTimeOut_Time;
   // while((--tmr)&&(!I2C_CheckEvent(I2C_MPU6500,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)));
		while(!((u16)(I2C2->SR1) & (u16)(0x0082)) == 0x0082);
	  while(!((u16)(I2C2->SR2) & (u16)(0x0007)) == 0x0007);
   // if(tmr==0) I2C_Err = 1;

    I2C_SendData(I2C_MPU6500, Reg_addr);
    //tmr = ulTimeOut_Time;
   // while((!I2C_CheckEvent(I2C_MPU6500,I2C_EVENT_MASTER_BYTE_TRANSMITTED)));
    while(!((u16)(I2C2->SR1) & (u16)(0x0084)) == 0x0084);
	  while(!((u16)(I2C2->SR2) & (u16)(0x0007)) == 0x0007);

    I2C_GenerateSTART(I2C_MPU6500, ENABLE);
    
//while((!I2C_CheckEvent(I2C_MPU6500, I2C_EVENT_MASTER_MODE_SELECT)));
    while(!((u16)(I2C2->SR1) & (u16)(0x0001)));
	  while(!((u16)(I2C2->SR2) & (u16)(0x0011)) == 0x0011);  

    I2C_Send7bitAddress(I2C_MPU6500, I2C_Addr, I2C_Direction_Receiver);
    //while((!I2C_CheckEvent(I2C_MPU6500, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED)));
		while(!((u16)(I2C2->SR1) & (u16)(0x0002)) == 0x0002);
	  while(!((u16)(I2C2->SR2) & (u16)(0x0003)) == 0x0003);
    

    I2C_AcknowledgeConfig(I2C_MPU6500, DISABLE);
    I2C_GenerateSTOP(I2C_MPU6500, ENABLE);
    //while((!(I2C_CheckEvent(I2C_MPU6500, I2C_EVENT_MASTER_BYTE_RECEIVED))));  /* EV7 */
    while(!((u16)(I2C2->SR1) & (u16)(0x0040)) == 0x0040);
	  while(!((u16)(I2C2->SR2) & (u16)(0x0003)) == 0x0003);

    readout = I2C_ReceiveData(I2C_MPU6500);

    I2C_AcknowledgeConfig(I2C_MPU6500, ENABLE);



  /* Delay */
  LongDelay(0x4000);
    return readout;

}






void LongDelay(u32 nCount)
{
  for(; nCount != 0; nCount--);
}