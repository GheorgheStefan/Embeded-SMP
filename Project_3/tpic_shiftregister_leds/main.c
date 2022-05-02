#include "stm32f10x.h"
#include <stdlib.h>
#include <string.h>

#define LSB 2
#define MSB 1


// Definitii
GPIO_InitTypeDef GPIO_InitStructure; // IO initialization structure
USART_InitTypeDef USART_InitStructure; //USART initialization structure

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
	 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //pp output - push-pull .
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
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 ;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init( GPIOA, &GPIO_InitStructure );
	
		// save pin speed and pin mode
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_5;
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


// Jocul 1 de lumini aprinde alternativ la 1 s ledurile de rang impar si ledurile de rang par
void joc1() {
	while(1) {
		GPIOB->BRR = GPIO_Pin_5; //Latch pin low
		shiftOut(MSB, 85);
		GPIOB->BSRR = GPIO_Pin_5; //latch pin high
		Delay(1000);
		GPIOB->BRR = GPIO_Pin_5; //Latch pin low
		shiftOut(MSB, 170);
		GPIOB->BSRR = GPIO_Pin_5; //latch pin high
		Delay(1000);
		}
	}
	

	//Jocul 2 aprinde si stinge led-urile pe rand
void joc2() {
		int i;
		int afisare =0;
		while (1) {
			for (i=0; i<9; i++) {
					afisare = afisare + (1<<i);
					GPIOB->BRR = GPIO_Pin_5; //Latch pin low
					shiftOut(MSB, afisare);
					GPIOB->BSRR = GPIO_Pin_5; //latch pin high
					Delay(500);
				}
				for(i=8; i>=0; i--) {
					afisare = afisare - (1<<i);
					GPIOB->BRR = GPIO_Pin_5; //Latch pin low
					shiftOut(MSB, afisare);
					GPIOB->BSRR = GPIO_Pin_5; //latch pin high
					Delay(500);
				}
			}
		}
	
		
// aprinde cate un led
void joc3() {
		int i;
		int afisare = 1;
		while (1) {
			for (i=0;i<8;i++) {
				GPIOB->BRR = GPIO_Pin_5; //Latch pin low
				shiftOut(MSB, afisare<<i);
				GPIOB->BSRR = GPIO_Pin_5; //latch pin high
				Delay(500);
			}
		}
	}
		

		
void joc4() {
	int configuratie;
	while (1) 
	{
		configuratie = rand()%256;
		GPIOB->BRR = GPIO_Pin_5; //Latch pin low
		shiftOut(MSB, configuratie);
		GPIOB->BSRR = GPIO_Pin_5; //latch pin high
		Delay(100);
		}
	}
		
		
	

int main ()
{
	char alege = '0';
	SysTick_Config( SystemCoreClock / 1000 ); //init timer
	initUsart();
	initGPIO_USART();
	initGPIO_LED();
	
	sendString("Joc de lumini \n");
	sendString("Alegeti varianta de joc (1 - 4) \n");

	
	while( ! (strchr("1234", alege)) )
	 { alege = readChar();
		 Delay(500);
	 }
	sendString("Ati ales jocul: ");
	sendChar(alege);
	sendString("\nPentru reinitializare apasati butonul Reset al placii ");
	 
	switch (alege) {
		 case '1':  joc1();
		 case '2':  joc2();
		 case '3':  joc3();
		 case '4':  joc4();
	 }
	}
