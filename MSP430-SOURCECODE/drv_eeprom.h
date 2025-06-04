/***************************************************************************//**
  @file     drv_eeprom.h
  @brief    Driver para manejo de memoria EEPROM.
  @author   Lucas Matias Neira
 ******************************************************************************/

#ifndef _DRV_EEPROM_H_
#define _DRV_EEPROM_H_

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/


/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define EEPROM_ADDRESS          0x50

#define COMM_EEPROM_NORMAL      0

#define COMM_EEPROM_ERROR       1

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

void drvEEPROMInit(void);

/* Escritura y lectura de caracter a partir de una dirección de memoria del EEPROM */

void writeCharacterIntoMemAddress(unsigned char character, unsigned char MSB_value, unsigned char LSB_value);

void readCharacterFromMemAddress(unsigned char* reciever,  unsigned char MSB_value, unsigned char LSB_value);

/* Escritura y lectura en espacio en memoria de multiples caracteres para un tipo de dato dado */

void writeStringIntoMemAddress(unsigned char* str, unsigned char MSB_value, unsigned char LSB_value, unsigned char frame_len);

void readStringFromMemAddress(unsigned char* str, unsigned char MSB_value, unsigned char LSB_value, unsigned char frame_len);

/* Escritura y Lectura de datos de strings de forma no manual */

void writeDataIntoEEPROM(unsigned char data_type, void* data, unsigned char cell_position);

void readDataFromEEPROM(unsigned char data_type, void* reciever, unsigned char cell_position);

/* Estado de la conexión con el EEPROM */

unsigned char checkCommStatusEEPROM(void);

/*******************************************************************************
 ******************************************************************************/

#endif // _DRV_EEPROM_H_
