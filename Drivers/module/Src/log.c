/*
 * log.c
 *
 *  Created on: Dec 7, 2017
 *      Author: jakub
 */
/*
 * This file shall contains functions for logging the data from inputs
 */

#include "log.h"
#include "stdio.h"
#include "string.h"
#include "rtc.h"


uint16_t index_log_wr = 0;
uint16_t index_log_read = 0xfffe;

uint8_t Log_Data(RTC_HandleTypeDef* RtcHandle, int16_t temperature, int16_t humidity, int16_t pressure, uint16_t diagnostics)
{
	char TimeMark[25] = {0};
	char buffer_data[50] = {0};
	RTC_TimeMark(RtcHandle, TimeMark);
	snprintf(buffer_data, 45, "%s;%d;%d;%d;%d;",TimeMark,temperature,humidity, pressure, diagnostics);

	return 1;
}

/* for logging of extra events */
uint8_t Log_Extra(RTC_HandleTypeDef* RtcHandle)
{
	char TimeMark[25] = {0};
	RTC_TimeMark(RtcHandle, TimeMark);


	return 1;
}

void RTC_TimeMark(RTC_HandleTypeDef* RtcHandle, char* showtime)
{
  RTC_DateTypeDef rtc_date_structure;
  RTC_TimeTypeDef rtc_time_structure;

  /* Get the RTC current Time */
  HAL_RTC_GetTime(RtcHandle, &rtc_time_structure, RTC_FORMAT_BIN);
  /* Get the RTC current Date */
  HAL_RTC_GetDate(RtcHandle, &rtc_date_structure, RTC_FORMAT_BIN);
  /* Display time Format : hh:mm:ss */
  sprintf((char*)showtime,"20%2d-%02d-%02d-%02d:%02d:%02d",rtc_date_structure.Year, rtc_date_structure.Month, rtc_date_structure.Date, rtc_time_structure.Hours, rtc_time_structure.Minutes, rtc_time_structure.Seconds);
}

void RTC_TimeMark_Log_Struct(log_item_t* log_handle, char* showtime)
{

  /* Display time Format : yy:mm:dd:hh:mm */
  sprintf((char*)showtime,"20%2d-%02d-%02d-%02d:%02d",log_handle->year, log_handle->month, log_handle->day, log_handle->hour , log_handle->minute);
}

void RTC_TimeMark_Log_Struct_Short(log_item_t* log_handle, char* showtime)
{

  /* Display time Format : yy:mm:dd:hh:mm */
  sprintf((char*)showtime,"%02d:%02d", log_handle->hour , log_handle->minute);
}


uint8_t Log_Temperature(RTC_HandleTypeDef* RtcHandle, int32_t temperature, int32_t humidity)
{
	humidity = humidity/1024;
	int16_t humidity_16 = (int16_t)humidity;
	int16_t temperature_16 = temperature;

	log_data[index_log_wr].temp_1= temperature_16;
	log_data[index_log_wr].hum_1 = humidity_16;


	RTC_TimeTypeDef rtc_time_structure;
	/* Get the RTC current Time */
	HAL_RTC_GetTime(RtcHandle, &rtc_time_structure, RTC_FORMAT_BIN);
	RTC_DateTypeDef rtc_date_structure;
	/* Get the RTC current Date */
	HAL_RTC_GetDate(RtcHandle, &rtc_date_structure, RTC_FORMAT_BIN);

	log_data[index_log_wr].year = rtc_date_structure.Year;
	log_data[index_log_wr].month = rtc_date_structure.Month;
	log_data[index_log_wr].day = rtc_date_structure.Date;
	log_data[index_log_wr].hour = rtc_time_structure.Hours;
	log_data[index_log_wr].minute = rtc_time_structure.Minutes;

	index_log_wr++;
	if (index_log_wr >= LOG_DATA_LENGTH)
		index_log_wr = 0;

return 1;
}

/*This function is for reading the logged data from memory. Separately - each call is one record
 * return statements:
 * 1 - finished, all (non zero) data were read
 * 2 - there are still data, that were not read.
 *
 */
uint8_t Log_Read(log_item_t* log_Handle){
	if (!flags_log.read_request){		// first time, when this function is used.
		index_log_read=index_log_wr-1;  // set the reading data to the latest log
		flags_log.read_request = TRUE;
	}
	if ( 0 != Log_memory_fullness() ) return 0;

#ifdef DEBUG_TERMOSTAT
char buffer_s [32];
lcd_setCharPos(1,0);
snprintf(buffer_s, 19, "rd=%3i;wr%3i  ", index_log_read, index_log_wr-1);
lcd_printString(buffer_s);
#endif

	do {	// Write je uz o jedno vetsi, tak neni treba ho navysovat...
		index_log_read++;
		if (index_log_read >= LOG_DATA_LENGTH)
			index_log_read = 0;
		// NEBEZPECI pri prazdnem bufferu se to zde zacykli a kousne se cely procesor
	} while (0 == log_data[index_log_read].day);



#ifdef DEBUG_TERMOSTAT
lcd_setCharPos(2,0);
snprintf(buffer_s, 10, "ctu %3i ", index_log_read);
lcd_printString(buffer_s);
#endif

	//memcpy(log_Handle, log_data[index_log_read], arraysize * sizeof (struct log_item_t));
	log_Handle->temp_1 = log_data[index_log_read].temp_1;
	log_Handle->hum_1 = log_data[index_log_read].hum_1;
	log_Handle->year = log_data[index_log_read].year;
	log_Handle->month = log_data[index_log_read].month;
	log_Handle->day = log_data[index_log_read].day;
	log_Handle->hour = log_data[index_log_read].hour;
	log_Handle->minute = log_data[index_log_read].minute;

	if (index_log_read != index_log_wr-1){
		return 2;
	}

	flags_log.read_request= FALSE;
	return 1;
}

uint8_t Log_To_String(char* field_of_char, uint8_t field_lenght){
	log_item_t log_Handle;
	uint8_t log_read_stat= 0;
	char* TimeMark[25];

	log_read_stat=Log_Read(&log_Handle);
	if (field_lenght<19){
		RTC_TimeMark_Log_Struct_Short(&log_Handle, TimeMark);
		snprintf((char*)field_of_char, 18, "%s;%d;%d;",TimeMark,log_Handle.temp_1, log_Handle.hum_1);
	}
	else
	{
		RTC_TimeMark_Log_Struct(&log_Handle, TimeMark);
		snprintf((char*)field_of_char, 32, "%s;%d;%d;",TimeMark,log_Handle.temp_1, log_Handle.hum_1);
	}
		return log_read_stat;
		// 2 - if there are more data to read, return 2;
		// 1 - if all memory was read, return 1;
		// 0 - nodata in memeory / ERROR

}

/**
 * Inicializace databaze - je to nutne na zacatku
 */
void Log_Init(){
	flags_log.read_request = FALSE;
// delete complete variable log_data
	Log_errase_database();
} // end Log_Init

void Log_errase_database(void){
	for (uint16_t index=0; index<LOG_DATA_LENGTH; index++){
		log_data[index_log_wr].temp_1= 0;
		log_data[index_log_wr].hum_1 = 0;
		log_data[index_log_wr].year = 0;
		log_data[index_log_wr].month = 0;
		log_data[index_log_wr].day = 0;
		log_data[index_log_wr].hour = 0;
		log_data[index_log_wr].minute = 0;
	} // end FOR

}
/**Return number of recorded data in the database
 *
 */
uint16_t Log_memory_fullness(void){
	uint16_t log_occupate = 0;
	for (uint16_t index=0; index<LOG_DATA_LENGTH; index++){
		if(0 != log_data[index].day)
			log_occupate++;
	}
	return log_occupate;
}

/** Vymaze poslednich "delete_last" zaznamu z databaze
 *
 */
uint8_t Log_delete_last(uint16_t delete_last){
	uint16_t index_log_delete = index_log_wr-1;  // set the reading data to the latest log
	do {	// Write je uz o jedno vetsi, tak neni treba ho navysovat...
		index_log_delete++;
		if (index_log_delete >= LOG_DATA_LENGTH)
			index_log_delete = 0;
		// NEBEZPECI pri prazdnem bufferu se to zde zacykli a kousne se cely procesor
	} while (0 == log_data[index_log_delete].day);
	for (uint16_t index=0; index<delete_last; index++){
		log_data[index_log_delete].temp_1=0;
		log_data[index_log_delete].hum_1=0;
		log_data[index_log_delete].year=0;
		log_data[index_log_delete].month=0;
		log_data[index_log_delete].day=0;
		log_data[index_log_delete].hour=0;
		log_data[index_log_delete].minute=0;
		index_log_delete++;

	}
	return 1;
}
