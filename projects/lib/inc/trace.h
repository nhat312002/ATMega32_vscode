/*
 * Title : trace.h
 * Copyright :
 * Author : thanhtrung210502
 * Creation Date : 20/08/2022
 * Description : < Briefly describe the purpose of the file. >
 * Dependencies : < H/W, S/W( Operating System, Compiler) >
 * ------- ---------- --------
 */

#ifndef _TRACE_H_
#define _TRACE_H_

#ifdef __cplusplus
extern "C"
{
#endif
/*--------------------------------------- Include ---------------------------------------*/
#include <stdio.h>

/*---------------------------------- Define constants -----------------------------------*/
#define TRACE_LEVEL_DEBUG 0x01
#define TRACE_LEVEL_INFO  0x02
#define TRACE_INT         0x04

#if defined(TEST_HW)
#define TRACE_LEVEL 0
#elif defined(ENABLE_TRACE_INT)
#define TRACE_LEVEL (TRACE_LEVEL_DEBUG | TRACE_LEVEL_INFO | TRACE_INT)
#else
#define TRACE_LEVEL (TRACE_LEVEL_DEBUG | TRACE_LEVEL_INFO)
#endif

#define trace(...)                           \
    {                                        \
        if (TRACE_LEVEL & TRACE_LEVEL_DEBUG) \
        {                                    \
            printf("[DEBUG] " __VA_ARGS__);  \
            printf("\r\n");                  \
        }                                    \
    }

#define trace_int(...)                       \
    {                                        \
        if (TRACE_LEVEL & TRACE_LEVEL_DEBUG) \
        {                                    \
            printf("[ INT ] " __VA_ARGS__);  \
            printf("\r\n");                  \
        }                                    \
    }

#define trace_var(var)                                    \
    {                                                     \
        if (TRACE_LEVEL & TRACE_LEVEL_DEBUG)              \
        {                                                 \
            printf("[DEBUG] " #var ": %d\r\n", (int)var); \
        }                                                 \
    }

#define trace_float(var)                                                \
    do                                                                  \
    {                                                                   \
        if (TRACE_LEVEL & TRACE_LEVEL_DEBUG)                            \
        {                                                               \
            int decimal = ((int)(var * 10000)) % 10000;                 \
            decimal *= (decimal > 0) ? 1 : -1;                          \
            printf("[DEBUG] " #var ": %d.%04d\r\n", (int)var, decimal); \
        }                                                               \
    } while (0)

#define trace_line()                                                     \
    {                                                                    \
        if (TRACE_LEVEL & TRACE_LEVEL_DEBUG)                             \
        {                                                                \
            printf("[LINE] Func: %s, line: %d\r\n", __func__, __LINE__); \
        }                                                                \
    }

#define trace_hex(str, arr, len)                 \
    {                                            \
        if (TRACE_LEVEL & TRACE_LEVEL_DEBUG)     \
        {                                        \
            uint8_t* pu8Arr = (uint8_t*)arr;     \
            printf("[HEX] %s: ", str);           \
            for (uint32_t i = 0; i < (len); i++) \
            {                                    \
                printf("%02X ", pu8Arr[i]);      \
            }                                    \
            printf("\r\n");                      \
        }                                        \
    }

#define trace_info(...)                     \
    {                                       \
        if (TRACE_LEVEL & TRACE_LEVEL_INFO) \
        {                                   \
            printf(__VA_ARGS__);            \
            printf("\r\n");                 \
        }                                   \
    }

#define trace_error()                                                     \
    {                                                                     \
        if (TRACE_LEVEL & TRACE_LEVEL_DEBUG)                              \
        {                                                                 \
            printf("[ERROR] Func: %s, line: %d\r\n", __func__, __LINE__); \
        }                                                                 \
    }

/*---------------------- Type definitions (Typedef, enum, struct) -----------------------*/

/*---------------------------- Export Function Declarations -----------------------------*/

#ifdef __cplusplus
}
#endif
#endif
