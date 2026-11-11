#ifndef __UART_H__
#define __UART_H__

extern volatile unsigned char Mode; // 声明一个全局变量Mode，用于存储接收到的蓝牙数据
extern volatile unsigned char speed;

void UART_Init();
void UART_forward(unsigned char speed);
void UART_backward(unsigned char speed);
void UART_left(unsigned char speed);
void UART_right(unsigned char speed);
//void UART_SendByte(unsigned char Byte);

#endif
