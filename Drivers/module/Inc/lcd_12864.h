/*
 * lcd_12864.h
 *
 *  Created on: Dec 6, 2016
 *      Author: jakub
 */

#ifndef NEW_LCD_12864_H_
#define NEW_LCD_12864_H_
// define of the variables

	uint8_t lcd_width;
	uint8_t lcd_height;
    uint8_t char_x;
    uint8_t char_y;
    uint8_t buffer[1024];
    uint8_t contrast;	// storage for contrast value
    uint8_t auto_up;	// variable for automatic uploading after any change.
    uint8_t* font;
    uint8_t magnit;

   //function
void lcd12864_init(SPI_HandleTypeDef *spiHandle);

void locate(uint8_t x, uint8_t y);
/**
 * return a number of the columns dependent on used font.
 */
	uint8_t columns();
/**
 * * return a number of the rows dependent on used font.
 */
	uint8_t rows();
void lcd_reset();
void wr_cmd(const uint8_t cmd);
void wr_dat(const uint8_t dat);
void set_font(uint8_t* f);
void set_contrast(uint8_t o);
void copy_to_lcd(void);
int _putc(uint8_t value);
void pixel(uint8_t x, uint8_t y, uint8_t color);
void line_clear(uint8_t y);
uint8_t width();
uint8_t height();
void char_magnitude(uint8_t input);

     void line(int x0, int y0, int x1, int y1, int color);
 /**
  *  this function would clear the display
  */
     void lcd_clear(void);
/**
     * Draw a character on given position out of the active font to the LCD
     *
     * @param x x-position of char (top left)
     * @param y y-position
     * @param c char to print
     */
     void character(uint8_t x, uint8_t y, char c);
/**
 * This function is for the writing characters to the display, even just one.
 */
// added function from N5110 display library
void lcd_printString(char * str2prt);
/**
 * set the character position taking the letter hight into acount
 */
uint8_t lcd_setCharPos(uint8_t row, uint8_t column);
#endif /* NEW_LCD_12864_H_ */
