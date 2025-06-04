/***************************************************************************//**
  @file     drv_ds28b20.c
  @brief    Driver para sensor de temperatura DS18B20.
  @author   Lucas Matias Neira
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "drv_ds18b20.h"

#include "board.h"
#include "gpio.h"
#include "timer.h"
#include "intrinsics.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

// Comandos 1-Wire
#define SKIP_ROM            0xCC
#define CONVERT_T           0x44
#define READ_SCRATCHPAD     0xBE

// Macros
#define CONVERSION_ON       1

#define READING_ON          1

#define CONVERSION_PERIOD   800 //ms

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

/*******************************************************************************
 * VARIABLES WITH GLOBAL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

static void delay_us(unsigned int us);

static unsigned char onewire_reset(void);

static void onewire_write_bit(unsigned char bit);

static unsigned char onewire_read_bit(void);

static void onewire_write_byte(unsigned char data);

static unsigned char onewire_read_byte(void);

static void beginTempConversion(void);

/*******************************************************************************
 * ROM CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

static unsigned char conversion_flag;

static float temperature_reading;

static ticks_t tim_conversion;


/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

// Inicialización del pin 1-Wire
void drvDS18B20Init(void)
{
    gpioMode(PIN_TEMPSENSOR, INPUT);

    conversion_flag = !CONVERSION_ON;

    temperature_reading = -999.0;
}

// Leer temperatura
void readTemperatureDS18B20(float* reciever)
{
    beginTempConversion();

    *reciever = temperature_reading;
}

/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

// Retardo aproximado en microsegundos
static void delay_us(unsigned int us)
{
    while(us--) {
        __delay_cycles(8); // 8 ciclos por microsegundo a 8 MHz
    }
}

// Reset del bus 1-Wire
static unsigned char onewire_reset(void)
{ 
    gpioMode(PIN_TEMPSENSOR, OUTPUT);
    gpioWrite(PIN_TEMPSENSOR, LOW);
    delay_us(480);                    // Retardo de reset
    
    gpioMode(PIN_TEMPSENSOR, INPUT);
    delay_us(70);                     // Esperar presencia

    unsigned char presence = !gpioRead(PIN_TEMPSENSOR);
    delay_us(410);                    // Tiempo restante del ciclo

    return presence;
}

// Escribir un bit
static void onewire_write_bit(unsigned char bit)
{
    gpioMode(PIN_TEMPSENSOR, OUTPUT);
    gpioWrite(PIN_TEMPSENSOR, LOW);
    //delay_us(1);                 // Esperar 1 us

    if (bit)
    {
        delay_us(1);
        gpioMode(PIN_TEMPSENSOR, INPUT);
    }
    
    delay_us(60);
    gpioMode(PIN_TEMPSENSOR, INPUT);
    delay_us(1);
}


// Leer un bit
static unsigned char onewire_read_bit(void)
{
    unsigned char bit = 0;
    
    gpioMode(PIN_TEMPSENSOR, OUTPUT);
    gpioWrite(PIN_TEMPSENSOR, LOW); 
    //delay_us(2);                      // Bajo 1µs
    //__delay_cycles(12);

    gpioMode(PIN_TEMPSENSOR, INPUT);  // Modo entrada
    //delay_us(15);                     // Esperar 15µs
    __delay_cycles(16);

    if (gpioRead(PIN_TEMPSENSOR))
    {
        bit = 1;
    }
    
    delay_us(45);                     // Terminar el slot
    return bit;
}

// Escribir un byte
static void onewire_write_byte(unsigned char data)
{
    unsigned char i;

    for (i = 0; i < 8; i++)
    {
        onewire_write_bit(data & 0x01);
        data >>= 1;
    }
}

// Leer un byte
static unsigned char onewire_read_byte(void)
{
    unsigned char data = 0;
    unsigned char i;

    for(i = 0; i < 8; i++)
    {
        data |= ( onewire_read_bit() << i);
    }
    return data;
}


static void beginTempConversion(void)
{
    if (conversion_flag == !CONVERSION_ON)
    {
        onewire_reset();
        onewire_write_byte(SKIP_ROM);
        onewire_write_byte(CONVERT_T);

        tim_conversion = timerStart(CONVERSION_PERIOD);
        conversion_flag = CONVERSION_ON;
    }

    if (timerExpired(tim_conversion))
    {
        onewire_reset();
        onewire_write_byte(SKIP_ROM);
        onewire_write_byte(READ_SCRATCHPAD);

        unsigned char temp_lsb = onewire_read_byte();
        unsigned char temp_msb = onewire_read_byte();

        int temp_raw = (temp_msb << 8) | temp_lsb;
        // Convertir a grados Celsius
        float temp_celsius = temp_raw / 16.0;

        if ( temp_celsius > -54.0 && temp_celsius < 126.0 )
        {
            //temperature_reading = temp_celsius;
            
            if ( temperature_reading == -999.0 )
            {
                if ( temp_celsius >= 25.0 && temp_celsius <= 55.0 )
                {
                    temperature_reading = temp_celsius;
                }
                
            }

            if ( temp_celsius > (temperature_reading - 5) && temp_celsius < (temperature_reading + 5))
            {
                temperature_reading = temp_celsius;
            }
            
        }

        conversion_flag = !CONVERSION_ON;
    }    
}
/******************************************************************************/
