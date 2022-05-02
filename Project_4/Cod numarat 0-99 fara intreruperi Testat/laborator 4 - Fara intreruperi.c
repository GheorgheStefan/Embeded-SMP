#include "stm32f10x.h"
#include <stdlib.h>
#include <string.h>

#define LSB 2
#define MSB 1


// Definitii
GPIO_InitTypeDef GPIO_InitStructure; // IO initialization structure
const int numbers[] = {63, 6, 91, 79, 102, 109, 125, 7, 127, 111};


static __IO uint32_t TimingDelay;

void SysTick_Handler(void)
{
	if (TimingDelay != 0x00)
	{ 
		TimingDelay--;
	}
}
	
void Delay(__IO uint32_t nTime)
{
	 TimingDelay = nTime;
	 while(TimingDelay != 0);
}


void initGPIO_LED()
{	// enable clock to GPIOA
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA, ENABLE );
	// enable clock to GPIOB
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB, ENABLE );
	
	// enable AFIO clock
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_AFIO, ENABLE );
	// disable JTAG and JTAG-DP
	GPIO_PinRemapConfig( GPIO_Remap_SWJ_NoJTRST, ENABLE );    
	GPIO_PinRemapConfig( GPIO_Remap_SWJ_JTAGDisable, ENABLE );
	
	// save pin speed and pin mode
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_8 ;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init( GPIOA, &GPIO_InitStructure );
	
		// save pin speed and pin mode
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_5 | GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init( GPIOB, &GPIO_InitStructure );
}


void shiftOut(uint8_t bitOrder, uint8_t val)
{
	uint8_t i;

	for (i = 0; i < 8; i++)  {
		if ( bitOrder == LSB)
				if (!!(val & (1 << i))) 
					GPIOA->BSRR = GPIO_Pin_10;
				else 
					GPIOA->BRR = GPIO_Pin_10;
				
		else   
				if (!!(val & (1 << (7 - i))))
					GPIOA->BSRR = GPIO_Pin_10;
				else
					GPIOA->BRR = GPIO_Pin_10;
					
		GPIOB->BSRR = GPIO_Pin_3; //clock signal
		GPIOB->BRR	= GPIO_Pin_3;
	}
}	

	

int main ()
{
	uint8_t i, j;
	initGPIO_LED();
	SysTick_Config( SystemCoreClock / 1000 ); //init timer
	GPIOA->BSRR = GPIO_Pin_8; //Alimenteaza Digit 1. 
	GPIOB->BSRR = GPIO_Pin_10; // Alimenteaza Digit 2.
	
	while (1) 
		{
			for (i=0;i<=99;i++) 
					{
						for (j=0;j<=50;j++)
						{
							GPIOB->BSRR = GPIO_Pin_10;
							GPIOB->BRR = GPIO_Pin_5; //Latch pin low
							shiftOut(MSB, numbers[i/10]);
							GPIOB->BSRR = GPIO_Pin_5; //latch pin high
							Delay(10);
							GPIOB->BRR = GPIO_Pin_10;
							
							
						
							GPIOA->BSRR = GPIO_Pin_8;
							GPIOB->BRR = GPIO_Pin_5; //Latch pin low
							shiftOut(MSB, numbers[i%10]);
							GPIOB->BSRR = GPIO_Pin_5; //latch pin high
							Delay(10);
							GPIOA->BRR = GPIO_Pin_8;

						}
					}
		}
	
}
