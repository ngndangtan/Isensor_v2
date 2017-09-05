
#ifndef EEPROM_H_
#define EEPROM_H_

#include    "msp430g2553.h"
#include "lcd16x2.h"
//I2C Port Definitions
#define   i2cAddress     0x00    //Address GPIO Address

//USCI I2C PIN Definition
#define   SDA_PIN     BIT7      //Bit 7 USCI Port 1(SDA)
#define   SCL_PIN     BIT6      //Bit 6 USCI Port 1(SCL)
#define   NOACK 2
#define   WRITE 0
#define   READ 1
/*****************************************************************************************
 *
 */
int status_i2c=0;
extern char temp;

void I2C_Init(void);
void I2C_Write_EEProm(unsigned char slave_address, unsigned int register_address, char * data, unsigned char DataLength );
void I2C_Read_EEProm(unsigned char slave_address, unsigned int memory_address, char * data, unsigned char DataLength );

void delay_ms(unsigned int delay);          //Delay

void I2C_Init(void)
{
    P1SEL|= SDA_PIN + SCL_PIN;                     // Assign I2C pins to USCI_B0
    P1SEL2|= SDA_PIN + SCL_PIN;
    UCB0CTL1= UCSWRST;                             // Enable SW reset, HOLD USCB in a reset state
    UCB0CTL0= UCMST + UCMODE_3 + UCSYNC;           // I2C Master, MODE 3 = I2C, synchronous mode
    UCB0CTL1= UCSSEL_2 + UCSWRST;                  // Use SMCLK, keep SW reset
    UCB0BR0 = 72;                                   // Set I2C master speed  72 gives approx 200Khz clock at 16Mhz
    UCB0BR1 = 0;                                    // Set I2C master speed
    UCB0CTL1 &= ~UCSWRST;                           // Clear SW reset, resume operation

}

void delay_ms(unsigned int delay)
{
    while (delay--)
    {
        __delay_cycles(1000);  //1ms = 1000 cycles per 1Mhz clock freq.
    }
}

void I2C_Read_EEProm(unsigned char slave_address, unsigned int memory_address, char * data, unsigned char DataLength )
{ //Reading from a 24LCxxx series is much easier then writing.  Reading doesn't have to be done in 64 byte pages.
    /*
         * Todo:
         * 1. add checks to make sure write does not exceed maximum length of EEprom
         * 2. check for valid memory_address
         *
         */
    int rLoop = 0;  //loop counter
    int address = memory_address; //EEprom memory starting address, this is different from the I2C slave address
    UCB0CTL1 &=~ UCTXNACK;

    while (UCB0STAT & UCBBUSY);         //wait for USCI B0 bus to be inactive
    UCB0I2CSA = 0b1010000|(slave_address<<1);          //set SLAVE address
    UCB0CTL1 |= UCTR + UCTXSTT;                     //set USCI to be I2C TX,  send start condition
    UCB0TXBUF= 0b10100000|(slave_address<<1);
    while (UCB0CTL1 & UCTXSTT);                     // waiting for slave address to transfer
    while ((IFG2 & UCB0TXIFG) != UCB0TXIFG);        //wait for TX IFG to clear

    UCB0TXBUF = (address & 0xff00) >> 8;     //transfer memory_address MSB
    while (UCB0CTL1 & UCTXSTT);                     // waiting for slave address to transfer
    while ((IFG2 & UCB0TXIFG) != UCB0TXIFG);        //wait for TX IFG to clear

    UCB0TXBUF = (address & 0x00ff);          //transfer memory_address LSB
    while (UCB0CTL1 & UCTXSTT);                     // waiting for slave address to transfer
    while ((IFG2 & UCB0TXIFG) != UCB0TXIFG);        //wait for TX IFG to clear



    UCB0CTL1 &= ~UCTR;              //set USCI to be RECEIVER
    UCB0CTL1 |= UCTXSTT;            //send restart
    UCB0TXBUF= 0b10100001|(slave_address<<1);

    while (UCB0CTL1 & UCTXSTT);                     // waiting for slave address to transfer
    for (rLoop=0; rLoop<DataLength+1; rLoop++)    //receive loop
    {
         while ((IFG2 & UCB0RXIFG) != UCB0RXIFG); //wait for RX buffer to have data
            data[rLoop] = UCB0RXBUF;  //Move rvcd data of or USCI buffer. This also clears the UCB0RXIFG flag

            if (rLoop == DataLength-1)  //NACK and STOP must be send before the last byte is read from the buffer.
                                        //if not the CPU will read an extra byte.
            {

                UCB0CTL1 |= UCTXNACK; //generate a NACK
                UCB0CTL1 |= UCTXSTP;  //generate a stop condition
                while (UCB0CTL1 & UCTXSTT);                     // waiting for slave address to transfer
            }
    }

}

void I2C_Write_EEProm(unsigned char slave_address, unsigned int memory_address, char * data, unsigned char DataLength )
{
    /*
     * Todo:
     * 1. add checks to make sure write does not exceed maximum length of EEprom
     * 2. check for valid memory_address
     *
     */
    int NumPages = (DataLength)/128 ; //Count of full 64 byte pages, 0 means the data is less than a full 64 byte page
    int PartialPageBytes = DataLength % 128;  //this is the number of bytes remaining that do not make up a full page

    int address = memory_address; //EEprom memory starting address, this is different from the I2C slave address
    int NP =1; //loop counter to iterate though pages of memory
    int offset = 0;
    int offsetlimit = 0;

    if (PartialPageBytes > 0)
    {
        NumPages++; //if we have left over bytes that do not make up a full page, this will add a page to handle those bytes.
    }

    __disable_interrupt(); //prevent interrupts from messing with the I2C functions

    while (UCB0STAT & UCBBUSY);     //wait for USCI B0 bus to be inactive
            UCB0I2CSA = 0b1010000|(slave_address<<1);          //set SLAVE address
    for (NP=1; NP<=NumPages; NP++)
    {

        address = ((NP-1) * 128) + memory_address; //this is the full page start address
        UCB0CTL1 |= UCTR + UCTXSTT;         //set USCI to be I2C TX,  send start condition
        UCB0TXBUF=0b10100000|(slave_address<<1);
        while (UCB0CTL1 & UCTXSTT);                 // waiting for slave address was transferred
        while ((IFG2 & UCB0TXIFG) != UCB0TXIFG);        //wait for TX IFG to clear


        UCB0TXBUF = (address & 0xff00) >> 8;        //transferring memory_address MSB
        while (UCB0CTL1 & UCTXSTT);                 // waiting for slave address was transferred
        while ((IFG2 & UCB0TXIFG) != UCB0TXIFG);        //wait for TX IFG to clear

        UCB0TXBUF = (address & 0x00ff);     //transferring memory_address LSB
        while (UCB0CTL1 & UCTXSTT);                 // waiting for slave address was transferred
        while ((IFG2 & UCB0TXIFG) != UCB0TXIFG);        //wait for TX IFG to clear

        offsetlimit = 127;       //set the offset limit to 127

        if ((NP == NumPages) && (PartialPageBytes > 0))     //if we are preparing to send the last/partial page
        {
            offsetlimit = PartialPageBytes-1;               //set the offset limit to the number of partial page bytes
        }
        for (offset=0; offset <=offsetlimit; offset++)
        {
            UCB0TXBUF = data[((NP-1)*128)+offset];                   //send data.
            //UART_outdec(offset,0);
            while (UCB0CTL1 & UCTXSTT);                 // waiting for slave address was transferred
            while ((IFG2 & UCB0TXIFG) != UCB0TXIFG);        //wait for TX IFG to clear
        }

        UCB0CTL1 |= UCTXSTP;                    // set I2C stop condition
        //UCB0CTL1 |= UCTR + UCTXNACK+ UCTXSTP;
        while ((UCB0STAT & UCSTPIFG) == UCSTPIFG); //wait for Stop condition to be set

                        //delay while the EEPROM completed its write cycle.
                        //It would be better to use NACK polling here as described in the datasheet.
        delay_ms(10);    //24LC256 has a max page write time of 5ms, so we will wait 6ms to be sure
    }


}





#endif /* EEPROM_H_ */
