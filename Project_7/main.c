#include "stm32f10x.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h> //rand();
#include "lcd.h"
#include "delay.h"

#define ADC1_DR_Address ((uint32_t)0x4001244C)


//Definitii
GPIO_InitTypeDef GPIO_InitStructure;
USART_InitTypeDef USART_InitStructure;
ADC_InitTypeDef ADC_InitStructure;
DMA_InitTypeDef DMA_InitStructure;
__IO uint16_t ADCConvertedValue[4];

static __IO uint32_t TimingDelay;


uint8_t man1[] = {0x0C, 0x0C, 0x00, 0x0E, 0x1C, 0x0C, 0x1A, 0x13};
uint8_t man2[] = {0x0C, 0x0C, 0x00, 0x0C, 0x0C, 0x0C, 0x0C, 0x0E};
uint8_t man3[] = {0x0C, 0x0C, 0x00, 0x1E, 0x1D, 0x1F, 0x10, 0x00};
uint8_t wall[] = {0x00, 0x00, 0x00, 0x0E, 0x0E, 0x0E, 0x0E, 0x0E};


void RCC_Configuration(void)
{
 #if defined (STM32F10X_LD_VL) || defined (STM32F10X_MD_VL) || defined (STM32F10X_HD_VL)
   /* ADCCLK = PCLK2/2 */
   RCC_ADCCLKConfig(RCC_PCLK2_Div2); 
 #else
   /* ADCCLK = PCLK2/4 */
   RCC_ADCCLKConfig(RCC_PCLK2_Div4); 
 #endif
   /* Enable peripheral clocks ------------------------------------------------*/
   /* Enable DMA1 clock */
   RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
 
   /* Enable ADC1 and GPIOC clock */
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_GPIOC, ENABLE);
}
 

 void GPIO_Configuration(void)
{
		//Configurarea prinilor analogici 
		 GPIO_InitTypeDef GPIO_InitStructure;
		 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
		 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 |  GPIO_Pin_5 | GPIO_Pin_3;
		 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
		 GPIO_Init(GPIOC, &GPIO_InitStructure);
		
		 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
		 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
		 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //pp output - push-pull .
		 GPIO_Init(GPIOA, &GPIO_InitStructure);
			
		 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
		 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
		 GPIO_Init(GPIOA, &GPIO_InitStructure);
}



void initUsart()
{
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


void initDma()
{
   DMA_DeInit(DMA1_Channel1);
   DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_Address;
   DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)ADCConvertedValue;
   DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
   DMA_InitStructure.DMA_BufferSize = 3;
   DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
   DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
   DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
   DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
   DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
   DMA_InitStructure.DMA_Priority = DMA_Priority_High;
   DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
   DMA_Init(DMA1_Channel1, &DMA_InitStructure);
   
   /* Enable DMA1 channel1 */
   DMA_Cmd(DMA1_Channel1, ENABLE);
	
   ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
   ADC_InitStructure.ADC_ScanConvMode = ENABLE;
   ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
   ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
   ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
   ADC_InitStructure.ADC_NbrOfChannel = 3;
   ADC_Init(ADC1, &ADC_InitStructure);
 
   /* ADC1 regular channel14 configuration */ 
  // ADC_RegularChannelConfig(ADC1, ADC_Channel_13, 3, ADC_SampleTime_55Cycles5); //pc3
	 ADC_RegularChannelConfig(ADC1, ADC_Channel_13, 3, ADC_SampleTime_55Cycles5); // pentru random
   ADC_RegularChannelConfig(ADC1, ADC_Channel_14, 1, ADC_SampleTime_55Cycles5); //pC4 
   ADC_RegularChannelConfig(ADC1, ADC_Channel_15, 2, ADC_SampleTime_55Cycles5); //pc5
 
   /* Enable ADC1 DMA */
   ADC_DMACmd(ADC1, ENABLE);
   
   /* Enable ADC1 */
   ADC_Cmd(ADC1, ENABLE);
 
   /* Enable ADC1 reset calibration register */   
   ADC_ResetCalibration(ADC1);
   /* Check the end of ADC1 reset calibration register */
   while(ADC_GetResetCalibrationStatus(ADC1));
 
   /* Start ADC1 calibration */
   ADC_StartCalibration(ADC1);
   /* Check the end of ADC1 calibration */
   while(ADC_GetCalibrationStatus(ADC1));
      
   /* Start ADC1 Software Conversion */ 
   ADC_SoftwareStartConvCmd(ADC1, ENABLE);
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

int main()
{		
		uint8_t i=0; uint8_t j=0;
		uint8_t y=0; uint8_t x=0;
		char chr[50];
	
		GPIO_Configuration();
		RCC_Configuration();
		initUsart();
		initDma();
	
		lcd16x2_init(LCD16X2_DISPLAY_ON_CURSOR_OFF_BLINK_OFF);
		lcd16x2_create_custom_char(0, man2);
		lcd16x2_create_custom_char(1, man3);
		lcd16x2_create_custom_char(2, man1);
		lcd16x2_create_custom_char(3, wall);
	
	
	
		while(1)
		{
			//sprintf(chr,"$%d %d %d;",ADCConvertedValue[0],ADCConvertedValue[1],ADCConvertedValue[2]);
			//sendString(chr);
			// 0 dreapta staga, dreapta
			// 1, 2 sus / jos
			if(ADCConvertedValue[1]<1500)
			{
				y =1;
			}
			if(ADCConvertedValue[1]>2500)
			{
				y = 0;
			}
			if(ADCConvertedValue[0]>2500)
			{
				x ++;
			}
			if(ADCConvertedValue[0]<1500)
			{
				x --;
			}
			// x, y, miscare
			lcd16x2_put_custom_char(x, y, i%4);
			lcd16x2_gotoxy(0, 0);
			//lcd16x2_puts(chr);
			DelayMs(300);
			if (x > 15)
				x--;
			if (x < 0)
				x = 0;
			
			lcd16x2_clrscr();
			i++;
		}
}
 