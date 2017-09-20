
#ifndef I2C_H_
#define I2C_H_
#include    "msp430g2553.h"
#define     DS1307_ADDRESS  0x68
#define     TC75_ADDRESS    0x48|0x00
#define     EEPROM_ADDRESS  0x50|0x00

#define   SDA_PIN     BIT7      //Bit 7 USCI Port 1(SDA)
#define   SCL_PIN     BIT6      //Bit 6 USCI Port 1(SCL)

#define   E_OK      0xFF
#define   ERR1      0x01
#define   ERR2      0x02
#define   ERR3      0x03
#define   ERR4      0x04
#define   ERR5      0x05
#define   TIME_OF_TRANSMITTER      5000//5ms
#define   TIME_OF_RECEIVER      5000//5ms
#define   TIME_OF_ACTIVE      5000//5ms
#define   TIME_OF_SLAVE_ADDRESS_TRANSFER      5000//5ms
#define   TIME_OF_STOP      5000//5ms


void I2C_Init(void)
{
    P1SEL|= SDA_PIN + SCL_PIN;                     // Assign I2C pins to USCI_B0
    P1SEL2|= SDA_PIN + SCL_PIN;
    //P1REN|=SDA_PIN + SCL_PIN;
    UCB0CTL1= UCSWRST;                             // Enable SW reset, HOLD USCB in a reset state
    UCB0CTL0= UCMST + UCMODE_3 + UCSYNC;           // I2C Master, MODE 3 = I2C, synchronous mode
    UCB0CTL1= UCSSEL_2 + UCSWRST;                  // Use SMCLK, keep SW reset
    UCB0BR0 = 72;                                   // Set I2C master speed  72 gives approx 200Khz clock at 16Mhz
    UCB0BR1 = 0;                                    // Set I2C master speed
    UCB0CTL1 &= ~UCSWRST;                           // Clear SW reset, resume operation

}
void I2C_Set_Address(unsigned char addr)
{
    UCB0CTL1 |= UCSWRST;
    UCB0I2CSA = addr;                           // Set slave address
    UCB0CTL1 &= ~UCSWRST;                       // Clear SW reset, resume operation

}

void delay_ms(unsigned int delay)
{
    while (delay--)
    {
        __delay_cycles(1000);  //1ms = 1000 cycles per 1Mhz clock freq.
    }
}

void I2C_Start_Bit(void)
{
    UCB0CTL1 |= UCTR + UCTXSTT;     // I2C as Transmitter, Generate START condition
}

void I2C_Stop_Bit(void)
{
    UCB0CTL1 |= UCTR + UCTXNACK+ UCTXSTP;     // I2C as Transmitter, Generate STOP condition
}

char wait_transmitter()
{
    int count;
    for(count=0;count<TIME_OF_TRANSMITTER;count++)
        {
            if(!(UCB0CTL1 & UCTXSTT)) // waiting for slave address to transfer
                {
                for(count=0;count<TIME_OF_TRANSMITTER;count++)
                    {
                        if((IFG2 & UCB0TXIFG) == UCB0TXIFG) return E_OK; //wait for TX IFG to clear
                    }
                    return ERR2;
                }
        }
        return ERR1;
}

char wait_receiver()
{
    int count;
    for(count=0;count<TIME_OF_RECEIVER;count++)
        {
            if((IFG2 & UCB0RXIFG) == UCB0RXIFG) // waiting for slave address to transfer
                {
                    return E_OK;
                }
        }
        return ERR3;
}
char wait_active()
{
    int count;
    for(count=0;count<TIME_OF_ACTIVE;count++)
        {
            if(!(UCB0STAT & UCBBUSY))
                {
                    return E_OK;
                }
        }
        return ERR4;
}
char wait_slave_address_transfer()
{
    int count;
    for(count=0;count<TIME_OF_SLAVE_ADDRESS_TRANSFER;count++)
        {
            if(!(UCB0CTL1 & UCTXSTT)) // waiting for slave address to transfer
                {
                    return E_OK;
                }
        }
        return ERR5;
}
char wait_stop()
{
    int count;
    for(count=0;count<TIME_OF_STOP;count++)
        {
            if((UCB0STAT & UCSTPIFG) != UCSTPIFG) //wait for Stop condition to be set
                {
                    return E_OK;
                }
        }
        return ERR5;
}

#endif /* I2C_H_ */
