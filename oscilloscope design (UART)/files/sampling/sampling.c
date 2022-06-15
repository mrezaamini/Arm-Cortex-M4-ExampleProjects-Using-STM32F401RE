#include "stm32f4xx.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

#define RS 0x20     
#define RW 0x40    
#define EN 0x80    


void delayMs(int n);
void LCD_command(unsigned char command);
void LCD_data(char data);
void LCD_init(void);
void PORTS_init(void);
uint8_t kp_input(void);
void usart_send(uint8_t data);
uint8_t usart_recieve(void);
void TIM5_IRQHandler(void);
void TIM2_IRQHandler(void);
void InformationLCD(void);
uint8_t mode = 0;
uint8_t ptr=0;
uint8_t ch1[128];
uint8_t ch2[128];
double A=1;
double B=0;

int main(void) {
    LCD_init();
		InformationLCD();
		TIM2->CR1 |= TIM_CR1_CEN;
    while(1){
			uint8_t val=kp_input();
			if(val==1){
				usart_send('1');
				A+=0.1;
			}else if(val==2){
				usart_send('2');
				A-=0.1;
			}else if(val==3){
				usart_send('3');
				B+=10;
			}else if(val==4){
				usart_send('4');
				B-=10;
			}else if(val==5){
				if(TIM2->ARR < 1000) TIM2->ARR+=100;
			}else if(val==6){
				if(TIM2->ARR > 400) TIM2->ARR-=100;
			}
			LCD_command(1);
			InformationLCD();
		}
		
}

void LCD_init(void) {
    PORTS_init();
    delayMs(30);          
    LCD_command(0x30);
    delayMs(10);
    LCD_command(0x30);
    delayMs(1);
    LCD_command(0x30);
    LCD_command(0x38);      
    LCD_command(0x06);      
    LCD_command(0x01);      
    LCD_command(0x0F);      
}

uint8_t kp_input(void){
	while(1){
		GPIOB->ODR = 0xE000;
		int thisCol = GPIOB->IDR & 0x0700;
		if(thisCol != 0x0700 ){
			while((GPIOB->IDR & 0x700) != 0x0700){
				__NOP();
			}
			if (thisCol == 0x600){
				return 1;
			}
			else if(thisCol == 0x500){
				return 2;
			}
			else if(thisCol == 0x300){
				return 3;
			}
		}
		
		GPIOB->ODR = 0xD000;
		thisCol = GPIOB->IDR & 0x0700;
		if(thisCol != 0x0700 ){
			while((GPIOB->IDR & 0x700) != 0x0700){
				__NOP();
			}
			if (thisCol == 0x600){
				return 4;
			}
			else if(thisCol == 0x500){
				return 5;
			}
			else if(thisCol == 0x300){
				return 6;
			}
		}
		
	}			
}

void PORTS_init(void) {
    RCC->AHB1ENR |=  0x07;        
		RCC->APB2ENR |= 0x00000010 ; /* enable USART1 clock */
		GPIOA->AFR[1] = 0x00000770; /* alt7 for USART1 */
		GPIOA->MODER = 0x0028003C; /* enable alternate function for PA9 and pa5 as led output */
		USART1->BRR = 0x0683; /* 9600 baud @ 16 MHz */
		USART1->CR1 = 0x000C; /* enable Tx, 8-bit data */
		USART1->CR2 = 0x0000; /* 1 stop bit */
		USART1->CR3 = 0x0000; /* no flow control */
		USART1->CR1 |= 0x2000; 
		RCC->APB2ENR |= RCC_APB2ENR_ADC1EN; /* enable ADC1 clock */
		//ADC1->CR1 = 0x02000000; /* 8BIT */
		ADC1->CR2 = 0; /* SW trigger */
		ADC1->SQR3 |= 1; /* conversion sequence starts at ch 1 */
		ADC1->SQR1 |= 0; /* conversion sequence length 1 */
		ADC1->CR2 |= 1; /* enable ADC1 */
		/*set-up timer*/
		RCC->APB1ENR |= RCC_APB1ENR_TIM5EN;   /* enable TIM5 clock */
		TIM5->PSC = 16 - 1; 
		TIM5->ARR = 20 - 1 ;  
		TIM5->DIER |= TIM_DIER_UIE;
		TIM5->CR1 = TIM_CR1_CEN;       /* enable counter */
		NVIC_EnableIRQ(TIM5_IRQn); /* enable interrupt in NVIC */
		RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;   /* enable TIM5 clock */
		TIM2->PSC = 16000-1;
		TIM2->ARR = 400-1; 
		TIM2->DIER |= TIM_DIER_UIE;
		//TIM2->CR1 = TIM_CR1_CEN;       /* enable counter */
		NVIC_EnableIRQ(TIM2_IRQn); /* enable interrupt in NVIC */
		__enable_irq();
    GPIOB->MODER &= ~0x0000FC00;    /* clear pin mode */
    GPIOB->MODER  =  0x00005400;    /* set pin output mode */
	GPIOB->MODER |=  0x55000004;    /* set KP output AND INPUT mode and pbtn input */
	GPIOB->PUPDR &= ~0x00000100;    /* clear PULL UP  */
	GPIOB->PUPDR  =  0x00150010;		/* pull up keypad and button */
    GPIOB->BSRR = 0x00C00000;       /* turn off EN and R/W */
    GPIOC->MODER &= ~0x0000FFFF;    /* clear pin mode */
    GPIOC->MODER |=  0x00005555;    /* set pin output mode */
}

void LCD_command(unsigned char command) {
    GPIOB->BSRR = (RS | RW) << 16;  
    GPIOC->ODR = command;          
    GPIOB->BSRR = EN;              
    delayMs(0);
    GPIOB->BSRR = EN << 16;         
    if (command < 4)
        delayMs(2);        
    else
        delayMs(1);        
}

void delayMs(int n) {
    int i;
    for (; n > 0; n--)
        for (i = 0; i < 3195; i++)
						__NOP();
}

void LCD_data(char data) {
    GPIOB->BSRR = RS;               
    GPIOB->BSRR = RW << 16;         
    GPIOC->ODR = data;              
    GPIOB->BSRR = EN;               
    delayMs(0);
    GPIOB->BSRR = EN << 16;         
    delayMs(1);
}


void usart_send(uint8_t data) {
	while(!(USART1->SR & 0x0080)){}
	USART1->DR=(data & (uint8_t)0xFF);
}

uint8_t usart_recieve(void){
	while(!(USART1->SR & 0x0020)){}
	return USART1->DR;
}

void TIM2_IRQHandler(void){
	
	usart_send('!');
	
	for(uint8_t i = ptr ; i < 128 ; i++){
		usart_send(ch1[i]);
	}
	for(uint8_t i = 0 ; i < ptr ; i++){
		usart_send(ch1[i]);
	}
	
	//usart_send('#');
	
	//for(uint8_t i=0 ;i<128;i++){
	//	usart_send(ch2[i]);
	//}
	
}

void TIM5_IRQHandler(void)
{
    ADC1->SQR3=1;
    ADC1->CR2|=ADC_CR2_SWSTART;
	while(!(ADC1->SR & 0x02)){
		__NOP();
	}
	ch1[ptr]= ADC1->DR >> 4;
    /*
    ADC1->SQR3=2;
    ADC1->CR2|=ADC_CR2_SWSTART;
	while(!(ADC1->SR & 0x02)){
		__NOP();
	}
	ch2[ptr]= ADC1->DR >> 4;
    */
	ptr++;
	if(ptr==128)	ptr=0;
}

void InformationLCD(void){
	LCD_command(1);
	LCD_data('A');
	LCD_data(':');
	char myA[16];
	sprintf(myA, "%.1lf", A);
	uint8_t i = 0;
	while (myA[i] != '\0' && i < 16)
	{
		LCD_data(myA[i]);
		i++;
	}
	LCD_data(' ');
	LCD_data('B');
	LCD_data(':');
	char myB[16];
	sprintf(myB,"%.1lf",B);
	i=0;
	while(myB[i] != '\0' && i < 16){
		LCD_data(myB[i]);
		i++;
	}
	LCD_command(0x0C0);
	LCD_data('T');
	LCD_data('I');
	LCD_data('M');
	LCD_data('E');
	LCD_data(' ');
	LCD_data('U');
	LCD_data(':');
	LCD_data(' ');
	char tu[16];
	sprintf(tu,"%d",(TIM2->ARR)+1);
	i=0;
	while(tu[i] != '\0' && i < 16){
		LCD_data(tu[i]);
		i++;
	}
	LCD_data('m');
	LCD_data('s');
}

