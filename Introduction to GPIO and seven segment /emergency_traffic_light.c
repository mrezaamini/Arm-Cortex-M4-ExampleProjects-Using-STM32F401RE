#include "stm32f4xx.h"

void delayMs(int n);
void redOnGreenOff(void);
void redOffGreenOn(void);
void showDigit(void);
void detectP(int bId);
int BCDTo7SegDec(int num);
void initGPIO(void);

int B1P = 0;
int B2P = 0;
int ctr = 9;
int p = 0;
int ledId = 0;
int isCounting = 1;
int MSP = 0;

int main(void)
{
	initGPIO();
	redOnGreenOff();
	while (1)
	{
		if (B1P == 2)
		{
			B1P = 0;
			ctr = 9;
			ledId = 1;
			redOffGreenOn();
			showDigit();
		}
		if (B2P == 3)
		{
			isCounting = 0;
		}
		if (B2P == 4)
		{
			B2P = 0;
			isCounting = 1;
		}
		detectP(0x0800);

		detectP(0x0400);

		delayMs(1);

		if (MSP == 0)
		{
			MSP = 1000;
			if (isCounting == 1)
			{

				if (ctr == 0)
				{
					showDigit();
					ctr = 9;

					if (ledId == 0)
					{
						ledId = 1;
					}
					else
					{
						ledId = 0;
					}
				}
				else
				{
					if (ledId == 1)
					{

						redOffGreenOn();
					}
					else
					{

						redOnGreenOff();
					}
					showDigit();
					ctr--;
				}
			}
		}
		else
		{
			MSP--;
		}
	}
}

void initGPIO(void)
{
	RCC->AHB1ENR |= 0x04;
	GPIOC->MODER = 0x00055555;
	GPIOC->PUPDR = 0x00A00000;
}

int BCDTo7SegDec(int num)
{
	switch (num)
	{
	case 0:
		return 0b1111110;
	case 1:
		return 0b0110000;
	case 2:
		return 0b1101101;
	case 3:
		return 0b1111001;
	case 4:
		return 0b0110011;
	case 5:
		return 0b1011011;
	case 6:
		return 0b1011111;
	case 7:
		return 0b1110000;
	case 8:
		return 0b1111111;
	case 9:
		return 0b1111011;
	}
	return 0;
}

void showDigit(void)
{
	if (isCounting == 1)
	{
		if (ledId == 1)
		{
			GPIOC->ODR &= ~0x0080;
		}
		else
		{
			GPIOC->ODR |= 0x0080;
		}
		GPIOC->ODR &= ~0x7f;
		GPIOC->ODR |= (BCDTo7SegDec(ctr) & 0x7f);
	}
}

void detectP(int bId)
{

	if (p == 0 && (GPIOC->IDR & bId) == bId)
	{
		p = bId;
	}
	else if (p != 0 && (GPIOC->IDR & 0x0c00) == 0)
	{
		if (p == 0x0400)
		{
			B1P++;
		}
		else
		{
			if (ledId == 1)
				B2P++;
		}
		p = 0;
	}
}

void redOnGreenOff(void)
{
	GPIOC->ODR |= 0x0200;
	GPIOC->ODR &= ~0x0100;
}

void redOffGreenOn(void)
{
	GPIOC->ODR |= 0x0100;
	GPIOC->ODR &= ~0x0200;
}
void delayMs(int n)
{
	int i;
	for (; n > 0; n--)
		for (i = 0; i < 4000; i++)
			__NOP();
}