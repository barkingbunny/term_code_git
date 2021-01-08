/*
 * rtc_api.h
 *
 *  Created on: Jan 5, 2017
 *      Author: jakub
 */

#ifndef MODULE_RTC_API_H_
#define MODULE_RTC_API_H_

#include "global.h"
#include "stm32l0xx_hal.h"


void RTC_TimeShow(RTC_HandleTypeDef* RtcHandle, char* showtime);
void RTC_DateShow(RTC_HandleTypeDef* RtcHandle, char* showtime);
void RTC_TimeDateShow(RTC_HandleTypeDef* RtcHandle, char* showtime);
void RTC_TimeShow_time(RTC_TimeTypeDef* stimestructureget, char* showtime);
void RTC_DateShow_date(RTC_DateTypeDef* sdatestructureget, char* showtime);

#endif /* MODULE_RTC_API_H_ */
