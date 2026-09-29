/*
 * Title : spi.h
 * Author : nhatv
 * Creation Date : 26/09/2026 (DD/MM/YYYY)
 * ------- ---------- --------
 */

#ifndef _SPI_H_
#define _SPI_H_

#ifdef __cplusplus
extern "C"
{
#endif
/*--------------------------------------- Include ---------------------------------------*/
#include "common.h"

/*---------------------------------- Define constants -----------------------------------*/

/*---------------------- Type definitions (Typedef, enum, struct) -----------------------*/
typedef enum SpiMode
{
    eSPI_SLAVE  = 0,
    eSPI_MASTER = 1,
} enSpiMode;

typedef enum DataOrder
{
    eMSB = 0,
    eLSB = 1,
} enDataOrder;

typedef struct SpiConfig
{
    enSpiMode   mode;
    enDataOrder dataOrder;
    uint8_t     cpol;
    uint8_t     cpha;
    uint8_t     prescale;
    bool        enableInt;
} stSpiConfig;

/*---------------------------- Export Function Declarations -----------------------------*/
enStatus SPI_Init(const stSpiConfig* spiConfig);
enStatus SPI_Enable(bool enable);
enStatus SPI_Transmit(const uint8_t* sendData, uint8_t* recvData, uint8_t len);
enStatus SPI_Send(const uint8_t* sendData, uint8_t len);
enStatus SPI_Receive(uint8_t* sendData, uint8_t len);

#ifdef __cplusplus
}
#endif

#endif
