/*
 * Pinmap.h
 * In this file would be included all input/output signal and their names.
 *  projekt - TERMOSTAT
 *  Created on: Dec 2, 2016
 *      Author: jakub
 */
/*
 * In the file main.h are defined the GPIO names of the pins.
 * The file main.h is generated by cube SW
 */
#include "main.h"
#include "spi.h"
#include "tim.h"
#include "gpio.h"
#include "global.h"

#ifndef PINMAP_H_
#define PINMAP_H_

//definice nazvu pro PWM periferii
#define LED1 21
#define LED2 22
#define LCD_LIGHT 23

/*
#define Led1Set HAL_GPIO_WritePin(D_LED_1_GPIO_Port,D_LED_1_Pin,GPIO_PIN_SET)
#define Led2Set HAL_GPIO_WritePin(D_LED_2_GPIO_Port,D_LED_2_Pin,GPIO_PIN_SET)

#define Led1Clear HAL_GPIO_WritePin(D_LED_1_GPIO_Port,D_LED_1_Pin,GPIO_PIN_RESET)
#define Led2Clear HAL_GPIO_WritePin(D_LED_2_GPIO_Port,D_LED_2_Pin,GPIO_PIN_RESET)

#define Led1Tgl HAL_GPIO_TogglePin(D_LED_1_GPIO_Port,D_LED_1_Pin)
#define Led2Tgl HAL_GPIO_TogglePin(D_LED_2_GPIO_Port,D_LED_2_Pin)
*/
#define LCD_dcSet HAL_GPIO_WritePin(D_LCD_D_C_GPIO_Port, D_LCD_D_C_Pin, GPIO_PIN_SET)
#define LCD_dcClear HAL_GPIO_WritePin(D_LCD_D_C_GPIO_Port, D_LCD_D_C_Pin, GPIO_PIN_RESET)

#define LCD_rstSet HAL_GPIO_WritePin(D_LCD_RST_GPIO_Port, D_LCD_RST_Pin, GPIO_PIN_SET)
#define LCD_rstClear HAL_GPIO_WritePin(D_LCD_RST_GPIO_Port, D_LCD_RST_Pin, GPIO_PIN_RESET)

#define LCD_csSet HAL_GPIO_WritePin(D_LCD_CS_GPIO_Port, D_LCD_CS_Pin, GPIO_PIN_SET)
#define LCD_csClear HAL_GPIO_WritePin(D_LCD_CS_GPIO_Port, D_LCD_CS_Pin, GPIO_PIN_RESET)

#define OUT1_Set HAL_GPIO_WritePin(D_OUT1_GPIO_Port, D_OUT1_Pin, GPIO_PIN_SET)
#define OUT1_Clear HAL_GPIO_WritePin(D_OUT1_GPIO_Port, D_OUT1_Pin, GPIO_PIN_RESET);

#define OUT2_Set HAL_GPIO_WritePin(D_OUT2_GPIO_Port, D_OUT2_Pin, GPIO_PIN_SET)
#define OUT2_Clear HAL_GPIO_WritePin(D_OUT2_GPIO_Port, D_OUT2_Pin, GPIO_PIN_RESET);

#define SW1 HAL_GPIO_ReadPin(D_SW1_GPIO_Port, D_SW1_Pin)
#define SW2 HAL_GPIO_ReadPin(D_SW2_GPIO_Port, D_SW2_Pin)
#define SW_ENC HAL_GPIO_ReadPin(D_SW_ENC_GPIO_Port, D_SW_ENC_Pin)

//TESTOVACI PROMENE
uint32_t testing1;

/**
 * Configure pin ( output type)
 */
void pinmap_remap(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, uint32_t pin_mode);

#endif /* PINMAP_H_ */
