/*
 * ds18b20.h
 *
 *  Created on: Dec 12, 2016
 *      Author: jakub
 *
 *      @code
 * #include "DS18B20.h"
 *
 *  ds18b20Init(GPIO_PORT,GPIO_PIN,RES_9_BIT);
 *
 * int main() {
 *   pc.printf("DS18B20 Configuration\n\r");
 *
 *   ROM_Code_t ROM_Code;
 *   ReadROM(&ROM_Code);
 *   pc.printf("Family code: 0x%X\n\r", ROM_Code.BYTES.familyCode);
 *   pc.printf("Serial Number: ");
 *   for (unsigned i = 6; i != 0; --i) {
 *       pc.printf("%02X%s", ROM_Code.BYTES.serialNo[i-1], (i != 1)?":":"\r\n");
 *   }
 *   pc.printf("CRC: 0x%X\r\n", ROM_Code.BYTES.crc_dalas);
 *
 *   pc.printf("\n\rRunning temperature conversion...\n\r");
 *   while (1) {
 *       pc.printf("Temperature is: %.4fC\n\r", GetTemperature());
 *       wait(10);
 *   }
 * }
 * @endcode
 */

#ifndef MODULE_DS18B20_H_
#define MODULE_DS18B20_H_

/** Value to return when Reset() fails */
    enum {INVALID_TEMPERATURE = -10000};

    /** Temperature conversion dit width resolutions */
    enum RESOLUTION { RES_9_BIT=0x1f,    /**< 93.75ms */
                      RES_10_BIT=0x3f,   /**< 187.5ms */
                      RES_11_BIT=0x5f,   /**< 375ms */
                      RES_12_BIT=0x7f    /**< 750ms */
    };
    /** Device onboard register layout (for reference only, not currently used) */
       typedef struct {
           uint8_t    LSB; /**< LSB of converted temperature */
           uint8_t    MSB; /**< MSB of converted temperature */
           uint8_t    Th; /**< Threshold for high alarm */
           uint8_t    Tl; /**< Threshold for low alarm */
           uint8_t    config; /**< Conversion resultion */
           uint8_t    reserved0xFF;
           uint8_t    reserved0xCH;
           uint8_t    reserved0x10;
           uint8_t    crc_dalas; /**< CRC check byte */
       } ScratchPad_t;

       /** Holds 8-byte internal ROM */
              typedef union {
                  uint8_t rom[8];
                  struct {
                      uint8_t familyCode;  /**< Family Code */
                      uint8_t serialNo[6]; /**< Serial Number */
                      uint8_t crc_dalas;   /**< CRC check byte */
                  } BYTES;
              } ROM_Code_t;

// Device byte commands over 1-wire serial
    enum COMMANDS { READ_ROM = 0x33, CONVERT = 0x44, READ_SCRATCHPAD = 0xBE,  WRITE_SCRATCHPAD = 0x4E, SKIP_ROM = 0xCC };

    void ds18b20Init(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin,uint8_t resolution);
    void ds18b20Init_2W(GPIO_TypeDef* GPIOx_out, uint16_t GPIO_Pin_out,GPIO_TypeDef* GPIOx_in, uint16_t GPIO_Pin_in,uint8_t resolution);

    float GetTemperature();

    /** Performs conversion but does not read back temperature. Not needed if
     *  GetTemperature() is used as this calls DoConversion() itself. */
    uint8_t DoConversion();

    /** The method that GetTemperature() calls to do all the conversion and reading
     *  but this method returns a 32-bit signed integer. The integer contains 4
     *  fractional LSBs. Sometimes referred to as s28.4 format. */
    int RawTemperature();

    /** Reads and returns the 8-byte internal ROM */
    uint8_t ReadROM(ROM_Code_t *ROM_Code);

    /** Sets the conversion resolution with RESOLUTION enum (9-12 bits signed) */
    uint8_t SetResolution(uint8_t resolution);


#endif /* MODULE_DS18B20_H_ */
