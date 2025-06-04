/***************************************************************************//**
  @file     main.c
  @brief    Main file
  @author   GRUPO3
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "system.h"
#include "gpio.h"
#include "board.h"
#include "timer.h"
#include "drv_sw.h"
#include "drv_serial_comm.h"
#include "drv_pwm.h"
#include "drv_eeprom.h"
#include "drv_spi.h"
#include "drv_ds18b20.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define LOOP_DELAY_MS                     10

/* Período para actualización de estado de conexión con EEPROM */
#define EEPROM_STATUS_CHECK_PERIOD        1000

/* Período de comunicación por defecto */
#define SERIAL_COMM_DEFAULT_PERIOD        1000

/* Período de actualización de Shift Register */
#define SHIFT_REGISTER_DEFAULT_PERIOD     500

// Períodos de LEDs de estado de comunicación
#define PIN_LED_SERIAL_BLINK_MS           250 
#define PIN_LED_EEPROM_BLINK_MS           250  

/* Macros para errores de comunicación */
#define ERROR_IN_COMM                     1

/* Macros para flags */
#define STEP_RESPONSE_ON                  1

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

/*******************************************************************************
 * VARIABLES WITH GLOBAL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

void AppInit (void);
void AppRun (void);

static void changeOnSwitch(void);

/* Actualizadores de LEDs de estado de comunicaciones */
/* En ambos casos: si el LED parpadea, entonces hay un error en la comunicación */
static void EEPROMStatusLEDBlinkUpdater(void);
static void SERIALStatusLEDBlinkUpdater(void);

/* Actualización del 74HC959 */
static void IntegratedChipUpdater(void);

/* Sistema de control */
static void controlSystem(void);

/* Recuperador de valores de variables modificables: set_point, hysteresis, transmission_period */
static void modifiableVariablesEEPROMRecovery(void);

/* Almacenador de nuevos valores de variables modificables en la EEPROM */
static void modifiableVariablesEEPROMSaver(void);

/* Recuperador de valores almacenados en la EEPROM en caso de desconexión MSP-PC */
static void recoveryOnMSPPCConnectionError(void);

/* Respuesta al escalón */
static void stepResponseHandler(void);

/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

static ticks_t timer_eeprom_status_check;

static ticks_t timer_comm_serial;

static ticks_t timer_SftReg_updater;

/* Variables para el manejo de LEDs de Estado */

// EEPROM
static ticks_t timer_blink_eeprom;
static unsigned char blink_eeprom_flag;

// SERIAL
static ticks_t timer_blink_serial;
static unsigned char blink_serial_flag;

// SPI
static unsigned char byte_to_spi;

/* Temperatura y PWM */
float pwm_value;
float temperature_reading;

/* Variables del sistema de control */
float set_point;
float hysteresis;
int temperature_transmission_period;
unsigned char step_response_flag;
unsigned char step_response_mode;

/* Variables para eeprom */
float set_point_previous;
float hysteresis_previous;
int temperature_transmission_period_previous;

/* Manejo de la desconexión MSP-PC */
unsigned char msp_pc_comm_flag;
unsigned char recovery_counter;

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

void main (void)
{ // NO TOCAR
    systemInitFirst();
    boardInit();
    AppInit();
    systemInitLast();

    for(;;)
        AppRun();
} // NO TOCAR


void AppInit (void)
{
  gpioWrite(PIN_LED_RED, LED_ACTIVE);

  /* Inicializadores de drivers*/
  
  timerInit();
  drvSWInit();
  drvSerialCommInit();
  drvEEPROMInit();
  drvPWMInit();
  drvSPIInit();
  drvDS18B20Init();

  /* Inicialización de timers del main */
  timer_eeprom_status_check = timerStart(EEPROM_STATUS_CHECK_PERIOD);
  timer_comm_serial = timerStart(SERIAL_COMM_DEFAULT_PERIOD);
  timer_SftReg_updater = timerStart(SHIFT_REGISTER_DEFAULT_PERIOD);
  timer_blink_eeprom = timerStart(PIN_LED_EEPROM_BLINK_MS);
  timer_blink_serial = timerStart(PIN_LED_SERIAL_BLINK_MS);

  /* Inicialización de flags */
  blink_eeprom_flag = !ERROR_IN_COMM;
  blink_serial_flag = !ERROR_IN_COMM;
  msp_pc_comm_flag = !ERROR_IN_COMM;
  step_response_flag = !STEP_RESPONSE_ON;

  /* Inicialización de variables del main */
  pwm_value = 0;
  set_point = 0;
  hysteresis = 0;
  temperature_transmission_period = 1000;
  temperature_reading = -999.0;
  byte_to_spi = 0x01;
  recovery_counter = 1;

  step_response_flag = !STEP_RESPONSE_ON;
  step_response_mode = 0;

  if ( checkCommStatusEEPROM() == COMM_EEPROM_NORMAL )
  {
    /* Inicialización de variables con datos del EEPROM */
    modifiableVariablesEEPROMRecovery();

    set_point_previous = set_point;
    hysteresis_previous = hysteresis;
    temperature_transmission_period_previous = temperature_transmission_period;
  }
}


void AppRun (void)
{

  /* -- SECCIÓN DE CHECKEO DE ESTADO DE COMUNICACIÓN SERIAL Y EEPROM -- */

  EEPROMStatusLEDBlinkUpdater();

  SERIALStatusLEDBlinkUpdater();

  unsigned char serial_status = checkCommStatusSerial();
  
  if ( serial_status == COMM_SERIAL_ERROR )
  {
    blink_serial_flag = ERROR_IN_COMM;
    msp_pc_comm_flag = ERROR_IN_COMM;
  }
  else if ( serial_status == COMM_SERIAL_NORMAL )
  {
    blink_serial_flag = !ERROR_IN_COMM;
    msp_pc_comm_flag = !ERROR_IN_COMM;
    recovery_counter = 1;
  }

  // Checkeo cada 1 segundo el estado de la comunicación con el EEPROM
  if (timerExpired(timer_eeprom_status_check))
  {
    timer_eeprom_status_check = timerStart(EEPROM_STATUS_CHECK_PERIOD);
    gpioToggle(PIN_LED_RED);

    unsigned char eeprom_status = checkCommStatusEEPROM();
    
    if ( eeprom_status == COMM_EEPROM_ERROR )
    {
      blink_eeprom_flag = ERROR_IN_COMM;
    }
    else if ( eeprom_status == COMM_EEPROM_NORMAL )
    {
      blink_eeprom_flag = !ERROR_IN_COMM;
    }
  }

  /* -- SECCIÓN DE COMUNICACIÓN SERIAL -- */

  // Recepción de información
  recieveDataSerialComm('F', &set_point, '1');
  recieveDataSerialComm('F', &hysteresis, '2');
  recieveDataSerialComm('I', &temperature_transmission_period, '3');
  recieveDataSerialComm('C', &step_response_mode, '4');

  // Transmisión de información
  if ( timerExpired(timer_comm_serial) )
  {
    timer_comm_serial = timerStart(temperature_transmission_period);

    transmitDataSerialComm('F', &pwm_value, '%');
    transmitDataSerialComm('F', &set_point, 'S');
    transmitDataSerialComm('F', &hysteresis, 'H');
    transmitDataSerialComm('I', &temperature_transmission_period, 'P');
    transmitDataSerialComm('F', &temperature_reading, 'T');
    //transmitDataSerialComm('C', &step_response_mode, '4');
  }
  
  /* -- SECCIÓN DE GUARDADO DE INFORMACIÓN EN LA EEPROM -- */
  modifiableVariablesEEPROMSaver();

  /* -- SECCIÓN DE CONTROL DE TEMPERATURA -- */

  // Lectura de temperatura
  readTemperatureDS18B20(&temperature_reading);
  
  // Sistema de control
  controlSystem();

  // Handler de la Respuesta al escalón
  stepResponseHandler();

  /* -- SECCIÓN DE ACTUALIZACIÓN DE SHIFT REGISTER -- */

  if ( timerExpired(timer_SftReg_updater) )
  {
    timer_SftReg_updater = timerStart(SHIFT_REGISTER_DEFAULT_PERIOD);
    IntegratedChipUpdater();
  }

  /* -- SECCIÓN DE RECUPERACIÓN DE DATOS ANTE DESCONEXIÓN MSP-PC -- */
  recoveryOnMSPPCConnectionError();

  /* -- EXTRA -- */

  // Seguridad extra con uso de botón integrado
  executeOnSwitchChange(PIN_SWITCH_S2,changeOnSwitch);
  timerDelay(LOOP_DELAY_MS);
}


/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

static void changeOnSwitch(void)
{
  temperature_transmission_period = SERIAL_COMM_DEFAULT_PERIOD;

  if ( checkCommStatusEEPROM() == COMM_EEPROM_NORMAL )
  {
    modifiableVariablesEEPROMRecovery();
  }

  // Descomentar esta linea para registrar el valor '!' de seguridad en la EEPROM
  //writeCharacterIntoMemAddress('!',0x25,0x55);
}

static void EEPROMStatusLEDBlinkUpdater(void)
{
  if (timerExpired(timer_blink_eeprom))
  {
    timer_blink_eeprom = timerStart(PIN_LED_EEPROM_BLINK_MS);

    if ( blink_eeprom_flag )
    {
      gpioToggle(PIN_LED_EEPROM_STATUS);
    }
    else
    {
      gpioWrite(PIN_LED_EEPROM_STATUS, HIGH);
    }

  }
}

static void SERIALStatusLEDBlinkUpdater(void)
{
  if (timerExpired(timer_blink_serial))
  {
    timer_blink_serial = timerStart(PIN_LED_SERIAL_BLINK_MS);
    
    if ( blink_serial_flag )
    {
      gpioToggle(PIN_LED_SERIAL_STATUS);
    }
    else
    {
      gpioWrite(PIN_LED_SERIAL_STATUS, HIGH);
    }
  }
}

static void IntegratedChipUpdater(void)
{
  sendByteSPI(byte_to_spi);

  if ( temperature_reading < 27.5 )
  {
    byte_to_spi = 0x01;
  }
  else if ( temperature_reading >= 27.5 && temperature_reading < 31.25)
  {
    byte_to_spi = 0x03;
  }
  else if ( temperature_reading >= 31.25 && temperature_reading < 35.00 )
  {
    byte_to_spi = 0x07;
  }
  else if ( temperature_reading >= 35.00 && temperature_reading < 38.75 )
  {
    byte_to_spi = 0x0F;
  }
  else if ( temperature_reading >= 38.75 && temperature_reading < 42.50 )
  {
    byte_to_spi = 0x1F;
  }
  else if ( temperature_reading >= 42.50 && temperature_reading < 46.25 )
  {
    byte_to_spi = 0x3F;
  }
  else if ( temperature_reading >= 46.25 && temperature_reading < 50.00 )
  {
    byte_to_spi = 0x7F;
  }
  else if ( temperature_reading >= 50.00 )
  {
    byte_to_spi = 0xFF;
  }

}

static void controlSystem(void)
{
  float error = set_point - temperature_reading;
  float hyst_2 = hysteresis/2;
  if ( error > hyst_2 )
    pwm_value = 1.0;
  else if ( error > 0 && error <= hyst_2 )
    pwm_value = 0.75;
  else if ( error < (hyst_2*(-1)) )
    pwm_value = 0.0;
  
  if ( step_response_flag == STEP_RESPONSE_ON )
  {
    pwm_value = 1.0;
  }
  
  modifyDC(&pwm_value);
}

static void modifiableVariablesEEPROMRecovery(void)
{
  readDataFromEEPROM('F', &set_point, 1);

  timerDelay(500);

  readDataFromEEPROM('F', &hysteresis, 2);

  timerDelay(500);

  readDataFromEEPROM('I', &temperature_transmission_period, 1);

  timerDelay(500);
}

static void modifiableVariablesEEPROMSaver(void)
{
  if ( set_point != set_point_previous )
  {
    set_point_previous = set_point;
    writeDataIntoEEPROM('F', &set_point, 1);

    timerDelay(500);
  }

  if ( hysteresis != hysteresis_previous )
  {
    hysteresis_previous = hysteresis;
    writeDataIntoEEPROM('F', &hysteresis, 2);

    timerDelay(500);
  }

  if ( temperature_transmission_period != temperature_transmission_period_previous )
  {
    temperature_transmission_period_previous = temperature_transmission_period;
    writeDataIntoEEPROM('I', &temperature_transmission_period, 1);

    timerDelay(500);
  }
}

static void recoveryOnMSPPCConnectionError(void)
{
  if ( msp_pc_comm_flag == ERROR_IN_COMM && recovery_counter)
  {
    if ( checkCommStatusEEPROM() == COMM_EEPROM_NORMAL )
    {
      modifiableVariablesEEPROMRecovery();
      recovery_counter = 0;
      step_response_mode = 0;
      step_response_flag = !STEP_RESPONSE_ON;
    }
  }
}

static void stepResponseHandler(void)
{
  if ( step_response_mode == '$' )
  {
    // Reinicio de la variable
    step_response_mode = 0;

    if ( step_response_flag == STEP_RESPONSE_ON )
    {
      step_response_flag = !STEP_RESPONSE_ON;
    }
    else if ( step_response_flag == !STEP_RESPONSE_ON )
    {
      step_response_flag = STEP_RESPONSE_ON;
    } 
  }
}
/******************************************************************************/

