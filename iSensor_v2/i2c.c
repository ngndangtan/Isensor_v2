/*
 * i2c.c
 *
 *  Created on: Aug 4, 2017
 *      Author: thinhpc
 */


#include <msp430.h>
#include "i2c.h"


void I2C_USCI_Init(unsigned char addr)
{
    P1SEL |= BIT6 + BIT7;                   /* Assign I2C pins to UCB0SCL, UCB0SDA */
    P1SEL2|= BIT6 + BIT7;                   /* Assign I2C pins to UCB0SCL, UCB0SDA */

    UCB0CTL1 |= UCSWRST;                    /* Enable SW reset to config I2C       */
    UCB0CTL0 = UCMST+UCMODE_3+UCSYNC;       /* I2C Master, synchronous mode        */
    UCB0CTL1 = UCSSEL_2+UCSWRST;            /* Use SMCLK, keep SW reset            */
    UCB0BR0 = 40;                           /* fSCL = SMCLK/40 = ~400kHz           */
    UCB0BR1 = 0;

    UCB0I2CSA = addr>>1;                    /* Set slave address 0x48              */
    UCB0CTL1 &= ~UCSWRST;                   /* Clear SW reset, resume operation    */
}

void I2C_Start_Bit(void)
{
    UCB0CTL1 |= UCTR + UCTXSTT;     // I2C as Transmitter, Generate START condition
}

void I2C_Stop_Bit(void)
{
    UCB0CTL1 |= UCTR + UCTXNACK+ UCTXSTP;     // I2C as Transmitter, Generate STOP condition
}

unsigned char I2C_USCI_Write_Byte(unsigned char address, unsigned char data)
{
    UCB0CTL1 |= UCTR;                       // I2C as TX
    UCB0TXBUF = address;                    // Send Addr to slave
    if(UCB0STAT & UCNACKIFG) return 1;      // Return Error

    UCB0TXBUF = data;                    // Send pointer
    if(UCB0STAT & UCNACKIFG) return 1;      // Return Error

    return 0;
}

unsigned char I2C_USCI_Write_Word(unsigned char address, unsigned char pointer, unsigned char data)
{
    UCB0CTL1 |= UCTR;                       // I2C as TX
    UCB0TXBUF = address;                    // Send Addr to slave
    if(UCB0STAT & UCNACKIFG) return 1;      // Return Error

    UCB0TXBUF = pointer;                    // Send pointer
    if(UCB0STAT & UCNACKIFG) return 1;      // Return Error

    __delay_cycles(1000);
    UCB0TXBUF = data;                    // Send pointer
    if(UCB0STAT & UCNACKIFG) return 1;      // Return Error

    return 0;
}

unsigned char I2C_USCI_Read_Word(unsigned char Addr_Data,unsigned char *Data, unsigned char Length)
{
    unsigned char i2c_index;

    UCB0CTL1 |= UCTR;                       // I2C as TX
    UCB0TXBUF = (Addr_Data|0x01);                  // Send Addr to slave
    if(UCB0STAT & UCNACKIFG) return 1;      // Return error

    UCB0CTL1 &= ~UCTR;                      // I2C RX
    for(i2c_index = Length; i2c_index > 0; i2c_index--)
    {
        Data[i2c_index] = UCB0RXBUF;
    }

    return 0;
}




