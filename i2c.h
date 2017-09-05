/*
 * i2c.h
 *
 *  Created on: Aug 4, 2017
 *      Author: thinhpc
 */

#ifndef I2C_H_
#define I2C_H_

//TCN75 Address
#define TCN75_ADDRESS   0x90 // addr 0x48<<1 (include write bit)
//#define TCN_CONF    0xE1

/*******************************
 *PROTOTYPE
 ******************************/
extern void I2C_USCI_Init(unsigned char addr); // I2C initialize
extern void I2C_USCI_Set_Address(unsigned char addr);  // Thay doi Sia chi Slave
extern unsigned char I2C_USCI_Read_Byte(unsigned char address);    // Read 1 Byte
extern unsigned char I2C_USCI_Read_Word(unsigned char Addr_Data,unsigned char *Data, unsigned char Length); //Read data word
extern unsigned char I2C_USCI_Write_Byte(unsigned char address, unsigned char data);
extern unsigned char I2C_USCI_Write_Word(unsigned char address, unsigned char pointer, unsigned char data);
extern void I2C_Start_Bit(void);
extern void I2C_Stop_Bit(void);

#endif /* I2C_H_ */
