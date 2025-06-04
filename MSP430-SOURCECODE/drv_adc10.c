/***************************************************************************//**
  @file     drv_adc10.c
  @brief    Driver ADC10 para una sola entrada analógica.
  @author   Lucas Matias Neira
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/
#include "drv_adc10.h"

#include "board.h"

#include "timer.h"

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

static void pinSelect2Conv(uint8_t pin);

static void ADCsetup(uint8_t mode);

void pisrADC(void);

/*******************************************************************************
 * ROM CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/
static unsigned int adcval;

static ticks_t timRead;

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

/** INIT - DEFINICIÓN **/

void drvADC10Init(void)
{
    pinSelect2Conv(PIN_ANIN);

    ADCsetup(ADCMODE_VREF_EXT);

    adcval = 0;

    timRead = timerStart(READING_PERIOD);
}

unsigned int readADCvalue(void)
{
   if (timerExpired(timRead))
   {
      timRead = timerStart(READING_PERIOD);
      pisrADC();
   }

   return adcval;
}

/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

static void pinSelect2Conv(uint8_t pin)
{
    uint8_t num;

    num = PIN2NUM(pin);

    // Se habilita el pin elegido como analog input
    ADC10AE0 |= (1<<num);

    // Se habilita el canal del pin anterior para que pueda ser convertido
    switch (num)
    {
        case 0: ADC10CTL1 = INCH_0; break;
        case 1: ADC10CTL1 = INCH_1; break;
        case 2: ADC10CTL1 = INCH_2; break;
        case 3: ADC10CTL1 = INCH_3; break;
        case 4: ADC10CTL1 = INCH_4; break;
        case 5: ADC10CTL1 = INCH_5; break;
        case 6: ADC10CTL1 = INCH_6; break;
        case 7: ADC10CTL1 = INCH_7; break;
    }
}

static void ADCsetup(uint8_t mode)
{
    switch (mode)
    {
        case ADCMODE_VREF_EXT: ADC10CTL0 = SREF_0 + ADC10SHT_2 + ADC10ON; break;
        case ADCMODE_VREF_INT: ADC10CTL0 = SREF_1 + ADC10SHT_2 + ADC10ON + REF2_5V + REFON; break;
    }
}

/** PISR - DEFINICIÓN **/
void pisrADC(void)
{
    adcval = ADC10MEM; // Read adc value
    ADC10CTL0 |= ENC + ADC10SC;
}

/******************************************************************************/
