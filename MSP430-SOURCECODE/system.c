/***************************************************************************//**
  @file     system.c
  @brief    MCU system driver
  @author   GRUPO3
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "system.h"

#include "board.h"
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

void systemInitFirst(void)
{
    WDTCTL = WDTPW | WDTHOLD; // stop watchdog timer

    // Configure DCO
    DCOCTL = 0;
#if DCOCLK_FREQ_HZ == 1000000UL
    BCSCTL1 = CALBC1_1MHZ;
    DCOCTL = CALDCO_1MHZ;
#elif DCOCLK_FREQ_HZ == 8000000UL
    BCSCTL1 = CALBC1_8MHZ;
    DCOCTL = CALDCO_8MHZ;
#elif DCOCLK_FREQ_HZ == 12000000UL
    BCSCTL1 = CALBC1_12MHZ;
    DCOCTL = CALDCO_12MHZ;
#elif DCOCLK_FREQ_HZ == 16000000UL
    BCSCTL1 = CALBC1_16MHZ;
    DCOCTL = CALDCO_16MHZ;
#else
#error Mal definido el clock del sistema
#endif

    // Configure MCLK and SMCLK
    BCSCTL2 = SELM_0 | (MCLK_PRESCALER_2POW<<4) | (SMCLK_PRESCALER_2POW<<4);
}


void boardInit(void)
{
    gpioWrite(PIN_LED_RED, !LED_ACTIVE); // configure LEDs
    gpioMode(PIN_LED_RED, OUTPUT);
    gpioMode(PIN_SWITCH_S2, SWITCH_INPUT); // configure switches

    // TODO: COMPLETAR CON LA CONFIGURACIÓN DEL RESTO DE LOS PINES UTILIZADOS

    // Configuración de pines de LEDs de estado de comunicación
    gpioWrite(PIN_LED_SERIAL_STATUS, !LED_ACTIVE);
    gpioWrite(PIN_LED_EEPROM_STATUS, !LED_ACTIVE);

    gpioMode(PIN_LED_SERIAL_STATUS, OUTPUT);
    gpioMode(PIN_LED_EEPROM_STATUS, OUTPUT);

    // Configuración de pin de PWM
    gpioMode(PIN_PWM, OUTPUT);
}


void systemInitLast(void)
{
    enable_interrupts();  // Enable General interrupts
}



/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/



/******************************************************************************/

