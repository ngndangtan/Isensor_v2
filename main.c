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
long count;
extern struct Times Times_st;
unsigned char Update_time_to_LCD(void);
unsigned char Get_time_form_DS1307(void);
unsigned char Display_time_to_LCD(void);
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

    temp=0x10;
    I2C_DS1307_Write(DS1307_ADDRESS,DS1307_CONTROL_ADDRESS,&temp,1);
    buff_tx[0]=10;
    buff_tx[1]=9;
    buff_tx[2]=8;
    buff_tx[3]=7;
    buff_tx[4]=6;
    buff_tx[5]=5;
    buff_tx[6]=4;
    buff_tx[7]=3;
    buff_tx[8]=2;
    buff_tx[9]=1;
    I2C_Write_EEProm(0x00,0x0a,buff_tx,10);

    while(1)
     {

         if(count++ ==1000)
         {
             count=0;
             I2C_Read_EEProm(0x0,0x0a,buff_rx,10);
             UartCharPut(buff_rx[0]);
             UartCharPut(buff_rx[1]);
             UartCharPut(buff_rx[2]);
             UartCharPut(buff_rx[3]);
             UartCharPut(buff_rx[4]);
             UartCharPut(buff_rx[5]);
             UartCharPut(buff_rx[6]);
             UartCharPut(buff_rx[7]);
             UartCharPut(buff_rx[8]);
             UartCharPut(buff_rx[9]);


             //buff_tx[4]++;
             //I2C_Write_EEProm(0x0,0x00,buff_tx,5);
             /*
             Get_time_form_DS1307();
             UartCharPut(Times_st.Seconds);
             UartCharPut(':');
             UartCharPut(Times_st.Minutes);
             UartCharPut(':');
             UartCharPut(Times_st.Hours);
             UartCharPut('\r');
             UartCharPut('\n');

             UartCharPut(Times_st.Day);
             UartCharPut(',');
             UartCharPut(Times_st.Date);
             UartCharPut('-');
             UartCharPut(Times_st.Month);
             UartCharPut('-');
             UartCharPut(Times_st.Year);
             UartCharPut('\r');
             UartCharPut('\n');
             UartCharPut('\r');
             UartCharPut('\n');
             */

         }
     }
 }
 unsigned char Get_time_form_DS1307(void)
 {
     char data[7];
     I2C_DS1307_Read(DS1307_ADDRESS,0x00,data,7);
     Times_st.Seconds    = ((data[0]&0x70)>>4)*10+(data[0]&0x0F);
     Times_st.Minutes    = ((data[1]&0x70)>>4)*10+(data[1]&0x0F);
     Times_st.Hours      = ((data[2]&0x30)>>4)*10+(data[2]&0x0F);
     Times_st.Day        = data[3]&0x07;
     Times_st.Date       = ((data[4]&0x30)>>4)*10+(data[4]&0x0F);
     Times_st.Month      = ((data[5]&0x10)>>4)*10+(data[5]&0x0F);
     Times_st.Year       = 2000+((data[6]&0xF0)>>4)*10+(data[6]&0x0F);

     return 1;
 }
 unsigned char Display_time_to_LCD(void)
 {
     LCD_Clear();
     LCD_Goto(1,0);
     LCD_PrintUnDecimal(Times_st.Day,1);
     LCD_PrintString(",");
     LCD_PrintUnDecimal(Times_st.Date,2);
     LCD_PrintString("-");
     LCD_PrintUnDecimal(Times_st.Month,2);
     LCD_PrintString("-");
     LCD_PrintUnDecimal(Times_st.Year,4);
     LCD_Goto(2,0);
     LCD_PrintUnDecimal(Times_st.Seconds,2);
     LCD_PrintString(":");
     LCD_PrintUnDecimal(Times_st.Minutes,2);
     LCD_PrintString(":");
     LCD_PrintUnDecimal(Times_st.Hours,2);
     return 1;
 }
