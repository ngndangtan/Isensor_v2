#ifndef UART_H_
#define UART_H_
#include "msp430g2553.h"
#include <stdlib.h>
#include <ctype.h>

void set_float_value (char *string,float *kp);
void set_int_value(unsigned char *string, unsigned char *kp);
void UartSendEvent(unsigned char a1,unsigned char a2);

/* ------------init uart module --------------------
* clock input to module: 16Mhz
* baudrate: 96000bps
* interrupt: enable or disable, you choise  */
 void Uart_Init();
 /*------------ send a byte -----------------*/
 void UartCharPut(unsigned char data_put);
 /*------------ get a byte -----------------
 wait untill uart module receive a byte */
 unsigned char UartCharGet();

 /*------------ send a string-----------------
 * terminator characte: '\0' = 0x00h = 0b */
 void UartStrPut(unsigned char *uart_str);
 /*------------ get a string-----------------
 * wait untill uart module get a string
 * terminator characte: '\0' = 0x00h = 0b  */
 void UartStrGet(unsigned char *uart_str);

 ////////////////////////////////////////////////////////////////////////////
 void set_float_value (char *string,float *kp)
 {
     while (!(isdigit(*string) || *string == '-')) (string++);
     *kp=atof(string);
 }

 void set_int_value(unsigned char *string, unsigned char *kp)
 {
     while (! (isdigit(*string) || *string == '-')) (string++);
     *kp=atoi(string);
 }
 void UartSendEvent(unsigned char a1,unsigned char a2)
 {
     UartCharPut(a1);
     UartCharPut(a2);
     UartCharPut(0);
 }

 /* ------------init uart module --------------------
 * clock input to module: 16Mhz
 * baudrate: 96000bps
 * interrupt: enable or disable, you choise  */
 void Uart_Init()
 {
     P1SEL |= BIT1 + BIT2 ;                   // P1.1 = RXD, P1.2=TXD
     P1SEL2 |= BIT1 + BIT2 ;                  // P1.1 = RXD, P1.2=TXD
     UCA0CTL1 |= UCSSEL_2;                   // SMCLK for Uart module
     UCA0BR0 = 104;                          // 1MHz 9600
     UCA0BR1 = 0;                             // 1MHz 9600
     UCA0MCTL = 0;                           // Modulation UCBRSx = 0
     UCA0CTL1 &= ~UCSWRST;                   // **Initialize USCI state machine**
     IE2 |= UCA0RXIE;
     // Enable USCI_A0 RX interrupt
     //__bis_SR_register(GIE);                   // interrupts enabled
 }
 /*------------ send a byte -----------------
 *
 */
 void UartCharPut(unsigned char data_put)
 {
     while(!(IFG2 & UCA0TXIFG));//wait for ready
     UCA0TXBUF=data_put;
     //while (UCA0CTL1 & UCTXSTT);                     // waiting for slave address to transfer
     //while ((IFG2 & UCA0TXIFG) != UCA0TXIFG);        //wait for TX IFG to clear
 }
 /*------------ get a byte -----------------
 * wait untill uart module receive a byte
 */
 unsigned char UartCharGet()
 {
     while(!(IFG2 & UCA0RXIFG));//wait for data comming
     return UCA0RXBUF;
 }
 /*------------ send a string-----------------
 * terminator characte: '\0' = 0x00h = 0b
 */
 void UartStrPut(unsigned char *uart_str)
 {
     while(*uart_str != '\0') {UartCharPut(*uart_str++ );}
     UartCharPut('\0'); // attention end character !!!!!!!!!!!. in this case, i use '\0'
 }
 /*------------ get a string-----------------
 * wait untill uart module get a string
 * terminator characte: '\0' = 0x00h = 0b
 */
 void UartStrGet(unsigned char *uart_str)
 {
     *uart_str= UartCharGet();
     while (*uart_str != '\0')
     {
         uart_str++;
         *uart_str= UartCharGet();
     }
 }






 //===================================number display=========================================================


 /*---------- hi?n th? s? th?p ph�n d?ng chu?i-------------------
  * s? c� d?ng xxx.xx (5 ch? s? c� d?u)
  * k� t? k?t th�c '\n'
  * khuy�n d�ng: h�m t?n r?t nhi?u th?i gian x? l� n�n b?n ph?i
  * ch?c ch?n r�ng k ?nh hu?ng d?n c�c h�m kh�c
  * gi?i h?n s? do bi?n k quy?t d?nh (2byte k d?u)
  * => max,min = +-655.36
 *-----------------------------------------------------------*/
 void Uart_Float_Display(float num)
 {
  unsigned int k;
  unsigned char a;


  num*=100.0;
  if (num <0)
  {
     UartCharPut('-');
     k = (unsigned int)(-num);
  }
  else
     k=num;


  a=k/10000 + 48;
  UartCharPut(a);


  a=(k%10000)/1000+48;
  UartCharPut(a);


  a=(k%10000)%1000/100+48;
  UartCharPut(a);


  UartCharPut('.');


  a=(k%10000)%1000%100/10+48;
  UartCharPut(a);


  a=(k%10000)%1000%100%10+48;
  UartCharPut(a);


  UartCharPut('\0');// k� t? k?t th�c 0x0A - k� t? xu?ng d�ng (LF)
 }




 /*---------- hi?n th? s? nguy�n d?ng chu?i-------------------
  * s? c� d?ng xxxxx (5 ch? s? c� d?u)
  * k� t? k?t th�c '\n'
  * ch� �: s? 16bit c� d?u
  *-----------------------------------------------------------*/
 void Uart_int_display(int num )
 {
  unsigned short k;
  unsigned char a;
  if (num <0)
  {
     UartCharPut('-');
     k = - num;
  }
  else
     k=num;


  a=k/10000 + 48;
  UartCharPut(a);


  a=(k%10000)/1000+48;
  UartCharPut(a);


  a=(k%10000)%1000/100+48;
  UartCharPut(a);


  a=(k%10000)%1000%100/10+48;
  UartCharPut(a);


  a=(k%10000)%1000%100%10+48;
  UartCharPut(a);


  UartCharPut('\0');// k� t? k?t th�c 0x0A - k� t? xu?ng d�ng (LF)
 }
 ///////////////////////////////////////////////////////////////////////
#endif /* UART_H_ */

