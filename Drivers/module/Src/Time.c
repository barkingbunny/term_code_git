/** @file Time.c
 *  @brief Casovac.
 *  Created on: Feb 5, 2017
 * 
 * Driver pracuje s HAL knihovnami. Jdou zde funkce pro hlidani delky cyklu,
 * casovani atd.
 *  @author Jakub S
 *  @bug No know bugs.
 */

#include "tim.h"
#include "global.h"
#include "rtc.h"
#include <Time.h>
/**
 * @brief Get the actual HAL tick object
 * 
 * ziskani aktualniho cisla od system ticku - cas ktery bezi stale na pozadi
 * 
 */
void get_actual_HAL_tick(void)
{
	actual_HALtick.tick = HAL_GetTick();
	if (past_HALtick.tick > actual_HALtick.tick)
	{
		if (past_HALtick.overflow)
			actual_HALtick.overflow = FALSE;
		else
			actual_HALtick.overflow = TRUE;

		past_HALtick.overflow = actual_HALtick.overflow;
	}
	past_HALtick.tick = actual_HALtick.tick;
}

/**
 * @brief nastaveni casoveho bodu dle periody
 * 
 * Input - number with specific time wait - in MILISECOND
 * 
 * @param periode cas v milisekundach, ktery se bude cekat; nastavi se casovac odted
 * @param comparer_struct struktura, do ktere se zapisuje vysledna hodnota, kdy vyprsi nacasovani
 * @return uint8_t navratova hodnota, zda vse probehlo v poradku
 */
uint8_t fill_comparer(uint32_t periode, Compare_t *comparer_struct)
{

	get_actual_HAL_tick();
	comparer_struct->tick = actual_HALtick.tick + periode;
	comparer_struct->overflow = actual_HALtick.overflow;
	if (comparer_struct->tick < actual_HALtick.tick)
	{
		comparer_struct->overflow = TRUE;
	}
	return (1);
}
/**
 * input: number for delay in Second
 */
uint8_t fill_comparer_seconds(uint16_t periode, Compare_t *comparer_struct)
{
	get_actual_HAL_tick();
	comparer_struct->tick = periode * 1000 + actual_HALtick.tick;
	comparer_struct->overflow = actual_HALtick.overflow;
	if (comparer_struct->tick < actual_HALtick.tick)
	{
		comparer_struct->overflow = TRUE;
	}
	return (1);
}

/**
 * @brief  This function detect if the comparer value was reached by the HAL ticks
 * 
 * @param comparer_struct 
 * @return TRUE - time is end; FALSE - time is not elapsed 
 */
Bool comparer_timeout(Compare_t *comparer_struct)
{
	if (comparer_struct->tick <= actual_HALtick.tick)
	{
		if (comparer_struct->overflow == actual_HALtick.overflow)
		{
			return (TRUE);
		}
		else
			return (FALSE);
	}
	else
	{
		if (comparer_struct->overflow != actual_HALtick.overflow)
			return (TRUE);
	}
	return (FALSE);
}
/**
 * Tehle funkce vraci, zda ma v dany cas byt periferie - ON - TRUE nebo OFF - FALSE
 */
Bool timer_value(uint16_t timer_index)
{
	Bool timer_val = FALSE;
	///// DOPSAT NENI TO HOTOVE

	return timer_val;
}

/**
 * @brief Tato funkce zpocita cas do vyprseni daneho casu 
 * @note funkce nepocita s tim, ze by neco mohlo trvat vice jak 2^32 milisekund! Pokud to bude vetsi, tak je potreba upravit funkci!
 * @param comparer_struct 
 * @return uint16_t  zbyvajici cas v [s] ; 
 */
uint16_t end_of_timeout(Compare_t *comparer_struct)
{
	uint16_t timeToEnd = 0;
	if (comparer_struct->tick <= actual_HALtick.tick)
	{
		if (comparer_struct->overflow == actual_HALtick.overflow)
		{
			timeToEnd = 0;
		}
		else
		{ // pocita se s tim, ze casovac jednou pretekl a tudis je to obracene.
			timeToEnd = (comparer_struct->tick - actual_HALtick.tick) / 1000;
		}
	}
	else // jeste cas neubehl
	{
		if (comparer_struct->overflow != actual_HALtick.overflow)
			timeToEnd = 0;
		else
		{
			timeToEnd = (comparer_struct->tick - actual_HALtick.tick) / 1000;
		}
	}
	return (timeToEnd);
}

/**
 * @brief Inicializace automatickeho modu - nastavovaciho casovace
 * 			vymaze priznaky, ze je validni nastaveni casovace
 * 
 * @param actual_auto 
 */
void init_auto_mode(Mode_auto_s *actual_auto)
{
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
void sort_auto_mode(Mode_auto_s *actual_auto)
{
	uint16_t timeSorterMinute[AUTO_TIMERS];
	uint8_t buffer;
	for (uint8_t index = 0; index < AUTO_TIMERS; index++)
	{
		if (actual_auto->status[index].valid_timer)
			timeSorterMinute[index] = actual_auto->time_s[index].hours * 60 + actual_auto->time_s[index].minutes;
		else
			timeSorterMinute[index] = 65535; // toto oznacuje kdyz je vypnuty casovac.
	}
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