/***************************************************************************//**
  @file     drv_i2c.c
  @brief    Driver con elementos básicos de I2C.
  @author   Lucas Matias Neira
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "drv_i2c.h"

#include "hardware.h"
#include "board.h"
#include "gpio.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

// Macros para definición de pines
#define SCL          (1<<(PIN2NUM(PIN_SCL)))
#define SDA          (1<<(PIN2NUM(PIN_SDA)))

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

/*******************************************************************************
 * VARIABLES WITH GLOBAL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

void commStatusChecker(void);

/*******************************************************************************
 * ROM CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

static unsigned char comm_status;

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

/* Inicialización de I2C */
void drvI2CInit(unsigned char slave_address)
{
  // Se coloca el USCI en modo reset y stand-by
  UCB0CTL1 = UCSWRST;

  // Configuración de puertos
  P1SEL |= (SCL | SDA);
  P1SEL2 |= (SCL | SDA);

  // Configuración del modo de operación del MSP (Máster)
  UCB0CTL0 = UCSYNC; // Modo síncrono
  UCB0CTL0 |= UCMODE_3; // USCI en modo I2C
  UCB0CTL0 |= UCMST; // Modo Máster
  UCB0CTL0 &= ~UCSLA10; // Slave address de 7 bits 

  // Configuración de la fuente de clock
  UCB0CTL1 |= UCSSEL_2; // SMCLK
  UCB0BR0 = 80;
  UCB0BR1 = 0;

  // Configuración del Address del esclavo externo
  UCB0I2CSA |= slave_address;

  // Configuración de interrupciones
  UCB0I2CIE |= UCNACKIE;
  UCB0I2CIE |= UCSTPIE;
  UCB0I2CIE |= UCSTTIE;

  // Salida del estado de reset y stand-by
  UCB0CTL1 &= ~UCSWRST;


  /* Variable interna de control de estado de conexión */

  comm_status = COMM_ERROR;
}

/* Funciones para comienzo y final de comunicación */

void beginTransmissionI2C(void)
{
    commStatusChecker();

    if (comm_status != COMM_ERROR)
    {
        while (UCB0CTL1 & UCTXSTP);     // Esperar si hay una condición de stop pendiente
        UCB0CTL1 |= UCTR + UCTXSTT;     // Modo transmisor, enviar start
    }
}

void beginReceptionI2C(void)
{
    //commStatusChecker();

    if (comm_status != COMM_ERROR)
    {
        while (!(IFG2 & UCB0TXIFG));    // Esperar buffer listo
        UCB0CTL1 &= ~UCTR;              // Modo receptor
        UCB0CTL1 |= UCTXSTT;            // Enviar start para recepción
        while (UCB0CTL1 & UCTXSTT);     // Esperar a que se complete el start
    }
}

void sendNACKI2C(void)
{
    UCB0CTL1 |= UCTXNACK;
}

void endTransactionI2C(void)
{
    if (comm_status != COMM_ERROR)
    {
        while (!(IFG2 & UCB0TXIFG));
        UCB0CTL1 |= UCTXSTP;            // Enviar condición de stop
        while (UCB0CTL1 & UCTXSTP);     // Esperar a que se complete el stop
    }
}

/* Funciones para transmisión y recepción de bytes */

void transmitCharacterI2C(unsigned char character)
{
    if (comm_status == !COMM_ERROR)
    {
        while (!(IFG2 & UCB0TXIFG));    // Esperar buffer listo
        UCB0TXBUF = character;
    }
}

unsigned char recieveCharacterI2C(void)
{
    if (comm_status == !COMM_ERROR)
    {
        unsigned char character;

        while (!(IFG2 & UCB0RXIFG));    // Esperar dato recibido
        character = UCB0RXBUF;          // Leer dato recibido

        return character;
    }
    else
    {
        return ERROR_SYMBOL; // Caracter de error
    }
}

unsigned char readCommStatus(void)
{
    commStatusChecker();

    return comm_status;
}

/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

void commStatusChecker(void)
{   
    unsigned char status;

    UCB0CTL1 |= UCSWRST;

    P1SEL &= ~(SCL | SDA);             
    P1SEL2 &= ~(SCL | SDA);

    gpioMode(PIN_SCL, INPUT_PULLDOWN);
    gpioMode(PIN_SDA, INPUT_PULLDOWN);

    if ( gpioRead(PIN_SCL) == HIGH && gpioRead(PIN_SDA) == HIGH )
    {
        status = !COMM_ERROR;
    }
    else
    {
        status = COMM_ERROR;
        UCB0CTL1 |= UCTXSTP;
    }
    
    gpioMode(PIN_SCL, INPUT);
    gpioMode(PIN_SDA, INPUT);

    P1SEL |= (SCL | SDA);             
    P1SEL2 |= (SCL | SDA);

    comm_status = status;

    UCB0CTL1 &= ~UCSWRST;
}

/******************************************************************************/
