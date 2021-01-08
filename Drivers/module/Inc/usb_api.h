/*
 * usb_api.h
 *
 *  Created on: 31 May 2020
 *      Author: jakub
 */

#ifndef MODULE_INC_USB_API_H_
#define MODULE_INC_USB_API_H_

#include "pinmap.h"
#include "global.h"

uint8_t USB_transmit(char* buffer,uint16_t length);

#endif /* MODULE_INC_USB_API_H_ */
