/*
 * Title : spi.c
 * Author : nhatv
 * Creation Date : 26/09/2026
 * ------- ---------- --------
 */

/*----------------- ----------------------------------- System Include ------------------*/
#include <avr/io.h>
#include <stddef.h>

/*----------------- ------------------------------------ Local Include ------------------*/
#include "spi.h"

/*--------------- ------------------------------ Private define constants ---------------*/
#define MISO 6
#define MOSI 5
#define SS   4
#define SCK  7

/*----------------- ----------------------------------- Private macros ------------------*/

/*--------------- ------------------------------ Private type definitions ---------------*/

/*----------------- ---------------------------------- Static variables -----------------*/

/*----------- ---------------------- Private function prototypes declarations -----------*/
static bool SPI_IsTransmitComplete(void);

/*-------------- ---------------------------- Private functions definition --------------*/
static bool SPI_IsTransmitComplete(void)
{
    return SPSR & (1 << SPIF);
}

/*-------------- ----------------------------- Export functions definition --------------*/
enStatus SPI_Init(const stSpiConfig* spiConfig)
{
    if (spiConfig->mode == eSPI_MASTER)
    {
        SET_BIT(SPCR, MSTR);
        SET_BIT(DDRB, MOSI);
        SET_BIT(DDRB, SCK);
        SET_BIT(DDRB, SS);
        CLEAR_BIT(DDRB, MISO);
    }
    else
    {
        CLEAR_BIT(SPCR, MSTR);
        CLEAR_BIT(DDRB, MOSI);
        CLEAR_BIT(DDRB, SCK);
        CLEAR_BIT(DDRB, SS);
        SET_BIT(DDRB, MISO);
    }

    if (spiConfig->dataOrder == eLSB)
    {
        SET_BIT(SPCR, DORD);
    }
    else
    {
        CLEAR_BIT(SPCR, DORD);
    }

    if (spiConfig->cpol == 0)
    {
        CLEAR_BIT(SPCR, CPOL);
    }
    else
    {
        SET_BIT(SPCR, CPOL);
    }

    if (spiConfig->cpha == 0)
    {
        CLEAR_BIT(SPCR, CPHA);
    }
    else
    {
        SET_BIT(SPCR, CPHA);
    }

    CLEAR_BIT(SPCR, SPI2X);
    CLEAR_BIT(SPCR, SPR1);
    CLEAR_BIT(SPCR, SPR0);

    switch (spiConfig->prescale)
    {
    case 16:
        SET_BIT(SPCR, SPR0);
        break;

    case 64:
        SET_BIT(SPCR, SPR1);
        break;

    case 128:
        SET_BIT(SPCR, SPR0);
        SET_BIT(SPCR, SPR1);
        break;

    case 2:
        SET_BIT(SPCR, SPI2X);
        break;

    case 8:
        SET_BIT(SPCR, SPI2X);
        SET_BIT(SPCR, SPR0);
        break;

    case 32:
        SET_BIT(SPCR, SPI2X);
        SET_BIT(SPCR, SPR1);
        break;

    case 4:
    default:
        break;
    }

    return eSUCCESS;
}

enStatus SPI_Enable(bool enable)
{
    if (enable == false)
    {
        CLEAR_BIT(SPCR, SPE);
    }
    else
    {
        SET_BIT(SPCR, SPE);
    }

    return true;
}

enStatus SPI_Transmit(const uint8_t* sendData, uint8_t* recvData, uint8_t len)
{
    if (sendData == NULL || len == 0)
    {
        return false;
    }

    for (uint8_t i = 0; i < len; i++)
    {
        SPDR = sendData[i];
        while (SPI_IsTransmitComplete() == false);

        if (recvData != NULL)
        {
            recvData[i] = SPDR;
        }
    }
    return true;
}

enStatus SPI_Send(const uint8_t* sendData, uint8_t len)
{
    if (sendData == NULL || len == 0)
    {
        return false;
    }

    for (uint8_t i = 0; i < len; i++)
    {
        SPDR = sendData[i];
        while (SPI_IsTransmitComplete() == false);
    }
    return true;
}

enStatus SPI_Receive(uint8_t* sendData, uint8_t len)
{
    if (sendData == NULL || len == 0)
    {
        return false;
    }

    for (uint8_t i = 0; i < len; i++)
    {
        if (SET_BIT(SPSR, MSTR))
        {
            SPDR = 0xFF;
        }
        while (SPI_IsTransmitComplete() == false);
        sendData[i] = SPDR;
    }
    return true;
}
