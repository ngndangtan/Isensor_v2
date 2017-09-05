#ifndef DS1307_H_
#define DS1307_H_

//USCI I2C PIN Definition
#define   SDA_PIN     BIT7      //Bit 7 USCI Port 1(SDA)
#define   SCL_PIN     BIT6      //Bit 6 USCI Port 1(SCL)
#define     DS1307_ADDRESS  0x68

#define     DS1307_SECONDS_ADDRESS  0x00
#define     DS1307_MINUTES_ADDRESS  0x01
#define     DS1307_HOURS_ADDRESS    0x02
#define     DS1307_DAY_ADDRESS      0x03
#define     DS1307_DATE_ADDRESS     0x04
#define     DS1307_MONTH_ADDRESS    0x05
#define     DS1307_YEAR_ADDRESS     0x06
#define     DS1307_CONTROL_ADDRESS  0x07


/******************************************************************************\
*                   Struct                                                      *
\******************************************************************************/
struct Times {
    //seconds 0x00
    unsigned char Seconds;
    //minute 0x01
    unsigned char Minutes;
    //hours 0x02
    unsigned char Hours;
    //day
    unsigned char Day;
    unsigned char Date;
    unsigned char Month;
    int Year;
};

/******************************************************************************\
*                   Prototype                                 *
\******************************************************************************/

//void I2C_Init(unsigned char addr);
void I2C_Set_Address(unsigned char addr);
unsigned char I2C_DS1307_Write(unsigned char slave_address, unsigned int memory_address, char * data, unsigned char DataLength );
unsigned char I2C_DS1307_Read(unsigned char slave_address, unsigned int memory_address, char * data, unsigned char DataLength );
unsigned char reset_time(void);
unsigned char undate_seconds( char *data );
unsigned char undate_minutes( char *data );
unsigned char undate_hours( char *data );
unsigned char undate_day( char *data );
unsigned char undate_date( char *data );
unsigned char undate_month( char *data );
unsigned char undate_year( char *data );
/******************************************************************************\
*                   Function                            *
\******************************************************************************/


struct Times Times_st={0x00,0x00,0x00,0x00,0x00,0x00,0};
/*
void I2C_Init(void)
{
    P1SEL |= SDA_PIN + SCL_PIN;                     // Assign I2C pins to USCI_B0
    P1SEL2|= SDA_PIN + SCL_PIN;
    UCB0CTL1 = UCSWRST;                             // Enable SW reset, HOLD USCB in a reset state
    UCB0CTL0 = UCMST + UCMODE_3 + UCSYNC;           // I2C Master, MODE 3 = I2C, synchronous mode
    UCB0CTL1 = UCSSEL_2 + UCSWRST;                  // Use SMCLK, keep SW reset
    UCB0BR0 = 72;                                   // Set I2C master speed  72 gives approx 200Khz clock at 16Mhz
    UCB0BR1 = 0;                                    // Set I2C master speed
    UCB0CTL1 &= ~UCSWRST;                           // Clear SW reset, resume operation

}
*/
void I2C_Set_Address(unsigned char addr)
{
    UCB0CTL1 |= UCSWRST;
    UCB0I2CSA = addr;                           // Set slave address
    UCB0CTL1 &= ~UCSWRST;                       // Clear SW reset, resume operation

}


unsigned char I2C_DS1307_Write(unsigned char slave_address, unsigned int memory_address, char * data, unsigned char DataLength )
{
    int offsetlimit = 0;
    int offset = 0;
    int address = memory_address; //EEprom memory starting address, this is different from the I2C slave address

    offsetlimit=0x3f-memory_address+1;
    if (offsetlimit >= DataLength) offsetlimit=DataLength;
    else {
        //error
        return 0;
    }

    UCB0I2CSA=slave_address;
    while (UCB0STAT & UCBBUSY);     //wait for USCI B0 bus to be inactive
    UCB0CTL1 |= UCTR + UCTXSTT;         //set USCI to be I2C TX,  send start condition
    UCB0TXBUF = address;
    while (UCB0CTL1 & UCTXSTT);                 // waiting for slave address was transferred
    while ((IFG2 & UCB0TXIFG) != UCB0TXIFG);        //wait for TX IFG to clear
    for( offset = 0;offset<offsetlimit;offset++)
    {
        UCB0TXBUF = data[offset];
        while (UCB0CTL1 & UCTXSTT);                 // waiting for slave address was transferred
        while ((IFG2 & UCB0TXIFG) != UCB0TXIFG);        //wait for TX IFG to clear
    }
    UCB0CTL1 |= UCTXSTP;                    // set I2C stop condition
    while ((UCB0STAT & UCSTPIFG) == UCSTPIFG); //wait for Stop condition to be set

    return 1;
}
unsigned char I2C_DS1307_Read(unsigned char slave_address, unsigned int memory_address, char * data, unsigned char DataLength )
{
    int rLoop = 0;  //loop counter

    UCB0CTL1 &=~ UCTXNACK;
    UCB0I2CSA = slave_address;
    while (UCB0STAT & UCBBUSY);     //wait for USCI B0 bus to be inactive


    UCB0CTL1 |= UCTR + UCTXSTT;         //set USCI to be I2C TX,  send start condition
    UCB0TXBUF = memory_address;
    while (UCB0CTL1 & UCTXSTT);                     // waiting for slave address to transfer
    while ((IFG2 & UCB0TXIFG) != UCB0TXIFG);        //wait for TX IFG to clear

    UCB0CTL1 &= ~UCTR;                      // I2C RX
    UCB0CTL1 |= UCTXSTT;                    // I2C start condition
    UCB0TXBUF =(slave_address<<1)|0x01;

    while (UCB0CTL1 & UCTXSTT);                     // waiting for slave address to transfer
    for (rLoop=0; rLoop<DataLength; rLoop++)    //receive loop
    {
         while ((IFG2 & UCB0RXIFG) != UCB0RXIFG); //wait for RX buffer to have data
            data[rLoop] = UCB0RXBUF;  //Move rvcd data of or USCI buffer. This also clears the UCB0RXIFG flag

            if (rLoop == DataLength-2)  //NACK and STOP must be send before the last byte is read from the buffer.
                                        //if not the CPU will read an extra byte.
            {

                UCB0CTL1 |= UCTXNACK; //generate a NACK
                UCB0CTL1 |= UCTXSTP;  //generate a stop condition
                while (UCB0CTL1 & UCTXSTT);                     // waiting for slave address to transfer
            }
    }

    return 1;
}
unsigned char reset_time(void)
{
    char Reset_var=0;
    undate_seconds(&Reset_var);
    undate_minutes(&Reset_var);
    undate_hours(&Reset_var);
    undate_date(&Reset_var);
    undate_month(&Reset_var);
    undate_year(&Reset_var);
    return 1;
}


unsigned char undate_seconds( char *data )
{
    I2C_DS1307_Write(DS1307_ADDRESS,DS1307_SECONDS_ADDRESS,data,1);
    return 1;
}
unsigned char undate_minutes( char *data )
{
    I2C_DS1307_Write(DS1307_ADDRESS,DS1307_MINUTES_ADDRESS,data,1);
    return 1;
}
unsigned char undate_hours( char *data )
{
    I2C_DS1307_Write(DS1307_ADDRESS,DS1307_HOURS_ADDRESS,data,1);
    return 1;
}
unsigned char undate_day( char *data )
{
    I2C_DS1307_Write(DS1307_ADDRESS,DS1307_DAY_ADDRESS,data,1);
    return 1;
}
unsigned char undate_date( char *data )
{
    I2C_DS1307_Write(DS1307_ADDRESS,DS1307_DATE_ADDRESS,data,1);
    return 1;
}
unsigned char undate_month( char *data )
{
    I2C_DS1307_Write(DS1307_ADDRESS,DS1307_MONTH_ADDRESS,data,1);
    return 1;
}
unsigned char undate_year( char *data )
{
    I2C_DS1307_Write(DS1307_ADDRESS,DS1307_YEAR_ADDRESS,data,1);
    return 1;
}
#endif /* DS1307_H_ */
