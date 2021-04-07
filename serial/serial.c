#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include "serial.h"


#define ESC 0x1b
#define TX_BUFF_SIZE 64
#define RX_BUFF_SIZE 32

static int mijn_uart_putchar(char c, FILE *stream);
static int mijn_uart_getchar(FILE *stream);
static char trek_teken_uit(char *teken);

prog_char CLR[] = {ESC, '[','H', ESC, '[', '2', 'J',0};


static unsigned char UART_TX_buffer[TX_BUFF_SIZE];
static volatile unsigned char inptr, outptr;
static volatile unsigned char tx_buff_empty;

static unsigned char UART_RX_buffer[RX_BUFF_SIZE];
static volatile unsigned char rx_inptr, rx_outptr;
static volatile unsigned char rx_buff_empty;

static FILE mijnstdout = FDEV_SETUP_STREAM(mijn_uart_putchar, mijn_uart_getchar,
                                             _FDEV_SETUP_RW);

void init_usart1(void)
{
    inptr  = 0;
    outptr = 0;
    tx_buff_empty = 1;
    
    rx_inptr  = 0;
    rx_outptr = 0;
    rx_buff_empty = 1;
    
    
    UCSR1C |= 0x06;
    //disable while setting baud rate
    UCSR1B = 0x00; 
    // UCSR1A = 0x00 | (1<<U2X1);
    
    // 19200 @ 3.68 MHz / 38400 2x bit 
    // UBRR1L = 0x0B; 
    // UBRR1H = 0x00;
    
	// 38400 @ 16.000 MHz 1x bit
    UCSR1A = 0x00;
	
	UBRR1L = 0x19;
	UBRR1H = 0x00;

    // enable RX and TX
    UCSR1B |= (1 << TXEN1) | (1<<RXEN1); // TX enable and RX enable
    UCSR1B |= (1 << RXCIE1); //_BV(TXCIE1) transmit complete isr not used            // enable receive interrupt       
    
    // direct stdout to usart1
    stdout = &mijnstdout;
}

void clr(void)
{
    puts_P(CLR);                                // Send a 'clear screen' to a VT100 terminal
}

static int mijn_uart_putchar(char c, FILE *stream)
{
    //if (c == '\n')
    //    mijn_uart_putchar('\r',stream);
        
    // shutoff transmitter empty interrupt while the variables are altered
    UCSR1B &= ~_BV(UDRIE1);
    
    if (UCSR1A & _BV(UDRE1) && (tx_buff_empty)) {
    // if USART data register is empty
    // and tx buffer is empty
    // then send without buffering
        UDR1 = c;
        UCSR1B |= _BV(UDRIE1);                     // Enable empty interrupt
    }
    else 
	if (inptr!=outptr || tx_buff_empty)
    {
        UART_TX_buffer[inptr] = c;               // Put character into buffer
        inptr++;                                 // Increment pointer
        tx_buff_empty = 0;
        
        
        if (inptr >= TX_BUFF_SIZE) // Pointer wrapping
            inptr = 0; 
    } 

    UCSR1B |= _BV(UDRIE1);                       // Enable
                                                 // empty interrupt
    return 0;
}

// Interrupt driven transmitter

ISR (USART1_UDRE_vect)
{
    if (!tx_buff_empty) {
        UDR1 = UART_TX_buffer[outptr];           // Send next byte
        outptr++;                                // Increment pointer
    
        if (outptr >= TX_BUFF_SIZE)              // Pointer wrapping
            outptr = 0;

        if(inptr == outptr)  {                   // If buffer is empty:
            UCSR1B &= ~_BV(UDRIE1);              // disable transmitter empty interrupt
            tx_buff_empty = 1;
        }
    }  
    else
	  UCSR1B &= ~_BV(UDRIE1);

}


ISR (USART1_TXC_vect)
{


}

ISR (USART1_RXC_vect)
{
    if (rx_inptr!=rx_outptr || rx_buff_empty)
    {
        // sla character het buffer op
        UART_RX_buffer[rx_inptr] = UDR1;

        // duid naar de naxte sloot aan
        rx_inptr++;                                

        rx_buff_empty = 0;

        // Pointer wrapping
        if (rx_inptr >= RX_BUFF_SIZE)   
            rx_inptr = 0;


    } else {
        // zuvol
    }
}

static int mijn_uart_getchar(FILE *stream)
{
    char teken=0;
    //block until char rxed
    while (trek_teken_uit(&teken) == 0);
    return (int)teken;  
}    
    
static char trek_teken_uit(char *teken) 
{
    char goed;
    
    // shut off receive interrupt whilst altering
    UCSR1B &= ~_BV(RXCIE1);
     
    if (!rx_buff_empty) {
       
        // Increment pointer
        *teken = UART_RX_buffer[rx_outptr];
        rx_outptr++; 
    
        // Pointer wrapping
        if (rx_outptr >= RX_BUFF_SIZE)  
            rx_outptr = 0;
        //if outptr just moved to in so now buffer is empty 
        if (rx_inptr == rx_outptr)
            rx_buff_empty = 1;
        goed = 1;
    }
    else
    {
        goed = 0;
    }
    
    // reinstate receive interrupt
    UCSR1B |= _BV(RXCIE1);
    return goed;
}

void USART_Transmit( unsigned char data )
{
  // Wait for empty transmit buffer
  while ( !( UCSR1A & _BV(UDRE1)) )
  ;
  // Put data into buffer, sends the data
  UDR1 = data;
}

char __getc(char *c){
  
  char goed = trek_teken_uit(c);
  return goed;

}
