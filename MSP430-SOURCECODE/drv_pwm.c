/***************************************************************************//**
  @file     drv_pwm.h
  @brief    Driver para uso de PWM.
  @author   Lucas Matias Neira
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "drv_pwm.h"

#include "hardware.h"
#include "board.h"
#include "gpio.h"

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
static unsigned int pwm_value;

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

void drvPWMInit(void)
{
    uint8_t port = PIN2PORT(PIN_PWM);
    uint8_t num = PIN2NUM(PIN_PWM);
    
    switch (port)
    {
    case 1:
        if (num == 2)
        {
            P1DIR |= BIT2;
            P1SEL |= BIT2;
            P1SEL2 &= ~BIT2;
        }
        else if (num == 6)
        {
            P1DIR |= BIT6;
            P1SEL |= BIT6;
            P1SEL2 &= ~BIT6;
        }
        break;
    case 2:
        P2DIR |= BIT6;

        P2SEL |= BIT6;
        P2SEL &= ~BIT7;

        P2SEL2 &= ~(BIT6 | BIT7);
        break;
    } 

    pwm_value = PWM_PERIOD/2; // valor inicial de CCR1

    /* PWM output mode: 7 - PWM reset/set */
    TA0CCTL1 = OUTMOD_7 | CCIE;          // Output Compare Reset(CCR1)/Set(CCR0) On TA0.1 (P1.6) + CCR1 interrupt enable
    TA0CCR0 = PWM_PERIOD;                      // Set PWM period T1 = 1/(8*10^6 Hz/(8*PWM_PERIOD))= 1 ms

    //  Clk Source: SMCLK , Timer Counter: Up Continuous - divide by 8
    TA0CTL = TASSEL_2  | MC_1 | ID_3 ; // SMLK | UP_MODE | /8
}

/**
 * @brief Modificación del DC del PWM
 * @param fraction Puntero a un float que representa el Duty Cycle
 */
void modifyDC(float* fraction)
{
    pwm_value = (unsigned int)(*fraction * (float)PWM_PERIOD);
}

/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

#pragma vector=TIMER0_A1_VECTOR        //Interrupt Service Routine (ISR) for overflow and CCR<1-n> TimerA0
__interrupt void isr_myPWM(void)
{

  volatile int dummy;
  dummy=(TA0IV);            // Clear Interrupt flag


  switch(dummy)
  {
    case TA0IV_TACCR1:           // OC Interrupt

    //Set duty (50% in this case 500/1000(CR0)
    TA0CCR1 = pwm_value;                 // Reset here (ccr1) and  (set on ccr0) 1-999

    break;
    case TA0IV_TACCR2:           // Not used

    break;
    case TA0IV_TAIFG:            // Timer Overflow

    break;
  }
}


#pragma vector=TIMER0_A0_VECTOR        //Interrupt Service Routine (ISR) for CCR0 (only)
__interrupt void isr_myccr(void)
{
      // NOT USED
}

/******************************************************************************/
