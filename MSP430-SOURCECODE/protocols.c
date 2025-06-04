/***************************************************************************//**
  @file     protocols.c
  @brief    Capa con funciones para preparación de Data Frames.
  @author   Lucas Matias Neira
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "protocols.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

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


/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

/* DATA FRAME TYPE A: "(XY######)" */
/* 
  ( -> APERTURA 
  X -> TIPO DE DATO (I,F,C,S)
  Y -> CÓDIGO CARACTER
  # -> DATA
  ) -> CIERRE
*/

/**
 * @brief Coloca raw_data en el data frame de comunicación para transmisión
 * @param data_input Data a empaquetar
 * @param type Tipo de dato de la variable que almacena el dato a transmitir
 * @param code Código asociado al dato para su consecuente decodificación
 * @param data_output Recipiente del nuevo string generado
 */
void dataFrameTAPacking(unsigned char* data_input, unsigned char type, unsigned char code, unsigned char* data_output)
{
    unsigned char i = 3;

    data_output[0] = '(';
    data_output[1] = type;
    data_output[2] = code;
    
    while (*data_input != '\0')
    {
      data_output[i++] = *data_input++;
    }

    data_output[i++] = ')';
    data_output[i++] = '\n';
    data_output[i] = '\0';
}

/**
 * @brief Se obtiene raw_data del data frame de comunicación recibido
 * @param data_input Data a desempaquetar
 * @param data_output Recipiente del nuevo string generado
 */
void dataFrameTAUnpacking(unsigned char* data_input, unsigned char* data_output)
{
    unsigned char i = 0;

    while(data_input[i+3] != '\0')
    {
        data_output[i] = data_input[i+3];
        i++;
    }
    
    data_output[i] = '\0';
}

/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/


/******************************************************************************/
