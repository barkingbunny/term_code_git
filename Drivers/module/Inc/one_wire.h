/*
 * one_wire.h
 *
 *  Created on: Dec 12, 2016
 *      Author: jakub
 */

#ifndef MODULE_ONE_WIRE_H_
#define MODULE_ONE_WIRE_H_

#include "pinmap.h"
#include "stm32l0xx_hal_gpio.h"
// Timing delay for 1-wire serial standard option
    //enum DELAY { A = 6, B = 64, C = 60, D = 10, E = 9, F = 55, G = 0, H = 480, I = 70, J = 410 };
	enum DELAY { A = 2, B = 35, C =46, D = 2, E =1, F = 15, G = 0, H = 450, I =42, J = 420 };
static GPIO_TypeDef* one_wire;
static GPIO_TypeDef* one_wire_in;
static uint16_t one_wire_pin, one_wire_pin_in;
static Bool double_wire=FALSE; //Is used a two wires for one wire perihreals?

void OneWireInit(GPIO_TypeDef *GPIO_type, uint16_t GPIO_Pin);

void OneWireInit_2W(GPIO_TypeDef* GPIOx_out,uint16_t GPIO_Pin_out,GPIO_TypeDef* GPIOx_in,uint16_t GPIO_Pin_in);

unsigned OneWireReset();
void OneWireWriteBit(unsigned bit);
uint8_t OneWireReadBit();
void OneWireWriteByte(unsigned byte);
uint8_t OneWireReadByte();

static void _pin_output(void);
static void _pin_input(void);
static void _pin(uint8_t dat);
static uint8_t _pin_read(void);



#endif /* MODULE_ONE_WIRE_H_ */
