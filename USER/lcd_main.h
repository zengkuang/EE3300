#ifndef __LCD_MAIN_H
#define __LCD_MAIN_H

#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_spi.h"
#include "stm32f4xx_rcc.h"
//#include "ticks.h"
#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>

typedef enum {
	PIN_ON_TOP = 0,
	PIN_ON_LEFT = 1,
	PIN_ON_BOTTOM = 2,
	PIN_ON_RIGHT = 3
}TFT_ORIENTATION;

#define TFT_RST_PIN		GPIO_Pin_6
#define TFT_DC_PIN		GPIO_Pin_4
#define TFT_RST_PORT	GPIOA
#define TFT_DC_PORT		GPIOC

#define TFT_SPI			SPI1
#define GPIO_Pin_CS	GPIO_Pin_4
#define RCC_AHB1Periph_GPIO_CS	RCC_AHB1Periph_GPIOA
#define GPIO_CS			GPIOA
#define	BGR888_MODE		1

#if (!BGR888_MODE)
#define	RGB888TO565(RGB888)  (((RGB888 >> 8) & 0xF800) |((RGB888 >> 5) & 0x07E0) | ((RGB888 >> 3) & 0x001F))
#else 
#define	RGB888TO565(BGR888)  (((BGR888 >> 19) & 0x001F) |((BGR888 >> 5) & 0x07E0) | (((u32)BGR888 << 8) & 0xF800))
#endif

#define WHITE               (RGB888TO565(0xFFFFFF))
#define BLACK               (RGB888TO565(0x000000))
#define DARK_GREY           (RGB888TO565(0x555555))
#define GREY                (RGB888TO565(0xAAAAAA))
#define RED                 (RGB888TO565(0xFF0000))
#define DARK_RED            (RGB888TO565(0x800000))
#define ORANGE              (RGB888TO565(0xFF9900))
#define YELLOW              (RGB888TO565(0xFFFF00))
#define GREEN               (RGB888TO565(0x00FF00))
#define DARK_GREEN          (RGB888TO565(0x00CC00))
#define BLUE                (RGB888TO565(0x0000FF))
#define BLUE2               (RGB888TO565(0x202060))
#define SKY_BLUE            (RGB888TO565(0x11CFFF))
#define CYAN                (RGB888TO565(0x8888FF))
#define PURPLE              (RGB888TO565(0x00AAAA))
#define PINK                (RGB888TO565(0xFFB6C1))

#define MAX_WIDTH				128
#define MAX_HEIGHT				160
#define CHAR_WIDTH				8
#define CHAR_HEIGHT				16

#define CHAR_MAX_X_VERTICAL		16
#define CHAR_MAX_Y_VERTICAL		10

#define CHAR_MAX_X_HORIZONTAL	20
#define CHAR_MAX_Y_HORIZONTAL	8

#define CHAR_MAX_X_ANY	20
#define CHAR_MAX_Y_ANY	10

extern uint8_t tft_orientation;
extern uint8_t tft_width;
extern uint8_t tft_height;
extern uint16_t curr_bg_color;
extern uint16_t curr_text_color;
extern uint16_t curr_text_color_sp;
extern uint8_t char_max_x, char_max_y;
extern uint8_t pointer_to_curr_screen;

extern char text						[2][CHAR_MAX_X_ANY][CHAR_MAX_Y_ANY];
extern uint16_t text_color				[2][CHAR_MAX_X_ANY][CHAR_MAX_Y_ANY];
extern uint16_t bg_color					[2][CHAR_MAX_X_ANY][CHAR_MAX_Y_ANY];

void tft_spi_init(void);
void tft_write_command(uint8_t command);
void tft_write_data(uint8_t data);
void tft_config(void);
void tft_reset(void);

void tft_init(TFT_ORIENTATION orientation, uint16_t bg_color, uint16_t text_color, uint16_t sp_color);
void tft_enable(void);
void tft_disable(void);
void tft_set_bg_color(uint16_t in_bg_color);
void tft_set_text_color(uint16_t in_text_color);
void tft_set_special_color(uint16_t text_color_sp);
void tft_set_pixel_pos(uint8_t x, uint8_t y);
void tft_set_char_pos(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2);
void tft_force_clear(void);
void tft_clear_line(uint8_t line);
void tft_clear(void);
void tft_put_pixel(uint8_t x, uint8_t y, uint16_t color);
void tft_fill_color(uint16_t color);
bool tft_char_is_changed(uint8_t x, uint8_t y);
void tft_prints(uint8_t x, uint8_t y, const char * pstr, ...);
void tft_update(void);
uint8_t tft_get_orientation(void);
void tft_println(const char * pstr, ...);
void tft_stream(const char * pstr, ...);

void tft_put_mega_ass_num(uint8_t x, uint8_t y, uint8_t character, uint16_t color);
void tft_mega_update(void);
void tft_put_logo(uint8_t x, uint8_t y);

#endif		/* __LCD_RED_H */
