/***************************************************************************//**
  @file     drv_serial_comm.h
  @brief    Implementación UART + Data Frame para comunicación UART.
  @author   Lucas Matias Neira
 ******************************************************************************/

#ifndef _DRV_SERIAL_COMM_H_
#define _DRV_SERIAL_COMM_H_

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

//#include "hardware.h"
//#include "common.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define LEN_BUFFER      25

#define COMM_SERIAL_NORMAL      0

#define COMM_SERIAL_ERROR       1

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/


/*******************************************************************************
 * VARIABLE PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

void drvSerialCommInit(void);

/**
 * @brief Transmite información por Serial a través de comunicación Serial UART utilizando un protocolo
 * @param data_type Tipo de dato del dato a transmitir
 * @param data Dirección de la variable que almacena el dato a transmitir
 * @param code Código asociado al dato para su consecuente decodificación
 */
void transmitDataSerialComm(unsigned char data_type, void* data, unsigned char code);

/**
 * @brief Recibe información por Serial a través de comunicación UART utilizando un protocolo
 * @param data_type Tipo de dato del dato a recibir, necesario para su correcta decodificación
 * @param reciever Dirección de la variable que almacenará el dato ya decodificado
 * @param code Código asociado al dato esperado para que efectivamente se almacene la información en 'reciever'
 */
void recieveDataSerialComm(unsigned char data_type, void* reciever, unsigned char code);


unsigned char checkCommStatusSerial(void);
/*******************************************************************************
 ******************************************************************************/

#endif // _DRV_SERIAL_COMM_H_
