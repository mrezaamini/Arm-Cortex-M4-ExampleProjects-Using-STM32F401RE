#include "stm32f4xx.h"
#include <stdio.h>

#define RS 0x20     /* PB5 mask for reg select */
#define RW 0x40     /* PB6 mask for read/write */
#define EN 0x80     /* PB7 mask for enable */

void delayMs(int n);
void LCD_command(unsigned char command);
void LCD_data(char data);
void LCD_init(void);
void PORTS_init(void);
void EXTI9_5_IRQHandler(void);
void EXTI15_10_IRQHandler(void);
void EXTI2_IRQHandler(void);
void EXTI3_IRQHandler(void);
void checkInput(char input);
void EXTI3_IRQHandler(void);
void EXTI2_IRQHandler(void);
void showNames(void);

char inChar;
int num1 =0;
int num2=0;
int mode=0;
int result=0;
char opt;


int main(void) {
    /* initialize PORTS AND LCD controller */
    LCD_init();
		showNames();
	
    while(1) {
				GPIOC->ODR = 0x00001000;
				delayMs(1);
			  GPIOC->ODR = 0x00002000;
				delayMs(1);
			  GPIOC->ODR = 0x00004000;
				delayMs(1);
		   	GPIOC->ODR = 0x00008000;
				delayMs(1);
			
		}
}


void LCD_init(void) { 	/* initialize port then initialize LCD and making it ready for commanding */
    PORTS_init();       /* initialize port*/
    delayMs(30);            
    LCD_command(0x30);
    delayMs(10);
    LCD_command(0x30);
    delayMs(1);
    LCD_command(0x30);
    LCD_command(0x38);      /* set 8-bit data and two line of 5x7 font */
    LCD_command(0x06);      /*  cursor after each character */
    LCD_command(0x01);      /* clear screen and reset cursor */
    LCD_command(0x0F);      /* display turn on*/
}

void PORTS_init(void) {
    RCC->AHB1ENR |=  0x06;          /* enable GPIOB/GPIOC clock */
		RCC->APB2ENR |=  0x00004000;    /* enable syscfg en */
    GPIOB->MODER &= ~0x0000FC00;    /* clear pin mode */
    GPIOB->MODER  =  0x00005400;    /* set lcd rs rw en pin output mode */
	  GPIOC->MODER &= ~0x0000FFFF;    /* clear pin mode */
    GPIOC->MODER |=  0x00005555;    /* set lcd pin output mode */
		GPIOC->MODER |= 0x55000000;     /* pc12-pc15 as output for kp1-kp4 */
	  GPIOC->PUPDR = 0x00AA0000;
		SYSCFG->EXTICR[2] = 0x2222;
		EXTI->IMR = 0x0F00;             /* unmasking PC8-PC11 */
		EXTI->RTSR = 0x0F00;            /* enable rising trigger PC8-PC11 */
		SYSCFG->EXTICR[0] = 0x1100;
		EXTI->IMR |= 0xC;
		EXTI->RTSR |= 0xC;
		NVIC_EnableIRQ(EXTI15_10_IRQn);     /* enable IRQ_handler for PC8-P11 */
		NVIC_EnableIRQ(EXTI9_5_IRQn);
		NVIC_EnableIRQ(EXTI2_IRQn);
		NVIC_EnableIRQ(EXTI3_IRQn);
    GPIOB->BSRR = 0x00C00000;       /* turn off EN and R/W */
}

void LCD_command(unsigned char command) {
    GPIOB->BSRR = (RS | RW) << 16;  /* RS=R/W=0*/
    GPIOC->ODR = command;           /* force the command on the output bus */
    GPIOB->BSRR = EN;               /* make a pulse E */
    delayMs(0);
    GPIOB->BSRR = EN << 16;         /* clear pulse E */
    if (command < 4)
        delayMs(2);         /* first two commands need more time */
    else
        delayMs(1);         
}

void LCD_data(char data) {
    GPIOB->BSRR = RS;               /* RS = 1 */
    GPIOB->BSRR = RW << 16;         /* R/W = 0 */
    GPIOC->ODR = data;              /* put data on data bus */
    GPIOB->BSRR = EN;               /* pulse E high */
    delayMs(0);
    GPIOB->BSRR = EN << 16;         /* clear E */

    delayMs(1);
}

void delayMs(int n) {
    int i;
    for (; n > 0; n--)
        for (i = 0; i < 3195; i++)
						__NOP();
}

void EXTI9_5_IRQHandler(void){
	 while((GPIOC->IDR & 0x00000300) != 0){
		 __NOP();
	 }
	 int thisCol = GPIOC->ODR & 0x0000F000;
	 if(EXTI->PR ==0x00000100){
		if(thisCol == 0x00001000){
			inChar='7';
			EXTI->PR |= 0x00000100;
		}
	}
	 
	if(EXTI->PR ==0x00000100){
		if(thisCol == 0x00002000){

			inChar='8';
			EXTI->PR |= 0x00000100;
		}
	}
	
	if(EXTI->PR ==0x00000100){
		if(thisCol == 0x00004000){
	
			inChar='9';
			EXTI->PR |= 0x00000100;
		}
	}
	
	if(EXTI->PR ==0x00000100){
		if(thisCol == 0x00008000){
		
			inChar='/';
			EXTI->PR |= 0x00000100;
		}
	}
	 
	 
	if(EXTI->PR ==0x00000200){
		if(thisCol == 0x00001000){

			inChar='4';
			EXTI->PR |= 0x00000200;
		}
	}
	
	if(EXTI->PR ==0x00000200){
		if(thisCol == 0x00002000){
			
			inChar='5';
			EXTI->PR |= 0x00000200;
		}
	}
	
	if(EXTI->PR ==0x00000200){
		if(thisCol == 0x00004000){
			
			inChar='6';
			EXTI->PR |= 0x00000200;
		}
	}
	
	if(EXTI->PR ==0x00000200){
		if(thisCol == 0x00008000){

			inChar='*';
			EXTI->PR |= 0x00000200;
		}
	}

	checkInput(inChar);
	NVIC_ClearPendingIRQ(EXTI9_5_IRQn);
}

void EXTI15_10_IRQHandler(void){
	while((GPIOC->IDR & 0x00000C00) != 0){
		 __NOP();
	 }
	int thisCol = GPIOC->ODR & 0x0000F000;
	 
	 if(EXTI->PR == 0x00000400){
		 if(thisCol == 0x00001000){
			
			 inChar='1';
			 EXTI->PR |= 0x00000400;
		 }
	 }
	 
	 if(EXTI->PR == 0x00000400){
		 if(thisCol == 0x00002000){

			 inChar='2';
			 EXTI->PR |= 0x00000400;
		 }
	 }
	 
	 if(EXTI->PR == 0x00000400){
		 if(thisCol == 0x00004000){
		
			 inChar='3';
			 EXTI->PR |= 0x00000400;
		 }
	 }
	 
	 if(EXTI->PR == 0x00000400){
		 if(thisCol == 0x00008000){

			 inChar='-';
			 EXTI->PR |= 0x00000400;
		 }
	 }
	 
	 if(EXTI->PR == 0x00000800){
		 if(thisCol == 0x00001000){
	
			 inChar='!';
			 EXTI->PR |= 0x00000800;
		 }
	 }
	 
	 if(EXTI->PR == 0x00000800){
		 if(thisCol == 0x00002000){
			 
			 inChar='0';
			 EXTI->PR |= 0x00000800;
		 }
	 }
	 
	 if(EXTI->PR == 0x00000800){
		 if(thisCol == 0x00004000){
			 inChar='=';
			 EXTI->PR |= 0x00000800;
		 }
	 }
	 
	 if(EXTI->PR == 0x00000800){
		 if(thisCol == 0x00008000){
			
			 inChar='+';
			 EXTI->PR |= 0x00000800;
		 }
	 }
	
	 
	 if(inChar=='!'){ //reseting calculator
		 LCD_command(1);
		 showNames();
		 num1=0;
		 num2=0;
		 mode=0;
		 opt=0;
		 result=0;
	 }
	 checkInput(inChar);
	 NVIC_ClearPendingIRQ(EXTI15_10_IRQn);
}

void checkInput(char input){
	if('0'<=input&&input<='9'){
		if(mode==0){
			num1*=10;
			num1+=(input-48);
			LCD_data(input);
		}else if(mode==1){
			mode++;
			LCD_command(1);
			showNames();
			num2=input-48;
			LCD_data(input);
		}else if(mode==2){
			num2*=10;
			num2+=(input-48);
			LCD_data(input);
		}else if(mode==3){
			mode=0;
			num1=input-48;
			LCD_command(1);
			showNames();
			LCD_data(input);
		}
	}else if((input=='*' || input=='/' || input=='+' || input=='-')&&mode<=1){
			LCD_command(1);
			showNames();
			opt=input;
			LCD_data(input);
		mode = 1;
	}else if(input=='='&&mode==2){
		mode=3;
		LCD_command(1);
		showNames();
		switch(opt){
			case '+' : result = num1+num2 ; break;
			case '-' : result = num1-num2 ; break;
			case '*' : result = num1*num2 ; break;
			case '/' : result = num1/num2 ; break;
		}
		char resVal[100];
		sprintf(resVal, "%d", result);
		for(int i = 0; i<101 ; i++){
			if(resVal[i]=='\0') break;
			LCD_data(resVal[i]);
		}
		
	}
}

void EXTI2_IRQHandler(void){
	LCD_command(1);
	showNames();
	result++;
	char resVal[100];
	sprintf(resVal, "%d", result);
	for(int i = 0; i<101 ; i++){
		if(resVal[i]=='\0') break;
		LCD_data(resVal[i]);
	}
	 EXTI->PR |= 0x2;
	 NVIC_ClearPendingIRQ(EXTI2_IRQn);
}

void EXTI3_IRQHandler(void){
	LCD_command(1);
	showNames();
	result--;
	char resVal[100];
	sprintf(resVal, "%d", result);
	for(int i = 0; i<101 ; i++){
		if(resVal[i]=='\0') break;
		LCD_data(resVal[i]);
	}
	 EXTI->PR |= 0x4;
	 NVIC_ClearPendingIRQ(EXTI3_IRQn);
}


void showNames(void){
		LCD_data('A');
		LCD_data('M');
	  LCD_data('I');
		LCD_data('N');
		LCD_data('I');
		LCD_data(' ');
		LCD_data('A');
		LCD_data('M');
	  LCD_data('I');
		LCD_data('N');
		LCD_data('I');
	  LCD_command(0xC0);
}
