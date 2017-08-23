#ifndef UART_H_
#define UART_H_
void UART_init(void);
void UART_puts(char * s);
void UART_outdec(long data, unsigned char ndigits);

void UART_init(void)
{
    //UART Initial
    P1SEL = BIT1 + BIT2;                     // P1.1 = RX pin, P1.2=TX pin
    P1SEL2 = BIT1 + BIT2 ;                    // P1SEL and P1SEL2 = 11--- Secondary peripheral module function is selected.
    UCA0CTL1 |= UCSSEL_2;                     // SMCLK
    UCA0BR0 = 104;                             // 16MHz 230400
    UCA0BR1 = 0;                              // 16MHz 230400
    UCA0MCTL = UCBRS0;                        // Modulation UCBRSx = 1
    UCA0CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**
    IE2 |= UCA0RXIE;                          // Enable USCI_A0 RX interrupt
}
void UART_puts(char * s) {
   while (*s) {
       while (!(IFG2 & UCA0TXIFG));                // USCI_A0 TX buffer ready?
       UCA0TXBUF = *s++;
   }
}
 void UART_outdec(long data, unsigned char ndigits){    //UART_outdec modified/hacked to properly handle negative numbers.
      unsigned char sign, s[15];          //I copied this from a TI example or the 40oh forum, but I'm not sure of the original author.
      unsigned int i;
      sign = 0x00;

      if(data < 0) {
          sign='-';
          data = -data;
      }
      i = 0;

      do {
          s[i++] = data % 10 + '0'; //adds the value of data least sig digit to ascii value of '0'
          if(i == ndigits) {
              s[i++]='.';
          }
      } while( (data /= 10) > 0);

      if (i < ndigits) //fixes loss of leading 0 in fractional portion when number of digits is less than length of data
      {
          do {
              s[i++]='0';
              } while (ndigits > i) ;

          s[i++]='.';
      }

      if (sign == '-')  //if value is negative then include the '-' sign
      {
          s[i] = sign;
      } else
      {           //if value is positive then reduce 'i' counter by 1 to prevent the do loop from trying to output a sign character.
          i--;
      }

      do {
          while (!(IFG2 & UCA0TXIFG));
          UCA0TXBUF = s[i];
      } while(i--);
   }
#endif /* UART_H_ */
