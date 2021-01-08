/*
 * one_wire.c
 *
 *  Created on: Dec 12, 2016
 *      Author: jakub
 */
/**
 * Knihovna pro onewire pri pouziti dvou pinu!
 * Tento file jsem upravoval naposledu pro procesor STM32L072 -
 * je zde problem s tim, ze procesor nestiha prepinat mezi vstupem a vystupem dostatecne rychle, pri pouziti HAL knihoven
 * rozhodl jsem se pouzit dva piny pro ONE WIRE
 *
 *
 */
#include "one_wire.h"

#include <stdint.h>
//initializate of one wire bus. Taking the pin for communication
/**
 * inputs:
 * Pin and port needs to be defined first in the GPIO.c file as normal pin (input) first.
 */
void OneWireInit(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{
	one_wire = GPIOx;
	one_wire_pin = GPIO_Pin;
	double_wire = FALSE;
//	prec_wait_us(200);
}
void OneWireInit_2W(GPIO_TypeDef* GPIOx_out,uint16_t GPIO_Pin_out,GPIO_TypeDef* GPIOx_in,uint16_t GPIO_Pin_in)
{
	one_wire = GPIOx_out;
	one_wire_pin = GPIO_Pin_out;
	one_wire_in = GPIOx_in;
	one_wire_pin_in = GPIO_Pin_in;
	double_wire = TRUE;
}

// Reset 1-wire interface.
unsigned OneWireReset() {
    _pin_output();
    _pin(0);    // drive bus low
    prec_wait_us(H);
    _pin_input(); // release bus
    prec_wait_us(I);
    unsigned result = _pin_read();  // read bus value
    prec_wait_us(J);
    return result;
}

// Write bit to 1-wire.
void OneWireWriteBit(unsigned bit) {

    _pin_output();
    if (bit) {
        _pin(0);        // drive bus low
        prec_wait_us(A);        // delay A
        _pin(1);        // release bus - open drain
        prec_wait_us(B);        // delay B
    } else {
        _pin(0);    	// drive bus low
        prec_wait_us(C);    // delay C
        _pin(1);        // release bus - open drain
        prec_wait_us(D);    // delay D
    }
}

// Read bit from 1-wire.
uint8_t OneWireReadBit() {
	uint8_t bit_value;
	_pin_output();

	_pin(0);        // drive bus low
	_pin(1);
	if (double_wire) {
		prec_wait_us(A);        // delay A
		bit_value = HAL_GPIO_ReadPin(one_wire_in,one_wire_pin_in);
	}// IF double wire

	else
	{
		prec_wait_us(A);        // delay A
		_pin_input();      // release bus
		prec_wait_us(E);        // delay E


		bit_value = _pin_read();    // master sample bus
	} //else
	prec_wait_us(F);
	return bit_value;
}

// Write byte to 1-wire.
void OneWireWriteByte(unsigned byte) {
    for (unsigned bit = 0; bit < 8; ++bit) {
    	OneWireWriteBit(byte & 0x01); // lsb to msb
        byte >>= 1;    // right shift by 1-bit
    }
}

// Read byte from 1-wire.
uint8_t OneWireReadByte() {
	uint8_t byte = 0;
    for (unsigned bit = 0; bit < 8; ++bit) {
        byte |= (OneWireReadBit() << bit);    // Reads lsb to msb
    }
    return byte;
}

/**
 * function for changing the pin to output - open drain
 * the variables are taken from init of the one wire library
 */
static void _pin_output(void){
	_pin(1);
	pinmap_remap(one_wire, one_wire_pin, GPIO_MODE_OUTPUT_OD);

}

/**
 * function for changing the pin to input
 * the variables are taken from init of the one wire library
 */
static void _pin_input(void){
	pinmap_remap(one_wire, one_wire_pin, GPIO_MODE_INPUT);
}

static void _pin(uint8_t dat){
	if (dat==0) // dat = 0;
		HAL_GPIO_WritePin(one_wire,one_wire_pin,GPIO_PIN_RESET);
	else // dat = 1 or else
		HAL_GPIO_WritePin(one_wire,one_wire_pin,GPIO_PIN_SET);
}

uint8_t _pin_read(void){
	return ( HAL_GPIO_ReadPin(one_wire,one_wire_pin));
}
