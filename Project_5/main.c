#include "stm32f10x.h"
#include "stm32f10x_conf.h"
#include "lcd.h"
#include "delay.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

// Definitii
GPIO_InitTypeDef GPIO_InitStructure; // IO initialization structure
EXTI_InitTypeDef EXTI_InitStructure;
NVIC_InitTypeDef NVIC_InitStructure;
TIM_TimeBaseInitTypeDef timer_base;
TIM_TimeBaseInitTypeDef timer_base2;
volatile uint8_t flag1=0;
volatile uint8_t flag2=0;
volatile uint16_t count1=0;
volatile uint16_t count2=0;
volatile uint8_t ButtonPressed1 = 0;
volatile uint8_t ButtonPressed2 = 0;
volatile uint16_t timp1 = 0;
volatile uint16_t timp2 = 0;


uint8_t custom_char2[] = { 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F };

void interupt_Config()
{	
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
 
	 //Setare pini intrerupere GPIO
   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
   GPIO_Init(GPIOC, &GPIO_InitStructure);
	 GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource1);
	
	 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
   GPIO_Init(GPIOA, &GPIO_InitStructure);
	 GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource0);
 
   /* Configurarea liniilor de intrerupere */
    EXTI_InitStructure.EXTI_Line = EXTI_Line1;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);
	 
	  EXTI_InitStructure.EXTI_Line = EXTI_Line0;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);
		
   /* Configurarea NVIC corespunzator liniilor de intrerupere */
    NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	  NVIC_Init(&NVIC_InitStructure);
	 
	  NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	  NVIC_Init(&NVIC_InitStructure);
}

void timer_init()
{ 
		//Enable clock pentru timer 3. 
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	  //To do:  Seteaza clock-ul pentru TIM4 ! 
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);

		TIM_TimeBaseStructInit(&timer_base);
		timer_base.TIM_CounterMode = TIM_CounterMode_Up;
		timer_base.TIM_Prescaler = 7200;
		TIM_TimeBaseInit(TIM3, &timer_base);
		TIM_Cmd(TIM3, ENABLE);

		//to Do:  Seteaza TIM 4.
	
		TIM_TimeBaseStructInit(&timer_base);
		timer_base.TIM_CounterMode = TIM_CounterMode_Up;
		timer_base.TIM_Prescaler = 7200;
		TIM_TimeBaseInit(TIM4, &timer_base);
		TIM_Cmd(TIM4, ENABLE);
}

 //Handler pentru liniile 1
void EXTI1_IRQHandler(void)
{
	uint32_t i=0;

   if(EXTI_GetITStatus(EXTI_Line1) != RESET)
   {
		for(i=0;i<30000;i++)			// debouncing pt. buton
		 __nop();
		 
		 if((GPIOC->IDR & GPIO_Pin_1) != 0)  //IDR Data Input Register. Registru pe 32 de biti, stocheaza starea fiecarui pin la fiecare ciclu de ceas al magistralei APB2.
		 {
			 if(ButtonPressed1 == 0)
			 {
				 ButtonPressed1 = 1;
				 TIM3->CNT = 0;						//reinitializare counter
			 }
			 else
			 {
				 ButtonPressed1 = 0;
					 flag1=1;
					 flag2=1;
				 timp1 = TIM3->CNT;				// citirea valorii din counter
					 
			 }
		 }
		 EXTI_ClearITPendingBit(EXTI_Line1);
   }
}


void EXTI0_IRQHandler(void)
{
	uint32_t i=0;

   if(EXTI_GetITStatus(EXTI_Line0) != RESET)
   {
		for(i=0;i<30000;i++)			// debouncing pt. buton
		 __nop();
		 
		 if((GPIOA->IDR & GPIO_Pin_0) != 0)  //IDR Data Input Register. Registru pe 32 de biti, stocheaza starea fiecarui pin la fiecare ciclu de ceas al magistralei APB2.
		 {
			 if(ButtonPressed2 == 0)
			 {
				 ButtonPressed2 = 1;
				 TIM4->CNT = 0;						//reinitializare counter
			 }
			 else
			 {
				 ButtonPressed2 = 0;
					 flag1=1;
					 flag2=1;
				 timp2 = TIM4->CNT;				// citirea valorii din counter
					 
			 }
		 }
		 EXTI_ClearITPendingBit(EXTI_Line0);
   }
}

int main()
{
	char s[30];
	uint8_t p1r,p2r;
	float scor1;
	float scor2=5.0;//Scor minim pt player2.
	
	lcd16x2_init(LCD16X2_DISPLAY_ON_CURSOR_OFF_BLINK_OFF);
	interupt_Config();
	timer_init();
	lcd16x2_create_custom_char(0, custom_char2);
	
	while(1)
	{
		if(flag1==0 && flag2==0)
		{
				ButtonPressed1=0;
				ButtonPressed2=0;
				//Alegere timp random player 1
				p1r=rand()%5;
				while(p1r==0)
				{
					p1r=rand()%5; //
				}
				//Alegere timp random player 2
				p2r=rand()%5;
				while(p2r==0)
				{
					p2r=rand()%5;
				}
				
				while(flag1==0 && flag2==0)
				{
						//partitionare ecran in doua
						lcd16x2_gotoxy(0, 0);
						lcd16x2_put_custom_char(7, 0, 0);
						lcd16x2_put_custom_char(8, 0, 0);
						lcd16x2_put_custom_char(7, 1, 0);
						lcd16x2_put_custom_char(8, 1, 0);
					
					  //Scriere nume jucatori.
						lcd16x2_gotoxy(0, 0);
						lcd16x2_puts("Player1");
					  //Afisare timp intre cele doua apasari 
						lcd16x2_gotoxy(3, 1);
						sprintf(s, "%ds",p1r);
						lcd16x2_puts(s);
							
						lcd16x2_gotoxy(9, 0);
						lcd16x2_puts("Player2");
							
							
						lcd16x2_gotoxy(12, 1);
						sprintf(s, "%ds",p2r);
						lcd16x2_puts(s);
						
						DelayMs(1000);
				}
		}
			
		if(flag1==1 && flag2==1)
		{	
			lcd16x2_clrscr();
			scor1=fabs(p1r-(float)timp1/10000);
			scor2=fabs(p2r-(float)timp2/10000);

			
			sprintf(s, "%.2fs  S1=%.2f\n",(float)timp1/10000,scor1);
			lcd16x2_gotoxy(0, 0);
			lcd16x2_puts(s);
			
			sprintf(s, "%.2fs  S2=%.2f\n",(float)timp2/10000,scor2);
			lcd16x2_gotoxy(0, 1);
			lcd16x2_puts(s);

			DelayMs(3000);
			
			if(scor1>scor2)
			{
					lcd16x2_clrscr();
					lcd16x2_gotoxy(0, 0);
					lcd16x2_puts("Player2 a ");
			   	lcd16x2_gotoxy(0, 1);
					lcd16x2_puts("castigat!!");
					DelayMs(5000);
			}
			else
			{
					lcd16x2_clrscr();
					lcd16x2_gotoxy(0, 0);
					lcd16x2_puts("Player1 a ");
			   	lcd16x2_gotoxy(0, 1);
					lcd16x2_puts("castigat!!");
					DelayMs(5000);
			}
			
			lcd16x2_clrscr();
			flag1=0;
			flag2=0;

		}
	}
}
