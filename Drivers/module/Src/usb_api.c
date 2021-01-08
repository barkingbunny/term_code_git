/*
 * usb_api.c
 *
 *  Created on: 31 May 2020
 *      Author: jakub
 *
 *      Tato funkce pouziva klasicke funkce z HAL knihoven ST32
 *
 */
#include "usb_api.h"
/**
  * @brief  change the format of sending data to uint8_t as required
  *
  * @param  buffer: Buffer of data to be transmit
  * @param  length: Number of data to be transmit (in bytes)
  * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL or USBD_BUSY
  */
uint8_t USB_transmit(char *buffer,uint16_t length)
{
	uint8_t status = 255;
	if (length > 0 ) {
		uint8_t buffer_2[length];
		for (int j=0; j<length;j++){
			buffer_2[j] = (uint8_t *)&buffer[j];
		}
		uint8_t status;
		status = CDC_Transmit_FS(&buffer_2[0],length);
	}
	return status;
}
