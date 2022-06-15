#include "stm32f4xx.h"
#include <stdio.h>

#define RS 0x20 /* PB5 mask for reg select */
#define RW 0x40 /* PB6 mask for read/write */
#define EN 0x80 /* PB7 mask for enable */

void delayMs(int n);
void LCD_command(unsigned char command);
void LCD_data(char data);
void LCD_init(void);
void PORTS_init(void);
void EXTI2_IRQHandler(void);
void EXTI3_IRQHandler(void);
void EXTI4_IRQHandler(void);
void TIM2_IRQHandler(void);

int milis = 0;
int seconde = 0;
int minute = 0;
int pauseCount = 1;
int resetCount = 0;
int pushed;
int start;
int finished;
int turn_OffMode_Flag = 0;

int main(void)
{
	/* initialize PORTS AND LCD controller */
	LCD_init();
	LCD_data('W');
	LCD_data('E');
	LCD_data('L');
	LCD_data('C');
	LCD_data('O');
	LCD_data('M');
	LCD_data('E');
	TIM2->CR1 |= TIM_CR1_CEN;

	while (1)
	{
		if (pauseCount == 0 && turn_OffMode_Flag == 0)
		{
			LCD_command(1);
			char resVal[100];
			sprintf(resVal, "%.2d:%.2d:%.3d", minute, seconde, milis);
			for (int i = 0; resVal[i] != '\0'; i++)
			{
				LCD_data(resVal[i]);
			}
			delayMs(100);
		}
		if (turn_OffMode_Flag == 1)
			break;
	}
	while (1)
	{
		LCD_command(1);
		LCD_data('T');
		LCD_data('U');
		LCD_data('R');
		LCD_data('N');
		LCD_data(' ');
		LCD_data('O');
		LCD_data('F');
		LCD_data('F');
		delayMs(500);
		LCD_command(1);
		delayMs(500);
	}
}

void LCD_init(void)
{				  /* initialize port then initialize LCD and making it ready for commanding */
	PORTS_init(); /* initialize port*/
	delayMs(30);
	LCD_command(0x30);
	delayMs(10);
	LCD_command(0x30);
	delayMs(1);
	LCD_command(0x30);
	LCD_command(0x38); /* set 8-bit data and two line of 5x7 font */
	LCD_command(0x06); /*  cursor after each character */
	LCD_command(0x01); /* clear screen and reset cursor */
	LCD_command(0x0F); /* display turn on*/
}

void PORTS_init(void)
{
	RCC->APB1ENR |= RCC_APB1ENR_TIM5EN; /* enable TIM5 clock */
	TIM5->PSC = 16000 - 1;				/* divided by 16000 */
	TIM5->ARR = 10000 - 1;				/* divided by 10000 */
	TIM5->CR1 = TIM_CR1_CEN;
	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
	TIM2->PSC = 1600;
	TIM2->ARR = 10 - 1;
	TIM2->DIER |= TIM_DIER_UIE;
	NVIC_EnableIRQ(TIM2_IRQn);

	RCC->AHB1ENR |= 0x06;		/* enable GPIOB/GPIOC clock */
	RCC->APB2ENR |= 0x00004000; /* enable syscfg en */

	GPIOB->MODER &= ~0x0000FC00; /* clear pin mode */
	GPIOB->MODER = 0x00005400;	 /* set lcd rs rw en pin output mode */
	GPIOB->PUPDR = 0x000002A0;

	GPIOC->MODER &= ~0x0000FFFF; /* clear pin mode */
	GPIOC->MODER |= 0x00005555;	 /* set lcd pin output mode */

	EXTI->IMR = 0x001C;	 /* unmasking PC8-PC11 */
	EXTI->FTSR = 0x001C; /* enable rising trigger PC8-PC11 */
	EXTI->RTSR = 0x0010; /* enable rising trigger PC8-PC11 */
	SYSCFG->EXTICR[0] = 0x1100;
	SYSCFG->EXTICR[1] = 0x0001;
	NVIC_EnableIRQ(EXTI2_IRQn); /*enabling intrupts */
	NVIC_EnableIRQ(EXTI3_IRQn); /*enabling intrupts */
	NVIC_EnableIRQ(EXTI4_IRQn); /*enabling intrupts */

	GPIOB->BSRR = 0x00C00000; /* turn off EN and R/W */
}

void LCD_command(unsigned char command)
{
	GPIOB->BSRR = (RS | RW) << 16; /* RS=R/W=0*/
	GPIOC->ODR = command;		   /* force the command on the output bus */
	GPIOB->BSRR = EN;			   /* make a pulse E */
	delayMs(0);
	GPIOB->BSRR = EN << 16; /* clear pulse E */
	if (command < 4)
		delayMs(2); /* first two commands need more time */
	else
		delayMs(1);
}

void LCD_data(char data)
{
	GPIOB->BSRR = RS;		/* RS = 1 */
	GPIOB->BSRR = RW << 16; /* R/W = 0 */
	GPIOC->ODR = data;		/* put data on data bus */
	GPIOB->BSRR = EN;		/* pulse E high */
	delayMs(0);
	GPIOB->BSRR = EN << 16; /* clear E */

	delayMs(1);
}

void delayMs(int n)
{
	int i;
	for (; n > 0; n--)
		for (i = 0; i < 3195; i++)
			__NOP();
}

void EXTI2_IRQHandler(void)
{
	pauseCount = 0;
	EXTI->PR |= 0x2;
	NVIC_ClearPendingIRQ(EXTI2_IRQn);
}

void EXTI3_IRQHandler(void)
{
	pauseCount = 1;
	EXTI->PR |= 0x4;
	NVIC_ClearPendingIRQ(EXTI3_IRQn);
}

void EXTI4_IRQHandler(void)
{
	milis = 0;
	seconde = 0;
	minute = 0;
	pauseCount = 1;
	LCD_command(1);
	char resVal[100];
	sprintf(resVal, "%.2d:%.2d:%.3d", minute, seconde, milis);
	for (int i = 0; resVal[i] != '\0'; i++)
	{
		LCD_data(resVal[i]);
	}
	if (pushed == 0)
	{
		start = TIM5->CNT;
		pushed = 1;
	}
	else
	{
		pushed = 0;
		finished = TIM5->CNT;
		if (finished - start > 3000)
		{
			turn_OffMode_Flag = 1;
			__disable_irq();
		}
	}
	EXTI->PR |= 0x2;
	NVIC_ClearPendingIRQ(EXTI2_IRQn);
}

void TIM2_IRQHandler(void)
{
	if (pauseCount == 0)
	{
		milis += 1;
		if (milis == 1000)
		{
			seconde += 1;
			milis = 0;
			if (seconde == 60)
			{
				minute += 1;
				seconde = 0;
			}
			if (minute == 100)
				minute = 0;
		}
	}
	EXTI->PR |= 0x2;
	NVIC_ClearPendingIRQ(EXTI2_IRQn);
}