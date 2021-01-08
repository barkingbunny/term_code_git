/*
 * Debounce.c
 *
 *  Created on: Jul 2, 2019
 *      Author: jakub
 *      last Update: 3-Jul-2019
 */
/**
 * This file shall help with debouncing a switch and be able to select between short push and long push button
 * The code file include
 */

#include "debounce.h"

/** This function will be called from other function and when 12 occurrence registered a close switch, then returned = 1
 * input:	state of the button - assumed log 1 - released, log 0 pressed button.
 * output:	0 - No press detected
 *			1 - press of the button was detected - exact 12 cycles
 */

uint8_t DebounceSwitch(Bool button){
	static uint16_t state=0;
	state = (state<<1) | (!button) | (0xe000);
	if (0xf000 == state) return 1;
	return 0;
	}
