/***************************************************************************//**
  @file     board.h
  @brief    Board management
  @author   GRUPO3
 ******************************************************************************/

#ifndef _BOARD_H_
#define _BOARD_H_

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "gpio.h"


/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

// On Board User LEDs
#define PIN_LED_RED         PORTNUM2PIN(1,0) // P1.0

// Analog Input Pin
#define PIN_ANIN            PORTNUM2PIN(1,4) // P1.4

// Pines para comunicación UART
#define PINRX               PORTNUM2PIN(1,1) // P1.1
#define PINTX               PORTNUM2PIN(1,2) // P1.2

#define LED_ACTIVE          HIGH

// Pin para PWM
#define PIN_PWM             PORTNUM2PIN(2,6) // P2.6

// Pines para I2C
#define PIN_SCL             PORTNUM2PIN(1,6) // P1.6
#define PIN_SDA             PORTNUM2PIN(1,7) // P1.7

// Pines para SPI
#define PIN_CLK             PORTNUM2PIN(2,3) // P2.3
#define PIN_MOSI            PORTNUM2PIN(2,4) // P2.4
#define PIN_LATCH           PORTNUM2PIN(2,5) // P2.5

// Pines para LEDs de Estados de comunicación
#define PIN_LED_SERIAL_STATUS   PORTNUM2PIN(2,1) // P2.1
#define PIN_LED_EEPROM_STATUS   PORTNUM2PIN(2,2) // P2.2

// Pin para sensor de Temperatura DS18B20
#define PIN_TEMPSENSOR      PORTNUM2PIN(1,5) // P1.5

// On Board User Switches
#define PIN_SWITCH_S2       PORTNUM2PIN(1,3) // P1.3

#define SWITCH_INPUT        INPUT_PULLUP
#define SWITCH_ACTIVE       LOW


/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

/*******************************************************************************
 * VARIABLE PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/



/*******************************************************************************
 ******************************************************************************/

#endif // _BOARD_H_
