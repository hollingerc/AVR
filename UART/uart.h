/*
 * File:    uart.h
 * Date:    September 2, 2014
 * Author:  Craig Hollinger
 *
 * Primitive driver routines to run the USART peripheral.
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

void uart_init(uint32_t rate,  /* bit rate in bits per second (BAUD) */
               uint8_t size,   /* frame size, can be 5, 6, 7, 8, or 9 bits */
               uint8_t parity, /* parity, can be none, odd or even */
               uint8_t stop);  /* number of stop bits, can be 1 or 2 */
void uart_putchar(char c);
char uart_getchar(void);
void uart_putstr_P(const char *c);
void uart_putstr(char str[]);
void uart_put_UDR0(char c);
uint8_t uart_get_status(void);
uint8_t uart_available(void);
void uart_rx_EI(void);
void uart_rx_DI(void);
void uart_tx_EI(void);
void uart_tx_DI(void);
uint8_t get_uart_rx_IE(void);
uint8_t get_uart_tx_IE(void);
uint8_t get_uart_UDRE0(void);
uint8_t uart_write(uint8_t length, char buf[]);
uint8_t uart_write_P(uint8_t length, PGM_P buf);

#endif /* _UART_H_ */
