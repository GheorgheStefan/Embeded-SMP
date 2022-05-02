#include "stm32f10x.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>//rand();
int special_move = 0;



//Definitii
GPIO_InitTypeDef GPIO_InitStructure;
USART_InitTypeDef USART_InitStructure;
char tabla[]={'-','-','-','-','-','-','-','-','-'};

static __IO uint32_t TimingDelay;

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



void initGPIO()
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

void initTimer()
{
	SysTick_Config(SystemCoreClock / 1000);
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

void afisare() 
{
	uint8_t i;
	
    for (i=0; i<9; i++) 
		{
       sendChar(tabla[i]);
       sendString(" ");
       
			 if (i==2 || i==5 || i==8)
			 {				 
				 sendString("\n");
			 }
    }
}

_Bool victorie() 
{
  char winner = '-';
	uint8_t i;
  
  if (tabla[0]==tabla[1] && tabla[1]==tabla[2]) winner=tabla[0]; // linie 1
  if (tabla[3]==tabla[4] && tabla[4]==tabla[5]) winner=tabla[3]; // linie 2
  if (tabla[6]==tabla[7] && tabla[7]==tabla[8]) winner=tabla[6]; // linie 3
  if (tabla[0]==tabla[3] && tabla[3]==tabla[6]) winner=tabla[0]; // coloana 1
  if (tabla[1]==tabla[4] && tabla[4]==tabla[7]) winner=tabla[1]; // coloana 2
  if (tabla[2]==tabla[5] && tabla[5]==tabla[8]) winner=tabla[2]; // coloana 3
  if (tabla[0]==tabla[4] && tabla[4]==tabla[8]) winner=tabla[0]; // diagonala principala
  if (tabla[2]==tabla[4] && tabla[4]==tabla[6]) winner=tabla[2]; // diagonala secundara

  if (winner =='-') 
	{
    winner = '*';
    for (i=0; i<9; i++)
      if (tabla[i]=='-') 
				winner='-';
  }
      
  if (winner=='-') 
		return 0;
	
  else 
	{
     if (winner == 'X') 
		 {
       //afisare(); 
       //sendString("Ati castigat! Felicitari!\n");
     }
     if (winner == '0') 
		 {
			 //sendString("Nucleo a castigat! Mai incercati!\n");
		 }
     if (winner == '*') 
		 {
			 //sendString("Remiza! Plictisitor!\n");
		 }
     return 1;
   }
}
 int Win()
 {
	int i, j;
	int random_choice;
	int block_condition = 0;
	int poz_of_line = 0;
	 // in rows
		for(i=0; i<9; i++)
		{

    if(tabla[i] =='0')
    block_condition++;
    else if (tabla[i] =='-')
        poz_of_line = i;
		
    if (block_condition == 2 && tabla[poz_of_line] == '-')
    {
				tabla[poz_of_line]='0';
				//sendString("Nucleo a ales pozitia: \n");
				sendChar(i+1+'0');
				//sendString("\n");
        return 1;
    }

    if( (i+1) %3 == 0)
    {
        block_condition = 0;
        poz_of_line = 0;
    }
}
//end 
// in columns
	block_condition = 0;
	poz_of_line = 0;

   for(i=0; i<10; i+=3)
   {
			if(tabla[i] =='0')
				block_condition++;
			else if (tabla[i] == '-')
				poz_of_line = i;
			
				if (tabla[5] =='0' && tabla[2] =='0' && tabla[8] == '-')
	{
		tabla[8]='0';
		//sendString("Nucleo a ales pozitia: \n");
		sendChar(8+1+'0');
		//sendString("\n");

    return 1;
	}
			

			if (block_condition == 2 && tabla[poz_of_line] == '-')
				{
					tabla[poz_of_line]='0';
					//sendString("Nucleo a ales pozitia: \n");
					sendChar(poz_of_line+1+'0');
					//sendString("\n");
						return 1;
				}

if( i >= 6 && i != 8)
{
    block_condition = 0;
    poz_of_line = 0;
    i -= 8;
}

}
//end
// in diagonal

	int law = 0;
	block_condition = 0;
	poz_of_line = 0;
	for(i=2; i<= 8; i+=2)
		{
			if(tabla[i] =='0')
				block_condition++;
			else if (tabla[i] =='-')
		{
			poz_of_line = i;
		}

		if (block_condition == 2 && tabla[poz_of_line] == '-')
			{
				tabla[poz_of_line]='0';
				//sendString("Nucleo a ales pozitia: \n");
				sendChar(poz_of_line+1+'0');
				//sendString("\n");
					return 1;
			}
			
			if( i == 6 && law == 0)
			{
				block_condition = 0;
				poz_of_line = 0;
				i -= 8;
				law = 1;
		}
						if (law)
			{
				i+=2;
			}

		}
		return 0;
 }

void mutare0() 
{
  int i, j;
	int random_choice;
	int block_condition = 0;
	int poz_of_line = 0;
	int move = 0;

	//special cases
	if (tabla[4] == '-')
	{
				tabla[4]='0';
				random_choice = 5;
				//sendString("Nucleo a ales pozitia: \n");
				sendChar((random_choice+'0'));
				//sendString("\n");
		return;
	}
	
//victory
 if (Win() == 1)
	return;
// in rows
 
for(i=0; i<9; i++)
{

    if(tabla[i] =='X')
    block_condition++;
    else if (tabla[i] =='-')
        poz_of_line = i;
		
    if (block_condition == 2 && tabla[poz_of_line] == '-')
    {
				tabla[poz_of_line]='0';
				//sendString("Nucleo a ales pozitia: \n");
				sendChar(i+1+'0');
				//sendString("\n");
				move = 1;
				block_condition = 0;
				poz_of_line = 0;
        return;
    }

    if( (i+1) %3 == 0)
    {
        block_condition = 0;
        poz_of_line = 0;
    }
}
//end 
// in columns
	block_condition = 0;
	poz_of_line = 0;

for(i=0; i<10; i+=3)
{
	if(tabla[i] =='X')
    block_condition++;
	else if (tabla[i] == '-')
    poz_of_line = i;
	
	if (tabla[5] =='X' && tabla[2] =='X' && tabla[8] == '-')
	{
		tabla[8]='0';
		//sendString("Nucleo a ales pozitia: \n");
		sendChar(8+1+'0');
		//sendString("\n");
		move = 1;
    return;
	}

if (block_condition == 2 && tabla[poz_of_line] == '-')
{
		tabla[poz_of_line]='0';
		//sendString("Nucleo a ales pozitia: \n");
		sendChar(poz_of_line+1+'0');
		//sendString("\n");
		move = 1;
    return;
}

if( i >= 6 && i != 8)
{
    block_condition = 0;
    poz_of_line = 0;
    i -= 8;
}

}
//end
// in diagonal

	int law = 0;
	block_condition = 0;
	poz_of_line = 0;
	for(i=2; i<= 8; i+=2)
		{
			if(tabla[i] =='X')
				block_condition++;
			else if (tabla[i] =='-')
		{
			poz_of_line = i;
		}

		if (block_condition == 2 && tabla[poz_of_line] == '-')
			{
				tabla[poz_of_line]='0';
				//sendString("Nucleo a ales pozitia: \n");
				sendChar(poz_of_line+1+'0');
				//sendString("\n");
				move = 1;
				block_condition = 0;
				poz_of_line = 0;
					return;
			}
			
			if( i == 6 && law == 0)
			{
				block_condition = 0;
				poz_of_line = 0;
				i -= 8;
				law = 1;
		}
						if (law)
			{
				i+=2;
			}

}
//end
		if (!move)
		{
			random_choice = rand() % 9;
			
			while (tabla[random_choice] != '-')
				random_choice = rand() % 9;
			
			tabla[random_choice]='0';
      //sendString("Nucleo a ales pozitia: \n");
      sendChar(random_choice+1+'0');
      //sendString("\n");	
			move = 0;
			return;
		}
	
}
void mutareX() 
{
   _Bool valid = 0;
   uint8_t m0 = 0;
   while (!valid) 
	 {
     //sendString("Alege o pozitie de la 1 la 9.\n");
     while (m0<'1' || m0>'9') 
		 {
       m0 = readChar();
       Delay(1000);
     }
     
     //sendString("Ati ales pozitia: \n");
     sendChar(m0);
		 m0 = m0 - 48;
     //sendString("\n");
		 
     if (tabla[m0-1]=='-')
		 {
       tabla[m0-1]='X';
       valid = 1;  
		 }

		 
   }
}

int main()
{		
	
		uint8_t i;
		initGPIO();
		initTimer();
		initUsart();
		while(1)
		{
//				sendString("Salut!\n");
//				sendString("Tu vei muta primul - esti cu X.\n");
//				sendString("Tabla de joc are pozitiile numerotate de la 1 la 9.\n");
//				sendString("1 2 3\n");
//				sendString("4 5 6\n");
//	  		sendString("7 8 9\n");
			
				do
				{
					mutareX();
					if (victorie()) break;
					mutare0();
					//afisare();
				}while(!victorie());
				
				Delay(5000);
				

				for (i=0; i<9; i++) 
					tabla[i]='-';
		}
}
