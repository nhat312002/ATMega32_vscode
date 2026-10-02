/*
 * Title : i2c.c
 * Author : nhatv
 * Creation Date : 26/09/2026
 * ------- ---------- --------
 */

/*----------------- ----------------------------------- System Include ------------------*/
#include <avr/io.h>
#include <stddef.h>
#include <util/delay.h>

/*----------------- ------------------------------------ Local Include ------------------*/
#include "i2c.h"
#include "trace.h"

/*--------------- ------------------------------ Private define constants ---------------*/
#define I2C_STATUS_COMPLETE_START          0x08  // A START condition has been transmitted
#define I2C_STATUS_COMPLETE_REPEATED_START 0x10  // A repeated START condition has been transmitted
#define I2C_STATUS_TX_SLA_ACK              0x18  // SLA+W has been transmitted and ACK has been received
#define I2C_STATUS_TX_DATA_ACK             0x28  // Data byte has been transmitted and ACK has been received
#define I2C_STATUS_RX_SLA_ACK              0x40  // SLA+R has been transmitted and ACK has been received
#define I2C_STATUS_RX_DATA_ACK             0x50  // Data byte has been received, ACK has been returned
#define I2C_STATUS_RX_DATA_NACK            0x58  // Data byte has been received, NACK has been returned
#define I2C_TWBR_MAX                       255   // Maximum value for the TWI Bit Rate Register

/*----------------- ----------------------------------- Private macros ------------------*/

/*--------------- ------------------------------ Private type definitions ---------------*/

/*----------------- ---------------------------------- Static variables -----------------*/

/*----------- ---------------------- Private function prototypes declarations -----------*/
static enStatus I2C_Wait(uint16_t timeout);
static enStatus I2C_SendData(uint8_t data, uint8_t expectStatus, uint16_t timeout);
static enStatus I2C_Start(uint16_t timeout);
static enStatus I2C_Stop(uint16_t timeout);
static uint8_t  I2C_GetStatus(void);

/*-------------- ---------------------------- Private functions definition --------------*/
static enStatus I2C_Wait(uint16_t timeout)
{
    for (uint16_t i = 0; i < timeout; i++)
    {
        /* Check if the I2C Interrupt Flag (TWINT) is set (hardware finished current job)*/
        if (TWCR & (1 << TWINT))
        {
            return eSUCCESS;
        }
        _delay_ms(1);
    }

    return eFAIL;
}

static enStatus I2C_SendData(uint8_t data, uint8_t expectStatus, uint16_t timeout)
{
    /* Load data */
    TWDR = data;

    /* Clear TWINT to start transmission of data*/
    SET_BIT(TWCR, TWINT);

    if (I2C_Wait(timeout) == eFAIL)
    {
        trace_error();
        return eFAIL;
    }

    /* Get I2C status code*/
    uint8_t status = I2C_GetStatus();

    return (status == expectStatus) ? eSUCCESS : eFAIL;
}

static enStatus I2C_Start(uint16_t timeout)
{
    /* Set TWSTA (START condition), TWEN (TWI Enable), and TWINT (Clear interrupt flag to trigger start)*/
    TWCR = (1 << TWSTA) | (1 << TWEN) | (1 << TWINT);

    /* Wait for the START condition to be transmitted*/
    if (I2C_Wait(timeout) == eFAIL)
    {
        trace_error();
        return eFAIL;
    }

    /* Read status code from I2C Status Register*/
    uint8_t status = I2C_GetStatus();
    return (status == I2C_STATUS_COMPLETE_START ||
            status == I2C_STATUS_COMPLETE_REPEATED_START)
               ? eSUCCESS
               : eFAIL;
}

static enStatus I2C_Stop(uint16_t timeout)
{
    /* Transmit STOP condition by setting TWSTO, along with TWINT and TWEN*/
    TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);

    /* Wait until the hardware clears the TWSTO bit (indicating STOP is complete)*/
    for (uint16_t i = 0; i < timeout; i++)
    {
        /* Check if the I2C Interrupt Flag (TWINT) is set (hardware finished current job)*/
        if ((TWCR & (1 << TWSTO)) == 0)
        {
            return eSUCCESS;
        }
        _delay_ms(1);
    }

    return eFAIL;
}

static uint8_t I2C_GetStatus(void)
{
    /* Mask the prescaler bits (lower 3 bits) to extract the I2C status code */
    return (TWSR & 0xF8);
}

/*-------------- ----------------------------- Export functions definition --------------*/
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

        /* Set prescaler to 1 (TWPS1 = 0, TWPS0 = 0)*/
        TWSR = 0x00;

        /* Calculate the I2C Bit Rate Register (TWBR) value based on CPU frequency and desired clock*/
        uint32_t twbr = (F_CPU / i2cConfig->clock - 16UL) / 2UL;

        if (twbr > I2C_TWBR_MAX)
        {
            return eFAIL;
        }
        TWBR = twbr;
    }
    else
    {
        /* Set slave address and shift left by 1 (bit 0 is general call recognition enable bit)*/
        TWAR = i2cConfig->address << 1;
    }

    return eSUCCESS;
}

void I2C_Enable(bool enable)
{
    (enable) ? SET_BIT(TWCR, TWEN) : CLEAR_BIT(TWCR, TWEN);
}

enStatus I2C_Write(uint8_t address, const uint8_t* data, uint16_t size, uint16_t timeout)
{
    enStatus status = eFAIL;

    if (data == NULL || size == 0)
    {
        trace_error();
        return eFAIL;
    }

    /* Trigger I2C start condition */
    if (I2C_Start(timeout) == eFAIL)
    {
        trace_error();
        goto exit;
    }

    /* Send slave address + write request (bit 0 = 0) and expect ACK */
    if (I2C_SendData(address << 1 | 0, I2C_STATUS_TX_SLA_ACK, timeout) == eFAIL)
    {
        trace_error();
        goto exit;
    }

    /* Sequentially write data bytes to the slave */
    for (uint16_t i = 0; i < size; i++)
    {
        if (I2C_SendData(data[i], I2C_STATUS_TX_DATA_ACK, timeout) == eFAIL)
        {
            trace_error();
            goto exit;
        }
    }

    status = eSUCCESS;

exit:
    /* Ensure STOP condition is always issued before exiting */
    I2C_Stop(timeout);

    return status;
}

enStatus I2C_Read(uint8_t address, uint8_t* data, uint16_t size, uint16_t timeout)
{
    enStatus status = eFAIL;

    if (data == NULL || size == 0)
    {
        return eFAIL;
    }

    /* Trigger I2C start condition */
    if (I2C_Start(timeout) == eFAIL)
    {
        trace_error();
        goto exit;
    }

    /* Send slave address + read request (bit 0 = 1) and expect ACK */
    if (I2C_SendData(address << 1 | 1, I2C_STATUS_RX_SLA_ACK, timeout) == eFAIL)
    {
        trace_error();
        goto exit;
    }

    /* Read bytes sequentially */
    for (uint16_t i = 0; i < size; i++)
    {
        /* Configure ACK/NACK response based on whether it is the last byte*/
        uint8_t expectStatus = 0x00;
        if (i < size - 1)
        {
            expectStatus = I2C_STATUS_RX_DATA_ACK;

            /* Send ACK after reception (more bytes expected)*/
            SET_BIT(TWCR, TWEA);
        }
        else
        {
            expectStatus = I2C_STATUS_RX_DATA_NACK;

            /* Send NACK after the final byte (signals end of read)*/
            CLEAR_BIT(TWCR, TWEA);
        }

        /* Trigger read operation */
        SET_BIT(TWCR, TWINT);

        if (I2C_Wait(timeout) == eFAIL)
        {
            trace_error();
            goto exit;
        }

        /* Verify that the correct status code (ACK or NACK received) is active*/
        uint8_t status = I2C_GetStatus();
        if (status != expectStatus)
        {
            trace_error();
            goto exit;
        }

        /* Read the received byte from the TWI Data Register*/
        data[i] = TWDR;
    }

    status = eSUCCESS;

exit:
    /* Ensure STOP condition is always issued before exiting */
    I2C_Stop(timeout);

    return status;
}
