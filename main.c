#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_adc.h"
#include "stm32f10x_usart.h"
#include "stdio.h"
#define F_CPU 72000000UL




void Init_Timer();//объявление функции иницилизации таймера
void Init_ADC();//объявление функции иницилизации ADC
void Init_UART();//объявление функции иницилизации UART
void Send_UART(unsigned char* string); //объявление функции отправки по UART
void Delay();
int i=0;

unsigned char SteateConnection,PrevStateConnection;

int main()
{
  SystemInit();
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);//Для дисплея 
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);//Для таймера 
  RCC->APB2ENR	|= RCC_APB2ENR_USART1EN|RCC_APB2ENR_AFIOEN;//Для UART
  
  SteateConnection = 0;
  
  Init_UART();
  Init_ADC();
  Init_Timer();
  
  while(1)
  {
    if(SteateConnection != PrevStateConnection)
    {
      if(SteateConnection==1)
      {
        TIM_Cmd(TIM3, ENABLE);
        i = 0;
      }
      else 
        TIM_Cmd(TIM3, DISABLE);
      
      PrevStateConnection = SteateConnection;
    }
  }
  //while(SteateConnection == 0);
  //Init_Timer();
 
  /*
  Init_UART();
  //USART1->DR 	= 0x4E;
  Send_UART("Data\r");
  Init_ADC();
  Init_Timer();
  */
  
  while(1);
}


void Init_Timer()
{
  TIM_TimeBaseInitTypeDef base_timer;
  TIM_TimeBaseStructInit(&base_timer);
  
  //20000кГЦ
  base_timer.TIM_Prescaler = 360 - 1; //первый делитель частоты(максимум 65к)
  base_timer.TIM_Period = 10-1;         //второй делитель частоты
  //base_timer.TIM_Prescaler = 36000 - 1; //кажду секунду
  //base_timer.TIM_Period = 2000-1;       //+
  //base_timer.TIM_Prescaler = 3600 - 1; //кажду милисекунду
  //base_timer.TIM_Period = 2000-1;     //
  TIM_TimeBaseInit(TIM3, &base_timer);
  
  TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
  NVIC_EnableIRQ(TIM3_IRQn);
  NVIC_SetPriority (TIM3_IRQn, 2);
  //TIM_Cmd(TIM3, ENABLE);
  
  ;
}


void TIM3_IRQHandler()
{
  TIM_ClearITPendingBit(TIM3, TIM_IT_Update);

  ADC_Cmd (ADC1,ENABLE);  //enable ADC1
  ADC_SoftwareStartConvCmd(ADC1, ENABLE);
  i++;
  
}

void Init_ADC()
{
  //ADC
  ADC_InitTypeDef ADC_InitStructure;
  GPIO_InitTypeDef  GPIO_InitStructure;
  // input of ADC (it doesn't seem to be needed, as default GPIO state is floating input)
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AIN;
  GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_0 ;        // that's ADC1 (PA1 on STM32)
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  
  //clock for ADC (max 14MHz --> 72/6=12MHz)
  RCC_ADCCLKConfig (RCC_PCLK2_Div6);
  // enable ADC system clock
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
  
  // define ADC config
  ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
  ADC_InitStructure.ADC_ScanConvMode = DISABLE;
  ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;  // we work in continuous sampling mode
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_NbrOfChannel = 0;
  
  ADC_RegularChannelConfig(ADC1,ADC_Channel_1, 0,ADC_SampleTime_41Cycles5); // define regular conversion config
  ADC_Init ( ADC1, &ADC_InitStructure);   //set config of ADC1
  
  // enable ADC
  ADC_Cmd (ADC1,ENABLE);  //enable ADC1
  
  
  // ADC calibration (optional, but recommended at power on)
  ADC_ResetCalibration(ADC1); // Reset previous calibration
  while(ADC_GetResetCalibrationStatus(ADC1));
  ADC_StartCalibration(ADC1); // Start new calibration (ADC must be off at that time)
  while(ADC_GetCalibrationStatus(ADC1));
  
  ADC_ClearITPendingBit(ADC1,ADC_IT_EOC);
  ADC_ITConfig(ADC1, ADC_IT_EOC, ENABLE);
  NVIC_EnableIRQ(ADC1_2_IRQn);
  NVIC_SetPriority (ADC1_2_IRQn, 2);
}

void ADC1_2_IRQHandler()
{ 
  if(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) != RESET)
  {
    int16_t ADC_Result;
    ADC_Result = ADC_GetConversionValue(ADC1);
    ADC_ClearITPendingBit(ADC1,ADC_IT_EOC);
    ADC_Cmd (ADC1,DISABLE);  //enable ADC1
    
    
    /*
    unsigned char out_string[100];
    sprintf( out_string, "%d", ADC_Result );
    Send_UART(out_string);
    Send_UART(";\r");
    */
    
    USART1->DR = ADC_Result>>8;
    while(!(USART1->SR&0x80));
    USART1->DR = ADC_Result;
    while(!(USART1->SR&0x80));
    
    if(i==20000){
      
      __disable_irq (); 
      TIM_Cmd(TIM3, DISABLE);
      //Delay(7200000);
      //Send_UART("Подождем");
      //Delay(7200000);
      __enable_irq (); 
      i=0;
    }
    /*
    USART1->DR = ADC_Result>>8;
    while(!(USART1->SR&0x80));
    USART1->DR = (uint8_t)(ADC_Result&0xFF);
    */
  }
}


void USART1_IRQHandler()
{
  if (USART1->SR & USART_SR_RXNE)
	{ 
             unsigned char sim;
             sim = USART1->DR;
             if(sim == 'R')
             {
               SteateConnection = 1;
             }
             if(sim == 'E')
             {
               SteateConnection = 0;
             }
        }
}


void Init_UART()
{
  	GPIOA->CRH	&= ~GPIO_CRH_CNF9;	// Clear CNF bit 9
        GPIOA->CRH	|= GPIO_CRH_CNF9_1;	// Set CNF bit 9 to 10 - AFIO Push-Pull
        GPIOA->CRH	|= GPIO_CRH_MODE9_0;
        
        GPIOA->CRH	&= ~GPIO_CRH_CNF10;	// Clear CNF bit 10
        GPIOA->CRH	|= GPIO_CRH_CNF10_0;	// Set CNF bit 10 to 01 = HiZ
        GPIOA->CRH	&= ~GPIO_CRH_MODE10;	// Set MODE bit 10 to Mode 01 = 10MHz
        
        //((72 000 000/Bodrate)/16)
        //USART1->BRR 	= 0x1D4C;					        // Bodrate for 9600 on 72Mhz
        USART1->BRR 	= 0x4E;					        // Bodrate for 921600 on 72Mhz
        //USART1->BRR 	= 0x270;                                        // Bodrate for 115200 on 72Mhz
        USART1->CR1 	|= USART_CR1_UE | USART_CR1_TE | USART_CR1_RE |		// USART1 ON, TX ON, RX ON
                             USART_CR1_RXNEIE;					// RXNE Int ON	
        
        NVIC_EnableIRQ (USART1_IRQn);
        NVIC_SetPriority (USART1_IRQn, 1);
        __enable_irq ();
}


void Send_UART(unsigned char* string)
{
      int i = 0;
      while (string[i] != '\0')
      {
        USART1->DR = string[i];
        while(!(USART1->SR&0x80));
        i++;
      }
}


void Delay(int i)
{
  while(i>0)
    i--;
}
