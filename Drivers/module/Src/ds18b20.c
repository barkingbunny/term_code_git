/*
 * ds18b20.c
 *this source file shall drive the dalas temperature sensor DS18B20
 *
 *  Created on: Dec 12, 2016
 *      Author: jakub
 */
#include "pinmap.h"
#include "one_wire.h"
#include "ds18b20.h"

void ds18b20Init(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin,uint8_t resolution){
	OneWireInit(GPIOx,GPIO_Pin);
	SetResolution(resolution);

}
/* Initialization of one wire, when 2 wires are used*/
void ds18b20Init_2W(GPIO_TypeDef* GPIOx_out, uint16_t GPIO_Pin_out,GPIO_TypeDef* GPIOx_in, uint16_t GPIO_Pin_in,uint8_t resolution){
	OneWireInit_2W(GPIOx_out,GPIO_Pin_out,GPIOx_in,GPIO_Pin_in);
	SetResolution(resolution);

}

// Set number of bits in the conversion.
uint8_t SetResolution(uint8_t resolution) {
    if (OneWireReset() != 0)
        return 1;
    else {
    	OneWireWriteByte(SKIP_ROM);            // Skip ROM
    	OneWireWriteByte(WRITE_SCRATCHPAD);    // WRITE_SCRATCHPAD
    	OneWireWriteByte(0x7f);                // Alarm TH
    	OneWireWriteByte(0x80);                // Alarm TL
    	OneWireWriteByte(resolution);          // 0xx11111 xx=resolution (9-12 bits)
    }
    return 0;
}

// Trigger a temperature conversion but don't read the temperature.
uint8_t DoConversion() {
    if (OneWireReset() != 0)
        return 1;
    else {
    	OneWireWriteByte(SKIP_ROM);            // Skip ROM
    	OneWireWriteByte(CONVERT);             // Convert
        while (OneWireReadBit() == 0)
            ; // wait for conversion to complete
    }
    return 0;
}

// Do Conversion and get temperature as s8.4 sign-extended to 16-bits.
int RawTemperature() {
    // Perform the temperature conversion.
    if (DoConversion() != 0)
        return INVALID_TEMPERATURE;
    // Read the temperature back.
    if (OneWireReset() != 0)
        return INVALID_TEMPERATURE;
    else {
    	OneWireWriteByte(SKIP_ROM);    // Skip ROM
    	OneWireWriteByte(READ_SCRATCHPAD);    // Read Scrachpad
        unsigned LSB = OneWireReadByte();
        unsigned MSB = OneWireReadByte();
        // Terminate read as we only want temperature
        OneWireReset();
        // Ensure correct sign-extension.
        return (int)((int16_t)((MSB << 8) | LSB));
    }
}

// Read temperature in floating point format.
float GetTemperature() {
    int temperature = RawTemperature();
    return ((float)temperature) / 16.0;
}

// Read back DS18B20 ROM.
uint8_t ReadROM(ROM_Code_t *ROM_Code) {
    if (OneWireReset() != 0)
        return 1;
    else {
    	OneWireWriteByte(READ_ROM);    // Read ROM
        for (unsigned i = 0; i < 8; ++i) {
            ROM_Code->rom[i] = OneWireReadByte();
        }
    }
    return 0;
}

