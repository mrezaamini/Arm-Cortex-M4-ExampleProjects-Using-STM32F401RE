
#include "stm32F4xx.h"
#include <stdio.h>

#define ADC_CAPTURE_INIT 0x40000000;
#define MY_ADC_CHANNEL 15;

void PORTS_INIT(void);
void UART_SEND(int c);
void delayMs(int);
void print_label(void);

int LM35_TMP = 0;

int main(void)
{

    PORTS_INIT(); /* initialize PORTS */

    while (1)
    {
        print_label();

        ADC1->CR2 |= ADC_CAPTURE_INIT; // start adc input
        while (!(ADC1->SR & 2))
        {
        }
        int result = ADC1->DR; /* read conversion result */
        LM35_TMP = (double)result * 500 / 4095;
        char resVal[100];
        sprintf(resVal, "%d", LM35_TMP);
        for (int i = 0; i < 101; i++)
        {
            if (resVal[i] == '\0')
                break;
            UART_SEND(resVal[i]);
        }
        delayMs(500);
        UART_SEND('\r');
        UART_SEND('\n');
    }
}

/*----------------------------------------------------------------------------
  Initialize UART pins, Baudrate, ADC
 *----------------------------------------------------------------------------*/
void PORTS_INIT(void)
{
    RCC->AHB1ENR |= 0x01;       // GPIOA clock
    RCC->APB2ENR |= 0x00000010; // USART1 clock
    GPIOA->AFR[1] = 0x00000770; // alt7 for USART1
    GPIOA->MODER = 0x00280000;  // enable alternate function for PA9
    USART1->BRR = 0x0683;       // 9600 baud
    USART1->CR1 = 0x000C;       // enable Tx, 8-bit data
    USART1->CR2 = 0x0000;       // 1 stop bit *
    USART1->CR3 = 0x0000;       // no flow control
    USART1->CR1 |= 0x2000;
    // adc
    RCC->AHB1ENR |= 0x04;        // enable GPIOC clock
    GPIOC->MODER |= 0xC00;       // PC5 analog
    RCC->APB2ENR |= 0x00000100;  // enable ADC1 clock
    ADC1->CR2 = 0;               // SW trigger
    ADC1->SQR3 = MY_ADC_CHANNEL; // conversion ch 15 (pc5)
    ADC1->SQR1 = 0;              // conversion sequence length 1
    ADC1->CR2 |= 1;              // enable ADC1
}

void UART_SEND(int data)
{ // Write a character to UART1
    while (!(USART1->SR & 0x0080))
    {
    }
    USART1->DR = (data & (uint8_t)0xFF);
}

void delayMs(int n)
{
    int i;
    for (; n > 0; n--)
        for (i = 0; i < 3195; i++)
            __NOP();
}

void print_label(void)
{
    UART_SEND('L');
    UART_SEND('M');
    UART_SEND('3');
    UART_SEND('5');
    UART_SEND('_');
    UART_SEND('T');
    UART_SEND('M');
    UART_SEND('P');
    UART_SEND(':');
    UART_SEND(' ');
}