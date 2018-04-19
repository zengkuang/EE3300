#include<stdint.h>
#define RECEIVE_BUF_SIZE 8
extern int8_t ReceiveBuff[RECEIVE_BUF_SIZE];
void USART1_INIT(void);
void DMA_config(void);

