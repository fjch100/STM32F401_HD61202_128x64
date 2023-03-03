/**
 * This Library was originally written by Olivier Van den Eede (4ilo) in 2016.
 * Some refactoring was done and SPI support was added by Aleksander Alekseev (afiskon) in 2018.
 *
 * mofified by Felix Centeno, fjch100@gmail.com
 */

/**
 * This  LCD uses parallel communication
 *
 * Library features functions for drawing lines, rectangles and circles.
 *
 * It also allows you to draw texts and characters using appropriate functions provided in library.
 *
 * Default pinout
 *
LCD         |STM32F4xx    |DESCRIPTION

VCC         |3.3V         |
GND         |GND          |
 */


#ifndef __GLCD_H__
#define __GLCD_H__

#include <stddef.h>
#include <_ansi.h>

_BEGIN_STD_C

#include "glcd_conf.h"

#if defined(STM32F0)
#include "stm32f0xx_hal.h"
#elif defined(STM32F1)
#include "stm32f1xx_hal.h"
#elif defined(STM32F4)
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_gpio.h"
#elif defined(STM32L0)
#include "stm32l0xx_hal.h"
#elif defined(STM32L1)
#include "stm32l1xx_hal.h"
#elif defined(STM32L4)
#include "stm32l4xx_hal.h"
#elif defined(STM32F3)
#include "stm32f3xx_hal.h"
#elif defined(STM32H7)
#include "stm32h7xx_hal.h"
#elif defined(STM32F7)
#include "stm32f7xx_hal.h"
#elif defined(STM32G0)
#include "stm32g0xx_hal.h"
#elif defined(STM32G4)
#include "stm32g4xx_hal.h"
#else
#error "GLCD library was tested only on STM32F0, STM32F1, STM32F3, STM32F4, STM32F7, STM32L0, STM32L1, STM32L4, STM32H7, STM32G0, STM32G4 MCU families. Please modify glcd.h if you know what you are doing. Also please send a pull request if it turns out the library works on other MCU's as well!"
#endif

#ifdef GLCD_X_OFFSET
#define GLCD_X_OFFSET_LOWER (GLCD_X_OFFSET & 0x0F)
#define GLCD_X_OFFSET_UPPER ((GLCD_X_OFFSET >> 4) & 0x07)
#else
#define GLCD_X_OFFSET_LOWER 0
#define GLCD_X_OFFSET_UPPER 0
#endif

#include "glcd_fonts.h"

#define DISPLAY1        1
#define DISPLAY2        2

#define NUM_PAGES   LCD_HEIGHT/8    //8 paginas

#define COMMAND_TYPE     0x00
#define DATA_TYPE		 0x01
#define DISPLAYON   0x3F
#define DISPLAYOFF  0x3E
#define DISPLAY_START_LINE  0xC0
#define SET_PAGE_X          0xB8
#define SET_ADDRESS_Y       0x40
#define READ_STATUS         0x00

#ifndef LCD_CS1_Port
#define LCD_CS1_Port         GPIOA
#endif
#ifndef LCD_CS1_Pin
#define LCD_CS1_Pin          GPIO_PIN_3
#endif

#ifndef LCD_CS2_Port
#define LCD_CS2_Port         GPIOA
#endif
#ifndef LCD_CS2_Pin
#define LCD_CS2_Pin          GPIO_PIN_3
#endif

#ifndef LCD_DI_Port
#define LCD_DI_Port         GPIOA
#endif
#ifndef LCD_DI_Pin
#define LCD_DI_Pin          GPIO_PIN_1
#endif

#ifndef LCD_RST_Port
#define LCD_RST_Port      GPIOA
#endif
#ifndef LCD_RST_Pin
#define LCD_RST_Pin       GPIO_PIN_2
#endif

#ifndef LCD_E_Port
#define LCD_E_Port      GPIOA
#endif
#ifndef LCD_E_Pin
#define LCD_E_Pin       GPIO_PIN_2
#endif

#ifndef LCD_RW_Port
#define LCD_RST_Port      GPIOA
#endif
#ifndef LCD_RW_Pin
#define LCD_RW_Pin       GPIO_PIN_2
#endif

// GLCD height in pixels
#ifndef LCD_HEIGHT
#define LCD_HEIGHT          64
#endif

// GLCD width in pixels
#ifndef LCD_WIDTH
#define LCD_WIDTH           128
#endif

// Enumeration for screen colors
typedef enum {
    BLACK = 0x00, // Black color, no pixel
    WHITE = 0x01  // Pixel is set. Color depends on glcd
} LCD_COLOR;


typedef enum {
    LCD_OK = 0x00,
    LCD_ERR = 0x01  // Generic error.
} LCD_Error_t;


// Struct to store transformations
typedef struct {
    uint16_t CurrentX;
    uint16_t CurrentY;
    uint8_t Initialized;
    uint8_t DisplayOn;
} glcd_t;

typedef struct {
    uint8_t x;
    uint8_t y;
} glcd_VERTEX;

// Procedure definitions
uint8_t busyCheck(uint8_t CSsel);
void glcd_init(void);
void glcd_Fill(LCD_COLOR color);
void glcd_UpdateScreen(void);
void glcd_DrawPixel(uint8_t x, uint8_t y, LCD_COLOR color);
char glcd_WriteChar(char ch, FontDef Font, LCD_COLOR color);
char glcd_WriteString(char* str, FontDef Font, LCD_COLOR color);
void glcd_SetCursor(uint8_t x, uint8_t y);
void glcd_Line(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, LCD_COLOR color);
void glcd_DrawArc(uint8_t x, uint8_t y, uint8_t radius, uint16_t start_angle, uint16_t sweep, LCD_COLOR color);
void glcd_DrawArcWithRadiusLine(uint8_t x, uint8_t y, uint8_t radius, uint16_t start_angle, uint16_t sweep, LCD_COLOR color);
void glcd_DrawCircle(uint8_t par_x, uint8_t par_y, uint8_t par_r, LCD_COLOR color);
void glcd_FillCircle(uint8_t par_x,uint8_t par_y,uint8_t par_r,LCD_COLOR par_color);
void glcd_Polyline(const glcd_VERTEX *par_vertex, uint16_t par_size, LCD_COLOR color);
void glcd_DrawRectangle(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, LCD_COLOR color);
void glcd_FillRectangle(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, LCD_COLOR color);
void glcd_DrawBitmap(uint8_t x, uint8_t y, const unsigned char* bitmap, uint8_t w, uint8_t h, LCD_COLOR color);
void glcd_DrawHorzLine(uint8_t x1, uint8_t x2, uint8_t y, uint8_t color);
void glcd_showSpectrum(double *data, uint8_t num_samples);
char glcd_WriteChar2(char ch, FontDef Font, LCD_COLOR color);


// Low-level procedures
void glcd_Reset(void);
LCD_Error_t glcd_FillBuffer(uint8_t* buf, uint32_t len);
void glcd_send(uint8_t data, uint8_t cmd_data_sel, uint8_t CSsel);


//FONT 8x5
static const uint8_t fontArray [] = {
0x00, 0x00, 0x00, 0x00, 0x00,// (space)
0x00, 0x00, 0x5F, 0x00, 0x00,// !
0x00, 0x07, 0x00, 0x07, 0x00,// "
0x14, 0x7F, 0x14, 0x7F, 0x14,// #
0x24, 0x2A, 0x7F, 0x2A, 0x12,// $
0x23, 0x13, 0x08, 0x64, 0x62,// %
0x36, 0x49, 0x55, 0x22, 0x50,// &
0x00, 0x05, 0x03, 0x00, 0x00,// '
0x00, 0x1C, 0x22, 0x41, 0x00,// (
0x00, 0x41, 0x22, 0x1C, 0x00,// )
0x08, 0x2A, 0x1C, 0x2A, 0x08,// *
0x08, 0x08, 0x3E, 0x08, 0x08,// +
0x00, 0x50, 0x30, 0x00, 0x00,// ,
0x08, 0x08, 0x08, 0x08, 0x08,// -
0x00, 0x30, 0x30, 0x00, 0x00,// .
0x20, 0x10, 0x08, 0x04, 0x02,// /
0x3E, 0x51, 0x49, 0x45, 0x3E,// 0
0x00, 0x42, 0x7F, 0x40, 0x00,// 1
0x42, 0x61, 0x51, 0x49, 0x46,// 2
0x21, 0x41, 0x45, 0x4B, 0x31,// 3
0x18, 0x14, 0x12, 0x7F, 0x10,// 4
0x27, 0x45, 0x45, 0x45, 0x39,// 5
0x3C, 0x4A, 0x49, 0x49, 0x30,// 6
0x01, 0x71, 0x09, 0x05, 0x00,// 7
0x36, 0x49, 0x49, 0x49, 0x36,// 8
0x06, 0x49, 0x49, 0x29, 0x1E,// 9
0x00, 0x36, 0x36, 0x00, 0x00,// :
0x00, 0x56, 0x36, 0x00, 0x00,// ;
0x00, 0x08, 0x14, 0x22, 0x41,// <
0x14, 0x14, 0x14, 0x14, 0x14,// =
0x41, 0x22, 0x14, 0x08, 0x00,// >
0x02, 0x01, 0x51, 0x09, 0x06,// ?
0x32, 0x49, 0x79, 0x41, 0x3E,// @
0x7E, 0x11, 0x11, 0x11, 0x7E,// A
0x7F, 0x49, 0x49, 0x49, 0x36,// B
0x3E, 0x41, 0x41, 0x41, 0x22,// C
0x7F, 0x41, 0x41, 0x22, 0x1C,// D
0x7F, 0x49, 0x49, 0x49, 0x41,// E
0x7F, 0x09, 0x09, 0x01, 0x01,// F
0x3E, 0x41, 0x41, 0x51, 0x32,// G
0x7F, 0x08, 0x08, 0x08, 0x7F,// H
0x00, 0x41, 0x7F, 0x41, 0x00,// I
0x20, 0x40, 0x41, 0x3F, 0x01,// J
0x7F, 0x08, 0x14, 0x22, 0x41,// K
0x7F, 0x40, 0x40, 0x40, 0x40,// L
0x7F, 0x02, 0x04, 0x02, 0x7F,// M
0x7F, 0x04, 0x08, 0x10, 0x7F,// N
0x3E, 0x41, 0x41, 0x41, 0x3E,// O
0x7F, 0x09, 0x09, 0x09, 0x06,// P
0x3E, 0x41, 0x51, 0x21, 0x5E,// Q
0x7F, 0x09, 0x19, 0x29, 0x46,// R
0x46, 0x49, 0x49, 0x49, 0x31,// S
0x01, 0x01, 0x7F, 0x01, 0x01,// T
0x3F, 0x40, 0x40, 0x40, 0x3F,// U
0x1F, 0x20, 0x40, 0x20, 0x1F,// V
0x7F, 0x20, 0x18, 0x20, 0x7F,// W
0x63, 0x14, 0x08, 0x14, 0x63,// X
0x03, 0x04, 0x78, 0x04, 0x03,// Y
0x61, 0x51, 0x49, 0x45, 0x43,// Z
0x00, 0x00, 0x7F, 0x41, 0x41,// [
0x02, 0x04, 0x08, 0x10, 0x20,// "\"
0x41, 0x41, 0x7F, 0x00, 0x00,// ]
0x04, 0x02, 0x01, 0x02, 0x04,// ^
0x40, 0x40, 0x40, 0x40, 0x40,// _
0x00, 0x01, 0x02, 0x04, 0x00,// `
0x20, 0x54, 0x54, 0x54, 0x78,// a
0x7F, 0x48, 0x44, 0x44, 0x38,// b
0x38, 0x44, 0x44, 0x44, 0x20,// c
0x38, 0x44, 0x44, 0x48, 0x7F,// d
0x38, 0x54, 0x54, 0x54, 0x18,// e
0x08, 0x7E, 0x09, 0x01, 0x02,// f
0x08, 0x14, 0x54, 0x54, 0x3C,// g
0x7F, 0x08, 0x04, 0x04, 0x78,// h
0x00, 0x44, 0x7D, 0x40, 0x00,// i
0x20, 0x40, 0x44, 0x3D, 0x00,// j
0x00, 0x7F, 0x10, 0x28, 0x44,// k
0x00, 0x41, 0x7F, 0x40, 0x00,// l
0x7C, 0x04, 0x18, 0x04, 0x78,// m
0x7C, 0x08, 0x04, 0x04, 0x78,// n
0x38, 0x44, 0x44, 0x44, 0x38,// o
0x7C, 0x14, 0x14, 0x14, 0x08,// p
0x08, 0x14, 0x14, 0x18, 0x7C,// q
0x7C, 0x08, 0x04, 0x04, 0x08,// r
0x48, 0x54, 0x54, 0x54, 0x20,// s
0x04, 0x3F, 0x44, 0x40, 0x20,// t
0x3C, 0x40, 0x40, 0x20, 0x7C,// u
0x1C, 0x20, 0x40, 0x20, 0x1C,// v
0x3C, 0x40, 0x30, 0x40, 0x3C,// w
0x44, 0x28, 0x10, 0x28, 0x44,// x
0x0C, 0x50, 0x50, 0x50, 0x3C,// y
0x44, 0x64, 0x54, 0x4C, 0x44,// z
0x00, 0x08, 0x36, 0x41, 0x00,// {
0x00, 0x00, 0x7F, 0x00, 0x00,// |
0x00, 0x41, 0x36, 0x08, 0x00,// }
};

_END_STD_C

#endif // __GLCD_H__
