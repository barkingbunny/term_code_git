/*
 * pinmap.c
 *
 *  Created on: Dec 12, 2016
 *      Author: jakub
 */

#include "pinmap.h"

/** This function change the pinmaping of the device
 * inputs:
 *
 * pin_mode is declaration for macro from HAL library:
 * GPIO_MODE_INPUT; GPIO_MODE_OUTPUT_PP; GPIO_MODE_OUTPUT_OD
 *
 */

void pinmap_remap(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, uint32_t pin_mode)
{
	/* Check the parameters */
	  assert_param(IS_GPIO_PIN_AVAILABLE(GPIOx,GPIO_Pin));
	  assert_param(IS_GPIO_PIN_ACTION(PinState));

    // Configure GPIO
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.Pin       = GPIO_Pin;
    GPIO_InitStructure.Mode      = pin_mode;
    GPIO_InitStructure.Pull      = GPIO_NOPULL;
    GPIO_InitStructure.Speed     = GPIO_SPEED_HIGH;
//    GPIO_InitStructure.Alternate = 0;
    HAL_GPIO_Init(GPIOx, &GPIO_InitStructure);

}

