#include <msp430.h>
#include "i2c.h"
#include "stdio.h"
#include "lcd16x2.h"
#include "EEProm.h"


/**
 * main.c
 */

unsigned char Temp[2];
char buff_tx[30];
char buff_rx[16];
unsigned char test;
unsigned char retVal;
char temp=0x00;
int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
    BCSCTL1 = CALBC1_1MHZ;      // Configure DCO
    DCOCTL = CALDCO_1MHZ;

    LCD_Init();
    LCD_Clear();
    LCD_Goto(2,0);
    LCD_PrintString("Nguyen Dang Tan");

    I2C_Init();
    while(1)
    {
        buff_tx[0]  =0;
        buff_tx[1]  =1;
        buff_tx[2]  =2;
        buff_tx[3]  =3;
        buff_tx[4]  =4;

        I2C_Write_EEProm(0x00,0x00,buff_tx,sizeof(buff_tx));
        delay_ms(10);
        I2C_Read_EEProm(0x00,0x00,buff_rx,sizeof(buff_rx));
        delay_ms(10);


    }
}
