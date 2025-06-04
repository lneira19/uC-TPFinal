/***************************************************************************//**
  @file     drv_serial_comm.c
  @brief    Implementación UART + Data Frame para comunicación UART.
  @author   Lucas Matias Neira
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "drv_serial_comm.h"

#include "drv_uart.h"
#include "utils.h"
#include "protocols.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define MSG_SENT    1

#define MSG_READ    1

#define MAX_NOT_READ_MSGS   8

#define PROTOCOL_CHARS  DATAFRAMETA_CHARS

#define LEN_RAW_DATA    (LEN_BUFFER - PROTOCOL_CHARS)

#define DATAFRAME_START     DATAFRAMETA_START

#define DATAFRAME_STOP      DATAFRAMETA_STOP

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

typedef struct
{
    unsigned char msg[LEN_BUFFER];
    unsigned char state_msg;
} tx_data_t;

typedef struct
{
    unsigned char msg[LEN_BUFFER];
    unsigned char state_msg;
    unsigned char code;
} rx_data_t;

/*******************************************************************************
 * VARIABLES WITH GLOBAL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

// Transmisión de datos

static void transmission(void);

// Recepción de datos

static void reception(unsigned char rx_char);

/*******************************************************************************
 * ROM CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

// Transmisión
static tx_data_t tx_data;

// Recepción
static rx_data_t rx_data;
static unsigned char rx_interrupt_index;
static unsigned char msg_not_read_counter;

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

/** INIT - DEFINICIÓN **/
void drvSerialCommInit(void)
{
    /* Inicialización del UART */

    drvUARTInit();

    /** Variables internas **/
    
    // Transmisión de data
    tx_data.msg[0] = '\0';
    tx_data.state_msg = MSG_SENT;
    
    // Recepción de data
    rx_data.msg[0] = '\0';
    rx_data.state_msg = MSG_READ; //Leido
    rx_data.code = '\0';

    rx_interrupt_index = 0;
    msg_not_read_counter = 0;

    RXInterruptUART(reception);
}

/* Transmisión de un mensaje */

/**
 * @brief Transmite información por Serial a través de comunicación Serial UART utilizando un protocolo
 * @param data_type Tipo de dato del dato a transmitir
 * @param data Dirección de la variable que almacena el dato a transmitir
 * @param code Código asociado al dato para su consecuente decodificación
 */
void transmitDataSerialComm(unsigned char data_type, void* data, unsigned char code)
{
    if (tx_data.state_msg == MSG_SENT)
    {
        unsigned char raw_data[LEN_RAW_DATA];

        conversionDataTypeToString(data_type, raw_data, data);

        dataFrameTAPacking(raw_data, data_type, code, tx_data.msg);

        tx_data.state_msg = !MSG_SENT;
    }

    transmission();
}

/* Recepción de datos */

/**
 * @brief Recibe información por Serial a través de comunicación UART utilizando un protocolo
 * @param data_type Tipo de dato del dato a recibir, necesario para su correcta decodificación
 * @param reciever Dirección de la variable que almacenará el dato ya decodificado
 * @param code Código asociado al dato esperado para que efectivamente se almacene la información en 'reciever'
 */
void recieveDataSerialComm(unsigned char data_type, void* reciever, unsigned char code)
{
    /*
        La función toma el mensaje almacenado en rx_data.msg,
        verifica que se pueda leer, verifica si el código almacenado
        corresponde al código pasado por parámetro y recupera el mensaje
        eliminando las partes del protocolo. Luego, esta data se decodifica
        al tipo de dato necesario pasado por parámetro.
    */

    if (rx_data.state_msg == !MSG_READ)
    {
        if (rx_data.code == code)
        {
            msg_not_read_counter = 0;

            unsigned char data[LEN_RAW_DATA];

            dataFrameTAUnpacking(rx_data.msg, data);

            conversionStringToDataType(data_type, reciever, data);

            rx_data.state_msg = MSG_READ;
        }
        else
        {
            /* 
                Esta sección es para controlar los casos en las que por
                algún error rx_data.code != code. Esto deriva en que 
                rx_data.state_msg siempre quede como !MSG_READ entonces
                no es posible leer mensages nuevos. Para solucionar esto,
                se hace un conteo de la cantidad de veces que el mensaje
                almacenado no se leyó en el main. Este máximo es
                MAX_NOT_READ_MSGS: si el main utilizó MAX_NOT_READ_MSGS veces 
                recieveData y en ninguna de esas ocaciones se leyó el mensaje 
                entonces se coloca rx_data.state_msg = MSG_READ y se desbloquea 
                la lectura de nuevos mensajes.
            */

            ++msg_not_read_counter;
            if (msg_not_read_counter == MAX_NOT_READ_MSGS)
            {
                rx_data.state_msg = MSG_READ;
            }
        }
    }
}

/* Estado de conexión con el Serial */

unsigned char checkCommStatusSerial(void)
{
    unsigned char comm_status = readCommStatusUART();

    if (comm_status != COMM_ERROR)
    {
        return COMM_SERIAL_NORMAL;
    }
    else
    {
        return COMM_SERIAL_ERROR;
    }
}

/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

/* Transmisión de datos */

static void transmission(void)
{
    if (tx_data.state_msg == !MSG_SENT)
    {
        unsigned char *str = tx_data.msg;
    
        while(*str != '\0')
        {
            unsigned char state = transmitCharacterUART(str);

            if ( state == CHAR_TRANSMITTED )
            {
                str++;
            }
        }

        tx_data.state_msg = MSG_SENT;
    }    
}

/* Recepción de datos */

static void reception(unsigned char rx_char)
{
    if (rx_data.state_msg == MSG_READ)
    {
        if (rx_char == DATAFRAME_START)
        {
            rx_interrupt_index = 0;
            rx_data.msg[rx_interrupt_index++] = DATAFRAME_START;
        }
        else if ((rx_char == DATAFRAME_STOP && rx_interrupt_index > 2))
        {
            rx_data.msg[rx_interrupt_index++] = DATAFRAME_STOP;
            rx_data.msg[rx_interrupt_index] = '\0';
            rx_data.code = rx_data.msg[2];
            rx_data.state_msg = !MSG_READ;
        }
        else
        {
            rx_data.msg[rx_interrupt_index++] = rx_char;
        }
    }              
}

/******************************************************************************/
