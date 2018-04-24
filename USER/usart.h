#include<stdint.h>
#define RECEIVE_BUF_SIZE 8
extern unsigned char ReceiveBuff[RECEIVE_BUF_SIZE];
extern int channel[4];
void USART1_INIT(void);
void DMA_config(void);

