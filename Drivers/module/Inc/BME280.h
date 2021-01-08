/**
 *  BME280 Combined humidity and pressure sensor library
 *
 *  @author  Toyomasa Watarai
 *  @version 1.0
 *  @date    06-April-2015
 *
 *  Library for "BME280 temperature, humidity and pressure sensor module" from Switch Science
 *    https://www.switch-science.com/catalog/2236/
 *
 *  For more information about the BME280:
 *    http://ae-bst.resource.bosch.com/media/products/dokumente/bme280/BST-BME280_DS001-10.pdf
 */
 
#ifndef MBED_BME280_H
#define MBED_BME280_H

#include "pinmap.h"

//#define _DEBUG
#define DEFAULT_SLAVE_ADDRESS (0x76 << 1)
#define TIMEOUT_I2C 1000
 

    /** init BME280 with i2c handler.
     *  which is connected to specified I2C pins with specified address
     *
     * @param i2c_obj I2C object (instance)
     * @param slave_adr (option) I2C-bus address (default: 0x76)
     */
	void BME280_init(I2C_HandleTypeDef *hi2c, uint8_t slave_adr);

    /** Initializa BME280 sensor
     *
     *  Configure sensor setting and read parameters for calibration
     *
     */
    void BME280_initialize(void);

    /** Read the current temperature value (degree Celsius) from BME280 sensor
     *
     */
    uint32_t BME280_getTemperature(void);

    /** Read the current pressure value (hectopascal)from BME280 sensor
     *
     */
    uint32_t BME280_getPressure(void);

    /** Read the current humidity value (humidity %) from BME280 sensor
     *
     */
    uint32_t BME280_getHumidity(void);

    void i2c_write(uint8_t *pData, uint16_t Size);

    void i2c_read (uint8_t *pData, uint16_t Size);

    I2C_HandleTypeDef *_BME_hi2c;

    uint8_t     address;
    uint16_t    dig_T1;
    int16_t     dig_T2, dig_T3;
    uint16_t    dig_P1;
    int16_t     dig_P2, dig_P3, dig_P4, dig_P5, dig_P6, dig_P7, dig_P8, dig_P9;
    uint16_t    dig_H1, dig_H3;
    int16_t     dig_H2, dig_H4, dig_H5, dig_H6;
    int32_t     t_fine;


#endif // MBED_BME280_H
