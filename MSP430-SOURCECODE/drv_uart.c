/***************************************************************************//**
  @file     drv_uart.c
  @brief    Driver para conunicación UART
  @author   Lucas Matias Neira
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "drv_uart.h"

#include "hardware.h"
#include "common.h"
#include "board.h"
#include "timer.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

// Macros para definición de pines
#define RX          (1<<(PIN2NUM(PINRX)))
#define TX          (1<<(PIN2NUM(PINTX)))

// Macros para Recepción de caracteres
#define CHAR_READ               1
#define FUNCTION_FOR_INTERRUPT  1

// Macros para Estado de comunicación
#define MAX_NUMER_COMM_TRYS     20

#define READING_PERIOD          200

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

typedef struct
{
    unsigned char character;
    unsigned char state_char;
} rx_single_data_t;

typedef struct
{
    void (*ptr2func)(unsigned char);
    unsigned char state;
} func_for_interrupt_t;

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

// Recepción

static rx_single_data_t rx_single_data;

static func_for_interrupt_t func_for_interrupt;

// Estado de comunicación

static int err_counter;

static ticks_t timRead;

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

/** INIT - DEFINICIÓN **/
void drvUARTInit(void)
{
    /* Configure Pin Muxing P1.1 RXD and P1.2 TXD */
    P1SEL = RX | TX ;
    P1SEL2 = RX | TX ;

    /* Place UCA0 in Reset to be configured */
    UCA0CTL1 = UCSWRST;

    /* Configure */
    UCA0CTL1 |= UCSSEL_2; // SMCLK

    UCA0BR0 = 65; // 8MHz 9600
    UCA0BR1 = 3; // 8MHz 9600

    UCA0MCTL = UCBRS_2; // Modulation UCBRSx = 2

    /* Take UCA0 out of reset */
    UCA0CTL1 &= ~UCSWRST;

    /* Enable USCI_A0 RX interrupt */
    IE2 |= UCA0RXIE;


    /** Variables internas **/
    rx_single_data.character = '\0';
    rx_single_data.state_char = CHAR_READ;

    func_for_interrupt.state = !FUNCTION_FOR_INTERRUPT;

    err_counter = MAX_NUMER_COMM_TRYS;

    /* Timers */

    timRead = timerStart(READING_PERIOD);
}

/* Transmisión de caracteres */

unsigned char transmitCharacterUART(unsigned char* character)
{
    if ( !( IFG2 & UCA0TXIFG) )
    {
        return !CHAR_TRANSMITTED;
    }
    else
    {
        UCA0TXBUF = *character;
        return CHAR_TRANSMITTED;
    }
}

/* Recepción de caracteres */

unsigned char recieveCharacterUART(unsigned char* reciever)
{
    if ( rx_single_data.state_char == !CHAR_READ )
    {
        *reciever = rx_single_data.character;
        rx_single_data.state_char = CHAR_READ;
        return CHAR_RECIEVED;
    }
    else
    {
        return !CHAR_RECIEVED;
    }
}

void RXInterruptUART(void (*ptr)(unsigned char))
{
    func_for_interrupt.ptr2func = ptr;
    func_for_interrupt.state = FUNCTION_FOR_INTERRUPT;
} 

/* Estado de conexión */

unsigned char readCommStatusUART(void)
{
    if (timerExpired(timRead))
    {
        timRead = timerStart(READING_PERIOD);
        
        while( !( IFG2 & UCA0TXIFG) )
        {
            UCA0TXBUF = COMM_SYMBOL;
        }

        err_counter--;
    }

    if ( err_counter < 0)
    {
        err_counter = -1;

        return COMM_ERROR;
    }
    else
    {
        return !COMM_ERROR;
    }
}

/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=USCIAB0RX_VECTOR
__interrupt void USCI0RX_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(USCIAB0RX_VECTOR))) USCI0RX_ISR (void)
#else
#error Compiler not supported!
#endif
{
    unsigned char rx_char = UCA0RXBUF;
    
    if ( rx_char == COMM_SYMBOL )
    {
        err_counter = MAX_NUMER_COMM_TRYS;
    }
    else
    {
        if ( rx_single_data.state_char == CHAR_READ )
        {
            rx_single_data.state_char = !CHAR_READ;
            rx_single_data.character = rx_char;   
        }

        if ( func_for_interrupt.state == FUNCTION_FOR_INTERRUPT )
        {
            func_for_interrupt.ptr2func(rx_char);
        }
    } 
}
/******************************************************************************/
