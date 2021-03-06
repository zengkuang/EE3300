#include "lcd_main.h"
#include "lcd_font.h"

//private data
uint16_t curr_bg_color = BLACK;
uint16_t curr_text_color = BLACK;
uint16_t curr_text_color_sp = BLACK;

uint8_t tft_orientation = 0, tft_enabled = 1;
uint8_t tft_width = 0, tft_height = 0;
uint8_t tft_y_index = 0;
uint8_t char_max_x, char_max_y;

char text							[2][CHAR_MAX_X_ANY][CHAR_MAX_Y_ANY];
uint8_t pointer_to_curr_screen = 0;
uint16_t text_color				[2][CHAR_MAX_X_ANY][CHAR_MAX_Y_ANY];
uint16_t bg_color					[2][CHAR_MAX_X_ANY][CHAR_MAX_Y_ANY];
uint16_t print_pos = 0;

uint8_t tft_get_orientation(){
	return tft_orientation;
}

/**
  * @brief  Initialization of SPI for TFT
  * @param  None
  * @retval None
  */
void tft_spi_init(void){
   //SPI and GPIO Struct Initialization
   SPI_InitTypeDef   	SPI_InitStructure;
   GPIO_InitTypeDef 	GPIO_InitStructure;
   
   //Clock and Pins mapping to SPI1
   RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1,ENABLE);
   RCC_APB2PeriphClockLPModeCmd(RCC_APB2Periph_SPI1,ENABLE);
   GPIO_PinAFConfig(GPIOA, GPIO_PinSource5, GPIO_AF_SPI1);
   GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_SPI1);

   /* Enable GPIOA for RST pin */
   GPIO_InitStructure.GPIO_Pin = TFT_RST_PIN;
   GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
   GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
   GPIO_InitStructure.GPIO_Speed = GPIO_Medium_Speed;
   GPIO_Init(TFT_RST_PORT, &GPIO_InitStructure);

   /* Enable GPIOC for DC Pin */
   GPIO_InitStructure.GPIO_Pin = TFT_DC_PIN;
   GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
   GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
   GPIO_InitStructure.GPIO_Speed = GPIO_Medium_Speed;
   GPIO_Init(TFT_DC_PORT, &GPIO_InitStructure);

   /* Configure TFT_SPI Pin: SCK, MISO and MOSI */
   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 |GPIO_Pin_7;
   GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
   GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
   GPIO_InitStructure.GPIO_Speed = GPIO_Fast_Speed;
   GPIO_Init(GPIOA, &GPIO_InitStructure);
   
   /* Configure TFT_SPI Pin: CS */
   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_CS;
   GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
   GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
   GPIO_InitStructure.GPIO_Speed = GPIO_Fast_Speed;
   GPIO_Init(GPIO_CS, &GPIO_InitStructure);
   
   /* TFT_SPI configuration */
   SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
   SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
   SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
   SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
   SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
   SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
   SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;
   SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB; 
   SPI_Init(TFT_SPI, &SPI_InitStructure);
   
   /* Enable TFT_SPI   */
   SPI_Cmd(TFT_SPI, ENABLE);
   SPI_CalculateCRC( TFT_SPI , DISABLE );		// Disable the CRC checking
   SPI_SSOutputCmd( TFT_SPI , DISABLE );
}

/**
  * @brief  Sending a command
  * @param  command: one byte command to be sent
  * @retval None
  */
void tft_write_command(uint8_t command)
{
	GPIO_ResetBits(GPIO_CS, GPIO_Pin_CS);
	GPIO_ResetBits(TFT_DC_PORT, TFT_DC_PIN);
	/* Loop while DR register in not emplty */
   while (SPI_I2S_GetFlagStatus(TFT_SPI, SPI_I2S_FLAG_TXE) == RESET);
   /* Send byte through the TFT_SPI peripheral */
   SPI_I2S_SendData(TFT_SPI, command);
   while (SPI_I2S_GetFlagStatus(TFT_SPI, SPI_I2S_FLAG_RXNE) == RESET);
   SPI_I2S_ReceiveData(TFT_SPI);

   GPIO_SetBits(GPIO_CS, GPIO_Pin_CS);
}

/**
  * @brief  Sending a data
  * @param  data: one byte data to be sent
  * @retval None
  */
void tft_write_data(uint8_t data)
{
	GPIO_ResetBits(GPIO_CS, GPIO_Pin_CS);
	GPIO_SetBits(TFT_DC_PORT, TFT_DC_PIN);

   /* Loop while DR register in not emplty */
   while (SPI_I2S_GetFlagStatus(TFT_SPI, SPI_I2S_FLAG_TXE) == RESET);
   /* Send byte through the TFT_SPI peripheral */
   SPI_I2S_SendData(TFT_SPI, data);
   while (SPI_I2S_GetFlagStatus(TFT_SPI, SPI_I2S_FLAG_RXNE) == RESET);
   SPI_I2S_ReceiveData(TFT_SPI);

   GPIO_SetBits(GPIO_CS, GPIO_Pin_CS);
}

/**
  * @brief  Configuration of TFT
  * @param  None
  * @retval None
  */
void tft_config(void)
{
	tft_write_command(0x01);   //Sofeware setting
	_delay_ms(10);
	tft_write_command(0x11);//Sleep out
	_delay_ms(120);
	
	//ST7735R Frame Rate
	tft_write_command(0xB1);
	tft_write_data(0x01);
	tft_write_data(0x2C);
	tft_write_data(0x2D);
	tft_write_command(0xB2);
	tft_write_data(0x01);
	tft_write_data(0x2C);
	tft_write_data(0x2D);
	tft_write_command(0xB3);
	tft_write_data(0x01);
	tft_write_data(0x2C);
	tft_write_data(0x2D);
	tft_write_data(0x01);
	tft_write_data(0x2C);
	tft_write_data(0x2D);
	//------------------------------------End ST7735R Frame Rate-----------------------------------------//
	tft_write_command(0xB4);//Column inversion
	tft_write_data(0x07);
	//------------------------------------ST7735R Power Sequence-----------------------------------------//
	tft_write_command(0xC0);
	tft_write_data(0xA2);
	tft_write_data(0x02);
	tft_write_data(0x84);
	tft_write_command(0xC1);
	tft_write_data(0xC5);
	tft_write_command(0xC2);
	tft_write_data(0x0A);
	tft_write_data(0x00);
	tft_write_command(0xC3);
	tft_write_data(0x8A);
	tft_write_data(0x2A);
	tft_write_command(0xC4);
	tft_write_data(0x8A);
	tft_write_data(0xEE);
	//---------------------------------End ST7735R Power Sequence-------------------------------------//
	tft_write_command(0xC5);//VCOM
	tft_write_data(0x0E);
	tft_write_command(0x36);//MX, MY, RGB mode
	tft_write_data(0xC8);
	//------------------------------------ST7735R Gamma Sequence-----------------------------------------//
	tft_write_command(0xe0);
	tft_write_data(0x02);
	tft_write_data(0x1c);
	tft_write_data(0x07);
	tft_write_data(0x12);
	tft_write_data(0x37);
	tft_write_data(0x32);
	tft_write_data(0x29);
	tft_write_data(0x2d);
	tft_write_data(0x29);
	tft_write_data(0x25);
	tft_write_data(0x2b);
	tft_write_data(0x39);
	tft_write_data(0x00);
	tft_write_data(0x01);
	tft_write_data(0x03);
	tft_write_data(0x10);
	tft_write_command(0xe1);
	tft_write_data(0x03);
	tft_write_data(0x1d);
	tft_write_data(0x07);
	tft_write_data(0x06);
	tft_write_data(0x2e);
	tft_write_data(0x2c);
	tft_write_data(0x29);
	tft_write_data(0x2d);
	tft_write_data(0x2e);
	tft_write_data(0x2e);
	tft_write_data(0x37);
	tft_write_data(0x3f);
	tft_write_data(0x00);
	tft_write_data(0x00);
	tft_write_data(0x02);
	tft_write_data(0x10);
	tft_write_command(0x2A);
	tft_write_data(0x00);
	tft_write_data(0x00);
	tft_write_data(0x00);
	tft_write_data(0x7f);

	tft_write_command(0x2B);
	tft_write_data(0x00);
	tft_write_data(0x00);
	tft_write_data(0x00);
	tft_write_data(0x9f);
	//------------------------------------End ST7735R Gamma Sequence-----------------------------------------//

	tft_write_command(0x3A);
	tft_write_data(0x05);  
	tft_write_command(0x29);//Display on

	_delay_ms(10);
}

/**
  * @brief  Hardware reset for TFT
  * @param  None
  * @retval None
  */
void tft_reset(void)
{
	tft_y_index = 0;
 	GPIO_ResetBits(TFT_RST_PORT, TFT_RST_PIN);
	_delay_ms(100);
	GPIO_SetBits(TFT_RST_PORT, TFT_RST_PIN);
	_delay_ms(100);
}

/**
  * @brief  Initialization of TFT
  * @param  orientation: default orientation (0, 1, 2, 3) 
  	(PIN_ON_TOP = 0,
	PIN_ON_LEFT = 1,
	PIN_ON_BOTTOM = 2,
	PIN_ON_RIGHT = 3)
  * @param  in_bg_color: default background color
  * @param  in_text_color: default text color
  * @param  in_text_color_sp: default special text color
  * @retval None
  */
void tft_init(TFT_ORIENTATION orientation, uint16_t in_bg_color, uint16_t in_text_color, uint16_t in_text_color_sp){
	uint8_t x, y;
	tft_y_index = 0;
	pointer_to_curr_screen = 0;
	tft_spi_init();
	tft_reset();
	tft_config();
  tft_write_command(0x2C);
	tft_set_bg_color(in_bg_color);
	tft_set_text_color(in_text_color);
	tft_set_special_color(in_text_color_sp);
	tft_fill_color(in_bg_color);
	tft_orientation = orientation;
	
	if (orientation % 2){
		tft_width = MAX_HEIGHT;
		tft_height = MAX_WIDTH;
		char_max_x = CHAR_MAX_X_HORIZONTAL;
		char_max_y = CHAR_MAX_Y_HORIZONTAL;
	}else{
		tft_height = MAX_HEIGHT;
		tft_width = MAX_WIDTH;
		char_max_x = CHAR_MAX_X_VERTICAL;
		char_max_y = CHAR_MAX_Y_VERTICAL;
	}

	for (uint8_t pointer = 0; pointer<2; pointer++){
		for (x = 0; x <= CHAR_MAX_X_ANY; x++) {
			for (y = 0; y <= CHAR_MAX_Y_ANY; y++) {
				text[pointer][x][y] = ' ';
				text_color[pointer][x][y] = in_text_color;
				bg_color[pointer][x][y] = in_bg_color;
			}
		}
	}
}

/**
  * @brief  Enable using TFT
  * @param  None
  * @retval None
  */
void tft_enable(void)
{
	tft_enabled = 1;
}

/**
  * @brief  Disable using TFT
  * @param  None
  * @retval None
  */
void tft_disable(void)
{
	tft_enabled = 0;
}

/**
  * @brief  Set the current background color
  * @param  None
  * @retval None
  */
void tft_set_bg_color(uint16_t in_bg_color)
{
	curr_bg_color = in_bg_color;
}

/**
  * @brief  Set the current text color
  * @param  None
  * @retval None
  */
void tft_set_text_color(uint16_t in_text_color)
{
	curr_text_color = in_text_color;
}

/**
  * @brief  Set the current special text color
  * @param  None
  * @retval None
  */
void tft_set_special_color(uint16_t text_color_sp)
{
	curr_text_color_sp = text_color_sp;
}

/**
  * @brief  Set the position of one pixel
  * @param  None
  * @retval None
  */
void tft_set_pixel_pos(uint8_t x, uint8_t y)
{
	tft_write_command(0x2a);		// Column addr set
	tft_write_data(0x00);
	tft_write_data(x+2); 				// X START
	tft_write_data(0x00);
	tft_write_data(x+3); 			// X END

	tft_write_command(0x2b);		// Row addr set
	tft_write_data(0x00);
	tft_write_data(y+1);				// Y START
	tft_write_data(0x00);
	tft_write_data(y+2);			// Y END

	tft_write_command(0x2c); 		// write to RAM
}

/**
  * @brief  Set the position of some characters
  * @param  None
  * @retval None
  */
void tft_set_char_pos(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2)
{
	tft_write_command(0x2a);		// Column addr set
	tft_write_data(0x00);
	tft_write_data(x1+2); 			//X START
	tft_write_data(0x00);
	tft_write_data(x2+2); 			//X END
	
	tft_write_command(0x2b);		//Row addr set
	tft_write_data(0x00);
	tft_write_data(y1+1);			//Y START
	tft_write_data(0x00);
	tft_write_data(y2+1);		//Y END

	tft_write_command(0x2c); 		// write to RAM
}

/**
  * @brief  Clear every piexl on screen
  * @param  None
  * @retval None
  */
void tft_force_clear(void)
{
	uint8_t x, y;
	for (x = 0; x <= CHAR_MAX_X_ANY; x++) {
		for (y = 0; y <= CHAR_MAX_Y_ANY; y++) {
			text[0][x][y] = ' ';
			text[1][x][y] = ' ';
		}
	}
	tft_fill_color(curr_bg_color);
}

/**
  * @brief  Clear one line on screen
  * @param  line: the line to be cleared
  * @retval None
  */
void tft_clear_line(uint8_t line){
	for (uint8_t x = 0; x < CHAR_MAX_X_ANY; x++) {
		text[pointer_to_curr_screen][x][line] = ' ';
		text_color[pointer_to_curr_screen][x][line] = curr_text_color;
		bg_color[pointer_to_curr_screen][x][line] = curr_bg_color;
	}
}

/**
  * @brief  Clear all lines on screen
  * @param  None
  * @retval None
  */
void tft_clear(void){
	tft_y_index = 0;
	for(uint8_t y = 0; y < CHAR_MAX_Y_ANY; y++){
		tft_clear_line(y);
	}
}

/**
  * @brief  Print a single pixel on screen
  * @param  x: x-coordinate
  * @param  y: y-coordinate
  * @param  color: color of the pixel
  * @retval None
  */
void tft_put_pixel(uint8_t x, uint8_t y, uint16_t color)
{
	switch (tft_orientation) {
		case 0:
			tft_set_pixel_pos(x, y);
			break;
		case 1:
			tft_set_pixel_pos(MAX_WIDTH-y-1, x);
			break;
		case 2:
			tft_set_pixel_pos(MAX_WIDTH-x-1, MAX_HEIGHT-y-1);
			break;
		case 3:
			tft_set_pixel_pos(y, MAX_HEIGHT-x-1);
			break;
	}
    tft_write_data(color >> 8);
    tft_write_data(color);
}

/**
  * @brief  Fill the whole screen with a color
  * @param  color: color to be filled with
  * @retval None
  */
void tft_fill_color(uint16_t color)
{
	uint16_t i;					//160*128
	
	tft_write_command(0x2a);		// Column addr set
	tft_write_data(0x00);
	tft_write_data(0x02); 				// X START
	tft_write_data(0x00);
	tft_write_data(0x81); 			// X END

	tft_write_command(0x2b);		// Row addr set
	tft_write_data(0x00);
	tft_write_data(0x01);				// Y START
	tft_write_data(0x00);
	tft_write_data(0xA0);			// Y END

	tft_write_command(0x2c); 		// write to RAM
	
	for (i = 0; i < MAX_WIDTH*MAX_HEIGHT; i++) {
		tft_write_data(color >> 8);
		tft_write_data(color);
	}
}

bool tft_char_is_changed(uint8_t x, uint8_t y){
	return !((text[pointer_to_curr_screen][x][y] == text[(pointer_to_curr_screen+1)%2][x][y])
						&& (text_color[pointer_to_curr_screen][x][y] == text_color[(pointer_to_curr_screen+1)%2][x][y])
						&& (bg_color[pointer_to_curr_screen][x][y] == bg_color[(pointer_to_curr_screen+1)%2][x][y]));
}

/**
  * @brief  Print a string at certain position, use [] to indicate special words
  * @param  x: starting x-coordinate
  * @param  y: starting y-coordinate
  * @param  pstr: string to be printed
  */
void tft_prints(uint8_t x, uint8_t y, const char * pstr, ...){
	uint8_t buf[256], is_special = 0;
	uint8_t* fp = NULL;
	
	va_list arglist;
	va_start(arglist, pstr);
	vsprintf((char*)buf, (const char*)pstr, arglist);
	va_end(arglist);
	
	fp = buf;
	while (*fp)	{
		if (*fp == '[') {
			is_special = 1;
			fp++;
		} else if (*fp == ']') {
			is_special = 0;
			fp++;
		} else if (*fp == '\r' || *fp == '\n') {		  				 
			fp++;
		} else {
			if (x > char_max_x || y > char_max_y) {
				break;
			}
		
			text[pointer_to_curr_screen][x][y] = *fp++;
			text_color[pointer_to_curr_screen][x][y] = is_special ? curr_text_color_sp : curr_text_color;
			bg_color[pointer_to_curr_screen][x][y] = curr_bg_color;	
			x++;
		}
	}
}

/**
* Append a line to the tft screen. Not affected by tft_prints
* @param pstr: Stuff to be printed
**/

void tft_println(const char * pstr, ...){
	uint8_t buf[256], is_special = 0;
	uint8_t* fp = NULL;
	uint8_t x = 0;
	
	va_list arglist;
	va_start(arglist, pstr);
	vsprintf((char*)buf, (const char*)pstr, arglist);
	va_end(arglist);
	
	if (tft_y_index>char_max_y){
		tft_y_index = 0;
	}

	fp = buf;
	while (*fp)	{
		if (*fp == '[') {
			is_special = 1;
			fp++;
		} else if (*fp == ']') {
			is_special = 0;
			fp++;
		} else if (*fp == '\r' || *fp == '\n') {		  				 
			fp++;
		} else {
			if (x > char_max_x || tft_y_index > (char_max_y)) {
				break;
			}
			
			text[pointer_to_curr_screen][x][tft_y_index] = *fp++;
			text_color[pointer_to_curr_screen][x][tft_y_index] = is_special ? curr_text_color_sp : curr_text_color;
			bg_color[pointer_to_curr_screen][x][tft_y_index] = curr_bg_color;	
			x++;
		}
	}
	tft_y_index = tft_y_index > char_max_y ? 0 : tft_y_index+1;
}

void tft_stream(const char * pstr, ...){
	//Implementing
}


/**
  * @brief  Refresh the whole screen
  * @param  None
  * @retval None
  */
void tft_update(void)
{
	s16 x, y, x2, y2, px, py;
	s16 char_n = 0;
	uint16_t clr;
	
	if (!tft_enabled)
		return;

	switch (tft_orientation) {
		case 0:
			for (y = 0; y < CHAR_MAX_Y_VERTICAL; y++) {
				for (x = 0; x < CHAR_MAX_X_VERTICAL; x++) {
					if (tft_char_is_changed(x, y)) {
						char_n = 1;
						while (x+char_n < CHAR_MAX_X_VERTICAL && tft_char_is_changed(x+char_n, y)) {
							if (x+char_n >= CHAR_MAX_X_VERTICAL) {
								break;
							} else {
								char_n++;
							}
						}
						tft_set_char_pos(x*CHAR_WIDTH, y*CHAR_HEIGHT, (x+char_n)*CHAR_WIDTH-1, (y+1)*CHAR_HEIGHT-1);
						y2 = y;
						for (py = 0; py < CHAR_HEIGHT; py++) {
							for (px = 0; px < char_n*CHAR_WIDTH; px++) {
								x2 = x+px/CHAR_WIDTH;
								clr = ascii_8x16[((text[pointer_to_curr_screen][x2][y2] - 32) * CHAR_HEIGHT) + py] & (0x80 >> (px % CHAR_WIDTH)) ?
									text_color[pointer_to_curr_screen][x2][y2] : bg_color[pointer_to_curr_screen][x2][y2];
								tft_write_data(clr >> 8);
								tft_write_data(clr);
							}
						}
						x += char_n-1;
					}
				}
			}
			break;
		case 1:
			for (x = 0; x < CHAR_MAX_X_HORIZONTAL; x++) {
				for (y = CHAR_MAX_Y_HORIZONTAL-1; y >= 0; y--) {
					if (tft_char_is_changed(x, y)) {
						char_n = 1;
						while (y-char_n > -1 && tft_char_is_changed(x, y-char_n)) {
							if (y-char_n <= -1) {
								break;
							} else {
								char_n++;
							}
						}
						tft_set_char_pos((CHAR_MAX_Y_HORIZONTAL-y-1)*CHAR_HEIGHT, x*CHAR_WIDTH, (CHAR_MAX_Y_HORIZONTAL-y-1+char_n)*CHAR_HEIGHT-1, (x+1)*CHAR_WIDTH-1);
						x2 = x;
						for (px = 0; px < CHAR_WIDTH; px++) {
							for (py = 0; py < char_n*CHAR_HEIGHT; py++) {
								y2 = y-py/CHAR_HEIGHT;
								clr = ascii_8x16[((text[pointer_to_curr_screen][x2][y2] - 32) * CHAR_HEIGHT) + CHAR_HEIGHT-(py % CHAR_HEIGHT)-1] & (0x80 >> px) ?
									text_color[pointer_to_curr_screen][x2][y2] : bg_color[pointer_to_curr_screen][x2][y2];
								tft_write_data(clr >> 8);
								tft_write_data(clr);
							}
						}
						y -= char_n-1;
					}
				}
			}
			break;
		case 2:
			for (y = CHAR_MAX_Y_VERTICAL-1; y >= 0; y--) {
				for (x = CHAR_MAX_X_VERTICAL-1; x >= 0; x--) {
					if (tft_char_is_changed(x, y)) {
						char_n = 1;
						while (x-char_n > -1 && tft_char_is_changed(x-char_n, y)) {
							if (x-char_n <= -1) {
								break;
							} else {
								char_n++;
							}
						}
						tft_set_char_pos((CHAR_MAX_X_VERTICAL-x-1)*CHAR_WIDTH, (CHAR_MAX_Y_VERTICAL-y-1)*CHAR_HEIGHT, (CHAR_MAX_X_VERTICAL-x-1+char_n)*CHAR_WIDTH-1, (CHAR_MAX_Y_VERTICAL-y)*CHAR_HEIGHT-1);
						y2 = y;
						for (py = 0; py < CHAR_HEIGHT; py++) {
							for (px = 0; px < char_n*CHAR_WIDTH; px++) {
								x2 = x-px/CHAR_WIDTH;
								clr = ascii_8x16[((text[pointer_to_curr_screen][x2][y2] - 32) * CHAR_HEIGHT) + (CHAR_HEIGHT-py-1)] & (0x80 >> (CHAR_WIDTH-(px % CHAR_WIDTH)-1)) ?
									text_color[pointer_to_curr_screen][x2][y2] : bg_color[pointer_to_curr_screen][x2][y2];
								tft_write_data(clr >> 8);
								tft_write_data(clr);
							}
						}
						x -= char_n-1;
					}
				}
			}
			break;
		case 3:
			for (x = CHAR_MAX_X_HORIZONTAL-1; x >= 0; x--) {
				for (y = 0; y < CHAR_MAX_Y_HORIZONTAL; y++) {
					if (tft_char_is_changed(x, y)) {
						char_n = 1;
						while (y+char_n < CHAR_MAX_Y_HORIZONTAL && tft_char_is_changed(x, y+char_n)) {
							if (y+char_n >= CHAR_MAX_Y_HORIZONTAL) {
								break;
							} else {
								char_n++;
							}
						}
						tft_set_char_pos(y*CHAR_HEIGHT, (CHAR_MAX_X_HORIZONTAL-x-1)*CHAR_WIDTH, (y+char_n)*CHAR_HEIGHT-1, (CHAR_MAX_X_HORIZONTAL-x)*CHAR_WIDTH-1);
						x2 = x;
						for (px = 0; px < CHAR_WIDTH; px++) {
							for (py = 0; py < char_n*CHAR_HEIGHT; py++) {
								y2 = y+py/CHAR_HEIGHT;
								clr = ascii_8x16[((text[pointer_to_curr_screen][x2][y2] - 32) * CHAR_HEIGHT) + (py % CHAR_HEIGHT)] & (0x80 >> (CHAR_WIDTH-px-1)) ?
									text_color[pointer_to_curr_screen][x2][y2] : bg_color[pointer_to_curr_screen][x2][y2];
								tft_write_data(clr >> 8);
								tft_write_data(clr);
							}
						}
						y += char_n-1;
					}
				}
			}
			break;
	}
	pointer_to_curr_screen = (pointer_to_curr_screen+1) %2;
}

/**
* Place an awesome logo at x, y
* Like, very awesome
**/
/*
void tft_put_logo(uint8_t x, uint8_t y){
	//Place black background
	for (uint8_t loopX = 0; loopX<47; loopX++){
		for (uint8_t loopY = 0; loopY<39; loopY++){
			tft_put_pixel(x+loopX, y+loopY, BLACK);
		}
	}

	//Place DARK_RED parallelogram
	for (uint8_t loopX = 15; loopX<45; loopX++){
		tft_put_pixel(x+loopX, y+1, DARK_RED);
	}
	for (uint8_t loopX = 14; loopX<44; loopX++){
		tft_put_pixel(x+loopX, y+2, DARK_RED);
		tft_put_pixel(x+loopX, y+3, DARK_RED);
	}
	for (uint8_t loopX = 13; loopX<43; loopX++){
		tft_put_pixel(x+loopX, y+4, DARK_RED);
		tft_put_pixel(x+loopX, y+5, DARK_RED);
		tft_put_pixel(x+loopX, y+6, DARK_RED);
	}
	for (uint8_t loopX = 12; loopX<42; loopX++){
		tft_put_pixel(x+loopX, y+7, DARK_RED);
		tft_put_pixel(x+loopX, y+8, DARK_RED);
		tft_put_pixel(x+loopX, y+9, DARK_RED);
	}
	for (uint8_t loopX = 11; loopX<41; loopX++){
		tft_put_pixel(x+loopX, y+10, DARK_RED);
		tft_put_pixel(x+loopX, y+11, DARK_RED);
	}
	for (uint8_t loopX = 10; loopX<40; loopX++){
		tft_put_pixel(x+loopX, y+12, DARK_RED);
		tft_put_pixel(x+loopX, y+13, DARK_RED);
		tft_put_pixel(x+loopX, y+14, DARK_RED);
		tft_put_pixel(x+loopX, y+15, DARK_RED);
	}
	for (uint8_t loopX = 9; loopX<39; loopX++){
		tft_put_pixel(x+loopX, y+16, DARK_RED);
		tft_put_pixel(x+loopX, y+17, DARK_RED);
		tft_put_pixel(x+loopX, y+18, DARK_RED);
	}
	for (uint8_t loopX = 8; loopX<38; loopX++){
		tft_put_pixel(x+loopX, y+19, DARK_RED);
		tft_put_pixel(x+loopX, y+20, DARK_RED);
	}
	for (uint8_t loopX = 7; loopX<37; loopX++){
		tft_put_pixel(x+loopX, y+21, DARK_RED);
		tft_put_pixel(x+loopX, y+22, DARK_RED);
		tft_put_pixel(x+loopX, y+23, DARK_RED);
		tft_put_pixel(x+loopX, y+24, DARK_RED);
	}

	//Put the R letter
	tft_put_pixel(x+16, y+6, GREY);
	for (uint8_t loopX = 17; loopX<36; loopX++){
		tft_put_pixel(x+loopX, y+6, WHITE);
	}

	for (uint8_t loopX = 15; loopX<37; loopX++){
		tft_put_pixel(x+loopX, y+7, WHITE);
	}

	tft_put_pixel(x+14, y+8, DARK_GREY);
	for (uint8_t loopX = 15; loopX<37; loopX++){
		tft_put_pixel(x+loopX, y+8, WHITE);
	}
	tft_put_pixel(x+37, y+8, GREY);

	for (uint8_t loopX = 14; loopX<38; loopX++){
		tft_put_pixel(x+loopX, y+9, WHITE);
	}

	for (uint8_t loopX = 14; loopX<20; loopX++){
		tft_put_pixel(x+loopX, y+10, WHITE);
	}
	tft_put_pixel(x+20, y+10, GREY);
	tft_put_pixel(x+28, y+10, GREY);
	for (uint8_t loopX = 31; loopX<38; loopX++){
		tft_put_pixel(x+loopX, y+10, WHITE);
	}

	for (uint8_t loopX = 14; loopX<20; loopX++){
		tft_put_pixel(x+loopX, y+11, WHITE);
	}
	tft_put_pixel(x+28, y+11, GREY);
	tft_put_pixel(x+29, y+11, DARK_GREY);
	for (uint8_t loopX = 32; loopX<38; loopX++){
		tft_put_pixel(x+loopX, y+11, WHITE);
	}

	tft_put_pixel(x+13, y+12, GREY);
	for (uint8_t loopX = 14; loopX<19; loopX++){
		tft_put_pixel(x+loopX, y+12, WHITE);
	}
	tft_put_pixel(x+19, y+12, DARK_GREY);
	tft_put_pixel(x+21, y+12, WHITE);
	for (uint8_t loopX = 30; loopX<38; loopX++){
		tft_put_pixel(x+loopX, y+12, WHITE);
	}

	for (uint8_t loopX = 13; loopX<19; loopX++){
		tft_put_pixel(x+loopX, y+13, WHITE);
	}
	tft_put_pixel(x+19, y+13, GREY);
	tft_put_pixel(x+28, y+13, WHITE);
	tft_put_pixel(x+29, y+13, WHITE);
	for (uint8_t loopX = 32; loopX<38; loopX++){
		tft_put_pixel(x+loopX, y+13, WHITE);
	}

	for (uint8_t loopX = 13; loopX<19; loopX++){
		tft_put_pixel(x+loopX, y+14, WHITE);
	}
	tft_put_pixel(x+19, y+14, DARK_GREY);
	tft_put_pixel(x+20, y+14, DARK_GREY);
	tft_put_pixel(x+28, y+14, GREY);
	tft_put_pixel(x+31, y+14, GREY);
	for (uint8_t loopX = 32; loopX<38; loopX++){
		tft_put_pixel(x+loopX, y+14, WHITE);
	}

	//LEFT LEG
	for (uint8_t loopX = 12; loopX<18; loopX++){
		tft_put_pixel(x+loopX, y+15, WHITE);
		tft_put_pixel(x+loopX, y+16, WHITE);
		tft_put_pixel(x+loopX, y+17, WHITE);
	}
	for (uint8_t loopX = 11; loopX<17; loopX++){
		tft_put_pixel(x+loopX, y+18, WHITE);
		tft_put_pixel(x+loopX, y+19, WHITE);
		tft_put_pixel(x+loopX, y+20, WHITE);
	}
	for (uint8_t loopX = 10; loopX<16; loopX++){
		tft_put_pixel(x+loopX, y+21, WHITE);
		tft_put_pixel(x+loopX, y+22, WHITE);
	}
	for (uint8_t loopX = 10; loopX<15; loopX++){
		tft_put_pixel(x+loopX, y+23, WHITE);
	}
	for (uint8_t loopX = 9; loopX<15; loopX++){
		tft_put_pixel(x+loopX, y+24, WHITE);
	}
	for (uint8_t loopX = 9; loopX<14; loopX++){
		tft_put_pixel(x+loopX, y+25, WHITE);
		tft_put_pixel(x+loopX, y+26, WHITE);
	}

	//RIGHT LEG
	for (uint8_t loopX = 21; loopX<38; loopX++){
		tft_put_pixel(x+loopX, y+15, WHITE);
	}
	for (uint8_t loopX = 20; loopX<38; loopX++){
		tft_put_pixel(x+loopX, y+16, WHITE);
	}
	for (uint8_t loopX = 20; loopX<37; loopX++){
		tft_put_pixel(x+loopX, y+17, WHITE);
	}
	for (uint8_t loopX = 21; loopX<36; loopX++){
		tft_put_pixel(x+loopX, y+18, WHITE);
	}
	for (uint8_t loopX = 21; loopX<29; loopX++){
		tft_put_pixel(x+loopX, y+19, WHITE);
	}
	for (uint8_t loopX = 21; loopX<30; loopX++){
		tft_put_pixel(x+loopX, y+20, WHITE);
		tft_put_pixel(x+loopX+1, y+21, WHITE);
		tft_put_pixel(x+loopX+2, y+22, WHITE);
		tft_put_pixel(x+loopX+3, y+23, WHITE);
	}
	for (uint8_t loopX = 25; loopX<33; loopX++){
		tft_put_pixel(x+loopX, y+24, WHITE);
	}
	for (uint8_t loopX = 26; loopX<35; loopX++){
		tft_put_pixel(x+loopX, y+25, WHITE);
	}
	for (uint8_t loopX = 27; loopX<35; loopX++){
		tft_put_pixel(x+loopX, y+26, WHITE);
	}

	//Misc in R letter
	tft_put_pixel(x+11, y+17, DARK_GREY);
	tft_put_pixel(x+21, y+17, GREY);
	tft_put_pixel(x+17, y+18, DARK_GREY);
	tft_put_pixel(x+36, y+18, GREY);
	for (uint8_t loopX = 29; loopX<34; loopX++){
		tft_put_pixel(x+loopX, y+19, GREY);
	}
	tft_put_pixel(x+34, y+19, DARK_GREY);
	tft_put_pixel(x+10, y+20, GREY);
	tft_put_pixel(x+18, y+20, WHITE);
	tft_put_pixel(x+16, y+21, GREY);
	tft_put_pixel(x+20, y+21, WHITE);
	tft_put_pixel(x+18, y+22, WHITE);
	tft_put_pixel(x+18, y+22, GREY);
	tft_put_pixel(x+9, y+23, DARK_GREY);
	tft_put_pixel(x+33, y+24, GREY);
	tft_put_pixel(x+14, y+25, DARK_GREY);
	tft_put_pixel(x+8, y+26, GREY);
	tft_put_pixel(x+35, y+26, DARK_GREY);

	//Bright Red in red region
	tft_put_pixel(x+15, y+3, RED);
	tft_put_pixel(x+17, y+4, RED);
	tft_put_pixel(x+16, y+5, RED);
	tft_put_pixel(x+35, y+5, RED);
	tft_put_pixel(x+38, y+6, RED);
	tft_put_pixel(x+49, y+6, RED);
	tft_put_pixel(x+39, y+7, RED);
	tft_put_pixel(x+11, y+14, RED);
	tft_put_pixel(x+20, y+13, RED);
	tft_put_pixel(x+39, y+14, RED);
	tft_put_pixel(x+39, y+15, RED);
	tft_put_pixel(x+20, y+18, RED);
	tft_put_pixel(x+37, y+18, RED);
	tft_put_pixel(x+38, y+19, RED);
	tft_put_pixel(x+34, y+20, RED);
	tft_put_pixel(x+36, y+20, RED);
	tft_put_pixel(x+33, y+22, RED);
	tft_put_pixel(x+8, y+23, RED);
	tft_put_pixel(x+20, y+23, RED);
	tft_put_pixel(x+21, y+23, RED);
	tft_put_pixel(x+33, y+23, RED);
	tft_put_pixel(x+8, y+24, RED);
	tft_put_pixel(x+17, y+24, RED);
	tft_put_pixel(x+22, y+24, RED);
	tft_put_pixel(x+36, y+24, RED);
	tft_put_pixel(x+7, y+25, RED);
	tft_put_pixel(x+24, y+25, RED);
	tft_put_pixel(x+7, y+26, RED);
	tft_put_pixel(x+15, y+26, RED);
	tft_put_pixel(x+18, y+26, RED);
	tft_put_pixel(x+19, y+26, RED);
	tft_put_pixel(x+22, y+26, RED);
	tft_put_pixel(x+23, y+26, RED);
	tft_put_pixel(x+24, y+26, RED);

	//Put the HKUST word
	tft_put_pixel(x+6, y+28, WHITE);
	tft_put_pixel(x+10, y+28, WHITE);
	tft_put_pixel(x+11, y+28, WHITE);
	tft_put_pixel(x+13, y+28, WHITE);
	tft_put_pixel(x+17, y+28, WHITE);
	tft_put_pixel(x+20, y+28, WHITE);
	tft_put_pixel(x+23, y+28, WHITE);
	tft_put_pixel(x+24, y+28, WHITE);
	for (uint8_t loopX=27;loopX<36;loopX++){
		tft_put_pixel(x+loopX, y+28, WHITE);
	}

	tft_put_pixel(x+5, y+29, WHITE);
	tft_put_pixel(x+6, y+29, WHITE);
	tft_put_pixel(x+9, y+29, WHITE);
	tft_put_pixel(x+12, y+29, WHITE);
	tft_put_pixel(x+13, y+29, WHITE);
	tft_put_pixel(x+16, y+29, WHITE);
	tft_put_pixel(x+17, y+29, WHITE);
	tft_put_pixel(x+19, y+29, WHITE);
	tft_put_pixel(x+20, y+29, WHITE);
	tft_put_pixel(x+22, y+29, WHITE);
	tft_put_pixel(x+23, y+29, WHITE);
	tft_put_pixel(x+24, y+29, WHITE);
	for (uint8_t loopX=26;loopX<29;loopX++){
		tft_put_pixel(x+loopX, y+29, WHITE);
	}
	for (uint8_t loopX=29;loopX<35;loopX++){
		tft_put_pixel(x+loopX, y+29, WHITE);
	}

	tft_put_pixel(x+5, y+30, WHITE);
	tft_put_pixel(x+6, y+30, WHITE);
	tft_put_pixel(x+8, y+30, WHITE);
	tft_put_pixel(x+9, y+30, WHITE);
	tft_put_pixel(x+12, y+30, WHITE);
	tft_put_pixel(x+13, y+30, WHITE);
	tft_put_pixel(x+15, y+30, WHITE);
	tft_put_pixel(x+16, y+30, WHITE);
	tft_put_pixel(x+19, y+30, WHITE);
	tft_put_pixel(x+20, y+30, WHITE);
	tft_put_pixel(x+22, y+30, WHITE);
	tft_put_pixel(x+23, y+30, WHITE);
	tft_put_pixel(x+26, y+30, WHITE);
	tft_put_pixel(x+27, y+30, WHITE);
	tft_put_pixel(x+32, y+30, WHITE);

	for (uint8_t loopX=5;loopX<10;loopX++){
		tft_put_pixel(x+loopX, y+31, WHITE);
	}
	for (uint8_t loopX=11;loopX<16;loopX++){
		tft_put_pixel(x+loopX, y+31, WHITE);
	}
	tft_put_pixel(x+19, y+31, WHITE);
	tft_put_pixel(x+20, y+31, WHITE);
	tft_put_pixel(x+22, y+31, WHITE);
	tft_put_pixel(x+23, y+31, WHITE);
	tft_put_pixel(x+25, y+31, WHITE);
	tft_put_pixel(x+26, y+31, WHITE);
	tft_put_pixel(x+27, y+31, WHITE);
	tft_put_pixel(x+31, y+31, WHITE);
	tft_put_pixel(x+32, y+31, WHITE);

	for (uint8_t loopX=4;loopX<10;loopX++){
		tft_put_pixel(x+loopX, y+32, WHITE);
	}
	tft_put_pixel(x+11, y+32, WHITE);
	tft_put_pixel(x+12, y+32, WHITE);
	tft_put_pixel(x+14, y+32, WHITE);
	tft_put_pixel(x+15, y+32, WHITE);
	tft_put_pixel(x+18, y+32, WHITE);
	tft_put_pixel(x+19, y+32, WHITE);
	tft_put_pixel(x+21, y+32, WHITE);
	tft_put_pixel(x+22, y+32, WHITE);
	tft_put_pixel(x+26, y+32, WHITE);
	tft_put_pixel(x+27, y+32, WHITE);
	tft_put_pixel(x+31, y+32, WHITE);
	tft_put_pixel(x+32, y+32, WHITE);

	tft_put_pixel(x+4, y+33, WHITE);
	tft_put_pixel(x+5, y+33, WHITE);
	tft_put_pixel(x+7, y+33, WHITE);
	tft_put_pixel(x+8, y+33, WHITE);
	tft_put_pixel(x+11, y+33, WHITE);
	tft_put_pixel(x+12, y+33, WHITE);
	tft_put_pixel(x+14, y+33, WHITE);
	tft_put_pixel(x+15, y+33, WHITE);
	tft_put_pixel(x+18, y+33, WHITE);
	tft_put_pixel(x+19, y+33, WHITE);
	tft_put_pixel(x+21, y+33, WHITE);
	tft_put_pixel(x+22, y+33, WHITE);
	tft_put_pixel(x+25, y+33, WHITE);
	tft_put_pixel(x+26, y+33, WHITE);
	tft_put_pixel(x+27, y+33, WHITE);
	tft_put_pixel(x+30, y+33, WHITE);
	tft_put_pixel(x+31, y+33, WHITE);

	tft_put_pixel(x+3, y+34, WHITE);
	tft_put_pixel(x+4, y+34, WHITE);
	tft_put_pixel(x+5, y+34, WHITE);
	tft_put_pixel(x+7, y+34, WHITE);
	tft_put_pixel(x+8, y+34, WHITE);
	tft_put_pixel(x+10, y+34, WHITE);
	tft_put_pixel(x+11, y+34, WHITE);
	tft_put_pixel(x+14, y+34, WHITE);
	tft_put_pixel(x+15, y+34, WHITE);
	tft_put_pixel(x+18, y+34, WHITE);
	tft_put_pixel(x+19, y+34, WHITE);
	tft_put_pixel(x+20, y+34, WHITE);
	tft_put_pixel(x+21, y+34, WHITE);
	tft_put_pixel(x+24, y+34, WHITE);
	tft_put_pixel(x+25, y+34, WHITE);
	tft_put_pixel(x+26, y+34, WHITE);
	tft_put_pixel(x+30, y+34, WHITE);
	tft_put_pixel(x+31, y+34, WHITE);

	tft_put_pixel(x+3, y+35, WHITE);
	tft_put_pixel(x+4, y+35, WHITE);
	tft_put_pixel(x+7, y+35, WHITE);
	tft_put_pixel(x+19, y+35, WHITE);
	tft_put_pixel(x+20, y+35, WHITE);
	tft_put_pixel(x+24, y+35, WHITE);

	//Place minor detail of HKUST word
	tft_put_pixel(x+5, y+28, GREY);
	tft_put_pixel(x+12, y+28, GREY);
	tft_put_pixel(x+16, y+28, GREY);
	tft_put_pixel(x+8, y+29, GREY);
	tft_put_pixel(x+31, y+30, GREY);
	tft_put_pixel(x+4, y+31, GREY);
	tft_put_pixel(x+23, y+32, GREY);
	tft_put_pixel(x+10, y+35, GREY);
	tft_put_pixel(x+11, y+35, GREY);
	tft_put_pixel(x+14, y+35, GREY);
	tft_put_pixel(x+15, y+35, GREY);
	tft_put_pixel(x+18, y+35, GREY);
	tft_put_pixel(x+25, y+35, GREY);
	tft_put_pixel(x+30, y+35, GREY);
	tft_put_pixel(x+31, y+35, GREY);

	tft_put_pixel(x+7, y+28, DARK_GREY);
	tft_put_pixel(x+25, y+30, DARK_GREY);
	tft_put_pixel(x+13, y+32, DARK_GREY);
	tft_put_pixel(x+25, y+32, DARK_GREY);
	tft_put_pixel(x+24, y+33, DARK_GREY);
	tft_put_pixel(x+6, y+35, DARK_GREY);
	tft_put_pixel(x+12, y+34, DARK_GREY);
	tft_put_pixel(x+16, y+34, DARK_GREY);
	tft_put_pixel(x+16, y+35, DARK_GREY);
}
*/