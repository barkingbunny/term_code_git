/*
 * Time.c
 *
 *  Created on: Feb 5, 2017
 *      Author: jakub
 */

#include "tim.h"
#include "global.h"
#include "rtc.h"
#include <Time.h>


void get_actual_HAL_tick(void){
	actual_HALtick.tick = HAL_GetTick();
	if (past_HALtick.tick > actual_HALtick.tick)
	{
		if (past_HALtick.overflow)
			actual_HALtick.overflow = FALSE;
		else
			actual_HALtick.overflow = TRUE;

		past_HALtick.overflow=actual_HALtick.overflow;
	}
	past_HALtick.tick=actual_HALtick.tick;
}

/**
 * Input - number with specific time wait - in MILISECOND
 *
 *  */
uint8_t fill_comparer (uint32_t periode, Compare_t* comparer_struct){

	get_actual_HAL_tick();
	comparer_struct->tick = actual_HALtick.tick+periode;
	comparer_struct->overflow = actual_HALtick.overflow;
	if (comparer_struct->tick < actual_HALtick.tick) {
		comparer_struct->overflow=TRUE;
	}
	return (1);
}
/**
 * input: number for delay in Second
 */
uint8_t fill_comparer_seconds(uint16_t periode, Compare_t* comparer_struct){
	get_actual_HAL_tick();
	comparer_struct->tick = periode*1000+ actual_HALtick.tick;
	comparer_struct->overflow = actual_HALtick.overflow;
	if (comparer_struct->tick < actual_HALtick.tick) {
		comparer_struct->overflow=TRUE;
	}
	return (1);

}

/* This function detect if the comparer value was reached by the HAL ticks
 *
 */

Bool comparer_timeout(Compare_t* comparer_struct){
	if(comparer_struct->tick <= actual_HALtick.tick){
		if ( comparer_struct->overflow == actual_HALtick.overflow){
			return (TRUE);
		}
		else
			return (FALSE);
	}
	else
	{
		if ( comparer_struct->overflow != actual_HALtick.overflow)
			return(TRUE);
	}
	return (FALSE);
}
/**
 * Tehle funkce vraci, zda ma v dany cas byt periferie - ON - TRUE nebo OFF - FALSE
 */
Bool timer_value(uint16_t timer_index){
	Bool timer_val = FALSE;
///// DOPSAT NENI TO HOTOVE

	return timer_val;
}

