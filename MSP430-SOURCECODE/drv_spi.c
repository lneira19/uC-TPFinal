/***************************************************************************//**
  @file     drv_spi.c
  @brief    Driver SPI.
  @author   GRUPO3
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "drv_spi.h"

#include "timer.h"
#include "gpio.h"
#include "board.h"
#include "utils.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

// Macros para SPI

#define CLK_PERIOD        2

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
static ticks_t timer_clk_spi;

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

/* Inicialización */
void drvSPIInit(void)
{
    // Configuración de pines de SPI
    gpioMode(PIN_CLK, OUTPUT);
    gpioMode(PIN_MOSI, OUTPUT);
    gpioMode(PIN_LATCH, OUTPUT);
    
    gpioWrite(PIN_CLK, LOW);
    gpioWrite(PIN_MOSI, LOW);
    gpioWrite(PIN_LATCH, LOW);

    timer_clk_spi = timerStart(CLK_PERIOD);
}

/* Transmisión de un byte  */

void sendByteSPI(unsigned char byte_data)
{
  unsigned char expression[8];

  conversionCharToBinaryArray(byte_data, expression);

  unsigned char index = 0;
  
  gpioWrite(PIN_LATCH, HIGH);
  gpioWrite(PIN_CLK, LOW);

  while ( index < 8 )
  {
    if ( timerExpired(timer_clk_spi) )
    {
      timer_clk_spi = timerStart(CLK_PERIOD);

      if ( gpioRead(PIN_CLK) == LOW )
      {
        // Acá va la secuencia de bits
        gpioWrite(PIN_MOSI, expression[index]);
        index++;
      }

      gpioToggle(PIN_CLK);
    }
  }
  gpioWrite(PIN_LATCH, LOW);
}

/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/


/******************************************************************************/
