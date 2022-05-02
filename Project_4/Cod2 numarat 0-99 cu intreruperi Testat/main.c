#include "stm32f10x.h"
#include "stm32f10x_conf.h"
#include <stdlib.h>
#include <string.h>

#define LSB 2
#define MSB 1


// Definitii
GPIO_InitTypeDef GPIO_InitStructure; // IO initialization structure
EXTI_InitTypeDef EXTI_InitStructure;
NVIC_InitTypeDef NVIC_InitStructure;
uint8_t interupt_flag=0, fl_1=0, fl_0=0;
static __IO uint32_t TimingDelay;

const int numbers[] = {63, 6, 91, 79, 102, 109, 125, 7, 127, 111, 119, 124, 57, 94, 121, 113};



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
	
	// enable AFIO clock for PB4
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_AFIO, ENABLE );
	// disable JTAG and JTAG-DP PB4 main function. 
	GPIO_PinRemapConfig( GPIO_Remap_SWJ_NoJTRST, ENABLE );    
	GPIO_PinRemapConfig( GPIO_Remap_SWJ_JTAGDisable, ENABLE );
	
	// save pin speed and pin mode port  

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_8 | GPIO_Pin_5 ; //pin 8 for select digit1
																																				//Pin 10 for data out
																																				// Pin 5 for board led
																																				
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init( GPIOA, &GPIO_InitStructure );
	
		// save pin speed and pin mode port B
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_5 | GPIO_Pin_10; //pin 10 for select digit2
																																			 // pin 3 for clock shiftout 
																																			 // Pin 5 latch pin.
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init( GPIOB, &GPIO_InitStructure );

}

void interupt_Config()
{	
   /* Enable GPIOC clock */
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
 
   /* Configureaza pinul  Pc13 ca pin floating */
   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
   GPIO_Init(GPIOC, &GPIO_InitStructure);
/////////
	  //PA0
   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
   GPIO_Init(GPIOA, &GPIO_InitStructure);
    //PA1
   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
   GPIO_Init(GPIOA, &GPIO_InitStructure);
////////
   /* Connect EXTI13 Line to PC.13 pin */
   
   //Face legatura dintre configurarea GPIO si linia de intrerupere. 
   GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource13); 
   EXTI_InitStructure.EXTI_Line = EXTI_Line13; // Linia cu care lucram de la 0 la 15
   EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt; 			
   EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
   EXTI_InitStructure.EXTI_LineCmd = ENABLE;
   EXTI_Init(&EXTI_InitStructure);
 /////////
   GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource1); 
   EXTI_InitStructure.EXTI_Line = EXTI_Line1; // Linia cu care lucram de la 0 la 15
   EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt; 			
   EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
   EXTI_InitStructure.EXTI_LineCmd = ENABLE;
   EXTI_Init(&EXTI_InitStructure);

   GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource0); 
   EXTI_InitStructure.EXTI_Line = EXTI_Line0; // Linia cu care lucram de la 0 la 15
   EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt; 			
   EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
   EXTI_InitStructure.EXTI_LineCmd = ENABLE;
   EXTI_Init(&EXTI_InitStructure);
 /////////
   /* Enable and set EXTI15_10 Interrupt to the lowest priority */
   NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
   NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;
   NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
 
   NVIC_Init(&NVIC_InitStructure);

/////////
	 NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
	 NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;
	 NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	 
	 NVIC_Init(&NVIC_InitStructure);
     NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;
	 NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;
	 NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	 
	 NVIC_Init(&NVIC_InitStructure);
////////
 }

void EXTI0_IRQHandler(void)
{
   if(EXTI_GetITStatus(EXTI_Line0) != RESET)
   {
	    fl_1=1;
		EXTI_ClearITPendingBit(EXTI_Line0);
   }
}

void EXTI1_IRQHandler(void)
{
   if(EXTI_GetITStatus(EXTI_Line1) != RESET)
   {
	    fl_0=1;
		EXTI_ClearITPendingBit(EXTI_Line1);
   }
}
 
void EXTI15_10_IRQHandler(void)
{
   if(EXTI_GetITStatus(EXTI_Line13) != RESET)
   {
			interupt_flag=1;
     EXTI_ClearITPendingBit(EXTI_Line13);
   }
}


void shiftOut(uint8_t bitOrder, uint8_t val)
{
	uint8_t i;

	for (i = 0; i < 8; i++)  
	{
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

	

int main()
{
	uint8_t i,j,pause=0, k = 0;
	initGPIO_LED();
	SysTick_Config( SystemCoreClock / 1000 ); //initializare counter systick.
	//NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	//NVIC_SetPriority(SysTick_IRQn, 0);
	
	interupt_Config();
	GPIOA->BSRR = GPIO_Pin_8; //Alimenteaza Digit 1. 
	GPIOB->BSRR = GPIO_Pin_10; // Alimenteaza Digit 2.
	while (1) 
	{
			if(interupt_flag==1)
			 {
					for (i=0;i<=90;i++) 
					{
						for (j=0;j<=50;j++)
						{
							GPIOB->BSRR = GPIO_Pin_10;
							GPIOB->BRR = GPIO_Pin_5; //Latch pin low
							shiftOut(MSB, numbers[i/16]);
							GPIOB->BSRR = GPIO_Pin_5; //latch pin high
							Delay(10);
							GPIOB->BRR = GPIO_Pin_10;
							
							
						
							GPIOA->BSRR = GPIO_Pin_8;
							GPIOB->BRR = GPIO_Pin_5; //Latch pin low
							shiftOut(MSB, numbers[i%16]);
							GPIOB->BSRR = GPIO_Pin_5; //latch pin high
							Delay(10);
							GPIOA->BRR = GPIO_Pin_8;

						}

                while(fl_0 == 1 && fl_1 == 0)
						{
							pause = 1;
							j=0; 
							for (j=0;j<=50;j++)
								{
									GPIOB->BSRR = GPIO_Pin_10;
									GPIOB->BRR = GPIO_Pin_5; //Latch pin low
									shiftOut(MSB, numbers[i/16]);
									GPIOB->BSRR = GPIO_Pin_5; //latch pin high
									//Delay(5);
									GPIOB->BRR = GPIO_Pin_10;
							
							
						
									GPIOA->BSRR = GPIO_Pin_8;
									GPIOB->BRR = GPIO_Pin_5; //Latch pin low
									shiftOut(MSB, numbers[i%16]);
									GPIOB->BSRR = GPIO_Pin_5; //latch pin high
									//Delay(5);
									GPIOA->BRR = GPIO_Pin_8;
								}
						}
						if(pause == 1)
						{
							fl_1 = 0;
							fl_0 = 0;
							pause = 0;
						}
						
						fl_1 = 0;
						fl_0 = 0;
						
					}
		  }
	}
}
