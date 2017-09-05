#include <msp430.h>
#include "stdio.h"
#include "lcd16x2.h"
#include "EEProm.h"
#include "UART.h"
#include "DS1307.h"

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
    LCD_PrintString("Nguyen_Dang_Tan");

    Uart_Init();
    I2C_Init();

    I2C_DS1307_Write(DS1307_ADDRESS,DS1307_CONTROL_ADDRESS,0x10,1);
    while(1)
    {
        char buff[255]="hello i am Tan \r\n";
        UartStrPut(buff);
        //I2C_Write_EEProm(0x00,0x00,buff_tx,sizeof(buff_tx));
        //delay_ms(10);
        //I2C_Read_EEProm(0x00,0x00,buff_rx,sizeof(buff_rx));
        I2C_DS1307_Read(DS1307_ADDRESS,0x00,buff_rx,6);
        delay_ms(10);
        UartStrPut(buff_rx);
        //UartCharPut('\r');
        //UartCharPut('\n');


    }
}
