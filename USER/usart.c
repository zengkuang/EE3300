#include "usart.h"
#include "stm32f4xx.h"

unsigned char  ReceiveBuff[RECEIVE_BUF_SIZE];   
int16_t UART1_ReceiveSize = 0; 
int channel[4];
void USART1_INIT(void){
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure; 
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_AHB1Periph_GPIOA , ENABLE);
	
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART1); //GPIOA9USART1
  GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1); //GPIOA10

  
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 ; //
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_Init(GPIOA, &GPIO_InitStructure); 
	
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	USART_InitStructure.USART_BaudRate = 9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART1, &USART_InitStructure);

	
	

	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;        
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;  
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;  
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;           
	NVIC_Init(&NVIC_InitStructure); 
	USART_ITConfig(USART1, USART_IT_IDLE, ENABLE); 
	
	USART_Cmd(USART1, ENABLE); 
	USART_DMACmd(USART1,USART_DMAReq_Rx,ENABLE);

}

void DMA_config(void){
		DMA_InitTypeDef  DMA_InitStructure; 
		NVIC_InitTypeDef NVIC_InitStructure; 
    DMA_InitStructure.DMA_Channel = DMA_Channel_4;   
	  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
    DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&USART1->DR; 
    DMA_InitStructure.DMA_Memory0BaseAddr = (u32)ReceiveBuff;  
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory ;  
    DMA_InitStructure.DMA_BufferSize = RECEIVE_BUF_SIZE;   
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable; 
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable; 
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte; 
    DMA_InitStructure.DMA_Priority = DMA_Priority_Medium; 
    DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;           
    DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;  
    DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single; 
    DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single; 
    DMA_Init(DMA2_Stream2, &DMA_InitStructure);

    DMA_Cmd(DMA2_Stream2, ENABLE);  
		
      
    //DMA_ITConfig(DMA2_Stream5,DMA_IT_TC,ENABLE); 
}


void DMA2_Stream5_IRQHandler(void)  
{  
    
    if(DMA_GetFlagStatus(DMA2_Stream5,DMA_FLAG_TCIF5)!=RESET)  
    {   
        DMA_Cmd(DMA2_Stream5, DISABLE); 
        UART1_ReceiveSize =RECEIVE_BUF_SIZE - DMA_GetCurrDataCounter(DMA2_Stream5);  
        if(UART1_ReceiveSize !=0)  
        {  
            /*
						receive completed
					*/ 
        }  
          
        DMA_ClearFlag(DMA2_Stream5,DMA_FLAG_TCIF5 | DMA_FLAG_FEIF5 | DMA_FLAG_DMEIF5 | DMA_FLAG_TEIF5 | DMA_FLAG_HTIF5);//??DMA2_Steam7??????  
        DMA_SetCurrDataCounter(DMA2_Stream5, RECEIVE_BUF_SIZE);  
        DMA_Cmd(DMA2_Stream5, ENABLE);   
    }  
}  
  
void USART1_IRQHandler(void)  
{  
		
    u16 rxdata;  
    if(USART_GetITStatus(USART1,USART_IT_IDLE) != RESET)  
    {  
			
				DMA_Cmd(DMA2_Stream2, DISABLE);            
        UART1_ReceiveSize =RECEIVE_BUF_SIZE - DMA_GetCurrDataCounter(DMA2_Stream2);  
				if(ReceiveBuff[0] ==0xAA  && ReceiveBuff[1] ==0xAA && ReceiveBuff[6]==0x55 && ReceiveBuff[7] ==0x55){
						if(ReceiveBuff[2] < 0x70){
							channel[0] = ReceiveBuff[2];
						}
						else{
							channel[0] = 0 - (256- ReceiveBuff[2]);
						}
						if(ReceiveBuff[3] < 0x70){
							channel[1] = ReceiveBuff[3];
						}
						else{
							channel[1] = 0 - (256- ReceiveBuff[3]);
						}
						if(ReceiveBuff[4] < 0x70){
							channel[2] = ReceiveBuff[4];
						}
						else{
							channel[2] = 0 - (256- ReceiveBuff[4]);
						}
						if(ReceiveBuff[5] < 0x70){
							channel[3] = ReceiveBuff[5];
						}
						else{
							channel[3] = 0 - (256- ReceiveBuff[5]);
						}
							
				}
			
			
			
			
        if(UART1_ReceiveSize !=0)  
        {    
					GPIO_ResetBits(GPIOF,GPIO_Pin_9);//LED1_ON;
					GPIO_ResetBits(GPIOF,GPIO_Pin_10);//LED2_ON 
        }  
				else{
					GPIO_SetBits(GPIOF,GPIO_Pin_9);//LED1_ON;
					GPIO_SetBits(GPIOF,GPIO_Pin_10);//LED2_ON 
				}
        DMA_ClearFlag(DMA2_Stream2,DMA_FLAG_TCIF5 | DMA_FLAG_FEIF5 | DMA_FLAG_DMEIF5 | DMA_FLAG_TEIF5 | DMA_FLAG_HTIF5);
        DMA_SetCurrDataCounter(DMA2_Stream2, RECEIVE_BUF_SIZE);  
        DMA_Cmd(DMA2_Stream2, ENABLE);      
    }  
}