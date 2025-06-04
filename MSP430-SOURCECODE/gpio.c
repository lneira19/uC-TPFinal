/***************************************************************************//**
  @file     gpio.c
  @brief    Simple GPIO Pin services, similar to Arduino
  @author   GRUPO3
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "gpio.h"

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

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

void gpioMode (gpio_t pin, uint8_t mode)
{
    uint8_t port, num;

    port = PIN2PORT(pin);
    num = PIN2NUM(pin);

    switch (port)
    {
    case 1:
        if (mode == OUTPUT)
        {
            BITSET(P1DIR, num); // OUTPUT
        }
        else
        {
            BITCLR(P1DIR, num); // INPUT
            if (mode == INPUT)
            {
                BITCLR(P1REN, num); // pull disable
            }
            else
            {
                BITSET(P1REN, num); // pull enable
                if (mode == INPUT_PULLDOWN)
                {
                    BITCLR(P1OUT, num); // pull-down
                }
                else
                {
                    BITSET(P1OUT, num); // pull-up
                }
            }
        }
        break;

    case 2:
        if (mode == OUTPUT)
        {
            BITSET(P2DIR, num); // OUTPUT
        }
        else
        {
            BITCLR(P2DIR, num); // INPUT
            if (mode == INPUT)
            {
                BITCLR(P2REN, num); // pull disable
            }
            else
            {
                BITSET(P2REN, num); // pull enable
                if (mode == INPUT_PULLDOWN)
                {
                    BITCLR(P2OUT, num); // pull-down
                }
                else
                {
                    BITSET(P2OUT, num); // pull-up
                }
            }
        }
        break;
    }
}


void gpioWrite (gpio_t pin, uint8_t value)
{
    uint8_t port, num;

    port = PIN2PORT(pin);
    num = PIN2NUM(pin);

    switch (port) // set or clear bit from proper PxOUT
    {
    case 1:
        if (value)
            BITSET(P1OUT, num);
        else
            BITCLR(P1OUT, num);
        break;

    case 2:
        if (value)
            BITSET(P2OUT, num);
        else
            BITCLR(P2OUT, num);
        break;
    }
}


void gpioToggle (gpio_t pin)
{
    uint8_t port, num;

    port = PIN2PORT(pin);
    num = PIN2NUM(pin);

    switch (port) // toggle bit from proper PxOUT
    {
    case 1:
        BITTGL(P1OUT, num);
        break;

    case 2:
        BITTGL(P2OUT, num);
        break;
    }
}


uint8_t gpioRead (gpio_t pin)
{
    uint8_t port, num, value;

    port = PIN2PORT(pin);
    num = PIN2NUM(pin);

    switch (port) // check bit from proper PxIN
    {
    case 1:
        value = BITCHK(P1IN, num);
        break;

    case 2:
        value = BITCHK(P2IN, num);
        break;

    default:
        value = LOW;
        break;
    }

    return value;
}


/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/


/******************************************************************************/

