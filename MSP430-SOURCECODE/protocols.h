/***************************************************************************//**
  @file     protocols.c
  @brief    Capa con funciones para preparación de Data Frames.
  @author   Lucas Matias Neira
 ******************************************************************************/

#ifndef _PROTOCOLS_H_
#define _PROTOCOLS_H_

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#define DATAFRAMETA_CHARS       5

#define DATAFRAMETA_START       '('

#define DATAFRAMETA_STOP        ')'

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

/* DATA FRAME TYPE A: "(XY######)" */

void dataFrameTAPacking(unsigned char* data_input, unsigned char type, unsigned char code, unsigned char* data_output);

void dataFrameTAUnpacking(unsigned char* data_input, unsigned char* data_output);

/*******************************************************************************
 ******************************************************************************/

#endif // _PROTOCOLS_H_
