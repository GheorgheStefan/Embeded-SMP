#include "stm32f10x.h"
#include <stdlib.h>
#include <string.h>

volatile uint32_t msTicks;

// IO initialization structure
GPIO_InitTypeDef GPIO_InitStructure;
USART_InitTypeDef USART_InitStructure; 

uint16_t pin[] = {GPIO_Pin_10, GPIO_Pin_3, GPIO_Pin_5, GPIO_Pin_4, GPIO_Pin_10, GPIO_Pin_8, GPIO_Pin_9, GPIO_Pin_7 } ;
GPIO_TypeDef * port[]={GPIOA, GPIOB, GPIOB, GPIOB, GPIOB, GPIOA, GPIOA, GPIOC};


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

void initUsart()
{
		//GPIO_InitTypeDef GPIO_InitStructure;
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

		USART_InitStructure.USART_BaudRate = 115200;
		USART_InitStructure.USART_WordLength = USART_WordLength_8b;
		USART_InitStructure.USART_StopBits = USART_StopBits_1;
		USART_InitStructure.USART_Parity = USART_Parity_No;
		USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
		USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
		USART_Init(USART2, &USART_InitStructure);
		USART_Cmd(USART2, ENABLE);
}

void initGPIO_USART()
{
		 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
		 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
		 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 
		 GPIO_Init(GPIOA, &GPIO_InitStructure);
			
		 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
		 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
		 GPIO_Init(GPIOA, &GPIO_InitStructure);
}

void sendString(char a[])
{
		int i,n;
		n=strlen(a);
		for (i=0;i<n;i++)
		{
			while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);  
			USART_SendData(USART2, a[i]);
		}
}


void sendChar(char a)
{
		while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);  
		USART_SendData(USART2, a);
}

char readChar()
{
	char chr;
	
		if(USART_GetFlagStatus(USART2, USART_FLAG_RXNE) != RESET)
		{
					chr = USART_ReceiveData(USART2);
		} 
	 return chr;
}


void initGPIO_LED()
{
		// enable clock to GPIOA
		RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO, ENABLE );
		GPIO_PinRemapConfig( GPIO_Remap_SWJ_JTAGDisable, ENABLE );	
		// save pin speed and pin mode
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_8 | GPIO_Pin_9;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
		GPIO_Init( GPIOA, &GPIO_InitStructure );
		// save pin speed and pin mode
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_10;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
		GPIO_Init( GPIOB, &GPIO_InitStructure );
		// save pin speed and pin mode
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
		GPIO_Init( GPIOC, &GPIO_InitStructure );
}

// Jocul 1 
void joc0() 
{
	uint8_t i;
	while (1) 
		for (i = 0; i<8; i++)
			port[i]->BSRR = pin[i];
}


void joc1()
	{
		uint8_t i;
		while (1) 
		{
		for (i=0; i<8; i++)
		{
			if ((i%2)==0) 
					port[i]->BSRR = pin[i];
			else 
					port[i]->BRR = pin[i];
		}		
		Delay(1000);
	
		for ( i=0; i<8; i++)
		{		
			if ((i%2)==0)
					port[i]->BRR = pin[i];
			else 
					port[i]->BSRR = pin[i];
		}			
		
			Delay(1000);
	}
}


//Jocul 2 
void joc2() 
{
		uint8_t i;
		while (1) 
		{
			for (i=0; i<8; i++) 
			{
				port[i]->BSRR = pin[i];
				Delay(500); 
			}
			for (i=7;i>=0;i--) 
			{
				port[i]->BRR = pin[i];
				Delay(500);
			}
		}
	}
	
		
// jocul 3 

void joc3() 
{
	uint8_t i=0;
	while (1) 
	{
			port[i]->BSRR = pin[i];
		  Delay(500);
		  i++;
			port[i-1]->BRR = pin[i-1];
			Delay(500);
			if(i==8)
				i=0;
	}
}
// jocul 4 	
void joc4() 
{
		uint8_t pozitie;
		uint8_t aprins;
		while (1) 
		{
			pozitie = rand()%8;
			aprins = rand()%2;
			if( aprins == 0) 
				port[pozitie]->BRR = pin[pozitie];
			else 
				port[pozitie]->BSRR = pin[pozitie];
			
			Delay(100);
		}
}
		
void set_led()//pune 0 pe fiecare pin setat 
{
	uint8_t i;
	for(i=0;i<8;i++)
			port[i]->BRR = pin[i];
}			
	
int main ()
{
	char alege = '5';
	SysTick_Config( SystemCoreClock / 1000 );
	initUsart();
	initGPIO_USART();
	initGPIO_LED();
	set_led();
	
	sendString("Joc de lumini \n");
	sendString("Alegeti varianta de joc (0 - 4) \n");

	
	while(!(strchr("01234", alege)))
	{ 
		 alege = readChar();
		 Delay(500);
	}
	sendString("Ati ales jocul: ");
	sendChar(alege);
	sendString("\nPentru reinitializare apasati butonul Reset al placii ");
	 
	switch (alege) 
	{
		
			case '0':  joc0();
		  case '1':  joc1();
		  case '2':  joc2();
		  case '3':  joc3();
		  case '4':  joc4();
	 }
}
