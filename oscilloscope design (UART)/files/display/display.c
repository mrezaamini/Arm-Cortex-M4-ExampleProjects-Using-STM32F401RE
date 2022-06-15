#include "stm32f4xx.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#define RS 0x20     
#define RW 0x40   
#define EN 0x80   
#define GPIO0 ((uint16_t)0x0001) 
#define GPIO1 ((uint16_t)0x0002) 
#define GPIO2 ((uint16_t)0x0004)  
#define GPIO3 ((uint16_t)0x0008)  
#define GPIO4 ((uint16_t)0x0010)  
#define GPIO5 ((uint16_t)0x0020)  
#define GPIO6 ((uint16_t)0x0040)  
#define GPIO7 ((uint16_t)0x0080)  
#define GPIO8 ((uint16_t)0x0100)  
#define GPIO9 ((uint16_t)0x0200)  
#define GPIO10 ((uint16_t)0x0400)  
#define GPIO11 ((uint16_t)0x0800)  
#define GPIO12 ((uint16_t)0x1000)  
#define GPIO13 ((uint16_t)0x2000)  
#define GPIO14 ((uint16_t)0x4000)  
#define GPIO15 ((uint16_t)0x8000)  
#define ALLPINS ((uint16_t)0xFFFF)  
#define GLCDON 0x003F
#define GLCDY 0x0040
#define GLCDX 0x00B8
#define GLCDINIT 0x00C0

struct GLCD_DataStruct {
	unsigned int rs;
	unsigned int rw;
	unsigned int en;
	unsigned int cs1;
	unsigned int cs2;
	unsigned int res;
	unsigned int data_pins[8];
};
struct GLCD_DataStruct MYGLCD;
double A=1;
double B=0;
uint8_t myBuffer[256];

	
void drawMap(void);
void DrawPixel(unsigned char dpx, unsigned char CNUM, unsigned char mcs);
void command(unsigned char mcs, unsigned char cmd);
void data(unsigned char mcs, unsigned char data);
void setCol(unsigned char mcs, unsigned char y);
void setRow(unsigned char mcs, unsigned char line);
void fillCol(unsigned char RNUM, unsigned char CNUM, unsigned char mcs) ;
void clearCol(unsigned char RNUM, unsigned char CNUM, unsigned char mcs);
void cursorPos(unsigned char RNUM, unsigned char CNUM, unsigned char mcs);
void clearGlcd(void);
void delayMs(int n);
void PORTS_init(void);
void usart_send(uint8_t data);
uint8_t usart_recieve(void);
void GlcdInit(const int rows, const int cols, const int rw, const int res,
	const int rs, const int en, const int CS1, const int CS2,
	const int d0, const int d1, const int d2, const int d3, const int d4,
	const int d5, const int d6, const int d7);          

int main(void) {
		PORTS_init();
		GlcdInit(64,128, GPIO3, GPIO5, GPIO4, GPIO2, GPIO6, GPIO7,
			GPIO0, GPIO1, GPIO2, GPIO3, GPIO4, GPIO5, GPIO6, GPIO7);
		while(1){
			uint8_t val=usart_recieve();
			if(val=='1'){
				A+=0.1; 
			}else if(val=='2'){
				A-=0.1; 
			}else if(val=='3'){
				B+=10; 
			}else if(val=='4'){
				B-=10;
			}else if(val=='!'){
				for( uint8_t i = 0 ; i < 128 ; i++){
					clearCol(myBuffer[i]/8,i,(i<64));
					uint8_t newVal=usart_recieve();
					newVal=(newVal*A+B)/4;
					if(newVal < 64){
						myBuffer[i]=63 - newVal;
						DrawPixel(myBuffer[i],i,(i<64));
					}
				}
			}
		}
}

void PORTS_init(void) {
    RCC->AHB1ENR |=  0x03;         
	RCC->APB2ENR |= 0x00000010 ; // enable USART1 clock 
	GPIOA->AFR[1] = 0x00000770; //alt7 for USART1 
	GPIOA->MODER = 0x00285555; // enable alternate function for PA9 
	GPIOB->MODER = 0x00005555;
	USART1->BRR = 0x0683; // 9600 baud 
	USART1->CR1 = 0x000C; 
	USART1->CR2 = 0x0000; 
	USART1->CR3 = 0x0000; 
	USART1->CR1 |= 0x2000; 
}

void delayMs(int n) {
    int i;
    for (; n > 0; n--)
        for (i = 0; i < 3195; i++)
						__NOP();
}

void usart_send(uint8_t data) {
	while(!(USART1->SR & 0x0080)){}
	USART1->DR=(data & (uint8_t)0xFF);
}

uint8_t usart_recieve(void){
	while(!(USART1->SR & 0x0020)){}
	return USART1->DR;
}

void GlcdInit(const int rows, const int cols, const int rw, const int res,
		const int rs, const int en, const int CS1, const int CS2,
		const int d0, const int d1, const int d2, const int d3, const int d4,
		const int d5, const int d6, const int d7) {

		if ((rows < 0) || (rows > 64))
			return ;
		if ((cols < 0) || (cols > 128))
			return ;

		MYGLCD.data_pins[0] = d0;
		MYGLCD.data_pins[1] = d1;
		MYGLCD.data_pins[2] = d2;
		MYGLCD.data_pins[3] = d3;
		MYGLCD.data_pins[4] = d4;
		MYGLCD.data_pins[5] = d5;
		MYGLCD.data_pins[6] = d6;
		MYGLCD.data_pins[7] = d7;
        MYGLCD.cs1  = CS1;
		MYGLCD.cs2  = CS2;
		MYGLCD.rw   = rw;
		MYGLCD.res  = res;
		MYGLCD.en   = en;
		MYGLCD.rs   = rs;
		for(int i=0; i<8; ++i){
			GPIOA->BSRR=(MYGLCD.data_pins[i]<<16);

		}
		GPIOB->BSRR=(MYGLCD.rs<<16);
		GPIOB->BSRR=(MYGLCD.rw<<16);
		GPIOB->BSRR=(MYGLCD.res<<16);
		GPIOB->BSRR=(MYGLCD.en<<16);
		GPIOB->BSRR=(MYGLCD.cs1<<16);
		GPIOB->BSRR=(MYGLCD.cs2<<16);
		delayMs(5);
		GPIOB->BSRR=(MYGLCD.res);
		delayMs(50);
		command(0, GLCDON);
		command(1, GLCDON);
		command(0, GLCDINIT);
		command(1, GLCDINIT);
		clearGlcd();
}

void setRow(unsigned char mcs, unsigned char line) {
		unsigned char x_adrress = GLCDX + line;
		command(mcs,x_adrress);
}

void setCol(unsigned char mcs, unsigned char myy) {
		unsigned char y_address = GLCDY + myy;
		command(mcs,y_address);

}


void command(unsigned char mcs, unsigned char cmd) {
		GPIOB->BSRR=(MYGLCD.rs<<16);
		GPIOB->BSRR=(MYGLCD.rw<<16);
		if (mcs == 1) {
			GPIOB->BSRR=(MYGLCD.cs1<<16);
			GPIOB->BSRR=(MYGLCD.cs2);
		} else {
			GPIOB->BSRR=(MYGLCD.cs1);
			GPIOB->BSRR=(MYGLCD.cs2<<16);
		}
		for(int i=0; i<8; ++i) {
			if((cmd>>i) & 0x1)
				GPIOA->BSRR=(MYGLCD.data_pins[i]);
			else
				GPIOA->BSRR=(MYGLCD.data_pins[i]<<16);
		}
		GPIOB->BSRR=(MYGLCD.en);
		delayMs(0);
		GPIOB->BSRR=(MYGLCD.en<<16);
}

void cursorPos(unsigned char RNUM, unsigned char CNUM, unsigned char mcs) {
		if(CNUM>63) CNUM-=64;
		setRow(mcs, RNUM);
		setCol(mcs, CNUM);
}

void fillCol(unsigned char RNUM, unsigned char CNUM, unsigned char mcs) {
		cursorPos(RNUM, CNUM, mcs);
		data(mcs, 0xFF);
}

void clearCol(unsigned char RNUM, unsigned char CNUM, unsigned char mcs) {
		if(CNUM==63 && mcs==1) return;
		if(RNUM==4){ 
			DrawPixel(32,CNUM,mcs);
			return;
		}
		cursorPos(RNUM, CNUM, mcs);
		data(mcs, 0);
}

void data(unsigned char mcs, unsigned char data) {
		GPIOB->BSRR=(MYGLCD.rs);
		GPIOB->BSRR=(MYGLCD.rw<<16);
		if (mcs == 1) {
			GPIOB->BSRR=(MYGLCD.cs1<<16);
			GPIOB->BSRR=(MYGLCD.cs2);
		} else {
			GPIOB->BSRR=(MYGLCD.cs1);
			GPIOB->BSRR=(MYGLCD.cs2<<16);
		}
		for(int i=0; i<8; ++i) {
			if((data>>i) & 0x1)
				GPIOA->BSRR=(MYGLCD.data_pins[i]);
			else
				GPIOA->BSRR=(MYGLCD.data_pins[i]<<16);
		}
		GPIOB->BSRR=MYGLCD.en;
		delayMs(0);
		GPIOB->BSRR=(MYGLCD.en<<16);
}

void clearGlcd(void) {
		command(0, GLCDINIT);
		command(1, GLCDINIT);
		unsigned char row_index, col_index;
		for (row_index = 0; row_index < 8; ++row_index) {
			for (col_index = 0; col_index < 64; ++col_index) {
				clearCol(row_index, col_index,1);
				clearCol(row_index, col_index,0);
			}
		}
		command(0, GLCDINIT);
		command(1, GLCDINIT);
}
	
void DrawPixel(unsigned char dpx, unsigned char CNUM, unsigned char mcs) {
		if(CNUM==63 && mcs==1) return;
		cursorPos(dpx/8, CNUM, mcs);
		unsigned char whereto=0;
		if(dpx>32 && dpx<40)
			data(mcs, (1<<(dpx%8))+1);
		else
			data(mcs, (1<<(dpx%8)));
}

void drawMap(void){
	    for( int fi=0 ; fi < 127 ; fi++){
	    	if(fi<64){
	    		DrawPixel(32,fi,1);
	    	}
		    else{
		    	DrawPixel(32,fi-64,0);
	    	}
	    }
	    for(int fi=0;fi<8;fi++){
	    	fillCol(fi,63,1);
	    }
}