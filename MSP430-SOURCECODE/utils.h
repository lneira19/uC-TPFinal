/***************************************************************************//**
  @file     utils.h
  @brief    Compilation of util functions.
  @author   Lucas Matias Neira
 ******************************************************************************/

#ifndef _UTILS_H_
#define _UTILS_H_

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "common.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/



/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/



/*******************************************************************************
 * VARIABLE PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/* Tipos de dato númerico a string */

void intToString(int num, char *str);

void floatToString(float num, char *str, int precision);

/* String a tipo de dato numérico */

float stringToFloat(char *str);

/* Funciones de conversión automática */

void conversionDataTypeToString(unsigned char data_type, unsigned char* reciever, void* data);

void conversionStringToDataType(unsigned char data_type, void* reciever, unsigned char* data);

void conversionCharToBinaryArray(unsigned char input, unsigned char* output);

/*******************************************************************************
 ******************************************************************************/

#endif // _UTILS_H_
