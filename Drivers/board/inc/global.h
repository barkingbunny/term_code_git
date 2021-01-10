/*
 * global.h
  projekt - TERMOSTAT
 *  Created on: Dec 29, 2016
 *      Author: jakub
 */

#ifndef MODULE_GLOBAL_H_
#define MODULE_GLOBAL_H_

#include "stm32l0xx_hal.h"

#define DEBUG_TERMOSTAT 		// debug of the code is turned OFF
#define SW_VERSION 263 //verze softwaru
/**
 *
 *

   text	   data	    bss	    dec	    hex	filename

  33880	   1072	   9792	  44744	   aec8	termostat_git.elf
  33904	   1072	  14592	  49568	   c1a0	termostat_git.elf   // logovani size  - RAM pamet -  0x1ff
  33904	   1072	  17152	  52128	   cba0	termostat_git.elf   // logovani size - RAM pamet - 0x2ff

 */

#define BUT_DELAY 1		// in milisecond - I want to read it quckly
#define MAIN_LOOP 25		// in milisecond
#define MEASURE_PERIODE 5000 // every 5 secondn
#define LED_PERIODE 500 //
#define TIME_PERIODE 400 // ms definition of periode for checking time change (RTC change )
#define HEATING_PERIODE 1000 // every 5 minute check for change - turn on / off heater
#define LOG_PERIODE 600 // in seconds - every 10 minute LOG

#ifdef DEBUG_TERMOSTAT
	#define LOG_PERIODE 40
#endif

#define HEATING_INSTANT 900 // in seconds for 15 minutes is turned on the instant heating

#define HEATING_HYSTERESIS 50 // hysteresis is 0.5 deg C ( X/50)

#define TEMPERATURE_MAX 3000 // 30.00C maximum temperature, when this limit is reached, the radiator will stop.
#define TEMPERATURE_MIN -1000 //-10.00C maximum temperature, when this limit is reached, the radiator will stop.

// INTITIAL STATE values for PWM
#define LCD_BACKLITE_DUTY 50

typedef enum {FALSE = 0u, TRUE = 1u} Bool;
// the priority is selected by place in the list.
typedef enum {	MEASURING,
				HEATING,
				TEST,
				MENUn,
				TE_ERROR,
				SLEEP,
				TIME,
				IDLE,
				VOLTAGE,
				LOG
} States_loop;

typedef enum  	{
	menu,
	blind,
	desktop,
	debug,
	idle,
	temp_show
} Screen;

typedef struct {
	uint8_t new_data_to_show:1;
	uint8_t new_data:1;
	uint8_t measure_activate:1;
	uint8_t measure_running:1;
	uint8_t menu_activate:1;
	uint8_t menu_running:1;
	uint8_t temp_new_set:1;
	uint8_t temp_running:1;
	uint8_t enc_changed:1;
// regulation of the temperature
	uint8_t regulation_temp:1; // Signal when regulation of temp is enabled.
	uint8_t heating_up:1; // record that heating is UP
	uint8_t heating_instant_req:1; // heating instant request for turning ON
	uint8_t heating_instant:1; // heating instant - heating up for defined time period.
	uint8_t regulation_disabled:1;
	uint8_t log_enable:1;
	uint8_t log_requsition:1;

}Flags_main;

extern Flags_main flags;


#endif /* MODULE_GLOBAL_H_ */
