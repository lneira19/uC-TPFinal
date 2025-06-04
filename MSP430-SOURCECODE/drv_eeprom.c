/***************************************************************************//**
  @file     drv_eeprom.c
  @brief    Driver para manejo de memoria EEPROM.
  @author   Lucas Matias Neira
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "drv_eeprom.h"

#include "drv_i2c.h"
#include "utils.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

// Tamaño de celda máximo
#define MAX_DATA_FRAME_LEN          16

// Tamaño de espacio en memoria para cada tipo de dato (en cantidad de bytes ocupados)
#define FRAME_LEN_INT               8

#define FRAME_LEN_FLOAT             16

#define FRAME_LEN_CHAR              2

#define FRAME_LEN_STR               16

// MSBs de la dirección de memoria para cada tipo de dato
#define ADDRESS_MSB_INT             0x00

#define ADDRESS_MSB_FLOAT           0x01

#define ADDRESS_MSB_CHAR            0x02

#define ADDRESS_MSB_STR             0x03

// Seguridad de conexión con la EEPROM
#define COMM_SYMBOL            '!'

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

typedef struct
{
    unsigned char data[MAX_DATA_FRAME_LEN];
} tx_data_t;

typedef struct
{
    unsigned char data[MAX_DATA_FRAME_LEN];
} rx_data_t;

/*******************************************************************************
 * VARIABLES WITH GLOBAL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

/* Manejo de elementos de memoria */

static unsigned char getMSBValueAddress(unsigned char data_type);

static unsigned char getLSBValueAddress(unsigned char data_type, unsigned char cell_position);

static unsigned char getDataFrameLen(unsigned char data_type);

/*******************************************************************************
 * ROM CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/
// Transmisión
static tx_data_t tx_data;

static rx_data_t rx_data;

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

/* Inicialización del driver */
void drvEEPROMInit(void)
{
    drvI2CInit(EEPROM_ADDRESS);

    // Transmisión de data
    tx_data.data[0] = '\0';

    // Recepción de data
    rx_data.data[0] = '\0';
}

/* Escritura y Lectura de datos de strings de forma no manual */

/**
 * @brief Escribe en el EEPROM la data proporcionada de tipo INT, FLOAT, CHAR o STR sin nececesidad de aclarar la dirección en memoria
 * @param data_type Tipo de dato del dato a escribir
 * @param data Dirección de la variable que almacena el dato a escribir
 * @param cell_position Número de espacio en memoria asignado al tipo de variable que se desea ocupar
 */
void writeDataIntoEEPROM(unsigned char data_type, void* data, unsigned char cell_position)
{
    conversionDataTypeToString(data_type, tx_data.data, data);

    unsigned char MSB_value = getMSBValueAddress(data_type);

    unsigned char LSB_value = getLSBValueAddress(data_type, cell_position);

    unsigned char frame_len = getDataFrameLen(data_type);

    writeStringIntoMemAddress(tx_data.data, MSB_value, LSB_value, frame_len);
}

/**
 * @brief Lee en el EEPROM la data de tipo INT, FLOAT, CHAR o STR sin nececesidad de aclarar la dirección en memoria
 * @param data_type Tipo de dato del dato a leer
 * @param reciever Receptor del dato a leer
 * @param cell_position Número de espacio en memoria asignado al tipo de variable que se desea ocupar
 */
void readDataFromEEPROM(unsigned char data_type, void* reciever, unsigned char cell_position)
{
    unsigned char MSB_value = getMSBValueAddress(data_type);

    unsigned char LSB_value = getLSBValueAddress(data_type, cell_position);

    unsigned char frame_len = getDataFrameLen(data_type);

    readStringFromMemAddress(rx_data.data, MSB_value, LSB_value, frame_len);

    conversionStringToDataType(data_type, reciever, rx_data.data);
}

/* Escritura y lectura de caracter a partir de una dirección de memoria del EEPROM */

void writeCharacterIntoMemAddress(unsigned char character, unsigned char MSB_value, unsigned char LSB_value)
{
    unsigned char comm_status = readCommStatus();

    if (comm_status != COMM_ERROR)
    {
        beginTransmissionI2C();

        transmitCharacterI2C(MSB_value);        //MSB

        transmitCharacterI2C(LSB_value);        //LSB

        transmitCharacterI2C(character);        //Data

        endTransactionI2C();
    }
}

void readCharacterFromMemAddress(unsigned char* reciever,  unsigned char MSB_value, unsigned char LSB_value)
{
    unsigned char comm_status = readCommStatus();

    if (comm_status != COMM_ERROR)
    {
        unsigned char data;

        beginTransmissionI2C();

        transmitCharacterI2C(MSB_value);        //MSB

        transmitCharacterI2C(LSB_value);        //LSB

        beginReceptionI2C();

        // Por una razón que todavía no comprendo, la condición de stop
        // debe ser enviada antes de leer el caracter
        endTransactionI2C();                    

        data = recieveCharacterI2C();    

        *reciever = data;
    }
}

/* Escritura y lectura de strings de manera manual */

// Escritura

void writeStringIntoMemAddress(unsigned char* str, unsigned char MSB_value, unsigned char LSB_value, unsigned char frame_len)
{
    unsigned char comm_status = readCommStatus();

    if (comm_status != COMM_ERROR)
    {
        beginTransmissionI2C();

        transmitCharacterI2C(MSB_value);        //MSB

        transmitCharacterI2C(LSB_value);        //LSB

        unsigned char index_data = 0;

        while(str[index_data] != '\0')
        {
            transmitCharacterI2C(str[index_data]);
            index_data++;
        }

        unsigned char index_padding;

        for(index_padding = 0; index_padding < (frame_len - index_data); index_padding++)
        {
            transmitCharacterI2C('\0');
        }

        endTransactionI2C();
    }
}

// Lectura

void readStringFromMemAddress(unsigned char* str, unsigned char MSB_value, unsigned char LSB_value, unsigned char frame_len)
{
    unsigned char comm_status = readCommStatus();

    if (comm_status != COMM_ERROR)
    {
        beginTransmissionI2C();

        transmitCharacterI2C(MSB_value);        //MSB

        transmitCharacterI2C(LSB_value);        //LSB

        beginReceptionI2C();
        
        unsigned char index = 0;

        for (index = 0; index < frame_len + 1; index++)
        {
            unsigned char reading = recieveCharacterI2C();

            if (reading == ERROR_SYMBOL)
            {
                str[index] = str[index];
            }
            else
            {
                str[index] = reading;
            }

            if (index == frame_len - 1)
            {
                endTransactionI2C();
            }
        }
    }                  
}

/* Estado de la conexión con el EEPROM */

unsigned char checkCommStatusEEPROM(void)
{
    unsigned char comm_status = readCommStatus();

    unsigned char char_check = 0;

    readCharacterFromMemAddress(&char_check,0x25,0x55);

    if (comm_status != COMM_ERROR && char_check == COMM_SYMBOL)
    {
        return COMM_EEPROM_NORMAL;
    }
    else
    {
        return COMM_EEPROM_ERROR;
    }
}

/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

/* Manejo de datos */

static unsigned char getMSBValueAddress(unsigned char data_type)
{
    unsigned char MSB_value;

    if ( data_type == 'I' )
    {
        MSB_value = ADDRESS_MSB_INT;
    }
    else if ( data_type == 'F' )
    {
        MSB_value = ADDRESS_MSB_FLOAT;
    }
    else if ( data_type == 'C' )
    {
        MSB_value = ADDRESS_MSB_CHAR;
    }
    else if ( data_type == 'S')
    {
        MSB_value = ADDRESS_MSB_STR;
    }

    return MSB_value;
}

static unsigned char getLSBValueAddress(unsigned char data_type, unsigned char cell_position)
{
    unsigned char LSB_value;

    if ( data_type == 'I' )
    {
        LSB_value = FRAME_LEN_INT * cell_position;
    }
    else if ( data_type == 'F' )
    {
        LSB_value = FRAME_LEN_FLOAT * cell_position;
    }
    else if ( data_type == 'C' )
    {
        LSB_value = FRAME_LEN_CHAR * cell_position;
    }
    else if ( data_type == 'S')
    {
        LSB_value = FRAME_LEN_STR * cell_position;
    }

    return LSB_value;
}

static unsigned char getDataFrameLen(unsigned char data_type)
{
    unsigned char frame_len;

    if ( data_type == 'I' )
    {
        frame_len = FRAME_LEN_INT;
    }
    else if ( data_type == 'F' )
    {
        frame_len = FRAME_LEN_FLOAT;
    }
    else if ( data_type == 'C' )
    {
        frame_len = FRAME_LEN_CHAR;
    }
    else if ( data_type == 'S')
    {
        frame_len = FRAME_LEN_STR;
    }

    return frame_len;
}

/******************************************************************************/
