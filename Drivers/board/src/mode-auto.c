/**
 * @file mode-auto.c
 * @author Jakub S
 * @brief knihovna pro Automaticky mod
 * @version 0.1
 * @date 2021-03-17
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#include "tim.h"
#include "global.h"
#include "peripherals.h"
#include "rtc.h"
#include "mode-auto.h"
#include "lcd_12864.h"

/**
 * @brief Inicializace automatickeho modu - nastavovaciho casovace
 * 			vymaze priznaky, ze je validni nastaveni casovace
 * 
 * @param actual_auto 
 */
void mode_auto_init(Mode_auto_s *actual_auto)
{
	actual_auto->activeIndex = 0;
	for (uint8_t index = 0; index < AUTO_TIMERS; index++)
	{
		actual_auto->status[index].valid_timer = FALSE;
		actual_auto->sortIndex[index] = index;
	}
}

/**
 * @brief Serazeni pozadavku na zapnuti/vypnuti , dle casove osy
 * vysledek se ulozi v poli, po sobe jdoucih indexu, jak jdou casove posloupne
 * 
 * @param actual_auto 
 */
void mode_auto_sort(Mode_auto_s *actual_auto)
{
	uint16_t timeSorterMinute[AUTO_TIMERS];
	uint8_t buffer = 0;
	for (uint8_t index = 0; index < AUTO_TIMERS; index++)
	{
		if (actual_auto->status[index].valid_timer)
		{
			timeSorterMinute[index] = actual_auto->time_s[index].hours * 60 + actual_auto->time_s[index].minutes;
			buffer++;
		}
		else
			timeSorterMinute[index] = 65535; // toto oznacuje kdyz je vypnuty casovac.
	}
	actual_auto->activeIndex = buffer; // ulozeni poctu zapnutych casovacu.

	for (uint8_t index_a = 0; index_a < (AUTO_TIMERS - 1); index_a++)
	{
		for (uint8_t index_b = 0; index_b < (AUTO_TIMERS - 1); index_b++)
		{
			if (timeSorterMinute[actual_auto->sortIndex[index_b]] > timeSorterMinute[actual_auto->sortIndex[index_b + 1]])
			{
				buffer = actual_auto->sortIndex[index_b + 1];
				actual_auto->sortIndex[index_b + 1] = actual_auto->sortIndex[index_b];
				actual_auto->sortIndex[index_b] = buffer;
			}
		}
	}
}

/**
 * @brief  vraci teplotu na kterou topit 
 * 
 * @param actual_auto  vstupni struktura, ktera udava data
 * @param RtcHandle 
 * @return int16_t hodnota 0-valid temp - status = TRUE; 
 * 					-20000 -> status = False - tedy vypni; 
 * 					-21000 -> neni v pameti zadny zapnuty casovac; 
 */
int16_t mode_auto_temperature(Mode_auto_s *actual_auto, RTC_HandleTypeDef *RtcHandle)
{
	// ziskani casu z RTC periferie
	RTC_TimeTypeDef stimestructureget;
	HAL_RTC_GetTime(RtcHandle, &stimestructureget, RTC_FORMAT_BIN);
	int16_t temperature_auto;
	uint16_t actualHoursMinutes = stimestructureget.Hours * 60 + stimestructureget.Minutes;

	uint16_t timeSorterMinute[AUTO_TIMERS];
	for (uint8_t index = 0; index < AUTO_TIMERS; index++)
	{
		if (actual_auto->status[index].valid_timer)
		{
			timeSorterMinute[index] = actual_auto->time_s[index].hours * 60 + actual_auto->time_s[index].minutes;
		}
		else
			timeSorterMinute[index] = 65535; // toto oznacuje kdyz je vypnuty casovac.
	}

	if (1 == actual_auto->activeIndex) // je aktivni prave jeden - tedy teplota je dana jim
	{
		if (actual_auto->status[0].state)
			temperature_auto = actual_auto->tempOn[0];
		else
			temperature_auto = -20000;
	}
	else if (actual_auto->activeIndex > 1)
	{
		// pokud je aktualni cas vetsi nez jakykoliv nastaveny, tak plati posledni nastaveny (casova osa)
		if (TRUE == actual_auto->status[actual_auto->sortIndex[actual_auto->activeIndex - 1]].state)
			temperature_auto = actual_auto->tempOn[actual_auto->sortIndex[actual_auto->activeIndex - 1]];
		else // topeni je v tuto dobu vypnute
		{
			temperature_auto = -20000;
		}

		for (uint8_t index = 0; index < actual_auto->activeIndex; index++)
		{
			if (timeSorterMinute[actual_auto->sortIndex[index]] > actualHoursMinutes)
			{
				if (0 == index)
				{
					if (TRUE == actual_auto->status[actual_auto->sortIndex[actual_auto->activeIndex - 1]].state)
						temperature_auto = actual_auto->tempOn[actual_auto->sortIndex[actual_auto->activeIndex - 1]];
					else // topeni je v tuto dobu vypnute
						temperature_auto = -20000;
					break;
				}
				else
				{
					if (TRUE == actual_auto->status[actual_auto->sortIndex[index - 1]].state)
						temperature_auto = actual_auto->tempOn[actual_auto->sortIndex[index - 1]];
					else // topeni je v tuto dobu vypnute
						temperature_auto = -20000;
					break;
				}
			}
		}
	}
	else // pravdepodobne je to 0, nebo chyba?
	{
		temperature_auto = -21000;
	}

	return temperature_auto;
}

/**
 * @brief vykresleni grafu nastaveni casu pro spinani
 * 
 * @param actual_auto 
 * @param RtcHandle 
 */
void mode_auto_graph(Mode_auto_s *actual_auto, RTC_HandleTypeDef *RtcHandle)
{

	if (actual_auto->activeIndex > 0) // jestli je tam aspon jeden zaznam
	{
		RTC_TimeTypeDef stimestructureget;

		uint16_t hoursMinutes[AUTO_TIMERS]; //MINUT_DEN
		const uint16_t hoursMinutesMax = 1440;
		const uint16_t lcd_usable_width = LCD_WIDTH / 24 * 24;
		uint16_t zacatek, konec;
		uint8_t index_next;

		HAL_RTC_GetTime(RtcHandle, &stimestructureget, RTC_FORMAT_BIN);
		uint16_t actualHoursMinutes = stimestructureget.Hours * 60 + stimestructureget.Minutes;
		konec = actualHoursMinutes * lcd_usable_width / hoursMinutesMax;

		line_clear(48);

		line((konec + 4), 48, (konec + 4), 55, 1); //casova znacka

		for (uint8_t index = 0; index < AUTO_TIMERS; index++)
		{
			if (actual_auto->status[actual_auto->sortIndex[index]].valid_timer)
				hoursMinutes[index] = actual_auto->time_s[actual_auto->sortIndex[index]].hours * 60 + actual_auto->time_s[actual_auto->sortIndex[index]].minutes;
			else
			{
				hoursMinutes[index] = hoursMinutesMax;
			}
		}

		/*		if (actual_auto->activeIndex == 1)
			konec = lcd_usable_width;
		else
			konec = hoursMinutes[actual_auto->activeIndex - 1] * lcd_usable_width / hoursMinutesMax;

		if (TRUE == actual_auto->status[actual_auto->sortIndex[actual_auto->activeIndex - 1]].state) // ZAPNI TOPENI
		{
			if (actual_auto->tempOn[actual_auto->sortIndex[actual_auto->activeIndex - 1]] > 2100) // teplota je nad 21 C
				line((4), 49, (konec + 4), 49, 1);												  //stav ZAPNUTP
			if (actual_auto->tempOn[actual_auto->sortIndex[actual_auto->activeIndex - 1]] > 2000) // teplota je nad 20 C
				line((4), 50, (konec + 4), 50, 1);												  //stav ZAPNUTP
			line((4), 51, (konec + 4), 51, 1);													  //stav ZAPNUTP
		}
		else // prikaz pro topeni - vypnout
		{
			line((4), 55, (konec + 4), 55, 1); //stav VYPNUTO
		}*/

		for (uint8_t index = 0; index < actual_auto->activeIndex; index++)
		{
			if (actual_auto->status[actual_auto->sortIndex[index]].valid_timer)
			{
				zacatek = hoursMinutes[index] * lcd_usable_width / hoursMinutesMax;
				index_next = index + 1;
				if (index_next == actual_auto->activeIndex)
					index_next = 0;
				if (actual_auto->status[actual_auto->sortIndex[index_next]].valid_timer)
				{
					konec = hoursMinutes[index_next] * lcd_usable_width / hoursMinutesMax;
				}
				else
				{
					konec = lcd_usable_width;
				}
				if (zacatek > konec) // posledni hodnota Indexu muze mit konec az v predchozi.
				{
					if (TRUE == actual_auto->status[actual_auto->sortIndex[index]].state) // ZAPNI TOPENI
					{
						if (actual_auto->tempOn[actual_auto->sortIndex[index]] > 2100) // teplota je nad 21 C
						{
							line((zacatek + 4), 49, (lcd_usable_width + 4), 49, 1); //stav ZAPNUTP
							line((4), 49, (konec + 4), 49, 1);						//stav ZAPNUTP
						}
						if (actual_auto->tempOn[actual_auto->sortIndex[index]] > 2000) // teplota je nad 20 C
						{
							line((4 + zacatek), 50, (lcd_usable_width + 4), 50, 1); //stav ZAPNUTP
							line((4), 50, (konec + 4), 50, 1);						//stav ZAPNUTP
						}
						line((4 + zacatek), 51, (lcd_usable_width + 4), 51, 1); //stav ZAPNUTP
						line((4), 51, (konec + 4), 51, 1);						//stav ZAPNUTP
					}
					else // prikaz pro topeni - vypnout
					{
						line((4 + zacatek), 55, (lcd_usable_width + 4), 55, 1); //stav vypnuto
						line((4), 55, (konec + 4), 55, 1);						//stav vypnuto
					}
				}
				else
				{
					if (TRUE == actual_auto->status[actual_auto->sortIndex[index]].state) // ZAPNI TOPENI
					{
						if (actual_auto->tempOn[actual_auto->sortIndex[index]] > 2100) // teplota je nad 21 C
							line((4 + zacatek), 49, (konec + 4), 49, 1);			   //stav ZAPNUTP
						if (actual_auto->tempOn[actual_auto->sortIndex[index]] > 2000) // teplota je nad 20 C
							line((4 + zacatek), 50, (konec + 4), 50, 1);			   //stav ZAPNUTP
						line((4 + zacatek), 51, (konec + 4), 51, 1);				   //stav ZAPNUTP
					}
					else // prikaz pro topeni - vypnout
					{
						line((zacatek + 4), 55, (konec + 4), 55, 1); //stav VYPNUTO
					}
				}
			}
		}
	}
} /* END OF  - SEM BUDU PSAT proceduru pro vygresleni GRAFU, co znazorni zap/vyp topeni v jednodenim cyklu*/

void mode_auto_graph_delete()
{
	line_clear(49);
}