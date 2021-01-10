/*
 * peripherals.h
 *  projekt - TERMOSTAT
 *  Created on: Dec 29, 2016
 *      Author: jakub
 */

#ifndef MODULE_PERIPHERALS_H_
#define MODULE_PERIPHERALS_H_

#define BUT_SCAN_PERIOD 500 //ms
//#define BACKLITE_TIMEOUT 30000 //30s
#define BACKLITE_TIMEOUT 60000 //60s
#define SHOW_TIMEOUT 60000 // 60s - time for showing different screen than default.

// PWM nastaveni timeru
#define TIMER2_PWM
//#define TIMER3_PWM  // odkomentovat jen kdyz je nainicializovany Timer 3 v PWM modu

// LCD - definovani parametru displeje / nastaveni displeje
#define LCD_WIDTH 128
#define LCD_HEIGHT 64

typedef enum {	BUT_NONE,
				BUT_1,
				BUT_2,
				BUT_ENC,
				ENCODER
} Buttons;


void togleLED(uint8_t led);
Buttons checkButtons(void);
uint8_t turnOnHeater(uint32_t temperature);
void turnOffHeater(void);
void backlite_init(void);
void backliteOn(void);
void backliteOff(void);
void debug_led_heartbeat(void);


//encoder constant
//#define ENCODER_HALF 128

#endif /* MODULE_PERIPHERALS_H_ */
