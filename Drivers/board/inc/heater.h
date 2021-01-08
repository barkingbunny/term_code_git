/*
 * heater.h
 *  projekt - TERMOSTAT
 *  Created on: 20 Nov 2018
 *      Author: js31
 */

#ifndef BOARD_INC_HEATER_H_
#define BOARD_INC_HEATER_H_

#include "pinmap.h"
#include "peripherals.h"
#include "Time.h"

#define NUMBER_HEATER_TIMERS 4

typedef struct {
	uint8_t hours;
	uint8_t minutes:1;
}timer_clock_t;


typedef struct {
	timer_clock_t time_on;
	timer_clock_t time_off;
	uint8_t enable:1;
	uint16_t temperature_req;
	uint8_t reserved:7;
}heat_timer_t;


//static heat_timer_t timer_setting[NUMBER_HEATER_TIMERS];


void hrej(void);

#endif /* BOARD_INC_HEATER_H_ */
