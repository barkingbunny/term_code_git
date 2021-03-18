/*
 * Time.h
 *
 *  Created on:  17 March, 2021
 *      Author: jakub
 */
#ifndef MODULE_MODE_AUTO_H_
#define MODULE_MODE_AUTO_H_

#define AUTO_TIMERS 6 // pocet casovych sekci pro nastavovani automatickeho topeni


typedef struct {
	uint8_t valid_timer:1; // false ->  timer dissabled
	uint8_t state:1; //OFF = false -> 0, ON = TRUE -> 1u
	uint8_t reserved:1;
	
	
}Flags_mode_s;

typedef struct {
	uint8_t hours; 	// hodiny 0 - 255
	uint8_t minutes; // minuty 0- 255
}Time_short_s;

/**
 * @brief struktura pro definici automatickeho - casoveho nastavovani termostatu
 * 
 */
typedef struct {
	uint16_t tempOn[AUTO_TIMERS]; // hodnota na kterou se zapne
	uint16_t housekeep; // hodnota pro standby rezim
	Time_short_s time_s[AUTO_TIMERS]; 	// casy pro zmenu
	Flags_mode_s status[AUTO_TIMERS]; 	// statusy pro kazdy timer
	uint8_t sortIndex[AUTO_TIMERS]; // promena pro seznam indexu, serazene jak jdou casove po sobe.
	uint8_t activeIndex; // pocet aktivnich indexu. Zacina jednickou. (1 - jeden timer aktivni)

}Mode_auto_s;

/**
 * Bude potreba promena, kde bude ulozen cely denni progam.
 *
 * Uint8_t program [24][4]    24 - hodin - 4 do hodiny
 * promena: 0 - vypni
 * 			1 - zapni
 *	
 * fan		100-200 - procentualni zapnuti (0 - 100%) // ventilator
 * heat		100 - 200 0 = 0degC, 1 = 0.5degC .... 10 = 5degC; 40 = 20degC ; 45 = 22.5degC.
 *
 */

void mode_auto_init(Mode_auto_s *actual_auto);
void mode_auto_sort(Mode_auto_s *actual_auto);
int16_t mode_auto_temperature(Mode_auto_s *actual_auto, RTC_HandleTypeDef *RtcHandle);
void mode_auto_graph(Mode_auto_s *actual_auto, RTC_HandleTypeDef *RtcHandle);
void mode_auto_graph_delete();


#endif