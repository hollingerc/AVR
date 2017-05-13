/*
 * File:   uart.c
 * Date:   September 2, 2014
 * Author: Craig Hollinger
 *
 * Primitive driver routines to run the USART peripheral.  
 *
 * Be sure F_CPU is defined when this file is called.  It is needed to
 * calculate the BAUD rate.
 *
 * These are very basic functions, the send and receive functions block until
 * the character is sent or received.  At the very least, they will waste CPU
 * time if the BAUD rate is low or a lot of data is being sent.  At the the
 * worst, they could block indefinitely if something goes wrong.  Interrupt
 * driven functions would be more efficient, but with added complexity. 
 * Something to address in the future.
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of either the GNU General Public License version 3
 * or the GNU Lesser General Public License version 3, both as
 * published by the Free Software Foundation.
 */

#include <avr/io.h> /* microcontroller register names */
#include <avr/pgmspace.h> /* store/retrieve data in FLASH */
#include "uart/uart.h"

/* Local storage for USART Status */
union USART
{
  char val;
  struct
  {
    unsigned RX_NINE:1;
    unsigned TX_NINE:1;
    unsigned FRAME_ERROR:1;
    unsigned OVERRUN_ERROR:1;
    unsigned PARITY_ERROR:1;
    unsigned :3;
  };
}usart_status;

/*
 * uart_init()
 *
 * Initialize the UART using the given bit (BAUD) rate, character size, parity
 * and number of stop bits.
 */
void uart_init(int rate,    /* bit rate in bits per second (BAUD) */
               char size,   /* frame size, can be 5, 6, 7, 8, or 9 bits */
               char parity, /* parity, can be none, odd or even */
               char stop)   /* number of stop bits, can be 1 or 2 */
{

  /* Temporary variable for Baud Rate Register. */
  int ubrr;

  /* clear the local error status */
  usart_status.val = 0;
  
  /* Set the USART mode.  In this case it is asynchronous only. */
  UCSR0C &= ~_BV(UMSEL00) & ~_BV(UMSEL01);

  /* Set the character size mode. */
  switch(size)
  {
    case USART_CHAR_SZ_FIVE: /* 5-bit */
      UCSR0C &= ~_BV(UCSZ00) & ~_BV(UCSZ01);
      UCSR0B &= ~_BV(UCSZ02);
      break;

    case USART_CHAR_SZ_SIX: /* 6-bit */
      UCSR0C |= _BV(UCSZ00);
      UCSR0C &= ~_BV(UCSZ01);
      UCSR0B &= ~_BV(UCSZ02);
      break;

    case USART_CHAR_SZ_SEVEN: /* 7-bit */
      UCSR0C &= ~_BV(UCSZ00);
      UCSR0C |= _BV(UCSZ01);
      UCSR0B &= ~_BV(UCSZ02);
      break;

    case USART_CHAR_SZ_EIGHT: /* 8-bit */
    default:
      UCSR0C |= _BV(UCSZ00) | _BV(UCSZ01);
      UCSR0B &= ~_BV(UCSZ02);
      break;

    case USART_CHAR_SZ_NINE: /* 9-bit */
      UCSR0C |= _BV(UCSZ00) | _BV(UCSZ01);
      UCSR0B |= _BV(UCSZ02);
      break;
  }/* end switch(size) */

  /* Set the parity mode. */
  switch(parity)
  {
    case USART_PARITY_NONE: /* no parity */
    default:
      UCSR0C &= ~_BV(UPM00) & ~_BV(UPM01);
      break;

    case USART_PARITY_EVEN: /* even parity */
      UCSR0C &= ~_BV(UPM00);
      UCSR0C |= _BV(UPM01);
      break;

    case USART_PARITY_ODD: /* odd parity */
      UCSR0C |= _BV(UPM00) | _BV(UPM01);
      break;

  }/* end switch(parity) */

  /* Set the number of stop bits. */
  if(stop == USART_STOP_BIT_ONE)
  {
    UCSR0C &= ~_BV(USBS0); /* one stop bit */
  }
  else
  {
    UCSR0C |= _BV(USBS0); /* two Stop bits */

  }/* end if(stop == USART_STOP_BIT_ONE) */
  
  /* Set up the BAUD rate generator. */
#if F_CPU < 2000000UL && defined(U2X0)/* low CPU clock frequency */
  ubrr = (F_CPU / (8UL * rate)) - 1;
  UBRR0H = (unsigned char)(ubrr >> 8);
  UBRR0L = (unsigned char)ubrr;
  UCSR0A |= _BV(U2X0);             /* improve baud rate error by using 2x clk */
#else
  ubrr = (F_CPU / (16UL * rate)) - 1;
  UBRR0H = (unsigned char)(ubrr >> 8);
  UBRR0L = (unsigned char)ubrr;
  UCSR0A &= ~_BV(U2X0);
#endif

  /* Enable the UART transmit and receive functions. */
  UCSR0B |= (_BV(TXEN0) | _BV(RXEN0)); /* tx/rx enable */
  
}/* end uart_init() */

/*
 * uart_putchar()
 *
 * Send character c to the UART Tx.
 *
 * This function will block until the character in the transmit buffer is sent.
 */
void uart_putchar(char c)
{

/* loop until the TX register is empty */
  while(!(UCSR0A & _BV(UDRE0)));

  UDR0 = c;

}/* end uart_putchar() */

/*
 * uart_getchar()
 *
 * Receive a character from the UART Rx.  If any errors are detected, set the
 * appropriate bits in usart_status.
 *
 * Note:
 * This function will block until a character is received, indefinitely if none
 * are received.  Best to call uart_available() first to make sure a character
 * is waiting.
 */
char uart_getchar(void)
{

/* loop until receive register is full */
  while(!(UCSR0A & _BV(RXC0)));

/* test for errors and update the status register */
	if(UCSR0A & _BV(FE0))
  {
	  usart_status.FRAME_ERROR = 1;
  }
	if(UCSR0A & _BV(DOR0))
  {
	  usart_status.OVERRUN_ERROR = 1;
  }
	if(UCSR0A & _BV(UPE0))
  {
  	usart_status.PARITY_ERROR = 1;
	}
  if(UCSR0B & _BV(RXB80))
  {
    usart_status.RX_NINE = 1;
  }
  return(UDR0);
  
}/* end uart_getchar() */

/*
 * uart_putstr_P()
 *
 * Send a null terminated string stored in Flash to the UART Tx, 
 */
void uart_putstr_P(PGM_P addr)
{

  char c;

  while ((c = pgm_read_byte_near(addr++)))
  {
    /* loop until the TX register is empty */
    while(!(UCSR0A & _BV(UDRE0)));
    UDR0 = c;
  }    

}/* end uart_putstr_P() */

/*
 * uart_putstr()
 *
 * Send a null terminated string stored in RAM to the UART Tx, 
 */
void uart_putstr(char str[])
{

  char c;
  int i = 0;

  while ((c = str[i++]) != 0)
  {
    /* loop until the TX register is empty */
    while(!(UCSR0A & _BV(UDRE0)));
    UDR0 = c;
  }    
    
}/* end uart_putstr() */

/*
 * uart_put_UDR0()
 *
 * Write the character to the UDR0.
 */
void uart_put_UDR0(char c)
{
  
  UDR0 = c;

}/* end uart_put_UDR0() */

/*
 * uart_get_status()
 *
 * Clear usart_status then return it's value.
 */
char uart_get_status(void)
{

  char c;
  
  c = usart_status.val;
  usart_status.val = 0;
  
  return(c);

}/* end uart_get_status() */

/*
 * uart_available()
 *
 * Test the various error bits in the UCSR0A, set the appropriate flags if
 * errors exist, then return an error code.  If a character has been received
 * but not read and there are no errors, return the available code.
 *
 * If there are no errors and no character has been received, return 0.
 */
char uart_available(void)
{
  
  if(UCSR0A & _BV(FE0))
  {
    usart_status.FRAME_ERROR = 1;
    return(UART_FRAME_ERROR);
  }
  if(UCSR0A & _BV(DOR0))
  {
    usart_status.OVERRUN_ERROR = 1;
    return(UART_OVERRUN_ERROR);
  }
  if(UCSR0A & _BV(UPE0))
  {
    usart_status.PARITY_ERROR = 1;
    return(UART_PARITY_ERROR);
  }
  if(UCSR0A & _BV(RXC0))
  {
    return(UART_AVAILABLE);
  }

  return(0);

}/* end uart_available() */

/*
 * uart_rx_EI()
 * uart_rx_DI()
 * uart_tx_EI()
 * uart_tx_DI()
 *
 * Enable or disable the UART receive and transmit interrupts.
 */
void uart_rx_EI(void)
{

  UCSR0B |= _BV(RXCIE0);
}

void uart_rx_DI(void)
{

  UCSR0B &= ~_BV(RXCIE0);
}

void uart_tx_EI(void)
{

  UCSR0B |= _BV(TXCIE0);
}

void uart_tx_DI(void)
{

  UCSR0B &= ~_BV(TXCIE0);
}

/*
 * get_uart_rx_IE()
 * get_uart_tx_IE()
 *
 * Return the TX and RX interrupt enable bits.
 */
char get_uart_rx_IE(void)
{
  
  if((UCSR0B & _BV(RXCIE0)) == 0)
  {
    return(0);
  }    
  else
  {
    return(1);
  }    
}/* end get_uart_rx_IE() */

char get_uart_tx_IE(void)
{
  
  if((UCSR0B & _BV(TXCIE0)) == 0)
  {
    return(0);
  }    
  else
  {
    return(1);
  }

}/* end get_uart_tx_IE() */

/*
 * uart_tx_status()
 *
 * Return the UDRE0 bit in the LSB.  This bit will be 1 if the transmit buffer
 * is empty.
 */
char uart_tx_status(void)
{
  
  if((UCSR0A & _BV(UDRE0)) == 1)
  {
    return(1);
  }    
  else
  {
    return(0);
  }    

}/* end uart_tx_status() */
