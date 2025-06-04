/***************************************************************************//**
  @file     utils.c
  @brief    Compilation of util functions.
  @author   Lucas Matias Neira
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "utils.h"

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

/* Tipos de dato númerico a string */

// Convierte un número entero a string
void intToString(int num, char *str) 
{
    int isNegative = 0;
    int i = 0;

    // Manejar números negativos
    if (num < 0) {
        isNegative = 1;
        num = -num;
    }

    // Extraer dígitos del número
    do {
        str[i++] = (num % 10) + '0'; // Obtener el dígito menos significativo
        num /= 10;                  // Reducir el número
    } while (num > 0);

    // Agregar el signo negativo si corresponde
    if (isNegative) {
        str[i++] = '-';
    }

    // Invertir el string para obtener el orden correcto
    str[i] = '\0'; // Agregar el terminador nulo
    
    int j;
    int k;

    for (j = 0, k = i - 1; j < k; j++, k--)
    {
        unsigned char temp = str[j];
        str[j] = str[k];
        str[k] = temp;
    }
}


// Convierte un número flotante a string
void floatToString(float num, char *str, int precision) 
{
    int integerPart = (int)num; // Parte entera
    float fractionalPart = num - integerPart; // Parte fraccionaria
    if (fractionalPart < 0) {
        fractionalPart = -fractionalPart;
    }

    // Convertir la parte entera
    intToString(integerPart, str);

    // Agregar el punto decimal
    int i = 0;
    while (str[i] != '\0') {
        i++;
    }
    str[i++] = '.';

    // Convertir la parte fraccionaria
    int j;
    for (j = 0; j < precision; j++) {
        fractionalPart *= 10;
        int digit = (int)fractionalPart;
        str[i++] = digit + '0';
        fractionalPart -= digit;
    }

    str[i] = '\0'; // Agregar el terminador nulo
}

/* String a tipo de dato numérico */

float stringToFloat(char *str)
{
    float result = 0.0f; // Almacena el valor final
    float sign = 1.0f;   // Maneja números negativos
    float fraction = 0.1f; // Factor para manejar la parte fraccionaria
    int isFraction = 0;  // Indica si estás en la parte fraccionaria del número

    // Ignorar espacios iniciales manualmente
    while (*str == ' ' || *str == '\t' || *str == '\n' || *str == '\r') {
        str++;
    }

    // Manejar el signo del número
    if (*str == '-') {
        sign = -1.0f;
        str++;
    } else if (*str == '+') {
        str++;
    }

    // Procesar cada carácter del string
    while (*str) {
        if (*str >= '0' && *str <= '9') {
            if (isFraction) {
                // Si estamos en la parte fraccionaria
                result += (*str - '0') * fraction;
                fraction /= 10.0f;
            } else {
                // Si estamos en la parte entera
                result = result * 10.0f + (*str - '0');
            }
        } else if (*str == '.' && !isFraction) {
            // Encontramos el punto decimal
            isFraction = 1;
        } else {
            // Caracter inválido, detener el procesamiento
            break;
        }
        str++;
    }

    return result * sign;
}

/* Funciones de conversión automática */

void conversionDataTypeToString(unsigned char data_type, unsigned char* reciever, void* data)
{   
    if (data_type == 'I')
    {
        // (int*) se castea a un dato tipo int
        // *(int*) obtiene el valor de data utilizando desreferenciación
        int int_data = *(int*)data;

        intToString(int_data, (char*)reciever);
    }
    else if (data_type == 'F')
    {
        // (float*) se castea a un dato tipo float
        // *(float*) obtiene el valor de data utilizando desreferenciación
        float float_data = *(float*)data;
        
        floatToString(float_data, (char*)reciever, 3); 
    }
    else if (data_type == 'C')
    {
        // (unsigned char*) se castea a un dato tipo char
        // *(unsigned char*) obtiene el valor de data utilizando desreferenciación
        unsigned char char_data = *(unsigned char*)data;

        reciever[0] = char_data;
        reciever[1] = '\0';
    }
    else if (data_type == 'S')
    {
        // (unsigned char*) se castea a un dato tipo unsigned char
        unsigned char *str_data = (unsigned char*)data;

        unsigned char i = 0;

        while(*str_data != '\0')
        {
            reciever[i++] = *str_data++;
        }
        
        reciever[i] = '\0';
    }
}

void conversionStringToDataType(unsigned char data_type, void* reciever, unsigned char* data)
{
  
    if (data_type == 'I')
    {
        int valueInt = atoi((char*) data);
        // Se castea el void* a int* y luego se le adjudica valueInt
        *(int*) reciever = valueInt;
    }
    else if (data_type == 'F')
    {
        float valueFloat = stringToFloat((char*) data);
        // Se castea el void* a float* y luego se le adjudica valueFloat
        *(float*) reciever = valueFloat;
    }
    else if (data_type == 'C')
    {
        unsigned char valueChar = data[0];
        // Se castea el void* a char* y luego se le adjudica valueChar
        *(unsigned char*) reciever = valueChar;
    }
    else if (data_type == 'S')
    {
        unsigned char i = 0;

        unsigned char* valueStr = (unsigned char*) reciever;

        while(data[i] != ')')
        {
            valueStr[i] = data[i];
            i++;
        }
        
        valueStr[i] = '\0';
    }
}

void conversionCharToBinaryArray(unsigned char input, unsigned char* output)
{
  unsigned char index;

  for (index = 0; index < 8; index++)
  {
    output[7-index] = input & 1; // Extrae el bit menos significativo
    input >>= 1;           // Desplaza a la derecha para procesar el siguiente bit
  }
}


/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/


/******************************************************************************/
