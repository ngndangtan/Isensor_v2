#ifndef DS1307_H_
#define DS1307_H_
#include    "i2c.h"
//USCI I2C PIN Definition
#define   SDA_PIN     BIT7      //Bit 7 USCI Port 1(SDA)
#define   SCL_PIN     BIT6      //Bit 6 USCI Port 1(SCL)


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



unsigned char I2C_DS1307_Write(unsigned char slave_address, unsigned int memory_address, char * data, unsigned char DataLength )
{
    int offsetlimit = 0;
    int offset = 0;
    int address = memory_address; //EEprom memory starting address, this is different from the I2C slave address
    char check_temp=0;

    offsetlimit=0x3f-memory_address+1;
    if (offsetlimit >= DataLength) offsetlimit=DataLength;
    else {
        //error
        return 0;
    }

    UCB0I2CSA=slave_address;
    check_temp=wait_active();
    if(check_temp!=E_OK) return check_temp;
    I2C_Start_Bit();
    UCB0TXBUF = address;
    check_temp=wait_transmitter();
    if(check_temp!=E_OK) return check_temp;
    for( offset = 0;offset<offsetlimit;offset++)
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
unsigned char I2C_DS1307_Read(unsigned char slave_address, unsigned int memory_address, char * data, unsigned char DataLength )
{
    char check_temp=0;
    int rLoop = 0;  //loop counter

    UCB0CTL1 &=~ UCTXNACK;
    UCB0I2CSA = slave_address;
    check_temp=wait_active();
    if(check_temp!=E_OK) return check_temp;


    I2C_Start_Bit();
    UCB0TXBUF = memory_address;
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
unsigned char reset_time(void)
{
    char Reset_var=0;
    undate_seconds(&Reset_var);
    undate_minutes(&Reset_var);
    undate_hours(&Reset_var);
    undate_date(&Reset_var);
    undate_month(&Reset_var);
    undate_year(&Reset_var);
    return E_OK;
}


unsigned char undate_seconds( char *data )
{
    I2C_DS1307_Write(DS1307_ADDRESS,DS1307_SECONDS_ADDRESS,data,1);
    return E_OK;
}
unsigned char undate_minutes( char *data )
{
    I2C_DS1307_Write(DS1307_ADDRESS,DS1307_MINUTES_ADDRESS,data,1);
    return E_OK;
}
unsigned char undate_hours( char *data )
{
    I2C_DS1307_Write(DS1307_ADDRESS,DS1307_HOURS_ADDRESS,data,1);
    return E_OK;
}
unsigned char undate_day( char *data )
{
    I2C_DS1307_Write(DS1307_ADDRESS,DS1307_DAY_ADDRESS,data,1);
    return E_OK;
}
unsigned char undate_date( char *data )
{
    I2C_DS1307_Write(DS1307_ADDRESS,DS1307_DATE_ADDRESS,data,1);
    return E_OK;
}
unsigned char undate_month( char *data )
{
    I2C_DS1307_Write(DS1307_ADDRESS,DS1307_MONTH_ADDRESS,data,1);
    return E_OK;
}
unsigned char undate_year( char *data )
{
    I2C_DS1307_Write(DS1307_ADDRESS,DS1307_YEAR_ADDRESS,data,1);
    return E_OK;
}
#endif /* DS1307_H_ */
