#include "stm32f10x.h" 
GPIO_InitTypeDef GPIO_InitStructure;
static __IO uint32_t TimingDelay;
void initGPIO()
{
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
 GPIO_Init(GPIOA, &GPIO_InitStructure);
}
void initTimer()
{
SysTick_Config(SystemCoreClock / 1000);
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
int main()
{
initGPIO();
initTimer();
while(1)
{
GPIOA->BSRR = GPIO_Pin_5;
Delay(50);
/* Reset PD0 and PD2 */
GPIOA->BRR = GPIO_Pin_5;
Delay(50);
}
}