#include <waiter.h>
#include "tim.h"
#include "pinmap.h"

// Timer selection
#define TIM_MST TIM21

static int us_ticker_inited = 0;
//volatile uint32_t SlaveCounter = 0; //counter of the over running the timer

// wait function
void wait_ms(const uint32_t ms) {
    wait_us(ms * 1000);
}

void wait_us(const uint32_t us) {
	//if(us_ticker_inited) __HAL_RCC_TIM21_FORCE_RESET(); //reset TIMERU TIM21 to zero
    uint32_t start = us_ticker_read();
    while ((us_ticker_read() - start) < (uint32_t)us)  	{;}
}

void us_ticker_init(void)
{
    if (us_ticker_inited) return;
    us_ticker_inited = 1;
    SlaveCounter = 0;
    HAL_TIM_Base_Start_IT(&htim21); //enable the timer and interupt;
        //htim21.Instance->CR1+=1;	// bit enable for the clock
//    HAL_InitTick(0); // The passed value is not used
}

/*uint32_t us_ticker_read(void){
	uint32_t counter2;
	if (!us_ticker_inited) us_ticker_init();
	counter2 = (uint32_t)(SlaveCounter << 16);
	counter2 += TIM_MST->CNT;
	testing1 = counter2;
	return counter2;
}*/

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  SlaveCounter++;
}


uint32_t us_ticker_read(void)
{
    uint32_t counter, counter2;
    if (!us_ticker_inited) us_ticker_init();
    // A situation might appear when Master overflows right after Slave is read and before the
    // new (overflowed) value of Master is read. Which would make the code below consider the
    // previous (incorrect) value of Slave and the new value of Master, which would return a
    // value in the past. Avoid this by computing consecutive values of the timer until they
    // are properly ordered.
    counter = (uint32_t)(SlaveCounter << 16);
    counter += TIM_MST->CNT;
    while (1) {
        counter2 = (uint32_t)(SlaveCounter << 16);
        counter2 += TIM_MST->CNT;
        if (counter2 > counter) {
            break;
        }
        counter = counter2;
    }
    return counter2;
}

void prec_wait_us(const uint16_t us){
	uint16_t start = TIM_MST->CNT;
	if (!us_ticker_inited) us_ticker_init();
	while ((uint16_t)((TIM_MST->CNT)-start) < us)  	{;}
}
