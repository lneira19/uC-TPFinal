/***************************************************************************//**
  @file     common.h
  @brief    Common types, definitions and macros
  @author   GRUPO3
 ******************************************************************************/

#ifndef _COMMON_H_
#define _COMMON_H_

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

//*** common constants **********************

#define MICROSECONDS_IN_MILISECOND  1000UL
#define MILISECONDS_IN_SECOND       1000UL
#define SECONDS_IN_MINUTE           60UL
#define MINUTES_IN_HOUR             60UL

#define BITS_IN_BYTE    8

#define FALSE           0
#define TRUE            1

#ifndef NULL
#define NULL            ((void*)0)
#endif // NULL


//*** util macros **********************

#ifndef BITSET
#define BITSET(d,b) ((d) |= 1U << (b))
#define BITCLR(d,b) ((d) &= ~(1U << (b)))
#define BITTGL(d,b) ((d) ^= 1U << (b))
#define BITCHK(d,b) (0 != ((d) & (1U << (b))))
#define BOOLEAN(v)  (0 != (v))
#endif // BITSET

#define LOBYTE(w)       (0x00FFU&(w))
#define HIBYTE(w)       (((uint16_t)(w))>>BITS_IN_BYTE)

#define INCTRUNC(d, m)  do { if ((d)<(m)) ++(d); } while (0)
#define DECTRUNC(d, m)  do { if ((d)>(m)) --(d); } while (0)
#define INCMOD(d, m)    do { d = ((d) < (m))? ((d)+1) : 0; } while (0)
#define DECMOD(d, m)    do { d = ((d) > 0)? ((d)-1) : ((m)-1); } while (0)

#define NUMEL(v)        (sizeof(v)/sizeof(*(v)))


/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

/*******************************************************************************
 * VARIABLE PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/


/*******************************************************************************
 ******************************************************************************/

#endif // _COMMON_H_
