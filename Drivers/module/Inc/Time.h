/*
 * Time.h
 *
 *  Created on: Feb 5, 2017
 *      Author: jakub
 */

#ifndef MODULE_TIME_H_
#define MODULE_TIME_H_

typedef struct {
	uint32_t tick;
	uint8_t overflow:1;
}Compare_t;

/**
 * Bude potreba promena, kde bude ulozen cely denni progam.
 *
 * Uint8_t program [24][4]    24 - hodin - 4 do hodiny
 * promena: 0 - vypni
 * 			1 - zapni
 *
 * fan		100-200 - procentualni zapnuti (0 - 100%) // ventilator
 * heat		100 - 200 0 = 0degC, 1 = 0.5degC .... 10 = 5degC; 40 = 20degC ; 45 = 22.5degC.
 *
 */

Compare_t actual_HALtick, past_HALtick;

void get_actual_HAL_tick(void);
uint8_t fill_comparer (uint32_t periode,Compare_t* comparer_struct);
uint8_t fill_comparer_seconds(uint16_t periode, Compare_t* comparer_struct);
Bool comparer_timeout(Compare_t* comparer_struct);
/*Function as timer evalution - there is checking if the heater shall be ON or OFF */
Bool timer_value(uint16_t timer_index);


#endif /* MODULE_TIME_H_ */
