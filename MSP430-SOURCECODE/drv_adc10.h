/***************************************************************************//**
  @file     drv_adc10.h
  @brief    Driver ADC10 para una sola entrada analógica.
  @author   Lucas Matias Neira
 ******************************************************************************/

#ifndef _DRV_ADC10_H_
#define _DRV_ADC10_H_

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "hardware.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define ADCMODE_VREF_EXT    0
#define ADCMODE_VREF_INT    1

#define READING_PERIOD      150

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/


/*******************************************************************************
 * VARIABLE PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

void drvADC10Init(void);

unsigned int readADCvalue(void);

/*******************************************************************************
 ******************************************************************************/

#endif // _DRV_ADC10_H_
