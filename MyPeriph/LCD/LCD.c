#include "LCD.h"
#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"

GPIO_InitTypeDef port;

//Массив для перевода в русские буквы
unsigned char mass_Russian_simbol[2][66] = {
  'А', 'Б', 'В', 'Г', 'Д', 'Е', 'Ё', 'Ж', 'З',
  'И', 'Й', 'К', 'Л', 'М', 'Н', 'О', 'П', 'Р',
  'С', 'Т', 'У', 'Ф', 'Х', 'Ц', 'Ч', 'Ш', 'Щ',
  'Ъ', 'Ы', 'Ь', 'Э', 'Ю', 'Я', 'а', 'б', 'в',
  'г', 'д', 'е', 'ё', 'ж', 'з', 'и', 'й', 'к',
  'л', 'м', 'н', 'о', 'п', 'р', 'с', 'т', 'у',
  'ф', 'х', 'ц', 'ч', 'ш', 'щ', 'ъ', 'ы', 'ь',
  'э', 'ю', 'я',
  0x41, 0xA0, 0x42, 0xA1, 0xE0, 0x45, 0xA2, 0xA3, 0xA4, 
  0xA5, 0xA6, 0x4B, 0xA7, 0x4D, 0x48, 0x4F, 0xA8, 0x50,
  0x43, 0x54, 0xA9, 0xAA, 0x58, 0xE1, 0xAB, 0xAC, 0xE2, 
  0xAD, 0xAE, 0x62, 0xAF, 0xB0, 0xB1, 0x61, 0xB2, 0xB3, 
  0xB4, 0xE3, 0x65, 0xB5, 0xB6, 0xB7, 0xB8, 0xA6, 0xBA, 
  0xBB, 0xBC, 0xBD, 0x6F, 0xBE, 0x70, 0x63, 0xBF, 0x79, 
  0xE4, 0x78, 0xE5, 0xC0, 0xC1, 0xE6, 0xC2, 0xC3, 0xC4,
  0xC5, 0xC6, 0xC7
};

//иницилизация портов дисплея
void LCD_port_init()
{
   //DATA LCD
   //DB0=Pin_0
   //...
   //DB7-Pin_7
   GPIO_StructInit(&port);
   port.GPIO_Mode = GPIO_Mode_Out_PP;
   port.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;
   port.GPIO_Speed = GPIO_Speed_2MHz;
   GPIO_Init(GPIOA, &port); 
   
   //COMAND LCD
   //E=0;RS=1
   GPIO_StructInit(&port);
   port.GPIO_Mode = GPIO_Mode_Out_PP;
   port.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1;
   port.GPIO_Speed = GPIO_Speed_2MHz;
   GPIO_Init(GPIOB, &port); 
}

//Базовая иницилизация дисплея и настройка портов
void LCD_Init(){
  LCD_port_init();
  send_LCD(0, 0x38); //Function set
  send_LCD(0, 0xE); //Display ON/OFF
  send_LCD(0, 0x01); //Clear Display
  send_LCD(0, 0x06); //Entry Mode Set
}

//Отправка команд на дисплей(нижний уровень)
void send_LCD(unsigned char d_RS, uint8_t data) {

  //порядок-от старшего к младшему;
  //0xF - D3,D2,D1,D0
  //0xF0 - D7,D6,D5,D4
  GPIOB->ODR |= 0x1;//Установка синхроимпульса E
  
  //Установка RS
  GPIOB->ODR &= 0xFD;
  GPIOB->ODR |= d_RS<<1;
  
  //Установка текущих данных
  GPIOA->ODR &= 0xFF00;    
  GPIOA->ODR = data;
  
  Delay(500);
  //Сброс синхроимпульса E
  GPIOB->ODR &= 0xFE;
  Delay(500);
}


//Перевод русских символов в код LCD
unsigned int searh_russ_code(unsigned int simbol)
{
  for (unsigned int i = 0; i < 66; i++)
  {
    if (mass_Russian_simbol[0][i] == simbol)
      return mass_Russian_simbol[1][i];
  }
  return 0xFF;
}

//Отправка данных на дисплей(средний уровень)
void send_data(uint8_t data)
{
  send_LCD(1, data);
}


//Вывод строки на дисплей(верхний уровень)
void LCD_write(unsigned char* string)
{
  int i = 0;
  while (string[i] != '\0')
  {
    uint8_t code = 0;
    code = searh_russ_code(string[i]);
    if(code == 0xFF) send_data(string[i]);
    else send_data(code);
    i++;
  }
}

//Сдвиг курсора в нужное положение
void set_cursor(uint8_t data)
{
  send_LCD(0, data | (1 << 7));
}

//задержка
void Delay( unsigned int Val) {
  for( ; Val != 0; Val--) {
    __NOP();
  }
}