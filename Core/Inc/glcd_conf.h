/**
 * Private configuration file for the GLCD library.
 * This example is configured for STM32F0, Parallel comunication and including all fonts.
 */

#ifndef __GLCD_CONF_H__
#define __GLCD_CONF_H__

#include "main.h"

// Choose a microcontroller family
//#define STM32F0
//#define STM32F1
#define STM32F4
//#define STM32L0
//#define STM32L1
//#define STM32L4
//#define STM32F3
//#define STM32H7
//#define STM32F7
//#define STM32G0

//need 6 pins for control
// tomado del main.h , creado por el cubeMXIDE...
#define LCD_DI_Port     LCD_DI_GPIO_Port  	//#define LCD_DI_GPIO_Port   GPIOA
#define LCD_DI_Pin      GPIO_PIN_2			//#define LCD_DI_Pin         GPIO_PIN_2
#define LCD_RW_Port     LCD_RW_GPIO_Port	//#define LCD_RW_GPIO_Port   GPIOA
#define LCD_RW_Pin      GPIO_PIN_3			//#define LCD_RW_Pin  		 GPIO_PIN_3
#define LCD_CS1_Port    LCD_CS1_GPIO_Port	//#define LCD_CS1_GPIO_Port  GPIOA
#define LCD_CS1_Pin     GPIO_PIN_4			//#define LCD_CS1_Pin    	 GPIO_PIN_4
#define LCD_CS2_Port    LCD_CS2_GPIO_Port	//#define LCD_CS2_GPIO_Port  GPIOA
//#define LCD_CS2_Pin     GPIO_PIN_5			//#define LCD_CS2_Pin 		 GPIO_PIN_5
#define LCD_E_Port      LCD_E_GPIO_Port		//#define LCD_E_GPIO_Port    GPIOA
//#define LCD_E_Pin       GPIO_PIN_6			//#define LCD_E_Pin          GPIO_PIN_6
#define LCD_RST_Port    LCD_RST_GPIO_Port	//#define LCD_RST_GPIO_Port  GPIOA
//#define LCD_RST_Pin     GPIO_PIN_7			//#define LCD_RST_Pin        GPIO_PIN_7

// Mirror the screen if needed
// #define GLCD_MIRROR_VERT
// #define GLCD_MIRROR_HORIZ

// Set inverse color if needed
// # define GLCD_INVERSE_COLOR

// Include only needed fonts
#define GLCD_INCLUDE_FONT_8x5
//#define GLCD_INCLUDE_FONT_6x8
//#define GLCD_INCLUDE_FONT_7x10
//#define GLCD_INCLUDE_FONT_11x18
//#define GLCD_INCLUDE_FONT_16x26

// The width of the screen can be set using this
// define. The default value is 128.
 #define LCD_WIDTH           128

// If your screen horizontal axis does not start
// in column 0 you can use this define to
// adjust the horizontal offset
// #define GLCD_X_OFFSET

// The height can be changed as well if necessary.
// It can be 32, 64 or 128. The default value is 64.
 #define LCD_HEIGHT          64

#endif /* __GLCD_CONF_H__ */
