/***************************************************************************//**
  @file     drv_sw.h
  @brief    Driver para uso de switch
  @author   Lucas Matias Neira
 ******************************************************************************/

#ifndef _DRV_SW_H_
#define _DRV_SW_H_

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "common.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define     ON      1

#define     RP_ON   1

//Período Refractario: tiempo en milisegundos
#define     RP_PERIOD   200

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

/*******************************************************************************
 * VARIABLE PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/**
 * @brief Función de inicialización para el driver
 */
void drvSWInit(void);

/**
 * @brief Lee el estado actual del switch
 * @param pin The pin id whose mode you wish to set (according PORTNUM2PIN)
 */
uint8_t readSwitchState(int pin);

/**
 * @brief Ejecuta un única vez la función pasada por puntero al activarse el switch
 * @param pin The pin id whose mode you wish to set (according PORTNUM2PIN)
 * @param ptr Puntero a función que tiene parámetro void y devuelve un void
 */
void executeOnSwitchChange(int pin, void (*ptr)(void));


/*******************************************************************************
 ******************************************************************************/

#endif // _DRV_SW_H_
