/*
 * Title : i2c.c
 * Author : thanhtrung210502
 * Creation Date : 26/09/2026
 * ------- ---------- --------
 */

/*---------------------------------------------------- System Include ------------------*/
#include <avr/io.h>
#include <stdio.h>
#include <stddef.h>
#include <util/delay.h>
/*----------------------------------------------------- Local Include ------------------*/
#include "i2c.h"
#include "trace.h"

/*---------------------------------------------- Private define constants ---------------*/

/*---------------------------------------------------- Private macros ------------------*/

/*---------------------------------------------- Private type definitions ---------------*/

/*--------------------------------------------------- Static variables -----------------*/

/*----------------------------------- Private function prototypes declarations -----------*/
static bool I2C_Wait(void);
/*-------------------------------------------- Private functions definition --------------*/
static bool I2C_Wait(void)
{
    return (TWCR & (1 << TWINT)) ? true : false;
}

/*--------------------------------------------- Export functions definition --------------*/
enStatus I2C_Start()
{
    uint8_t status;
    SET_BIT(TWCR, TWSTA);
    SET_BIT(TWCR, TWINT);
    while (I2C_Wait() == false);
    status = TWSR & 0xF8;
    trace_var(status);
    return (status == 0x08 || status == 0x10) ? eSUCCESS : eFAIL;
}

enStatus I2C_Stop(void)
{
    /* TWSTO and TWINT must be written simultaneously */
    TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);
    while (TWCR & (1 << TWSTO));

    return eSUCCESS;
}

enStatus I2C_Init(const stI2cConfig* i2cConfig)
{
    if (i2cConfig == NULL)
    {
        return eFAIL;
    }
    if (i2cConfig->mode == eI2C_MASTER)
    {
        if (i2cConfig->clock == 0)
        {
            return eFAIL;
        }
        TWSR          = 0x00;
        uint32_t twbr = (F_CPU / i2cConfig->clock - 16UL) / 2UL;
        if (twbr > 255)
        {
            return eFAIL;
        }
        TWBR = twbr;
    }
    else
    {
        TWAR = (i2cConfig->address << 1);
    }

    return eSUCCESS;
}

enStatus I2C_Enable(bool enable)
{
    if (enable == false)
    {
        CLEAR_BIT(TWCR, TWEN);
    }
    else
    {
        SET_BIT(TWCR, TWEN);
    }

    return eSUCCESS;
}

enStatus I2C_Write(uint8_t address, const uint8_t* data, uint16_t size)
{
    if (data == NULL || size == 0)
    {
        trace_error();
        return eFAIL;
    }

    if (I2C_Start() == eFAIL)
    {
        trace_error();
        return eFAIL;
    }

    /* Send address + write request */
    trace_var(address);
    TWDR = (address << 1) | 0;
    SET_BIT(TWCR, TWINT);
    while (I2C_Wait() == false);
    uint8_t status = TWSR & 0xF8;

    if (status != 0x18)
    {
        trace_error();
        I2C_Stop();
        return eFAIL;
    }

    for (uint16_t i = 0; i < size; i++)
    {
        TWDR = data[i];
        SET_BIT(TWCR, TWINT);

        while (I2C_Wait() == false);

        if ((TWSR & 0xF8) != 0x28)
        {
            trace_error();
            I2C_Stop();
            return eFAIL;
        }
    }
    I2C_Stop();

    return eSUCCESS;
}

enStatus I2C_Read(uint8_t address, uint8_t* data, uint16_t size)
{
    if (data == NULL || size == 0)
    {
        return eFAIL;
    }
    if (I2C_Start() == eFAIL)
    {
        return eFAIL;
    }

    /* Load TWDR first, then clear TWINT */
    TWDR = (address << 1) | 1;
    SET_BIT(TWCR, TWINT);
    while (I2C_Wait() == false);
    uint8_t status = TWSR & 0xF8;

    if (status != 0x40)
    {
        return eFAIL;
    }

    for (uint16_t i = 0; i < size; i++)
    {
        if (i < size - 1)
        {
            SET_BIT(TWCR, TWINT);
            SET_BIT(TWCR, TWEA);
        }
        else
        {
            /* Clear TWEA to send NACK on the last byte */
            SET_BIT(TWCR, TWINT);
            CLEAR_BIT(TWCR, TWEA);
        }
        while (I2C_Wait() == false);

        if (i < size - 1)
        {
            if ((TWSR & 0xF8) != 0x50)
            {
                I2C_Stop();
                return eFAIL;
            }
        }
        else
        {
            if ((TWSR & 0xF8) != 0x58)
            {
                I2C_Stop();
                return eFAIL;
            }
        }

        data[i] = TWDR;
    }
    I2C_Stop();

    return eSUCCESS;
}
