#ifndef TC75_H_
#define TC75_H_
#include    "i2c.h"
char I2C_Read_TC75(unsigned char slave_address, unsigned int memory_address, char * data, unsigned char DataLength );
char I2C_Write_TC75(unsigned char slave_address, unsigned int memory_address, char * data, unsigned char DataLength );


char I2C_Read_TC75(unsigned char slave_address, unsigned int memory_address, char * data, unsigned char DataLength )
{
    char check_temp=0;
    int rLoop = 0;  //loop counter
    if (DataLength >2) DataLength=2;
    UCB0CTL1 &=~ UCTXNACK;
    UCB0I2CSA=slave_address;
    check_temp=wait_active();
    if(check_temp!=E_OK) return check_temp;
    I2C_Start_Bit();
    UCB0TXBUF=memory_address;//= D1 D0 nomarly = 0x00 to read TEMP regedit
                             //= D1 D0 nomarly = 0x01 to read CONFIG regedit
                             //= D1 D0 nomarly = 0x00 to read THYST regedit
                             //= D1 D0 nomarly = 0x01 to read TSET regedit
    check_temp=wait_transmitter();
    if(check_temp!=E_OK) return check_temp;

    UCB0CTL1 &= ~UCTR;                      // I2C RX
    UCB0CTL1 |= UCTXSTT;                    // I2C start condition

    UCB0TXBUF =(slave_address<<1)|0x01;
    check_temp=wait_slave_address_transfer();
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
                check_temp=wait_slave_address_transfer();
                if(check_temp!=E_OK) return check_temp;
            }
    }
    return E_OK;
}
char I2C_Write_TC75(unsigned char slave_address, unsigned int memory_address, char * data, unsigned char DataLength )
{

    int offset = 0;
    int address = memory_address; //EEprom memory starting address, this is different from the I2C slave address
    char check_temp=0;

    UCB0I2CSA=slave_address;
    check_temp=wait_active();
    if(check_temp!=E_OK) return check_temp;
    I2C_Start_Bit();
    UCB0TXBUF = address;
    check_temp=wait_transmitter();
    if(check_temp!=E_OK) return check_temp;
    for( offset = 0;offset<DataLength;offset++)
    {
        UCB0TXBUF = data[offset];
        check_temp=wait_transmitter();
        if(check_temp!=E_OK) return check_temp;
    }
    I2C_Stop_Bit();
    check_temp=wait_stop();
    if(check_temp!=E_OK) return check_temp;

    return E_OK;
}


#endif /* TC75_H_ */
