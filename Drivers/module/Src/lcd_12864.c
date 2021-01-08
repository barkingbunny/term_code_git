/*
 * lcd_12864.c
 *
 *  Created on: Dec 6, 2016
 *      Author: jakub
 */

// tenhle file bude obsahovat zakladni nastaveni displeje 12864

#include <pinmap.h>
#include <Small_7.h>
#include <Font5x.h>
#include "spi_com.h"
#include "lcd_12864.h"
#include "stdio.h"
#include "string.h"


SPI_HandleTypeDef *_LCD_hspi;

void lcd12864_init(SPI_HandleTypeDef *spiHandle){
	_LCD_hspi=spiHandle;
	char_x = 0;
	lcd_width=128;	// up to 256
	lcd_height=64;
	magnit= 1;
	lcd_reset();
}

// write command to lcd controller
void wr_cmd(const uint8_t cmd){
    LCD_dcClear;
    LCD_csClear;
    SPI_writeByte(_LCD_hspi, cmd);
    LCD_csSet;
}

// write data to lcd controller
void wr_dat(const uint8_t dat){
    LCD_dcSet;
    LCD_csClear;
    SPI_writeByte(_LCD_hspi, dat);
    LCD_csSet;
}

// reset and init the lcd controller
void lcd_reset(){

    LCD_dcClear;			//default state of GPIOs pins.
    LCD_csSet;
    LCD_rstClear;           // display reset
    // wait_us(50);
    HAL_Delay(1);			// 1ms delay - waiting for the PSU is coretc
    LCD_rstSet;             // end reset
    HAL_Delay(5);
    // initialize sequence
    wr_cmd(0xaf);     // display on
    wr_cmd(0x2f);     // power control set
    wr_cmd(0x81);     // set electronic volume mode
    wr_cmd(0x10);     // electronic volume data 00-3f / CONTRAST
    wr_cmd(0x27);     // Voltage Regulator Internal Resister Ratio Set
    wr_cmd(0xa2);     // LCD Bias Set ... 1/9 bias
    wr_cmd(0xc8);     // Common Output Mode Select ... Reverse
    wr_cmd(0xa0);     // ADC Select ... Normal
    wr_cmd(0xa4);     // Display All Points ON/OFF ... normal
    wr_cmd(0xa6);     // Display Normal/Reverse ... normal
    wr_cmd(0xac);     // Static Indicator ... off
    wr_cmd(0x00);     // off
    wr_cmd(0x40);     // Display Start Line Set ... 0
    wr_cmd(0xe0);     // Write Mode Set
    // clear and update LCD
    memset(buffer,0x00,1024);  // clear display buffer
    copy_to_lcd();
    //auto_up = 1;              // switch on auto update
    // dont do this by default. Make the user call
    //claim(stdout);           // redirect printf to lcd
    locate(0,0);
 //   set_font((unsigned char*)Small_7);  // standard font
    set_font((unsigned char*)font5x);  // standard font
}

// update lcd from buffer.
void copy_to_lcd(void){
int i=0;

    // page 0
    wr_cmd(0x00);      // set column low nibble 0
    wr_cmd(0x10);      // set column hi  nibble 0
    wr_cmd(0xB0);      // set page address  0
    LCD_dcSet;
    for(i=0; i<128; i++) {
        wr_dat(buffer[i]);
    }
    // page 1
    wr_cmd(0x00);      // set column low nibble 0
    wr_cmd(0x10);      // set column hi  nibble 0
    wr_cmd(0xB1);      // set page address  1
    LCD_dcSet;
    for(i=128; i<256; i++) {
        wr_dat(buffer[i]);
    }
    // page 2
    wr_cmd(0x00);      // set column low nibble 0
    wr_cmd(0x10);      // set column hi  nibble 0
    wr_cmd(0xB2);      // set page address  2
    LCD_dcSet;
    for(i=256; i<384; i++) {
        wr_dat(buffer[i]);
    }
    // page 3
    wr_cmd(0x00);      // set column low nibble 0
    wr_cmd(0x10);      // set column hi  nibble 0
    wr_cmd(0xB3);      // set page address  3
    LCD_dcSet;
    for(i=384; i<512; i++) {
        wr_dat(buffer[i]);
    }
    if ((lcd_height == 48) || (lcd_height == 64)){
        // page 4
        wr_cmd(0x00);      // set column low nibble 0
        wr_cmd(0x10);      // set column hi  nibble 0
        wr_cmd(0xB4);      // set page address  4
        LCD_dcSet;
        for(i=512; i<640; i++) {
            wr_dat(buffer[i]);
        }
        // page 5
        wr_cmd(0x00);      // set column low nibble 0
        wr_cmd(0x10);      // set column hi  nibble 0
        wr_cmd(0xB5);      // set page address  5
        LCD_dcSet;
        for(i=640; i<768; i++) {
            wr_dat(buffer[i]);
        }
    }
    if (lcd_height == 64){
        // page 6
        wr_cmd(0x00);      // set column low nibble 0
        wr_cmd(0x10);      // set column hi  nibble 0
        wr_cmd(0xB6);      // set page address  6
        LCD_dcSet;
        for(i=768; i<896; i++) {
            wr_dat(buffer[i]);
        }
        // page 7
        wr_cmd(0x00);      // set column low nibble 0
        wr_cmd(0x10);      // set column hi  nibble 0
        wr_cmd(0xB7);      // set page address  7
        LCD_dcClear;
        for(i=896; i<1024; i++) {
            wr_dat(buffer[i]);
        }
    }
}

int _putc(uint8_t value){
    if (value == 0x0d) {    // new line
        char_x = 0;
        char_y = char_y + font[2];
        if (char_y >= height() - font[2]){
            char_y = 0;
        }
        line_clear(char_y);
    } else {
        character(char_x, char_y, value);
        if(auto_up){ copy_to_lcd();}
    }
    return value;
}

void lcd_printString(char* str2prt)
{
    uint8_t c;
    for (c=0; c<strlen(str2prt); c++)
    {
        _putc(str2prt[c]);
    }
    copy_to_lcd();
}

uint8_t lcd_setCharPos(uint8_t row, uint8_t column){

	if (column>(columns()-1) || row>(rows()-1)) return 1;
	column = column*font[1];
	row = row*font[2];
	locate(column,row);
	return 0;
}

void locate(uint8_t x, uint8_t y){
    char_x = x;
    char_y = y;
}

uint8_t columns(){
    return width() / font[1];
}

uint8_t rows(){
    return height() / font[2];
}

void set_font(uint8_t* f){
    font = f;
}

void set_contrast(uint8_t o){
    contrast = o;
    wr_cmd(0x81);      //  set volume
    wr_cmd(o & 0x3F);
}

void lcd_clear(void){
    memset(buffer,0x00,1024);  // clear display buffer
    copy_to_lcd();
}

void char_magnitude(uint8_t input){
	/* this change of the value is formagnitude the characters
	 * magnit = 1 - normal size, magnit = 2 dolble size ...
	 */
	magnit = input;
	if (magnit<1) magnit = 1;  // minimal value of magnit
	if (magnit>6) magnit = 6;	// maximum value of magnit

}

void character(uint8_t x, uint8_t y, char c){
	uint8_t hor,vert,offset,bpl,j,i,b;
	uint8_t magnit_count ;
	unsigned char* zeichen;
	unsigned char z,w;


	if ((c < 31) || (c > 132)){ return;}   // test char range
	// read font parameter from start of array
	offset = font[0];                    // bytes / char
	hor = font[1];                       // get hor size of font
	vert = font[2];                      // get vert size of font
	bpl = font[3];                       // bytes per line
	if ((char_x + (magnit*hor)) > width()){
		char_x = 0;
		char_y = char_y + (magnit*vert);
		if (char_y >= height() - font[2]){
			char_y = 0;
		}
	}
	zeichen = &font[((c -32) * offset) + 4]; // start of char bitmap

	w=font[0]*magnit;				// width of actual char just for FONT5x
	// construct the char into the buffer
	for (i=0; i<(hor*magnit); i++) {   //  horz line
	for (j=0; j<(vert*magnit); j++) {  //  vert line

			z =  zeichen[bpl * (i/magnit) + (((j/magnit) & 0xF8) >> 3)];
			b = 1 << (j/magnit & 0x07);
			magnit_count = 1;
			for(;;){
				if (( z & b ) == 0x00) {
					pixel(x+i,y+j,0);
				} else {
					pixel(x+i,y+j,1);
				} //else
				if(magnit_count>=magnit){
					break;
				}
				j++;
				magnit_count++;
			}

		}

	}
	char_x += w;
	if(char_x >= (width() - hor)) {
		char_x = 0;
		char_y = char_y + vert*magnit;
		if (char_y >= height() - font[2]){
			char_y = 0;
		}
		//zakomentovano - v pripade ze zapisu znak na konec radky, tak se maze dalsi - nechci
	//	line_clear(char_y);
	}
}

void line_clear(uint8_t y){
uint8_t vert,j,i,x;

    // read font parameter from start of array
    vert = font[2];                      // get vert size of font
    x = 0;
    for (j=0; j<vert; j++) {  //  vert line
    	for (i=0; i<lcd_width; i++) {  //  horz line
            pixel(x+i,y+j,0);
        }
    }
}
// set one pixel in buffer
void pixel(uint8_t x, uint8_t y, uint8_t color){
    // first check parameter
    if(x > lcd_width || y > lcd_height || x < 0 || y < 0) return;
        if(color == 0)
            buffer[x + ((y/8) * 128)] &= ~(1 << (y%8));  // erase pixel
        else
            buffer[x + ((y/8) * 128)] |= (1 << (y%8));   // set pixel

}
uint8_t width(){
    return(lcd_width);
}

uint8_t height(){
    return(lcd_height);
}
void line(int x0, int y0, int x1, int y1, int color){
int   dx = 0, dy = 0;
int   dx_sym = 0, dy_sym = 0;
int   dx_x2 = 0, dy_x2 = 0;
int   di = 0;

    dx = x1-x0;
    dy = y1-y0;
    //  if (dx == 0) {        /* vertical line */
    //      if (y1 > y0) vline(x0,y0,y1,color);
    //      else vline(x0,y1,y0,color);
    //      return;
    //  }
    if (dx > 0) {
        dx_sym = 1;
    } else {
        dx_sym = -1;
    }
    //  if (dy == 0) {        /* horizontal line */
    //      if (x1 > x0) hline(x0,x1,y0,color);
    //      else  hline(x1,x0,y0,color);
    //      return;
    //  }
    if (dy > 0) {
        dy_sym = 1;
    } else {
        dy_sym = -1;
    }
    dx = dx_sym*dx;
    dy = dy_sym*dy;
    dx_x2 = dx*2;
    dy_x2 = dy*2;
    if (dx >= dy) {
        di = dy_x2 - dx;
        while (x0 != x1) {
            pixel(x0, y0, color);
            x0 += dx_sym;
            if (di<0) {
                di += dy_x2;
            } else {
                di += dy_x2 - dx_x2;
                y0 += dy_sym;
            }
        }
        pixel(x0, y0, color);
    } else {
        di = dx_x2 - dy;
        while (y0 != y1) {
            pixel(x0, y0, color);
            y0 += dy_sym;
            if (di < 0) {
                di += dx_x2;
            } else {
                di += dx_x2 - dy_x2;
                x0 += dx_sym;
            }
        }
        pixel(x0, y0, color);
    }
    if(auto_up){ copy_to_lcd();}

}



