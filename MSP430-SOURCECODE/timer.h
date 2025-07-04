/***************************************************************************//**
  @file     timer.h
  @brief    Timer driver. Simple implementation, support multiple timers.
  @author   GRUPO3
 ******************************************************************************/

#ifndef _TIMER_H_
#define _TIMER_H_

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "common.h"


/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define TIMER_TICK_MS       1
#define TIMER_MS2TICKS(ms)  ((ms)/TIMER_TICK_MS)


/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

typedef int16_t ticks_t;


/*******************************************************************************
 * VARIABLE PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/**
 * @brief Initialice timer and corresponding peripheral
 */
void timerInit(void);


// Non-Blocking services ////////////////////////////////////////////////

/**
 * @brief Begin to run a new timer
 * @param ticks time until timer expires, in ticks
 * @return Timeout value
 */
ticks_t timerStart(ticks_t ticks);

/**
 * @brief Verify if a timer has run timeout
 * @param timeout timeout to check for expiration
 * @return 1 = timer expired!
 */
char timerExpired(ticks_t timeout);


// Blocking services ////////////////////////////////////////////////

/**
 * @brief Wait the specified time. Valid for delays smaller than 32 seconds
 * @param ticks time to wait in ticks
 */
void timerDelay(ticks_t ticks);


/*******************************************************************************
 ******************************************************************************/

#endif // _TIMER_H_
