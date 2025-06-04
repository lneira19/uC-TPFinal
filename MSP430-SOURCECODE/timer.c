/***************************************************************************//**
  @file     timer.c
  @brief    Timer driver. Simple implementation, support multiple timers
  @author   GRUPO3
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "timer.h"

#include "hardware.h"


/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

/*******************************************************************************
 * VARIABLES WITH GLOBAL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * ROM CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

static volatile ticks_t timer_main_counter;


/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

void timerInit(void)
{
    static char yaInit = 0;
    if (yaInit)
        return;
   
    WDTCTL= WDT_MDLY_8; // Interval timer mode ; according interval selection
    IE1 |=  WDTIE; // Enable the WDTIE bit
    
    yaInit = 1;
}


ticks_t timerStart(ticks_t ticks)
{
    ticks_t now_copy;
    
    if (ticks < 0)
        ticks = 0; // truncate min wait time
    
    disable_interrupts();
    now_copy = timer_main_counter; // esta copia debe ser atomic!!
    enable_interrupts();

    now_copy += ticks;

    return now_copy;
}


char timerExpired(ticks_t timeout)
{
    ticks_t now_copy;

    disable_interrupts();
    now_copy = timer_main_counter; // esta copia debe ser atomic!!
    enable_interrupts();

    now_copy -= timeout;
    return (now_copy >= 0);
}


void timerDelay(ticks_t ticks)
{
    ticks_t tim;
    
    tim = timerStart(ticks);
    while (!timerExpired(tim))
    {
        // wait...
    }
}


/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

#pragma vector = WDT_VECTOR             //Interval timer vector location
__interrupt void timer_isr(void)
{
    ++timer_main_counter; // update main counter
}


/******************************************************************************/
