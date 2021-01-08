
#ifndef MODULE_WAIT_H
#define MODULE_WAIT_H


/** Generic wait functions.
 *
 * Example:
 * @code
 * #include "wait.h"
 *
 *         wait_us(50); / waiting for 50us
 */
#include "tim.h"

 uint32_t SlaveCounter;

/** Waits a number of milliseconds.
 *
 *  @param ms the whole number of milliseconds to wait
 */
void wait_ms(const uint32_t ms);

/** Waits a number of microseconds.
 *
 *  @param us the whole number of microseconds to wait
 */
void wait_us(const uint32_t us);

void us_ticker_init(void);
uint32_t us_ticker_read(void);
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);
void prec_wait_us(const uint16_t us);

#endif
