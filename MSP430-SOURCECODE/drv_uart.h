/***************************************************************************//**
  @file     drv_uart.h
  @brief    Driver para conunicación UART
  @author   Lucas Matias Neira
 ******************************************************************************/

#ifndef _DRV_UART_H_
#define _DRV_UART_H_

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

// Macros para Transmisión de caracteres
#define CHAR_TRANSMITTED       1

// Macros para Recepción de caracteres
#define CHAR_RECIEVED          1

// Macros para Estado de comunicación
#define COMM_SYMBOL            '!'

#define COMM_ERROR              1

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/


/*******************************************************************************
 * VARIABLE PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

void drvUARTInit(void);

unsigned char transmitCharacterUART(unsigned char* character);

unsigned char recieveCharacterUART(unsigned char* reciever);

void RXInterruptUART(void (*ptr)(unsigned char));

unsigned char readCommStatusUART(void);

/*******************************************************************************
 ******************************************************************************/

#endif // _DRV_UART_H_
