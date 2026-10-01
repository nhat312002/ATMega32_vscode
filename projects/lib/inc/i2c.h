/*
 * Title : i2c.h
 * Author : thanhtrung210502
 * Creation Date : 26/09/2026 (DD/MM/YYYY)
 * ------- ---------- --------
 */

#ifndef _I2C_H_
#define _I2C_H_

#ifdef __cplusplus
extern "C"
{
#endif
/*--------------------------------------- Include ---------------------------------------*/
#include "common.h"

/*---------------------------------- Define constants -----------------------------------*/

/*---------------------- Type definitions (Typedef, enum, struct) -----------------------*/
typedef enum I2cMode
{
    eI2C_MASTER,
    eI2C_SLAVE,
} enI2cMode;

typedef struct I2cConfig
{
    enI2cMode mode;
    uint8_t   address;
    uint32_t  clock;
    bool      enableInt;
} stI2cConfig;

/*---------------------------- Export Function Declarations -----------------------------*/
enStatus I2C_Init(const stI2cConfig* i2cConfig);
enStatus I2C_Enable(bool enable);
enStatus I2C_Write(uint8_t address, const uint8_t* data, uint16_t size);
enStatus I2C_Read(uint8_t address, uint8_t* data, uint16_t size);

#ifdef __cplusplus
}
#endif

#endif
