
#include "glcd.h"
#include <math.h>
#include <stdlib.h>
#include "delay.h"

//Screen buffer, remember to actualize the screen with glcd_UpdateScreen()
static uint8_t lcdBuffer [LCD_WIDTH][NUM_PAGES];

// Screen object
static glcd_t glcd;



void glcd_init(void){
    glcd_Reset();

    // init display 1 (CS1)
    //glcd_send(DISPLAYOFF, COMMAND_TYPE, DISPLAY1);

    glcd_send(DISPLAYON, COMMAND_TYPE, DISPLAY1);
    glcd_send(DISPLAY_START_LINE, COMMAND_TYPE, DISPLAY1); // start line 0
    glcd_send((SET_PAGE_X), COMMAND_TYPE, DISPLAY1); // set page X = 0
    glcd_send((SET_ADDRESS_Y), COMMAND_TYPE, DISPLAY1); // set address Y = 0

    // init display 2 (CS2)
    //glcd_send(DISPLAYOFF, COMMAND_TYPE, DISPLAY2);

    glcd_send(DISPLAYON, COMMAND_TYPE, DISPLAY2);
    glcd_send(DISPLAY_START_LINE, COMMAND_TYPE, DISPLAY2); // start line 0
    glcd_send((SET_PAGE_X), COMMAND_TYPE, DISPLAY2); // set page X = 0
    glcd_send((SET_ADDRESS_Y), COMMAND_TYPE, DISPLAY2); // set address Y = 0
}


//    Draw one pixel in the screenbuffer
//    X => X Coordinate (uint8_t)
//    Y => Y Coordinate (uint8_t)
//    color => Pixel color-> enum {BLACK , WHITE}
void glcd_DrawPixel(uint8_t x, uint8_t y, LCD_COLOR color) {
	uint8_t page = y/8;
    if(x >= LCD_WIDTH || y >= LCD_HEIGHT) {// Don't write outside the buffer
        return;
    }

    // Draw in the right color    
    if(color == WHITE) {
		lcdBuffer[x][page] |= 1 << (y % 8);
    } else { 
		lcdBuffer[x][page] &= ~(1 << (y % 8));
    }
}


// Fill the whole screen with the given color
// color => Pixel color-> enum {BLACK , WHITE}
void glcd_Fill(LCD_COLOR color) {
    uint32_t i, j;
    uint8_t newColor = (color == BLACK) ? 0x00 : 0xFF;

    for(i = 0; i < NUM_PAGES; i++) {
		for(j = 0; j < LCD_WIDTH; j++){
			lcdBuffer[j][i] = newColor;
		}        
    }
} 

//GLCD busy flag check
//can return: 0b00110000={displayON & NO Initialized), 0b00100000={displayOn & Initialized}
//            0b00010000={displayOff & NO Initialize}, 0b00000000={displayOff & Initialized}
// or =0xFF if busy flag rutine run out the time (TODO)
uint8_t glcd_busyCheck(uint8_t CSsel){

	GPIOB->MODER &= 0xffff0000; //clear data pins
	GPIOB->MODER |= 0x55; 		// PA7-PA4 = Input, PB3-PB0 = outputs
	delay_us(1);
	HAL_GPIO_WritePin(LCD_E_Port, LCD_E_Pin, GPIO_PIN_RESET);//E = 0
	delay_us(1);
	HAL_GPIO_WritePin(LCD_RW_Port, LCD_RW_Pin, GPIO_PIN_SET);//RW = 1
	HAL_GPIO_WritePin(LCD_DI_Port, LCD_DI_Pin, GPIO_PIN_RESET);//DI = 0
	if(CSsel == DISPLAY1){
		HAL_GPIO_WritePin(LCD_CS1_Port, LCD_CS1_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(LCD_CS2_Port, LCD_CS2_Pin, GPIO_PIN_SET);
	}else {
		HAL_GPIO_WritePin(LCD_CS1_Port, LCD_CS1_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(LCD_CS2_Port, LCD_CS2_Pin, GPIO_PIN_RESET);
	}
	delay_us(1);
	uint8_t pb;
	do {
		HAL_GPIO_WritePin(LCD_E_Port, LCD_E_Pin, GPIO_PIN_SET);//E = 1
		delay_us(1);
		pb = (uint8_t)(GPIOB->IDR & 0x000000F0);
		HAL_GPIO_WritePin(LCD_E_Port, LCD_E_Pin, GPIO_PIN_RESET);//E = 0,  flanco de bajada actualiza el latch del GLCD
		delay_us(1);
	} while(!(pb && 0x80));//0x80 = 0b10000000 -> bit 7 = busy flag

	// set Both CS to 1
	HAL_GPIO_WritePin(LCD_CS1_Port, LCD_CS1_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(LCD_CS2_Port, LCD_CS2_Pin, GPIO_PIN_SET);

	HAL_GPIO_WritePin(LCD_DI_Port, LCD_DI_Pin, GPIO_PIN_SET);//DI = 1

	GPIOB->MODER &= 0xffff0000; //clear data pins
	GPIOB->MODER |= 0x5555; 	// PA7-PB0 = outputs
	return pb;
}


// write data to GLCD using parallel port
// data => Data/command to write to GLCD (uint8_t)
// cmd_data_sel => Pin Data/Instruction to GLCD (uint8_t), #defines => {COMMAND_TYPE = 1, DATA_TYPE = 0}
// CSsel => Pines CS1, CS2 to GLCD (uint8_t), select wich display to write, #defines => {DISPLAY1 = 1, DISPLAY2 = 2}
void glcd_send(uint8_t data, uint8_t cmd_data_sel, uint8_t CSsel){
	// check busy flag
	//uint8_t status_glcd = glcd_busyCheck(CSsel);
	HAL_GPIO_WritePin(LCD_E_Port, LCD_E_Pin, GPIO_PIN_RESET);// E = 0
	delay_us(1);
	//chip select 1 or 2
	if(CSsel == DISPLAY1){
		HAL_GPIO_WritePin(LCD_CS1_Port, LCD_CS1_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(LCD_CS2_Port, LCD_CS2_Pin, GPIO_PIN_SET);
	} else {
		HAL_GPIO_WritePin(LCD_CS1_Port, LCD_CS1_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(LCD_CS2_Port, LCD_CS2_Pin, GPIO_PIN_RESET);
	}
	HAL_GPIO_WritePin(LCD_RW_Port, LCD_RW_Pin, GPIO_PIN_RESET);// RW = 0
	//Command or data pin
	if(cmd_data_sel == COMMAND_TYPE){
		HAL_GPIO_WritePin(LCD_DI_Port, LCD_DI_Pin, GPIO_PIN_RESET);//COMMAND
	} else {
		HAL_GPIO_WritePin(LCD_DI_Port, LCD_DI_Pin, GPIO_PIN_SET);//DATA
	}
	delay_us(1);
	delay_us(1);
	HAL_GPIO_WritePin(LCD_E_Port, LCD_E_Pin, GPIO_PIN_SET);// E = 1
	delay_us(1);
	
	//write lcd
	GPIOB->ODR &= 0xffffff00; //clear data pins
	GPIOB->ODR |= data;       //write data to data pins
	
	//pulse E = Enable pin
	delay_us(1);
	HAL_GPIO_WritePin(LCD_E_Port, LCD_E_Pin, GPIO_PIN_RESET);// E = 0 , data latch on E falling
	delay_us(1);

	// DESELECT CHIPS
	HAL_GPIO_WritePin(LCD_CS1_Port, LCD_CS1_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(LCD_CS2_Port, LCD_CS2_Pin, GPIO_PIN_SET);
	delay_us(1);
	HAL_GPIO_WritePin(LCD_RW_Port, LCD_RW_Pin, GPIO_PIN_SET);// RW = 1
	delay_us(1);

}


// Write the screenbuffer with changes to the screen/GLCD
void glcd_UpdateScreen(void) {
	uint8_t counter, i;
    for(i = 0; i < NUM_PAGES; i++) { // pages, y
    	glcd_send((SET_PAGE_X+i), COMMAND_TYPE, DISPLAY2);//set X address, page
    	glcd_send(SET_ADDRESS_Y, COMMAND_TYPE, DISPLAY2);//Y address = 0
		for(counter = 0; counter<64 ; counter++){
			glcd_send(lcdBuffer[counter][i], DATA_TYPE, DISPLAY2);//Y addres increment automatically after every write
		}
    }
    for(i = 0; i < NUM_PAGES; i++) { // pages, y
		glcd_send((SET_PAGE_X+i), COMMAND_TYPE, DISPLAY1);
		glcd_send(SET_ADDRESS_Y, COMMAND_TYPE, DISPLAY1);
		for(counter = 64; counter<128 ; counter++){
			glcd_send(lcdBuffer[counter][i], DATA_TYPE, DISPLAY1);
		}
	}
}


//Function to reset the display pulsing RST pin
void glcd_Reset(void) {
	HAL_GPIO_WritePin(LCD_RST_Port, LCD_RST_Pin, GPIO_PIN_RESET);
	delay_us(10);
	HAL_GPIO_WritePin(LCD_RST_Port, LCD_RST_Pin, GPIO_PIN_SET);
	HAL_Delay(100);
}


// Draw 1 char to the screen buffer
// ch       => Character to draw (char)
// Font     => Font to use (FontDef)
// color    => Pixel color -> enum {BLACK , WHITE}
char glcd_WriteChar(char ch, FontDef Font, LCD_COLOR color) {
    uint8_t i, b, page;
    uint16_t index;

    // Check if character is valid
    if (ch < 32 || ch > 126)
        return 0;
    
    // Check remaining space on current line
    if (LCD_WIDTH < (glcd.CurrentX + Font.FontWidth) ||
        LCD_HEIGHT < (glcd.CurrentY + Font.FontHeight))
    {
        // Not enough space on current line
        return 0;
    }

    page = glcd.CurrentY / 8;
    // Use the font to write
    for(i = 0; i < Font.FontWidth; i++) {//Font.FontHeight = 8, Font.FontWidth = 5
    	index = ((ch - 32) * 5) + i;
        b = fontArray[index];
        lcdBuffer[glcd.CurrentX+i][page]= b;
    }
    
  /*
    // The current space is now taken
    glcd.CurrentX += Font.FontWidth+1 ;
    if(glcd.CurrentX > (LCD_WIDTH - Font.FontWidth)){
    	glcd.CurrentX = 0;
    	glcd.CurrentY += Font.FontHeight;
    }
 */
    // The current space is now taken
     glcd.CurrentX += Font.FontWidth+1;

    // Return written char for validation
    return ch;
}



// Draw 1 char to the screen buffer
// ch       => Character to draw (char)
// Font     => Font to use (FontDef)
// color    => Pixel color -> enum {BLACK , WHITE}
char glcd_WriteChar2(char ch, FontDef Font, LCD_COLOR color) {
    uint32_t i, j, page;
    uint32_t b, index;

    // Check if character is valid
    if (ch < 32 || ch > 126)
        return 0;

    // Check remaining space on current line
    if (LCD_WIDTH < (glcd.CurrentX + Font.FontWidth) ||
        LCD_HEIGHT < (glcd.CurrentY + Font.FontHeight))
    {
        // Not enough space on current line
        return 0;
    }

    // Use the font to write
        for(i = 0; i < Font.FontWidth; i++) {
        	index = ((ch - 32) * Font.FontWidth) + i;
        	b = (uint16_t)fontArray[index];
             //b = Font.data[(ch - 32) * Font.FontWidth + i];
            for(j = 0; j < Font.FontHeight; j++) {
            	uint32_t T1 =  (b >> j);
            	uint32_t T2 = T1 & 0x01;
                if(T2)  {
                    //glcd_DrawPixel(glcd.CurrentY + i, (glcd.CurrentX + j), (LCD_COLOR) color);//para dibujar Vertical
                    glcd_DrawPixel(glcd.CurrentX + i, (glcd.CurrentY + j), (LCD_COLOR) color);
                } else {
                	//glcd_DrawPixel(glcd.CurrentY + i, (glcd.CurrentX + j), (LCD_COLOR)!color);//para dibujar Vertical
                	glcd_DrawPixel(glcd.CurrentX + i, (glcd.CurrentY + j), (LCD_COLOR)!color);
                }
            }
        }

    // The current space is now taken
      glcd.CurrentX += Font.FontWidth+1;
    //glcd.CurrentX += Font.FontHeight; //para dibujar Vertical

    // Return written char for validation
    return ch;
}



// Write full string to screenbuffer
// str   => pointer to the string (char*)
// Font  => Font definition (FontDef) from glcd_fonts.h
// color => Pixel color -> enum {BLACK , WHITE}
// remember to actualize the screen with glcd_UpdateScreen()
char glcd_WriteString(char* str, FontDef Font, LCD_COLOR color) {
    // Write until null-byte
    while (*str) {
        if (glcd_WriteChar(*str, Font, color) != *str) {
            // Char could not be written
            return *str;
        }
        // Next char
        str++;
    }
    
    // Everything ok
    return *str;
}


// Position the cursor on the lcdBuffer 
void glcd_SetCursor(uint8_t x, uint8_t y) {
    glcd.CurrentX = x;
    glcd.CurrentY = y;
}



// Draw line by Bresenhem's algorithm
// color => Pixel color -> enum {BLACK , WHITE}
// remember to actualize the screen with glcd_UpdateScreen()
void glcd_Line(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, LCD_COLOR color) {
  int32_t deltaX = abs(x2 - x1);
  int32_t deltaY = abs(y2 - y1);
  int32_t signX = ((x1 < x2) ? 1 : -1);
  int32_t signY = ((y1 < y2) ? 1 : -1);
  int32_t error = deltaX - deltaY;
  int32_t error2;
    
  glcd_DrawPixel(x2, y2, color);
    while((x1 != x2) || (y1 != y2))
    {
		glcd_DrawPixel(x1, y1, color);
		error2 = error * 2;
		if(error2 > -deltaY)
		{
		  error -= deltaY;
		  x1 += signX;
		}
		else
		{
			/*nothing to do*/
		}

		if(error2 < deltaX)
		{
		  error += deltaX;
		  y1 += signY;
		}
		else
		{
			/*nothing to do*/
		}
   }
  return;
}


//Draw polyline
// color => Pixel color -> enum {BLACK , WHITE}
// remember to actualize the screen with glcd_UpdateScreen()
void glcd_Polyline(const glcd_VERTEX *par_vertex, uint16_t par_size, LCD_COLOR color) {
  uint16_t i;
  if(par_vertex != 0){
    for(i = 1; i < par_size; i++){
      glcd_Line(par_vertex[i - 1].x, par_vertex[i - 1].y, par_vertex[i].x, par_vertex[i].y, color);
    }
  }
  else
  {
    /*nothing to do*/
  }
  return;
}


/*Convert Degrees to Radians*/
static float glcd_DegToRad(float par_deg) {
    return par_deg * 3.14 / 180.0;
}


/*Normalize degree to [0;360]*/
static uint16_t glcd_NormalizeTo0_360(uint16_t par_deg) {
  uint16_t loc_angle;
  if(par_deg <= 360)
  {
    loc_angle = par_deg;
  }
  else
  {
    loc_angle = par_deg % 360;
    loc_angle = ((par_deg != 0)?par_deg:360);
  }
  return loc_angle;
}


/*DrawArc. Draw angle is beginning from 4 quart of trigonometric circle (3pi/2)
 * start_angle in degree
 * sweep in degree
 * color => Pixel color -> enum {BLACK , WHITE}
 * remember to actualize the screen with glcd_UpdateScreen()
 */
void glcd_DrawArc(uint8_t x, uint8_t y, uint8_t radius, uint16_t start_angle, uint16_t sweep, LCD_COLOR color) {
    static const uint8_t CIRCLE_APPROXIMATION_SEGMENTS = 36;
    float approx_degree;
    uint32_t approx_segments;
    uint8_t xp1,xp2;
    uint8_t yp1,yp2;
    uint32_t count = 0;
    uint32_t loc_sweep = 0;
    float rad;
    
    loc_sweep = glcd_NormalizeTo0_360(sweep);
    
    count = (glcd_NormalizeTo0_360(start_angle) * CIRCLE_APPROXIMATION_SEGMENTS) / 360;
    approx_segments = (loc_sweep * CIRCLE_APPROXIMATION_SEGMENTS) / 360;
    approx_degree = loc_sweep / (float)approx_segments;
    while(count < approx_segments)
    {
        rad = glcd_DegToRad(count*approx_degree);
        xp1 = x + (int8_t)(sin(rad)*radius);
        yp1 = y + (int8_t)(cos(rad)*radius);    
        count++;
        if(count != approx_segments)
        {
            rad = glcd_DegToRad(count*approx_degree);
        }
        else
        {            
            rad = glcd_DegToRad(loc_sweep);
        }
        xp2 = x + (int8_t)(sin(rad)*radius);
        yp2 = y + (int8_t)(cos(rad)*radius);    
        glcd_Line(xp1,yp1,xp2,yp2,color);
    }
    
    return;
}



/*
 * Draw arc with radius line
 * Angle is beginning from 4 quart of trigonometric circle (3pi/2)
 * start_angle: start angle in degree
 * sweep: finish angle in degree
 * color => Pixel color -> enum {BLACK , WHITE}
 * remember to actualize the screen with glcd_UpdateScreen() * 
 */
void glcd_DrawArcWithRadiusLine(uint8_t x, uint8_t y, uint8_t radius, uint16_t start_angle, uint16_t sweep, LCD_COLOR color) {
    static const uint8_t CIRCLE_APPROXIMATION_SEGMENTS = 36;
    float approx_degree;
    uint32_t approx_segments;
    uint8_t xp1 = 0;
    uint8_t xp2 = 0;
    uint8_t yp1 = 0;
    uint8_t yp2 = 0;
    uint32_t count = 0;
    uint32_t loc_sweep = 0;
    float rad;
    
    loc_sweep = glcd_NormalizeTo0_360(sweep);
    
    count = (glcd_NormalizeTo0_360(start_angle) * CIRCLE_APPROXIMATION_SEGMENTS) / 360;
    approx_segments = (loc_sweep * CIRCLE_APPROXIMATION_SEGMENTS) / 360;
    approx_degree = loc_sweep / (float)approx_segments;

    rad = glcd_DegToRad(count*approx_degree);
    uint8_t first_point_x = x + (int8_t)(sin(rad)*radius);
    uint8_t first_point_y = y + (int8_t)(cos(rad)*radius);   
    while (count < approx_segments) {
        rad = glcd_DegToRad(count*approx_degree);
        xp1 = x + (int8_t)(sin(rad)*radius);
        yp1 = y + (int8_t)(cos(rad)*radius);    
        count++;
        if (count != approx_segments) {
            rad = glcd_DegToRad(count*approx_degree);
        }
        else {            
            rad = glcd_DegToRad(loc_sweep);
        }
        xp2 = x + (int8_t)(sin(rad)*radius);
        yp2 = y + (int8_t)(cos(rad)*radius);    
        glcd_Line(xp1,yp1,xp2,yp2,color);
    }
    
    // Radius line
    glcd_Line(x,y,first_point_x,first_point_y,color);
    glcd_Line(x,y,xp2,yp2,color);
    return;
}



//Draw circle by Bresenhem's algorithm
// color => Pixel color -> enum {BLACK , WHITE}
// remember to actualize the screen with glcd_UpdateScreen()
void glcd_DrawCircle(uint8_t par_x,uint8_t par_y,uint8_t par_r, LCD_COLOR par_color) {
  int32_t x = -par_r;
  int32_t y = 0;
  int32_t err = 2 - 2 * par_r;
  int32_t e2;

  if (par_x >= LCD_WIDTH || par_y >= LCD_HEIGHT) {
    return;
  }

    do {
      glcd_DrawPixel(par_x - x, par_y + y, par_color);
      glcd_DrawPixel(par_x + x, par_y + y, par_color);
      glcd_DrawPixel(par_x + x, par_y - y, par_color);
      glcd_DrawPixel(par_x - x, par_y - y, par_color);
        e2 = err;
        if (e2 <= y) {
            y++;
            err = err + (y * 2 + 1);
            if(-x == y && e2 <= x) {
              e2 = 0;
            }
            else
            {
              /*nothing to do*/
            }
        }
        else
        {
          /*nothing to do*/
        }
        if(e2 > x) {
          x++;
          err = err + (x * 2 + 1);
        }
        else
        {
          /*nothing to do*/
        }
    } while(x <= 0);

    return;
}



// Draw filled circle. Pixel positions calculated using Bresenham's algorithm
// color => Pixel color -> enum {BLACK , WHITE}
// remember to actualize the screen with glcd_UpdateScreen()
void glcd_FillCircle(uint8_t par_x,uint8_t par_y,uint8_t par_r,LCD_COLOR par_color) {
    int32_t x = -par_r;
    int32_t y = 0;
    int32_t err = 2 - 2 * par_r;
    int32_t e2;

    if (par_x >= LCD_WIDTH || par_y >= LCD_HEIGHT) {
        return;
    }

    do {
        for (uint8_t _y = (par_y + y); _y >= (par_y - y); _y--)
        {
            for (uint8_t _x = (par_x - x); _x >= (par_x + x); _x--)
            {
                glcd_DrawPixel(_x, _y, par_color);
            }
        }

        e2 = err;
        if (e2 <= y) {
            y++;
            err = err + (y * 2 + 1);
            if (-x == y && e2 <= x) {
                e2 = 0;
            }
            else {
                /*nothing to do*/
            }
        }
        else
        {
            /*nothing to do*/
        }

        if(e2 > x) {
            x++;
            err = err + (x * 2 + 1);
        }
        else {
            /*nothing to do*/
        }
    } while(x <= 0);

    return;
}


// Draw rectangle
// color => Pixel color -> enum {BLACK , WHITE}
// remember to actualize the screen with glcd_UpdateScreen()
void glcd_DrawRectangle(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, LCD_COLOR color) {
    glcd_Line(x1,y1,x2,y1,color);
    glcd_Line(x2,y1,x2,y2,color);
    glcd_Line(x2,y2,x1,y2,color);
    glcd_Line(x1,y2,x1,y1,color);

    return;
}

// Draw filled rectangle
// color => Pixel color -> enum {BLACK , WHITE}
// remember to actualize the screen with glcd_UpdateScreen()
void glcd_FillRectangle(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, LCD_COLOR color) {
    for (uint8_t y = y2; y >= y1; y--) {
        for (uint8_t x = x2; x >= x1; x--) {
            glcd_DrawPixel(x, y, color);
        }
    }
    return;
}



//Draw bitmap - ported from the ADAFruit GFX library
// color => Pixel color -> enum {BLACK , WHITE}
// remember to actualize the screen with glcd_UpdateScreen()
void glcd_DrawBitmap(uint8_t x, uint8_t y, const unsigned char* bitmap, uint8_t w, uint8_t h, LCD_COLOR color)
{
    int16_t byteWidth = (w + 7) / 8; // Bitmap scanline pad = whole byte
    uint8_t byte = 0;

    if (x >= LCD_WIDTH || y >= LCD_HEIGHT) {
        return;
    }

    for (uint8_t j = 0; j < h; j++, y++) {
        for (uint8_t i = 0; i < w; i++) {
            if (i & 7)
                byte <<= 1;
            else
                byte = (*(const unsigned char *)(&bitmap[j * byteWidth + i / 8]));
            if (byte & 0x80)
                glcd_DrawPixel(x + i, y, color);
        }
    }
    return;
}



//DRAW A HORIZONTAL LINE
// color => Pixel color -> enum {BLACK , WHITE}
// remember to actualize the screen with glcd_UpdateScreen()
void glcd_DrawHorzLine(uint8_t x1, uint8_t x2, uint8_t y, LCD_COLOR color){
	uint8_t lenght = abs(x2 - x1);
	if(0 == lenght){
		return;
	}
	uint8_t sign = ((x2 - x1) >= 0)? 1: -1;
	glcd_DrawPixel(x1, y, color);
	while(x1 != x2){//TODO CHECK IF THIS COMPARATION WORKS
		x1 += sign ;
		glcd_DrawPixel(x1, y, color);
	}
}

//Draw a line spectrum from bottom GLCD to top
// color => Pixel color -> enum {BLACK , WHITE}
// remember to actualize the screen with glcd_UpdateScreen()
void glcd_showSpectrum(double *data, uint8_t num_samples){
	uint8_t height;
	glcd_Fill(BLACK);
	for (uint8_t i = 0; i < num_samples; i++){
		height = (uint8_t)data[i];
		glcd_Line(i, 63, i,(64 - height), WHITE);
	}
	glcd_UpdateScreen();
}


