/*
 * Title : common.h
 * Author : nguyen_trung
 * Creation Date : 07/09/2022
 * ------- ---------- --------
 */

#ifndef _COMMON_H_
#define _COMMON_H_

#ifdef __cplusplus
extern "C"
{
#endif
/*--------------------------------------- Include ---------------------------------------*/
#include <stdint.h>
#include <stdbool.h>

/*---------------------------------- Define constants -----------------------------------*/
#define STR_(X) #X
#define STR(X)  STR_(X) /* STR(TEST) ==> "234" if #define TEST 234*/

#define ARRAY_SIZE(X) (sizeof(X) / sizeof(X[0]))

/*---------------------- Type definitions (Typedef, enum, struct) -----------------------*/
typedef struct
{
    uint8_t  day;
    uint8_t  month;
    uint16_t year;
    uint8_t  second;
    uint8_t  minute;
    uint8_t  hour;
} stTime;

typedef enum
{
    eSUCCESS = 0,
    eFAIL,
    eBUSY,
} enStatus;

/*---------------------------- Export Function Declarations -----------------------------*/

#ifdef __cplusplus
}
#endif
#endif
