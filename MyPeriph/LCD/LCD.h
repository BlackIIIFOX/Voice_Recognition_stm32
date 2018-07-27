#include "stm32f10x.h"

#ifndef LCD
#define LCD
void LCD_port_init();//������������ ������ LCD
void LCD_Init();//������� ��������� LCD
void send_LCD(unsigned char d_RS, uint8_t data);//�������� ������ �� ����
void Delay(unsigned int Val); //�������� ��� ��������
void set_cursor(uint8_t data);//����� ������� � ������ ���������(�� �������)
void LCD_write(unsigned char* string);//����� ������ �� ����� LCD
unsigned int searh_russ_code(unsigned int simbol);//����� ������� �������� � ������� �������
void send_data(uint8_t data);//�������� ������ �� �������
#endif