#include <REGX52.H>
#include "motor.h"

volatile unsigned char Mode=0x00; // 声明一个全局变量Mode，用于存储接收到的蓝牙数据
volatile unsigned char speed=100;

/**
  * @brief  串口初始化，9600bps@11.0592MHz
  * @param  无
  * @retval 无
  */
void UART_Init()
{
	SCON=0x50;
	PCON |= 0x80;
	TMOD &= 0x0F;		//设置定时器模式
	TMOD |= 0x20;		//设置定时器模式
	TL1 = 0xFA;		//设定定时初值
	TH1 = 0xFA;		//设定定时器重装值
	ET1 = 0;		//禁止定时器1中断
	TR1 = 1;		//启动定时器1
	EA=1;
	ES=1;
}

void UART_forward(unsigned char speed_)
{
	motor_speed(speed_, speed_); // 设置电机速度为当前速度
	motor_left_forward();
	motor_right_forward();
}

void UART_backward(unsigned char speed_)
{
	motor_speed(speed_, speed_); // 设置电机速度为当前速度
	motor_left_backward();
	motor_right_backward();
}

void UART_left(unsigned char speed_)
{
	motor_speed(speed_, speed_); // 设置电机速度为当前速度
	motor_left_backward();
	motor_right_forward();
}

void UART_right(unsigned char speed_)
{
	motor_speed(speed_, speed_ ); // 设置电机速度为当前速度
	motor_left_forward();
	motor_right_backward();
}

/**
  * @brief  串口发送一个字节数据
  * @param  Byte 要发送的一个字节数据
  * @retval 无
  */
/*
void UART_SendByte(unsigned char Byte)
{
	SBUF=Byte;
	while(TI==0);
	TI=0;
}
*/
/*串口中断函数模板
void UART_Routine() interrupt 4
{
	if(RI==1)
	{
		
		RI=0;
	}
}
*/
