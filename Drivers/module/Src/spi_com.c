/*
 * spi_com.c
 *
 *  Created on: Dec 6, 2016
 *      Author: jakub
 */

#include "spi.h"
#include "spi_com.h"

#define SPI_TIMEOUT	1000u //ms

/* Private define ------------------------------------------------------------*/


void SPI_writeByte(SPI_HandleTypeDef* spi_x, uint8_t byte){
	if(HAL_SPI_Transmit(spi_x, &byte, 1u, SPI_TIMEOUT) != HAL_OK)
	  {
	    /* Transfer error in transmission process */
	    Error_Handler();
	  }

}
