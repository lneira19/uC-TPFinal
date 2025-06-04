/***************************************************************************//**
  @file     drv_i2c.h
  @brief    Driver con elementos básicos de I2C.
  @author   Lucas Matias Neira
 ******************************************************************************/

#ifndef _DRV_I2C_H_
#define _DRV_I2C_H_

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/


/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define COMM_ERROR  1

#define ERROR_SYMBOL '#'

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

/*******************************************************************************
 * VARIABLE PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/* Inicialización */

void drvI2CInit(unsigned char slave_address);

/* Funciones para comienzo y final de comunicación */

void beginTransmissionI2C(void);

void beginReceptionI2C(void);

void sendNACKI2C(void);

void endTransactionI2C(void);

/* Funciones para transmisión y recepción de bytes */

void transmitCharacterI2C(unsigned char character);

unsigned char recieveCharacterI2C(void);

/* Lectura del estado de la conexión */

unsigned char readCommStatus(void);

/*******************************************************************************
 ******************************************************************************/

#endif // _DRV_I2C_H_
