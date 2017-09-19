
#ifndef EEPROM_H_
#define EEPROM_H_

#include    "i2c.h"

/*******************************************************************************************/
char I2C_Write_EEProm(unsigned char slave_address, unsigned int register_address, char * data, unsigned char DataLength );
char I2C_Read_EEProm(unsigned char slave_address, unsigned int memory_address, char * data, unsigned char DataLength );

char I2C_Read_EEProm(unsigned char slave_address, unsigned int memory_address, char * data, unsigned char DataLength )
{ //Reading from a 24LCxxx series is much easier then writing.  Reading doesn't have to be done in 64 byte pages.
    /*
         * Todo:
         * 1. add checks to make sure write does not exceed maximum length of EEprom
         * 2. check for valid memory_address
         *
         */
    char check_temp;
    int rLoop = 0;  //loop counter
    int address = memory_address; //EEprom memory starting address, this is different from the I2C slave address
    UCB0CTL1 &=~ UCTXNACK;

    check_temp=wait_active();
    if(check_temp!=E_OK) return check_temp;

    UCB0I2CSA = 0b1010000|(slave_address);          //set SLAVE address
    I2C_Start_Bit();                     //set USCI to be I2C TX,  send start condition
    //UCB0TXBUF= 0b10100000|(slave_address<<1);
    //while (UCB0CTL1 & UCTXSTT);                     // waiting for slave address to transfer
   // while ((IFG2 & UCB0TXIFG) != UCB0TXIFG);        //wait for TX IFG to clear

    UCB0TXBUF = (address & 0xff00) >> 8;     //transfer memory_address MSB

    check_temp=wait_transmitter();
    if(check_temp!=E_OK) return check_temp;


    UCB0TXBUF = (address & 0x00ff);          //transfer memory_address LSB

    check_temp=wait_transmitter();
    if(check_temp!=E_OK) return check_temp;


    UCB0CTL1 &= ~UCTR;              //set USCI to be RECEIVER
    UCB0CTL1 |= UCTXSTT;            //send restart
    UCB0TXBUF= 0b10100001|(slave_address<<1);

    check_temp=wait_transmitter();
    if(check_temp!=E_OK) return check_temp;

    for (rLoop=0; rLoop<DataLength; rLoop++)    //receive loop
    {
            check_temp=wait_receiver();
            if(check_temp!=E_OK) return check_temp;
            data[rLoop] = UCB0RXBUF;  //Move rvcd data of or USCI buffer. This also clears the UCB0RXIFG flag
            if (rLoop == DataLength-2)  //NACK and STOP must be send before the last byte is read from the buffer.
                                        //if not the CPU will read an extra byte.
            {

                UCB0CTL1 |= UCTXNACK; //generate a NACK
                UCB0CTL1 |= UCTXSTP;  //generate a stop condition

                check_temp=wait_transmitter();
                if(check_temp!=E_OK) return check_temp;
            }
    }
    return E_OK;

}

char I2C_Write_EEProm(unsigned char slave_address, unsigned int memory_address, char * data, unsigned char DataLength )
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
            UCB0I2CSA = 0b1010000|slave_address;          //set SLAVE address
    for (NP=1; NP<=NumPages; NP++)
    {

        address = ((NP-1) * 128) + memory_address; //this is the full page start address
        UCB0CTL1 |= UCTR + UCTXSTT;         //set USCI to be I2C TX,  send start condition
        //UCB0TXBUF=0b10100000|(slave_address<<1);
        //while (UCB0CTL1 & UCTXSTT);                 // waiting for slave address was transferred
        //while ((IFG2 & UCB0TXIFG) != UCB0TXIFG);        //wait for TX IFG to clear


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

    return E_OK;
}





#endif /* EEPROM_H_ */
