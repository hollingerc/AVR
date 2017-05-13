/*
 * File:    uart.h
 * Date:    September 2, 2014
 * Author:  Craig Hollinger
 *
 * Primitive driver routines to run the USART peripheral.
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of either the GNU General Public License version 3
 * or the GNU Lesser General Public License version 3, both as
 * published by the Free Software Foundation.
 */
#ifndef _UART_H_
#define _UART_H_ 1

/* The following defines are for bits in the UCSR0C register.

   these define the different modes of the USART */
#define USART_MODE_ASYNC 0
#define USART_MODE_SYNC  1
#define USART_MODE_MSPIM 3

/* these define the different parity modes of the USART */
#define USART_PARITY_NONE 0
#define USART_PARITY_EVEN 2
#define USART_PARITY_ODD  3

/* these define the number of Stop Bits */
#define USART_STOP_BIT_ONE 0
#define USART_STOP_BIT_TWO 1

/* these define the character size */
#define USART_CHAR_SZ_FIVE  0
#define USART_CHAR_SZ_SIX   1
#define USART_CHAR_SZ_SEVEN 2
#define USART_CHAR_SZ_EIGHT 3
#define USART_CHAR_SZ_NINE  7

/* these are for the uart_available() function */
#define UART_AVAILABLE 1
#define UART_FRAME_ERROR 2
#define UART_OVERRUN_ERROR 3
#define UART_PARITY_ERROR 4

/*
 * UART start up initialization.
 */
void uart_init(int rate,    /* BAUD rate in bits per second */
               char size,   /* # bits transmit - 5, 6, 7, 8, or 9, default 8 */
               char parity, /* none, even or odd - default is none */
               char stop);  /* # stop bits - 1 or 2, default 1 */

/*
 * Send one character to the UART.
 */
void uart_putchar(char c);

/*
 * Receive one character from the UART.  
 */
char uart_getchar(void);

/*
 * Send a null-terminated string stored in Program Space to the UART.  
 */
void uart_putstr_P(const char *c);

/*
 * Send a null-terminated string stored in RAM to the UART.  
 */
void uart_putstr(char str[]);

/*
 * Put the character into the UART data register for transmission.
 */
void uart_put_UDR0(char c);

/*
 * Return the status of any errors that may have occurred.
 */
char uart_get_status(void);

/*
 * Return the status of the receive buffer.  
 */
char uart_available(void);

/*
 * Return the rx and tx interrupt enable bits.
 */
void uart_rx_EI(void);
void uart_rx_DI(void);
void uart_tx_EI(void);
void uart_tx_DI(void);

/*
 * Return the tx and rx interrupt enable bits.
 */
char get_uart_rx_IE(void);
char get_uart_tx_IE(void);

/*
 * Return the UDRE0 bit in the LSB.
 */
char get_uart_UDRE0(void);

#endif /* _UART_H_ */
