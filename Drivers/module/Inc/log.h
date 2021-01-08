/*
 * log.h
 *
 *  Created on: Dec 7, 2017
 *      Author: jakub
 */

#ifndef MODULE_LOG_H_
#define MODULE_LOG_H_

#include "pinmap.h"
#include "global.h"

#define LOG_DATA_LENGTH 0x02ff // number of logged samples (max 16bits)

typedef struct {
	uint8_t minute;
	uint8_t hour;
	uint8_t day;
	uint8_t month;
	uint8_t year;
	int16_t temp_1;
	int16_t hum_1;
}log_item_t;

typedef struct {
	uint8_t new_data:1;
	uint8_t read_request:1;
}Flags_logs;

Flags_logs flags_log;

uint8_t Log_Data(RTC_HandleTypeDef* RtcHandle, int16_t temperature, int16_t humidity, int16_t pressure, uint16_t diagnostics);
void RTC_TimeMark(RTC_HandleTypeDef* RtcHandle, char* showtime);
/*
 * Returned complete date mark - for long period logs
 */
void RTC_TimeMark_Log_Struct(log_item_t* log_handle, char* showtime);
/*
 * Returned time just in hours and minutes - for short period logs
 */
void RTC_TimeMark_Log_Struct_Short(log_item_t* log_handle, char* showtime);
uint8_t Log_Temperature(RTC_HandleTypeDef* RtcHandle, int32_t temperature, int32_t humidity);
uint8_t Log_Read(log_item_t* log_Handle);

uint8_t Log_To_String(char* field_of_char, uint8_t field_lenght);
void Log_Init();
void Log_errase_database(void);
uint16_t Log_memory_fullness(void);
uint8_t Log_delete_last(uint16_t delete_last);

log_item_t log_data[LOG_DATA_LENGTH];

#endif /* MODULE_LOG_H_ */
