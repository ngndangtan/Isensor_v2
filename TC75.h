#ifndef TC75_H_
#define TC75_H_
#include    "i2c.h"
char I2C_Read_TC75(unsigned char slave_address, unsigned int memory_address, char * data, unsigned char DataLength );
char I2C_Write_TC75(unsigned char slave_address, unsigned int register_address, char * data, unsigned char DataLength );


char I2C_Read_TC75(unsigned char slave_address, unsigned int memory_address, char * data, unsigned char DataLength )
{
    return E_OK;
}
char I2C_Write_TC75(unsigned char slave_address, unsigned int register_address, char * data, unsigned char DataLength )
{
    return E_OK;
}


#endif /* TC75_H_ */
