#include "stm32f10x.h"

#ifndef LCD
#define LCD
void LCD_port_init();//иницилизация портов LCD
void LCD_Init();//базовая настройка LCD
void send_LCD(unsigned char d_RS, uint8_t data);//отправка данных на порт
void Delay(unsigned int Val); //Задержка при передаче
void set_cursor(uint8_t data);//сдвиг курсора в нужное положение(по таблице)
void LCD_write(unsigned char* string);//Вывод строки на экран LCD
unsigned int searh_russ_code(unsigned int simbol);//поиск русских символов в таблице дисплея
void send_data(uint8_t data);//отправка данных на дисплей
#endif