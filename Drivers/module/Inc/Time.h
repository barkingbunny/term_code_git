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



Compare_t actual_HALtick, past_HALtick;

void get_actual_HAL_tick(void);
uint8_t fill_comparer (uint32_t periode,Compare_t* comparer_struct);
uint8_t fill_comparer_seconds(uint16_t periode, Compare_t* comparer_struct);
Bool comparer_timeout(Compare_t* comparer_struct);
/*Function as timer evalution - there is checking if the heater shall be ON or OFF */
Bool timer_value(uint16_t timer_index);
uint16_t end_of_timeout(Compare_t *comparer_struct);

#endif /* MODULE_TIME_H_ */
