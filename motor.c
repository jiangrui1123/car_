#include <REGX52.h>
#include "Timer0.h"
#include "motor.h"

// 声明左右电机速度的全局变量
volatile unsigned char duty_L = 0, duty_R = 0;

void motor_Init(void)
{
    // 初始化电机控制引脚为低电平
    Left_IN1 = 0;
    Left_IN2 = 0;
    Right_IN1 = 0;
    Right_IN2 = 0;
    Timer0_Init(); // 初始化定时器0，用于PWM控制电机速度
}

void motor_left_forward(void)
{
    // 左电机前进
    Left_IN1 = 0; // “Left_IN1”设置为“high”
    Left_IN2 = 1; // “Left_IN2”设置为“low”
}

void motor_left_backward(void)
{
    // 左电机后退
    Left_IN1 = 1; // “Left_IN1”设置为“low”
    Left_IN2 = 0; // “Left_IN2”设置为“high”
}

void motor_right_forward(void)
{
    // 右电机前进
    Right_IN1 = 0; // “Right_IN1”设置为“high”
    Right_IN2 = 1; // “Right_IN2”设置为“low”
}

void motor_right_backward(void)
{
    // 右电机后退
    Right_IN1 = 1; // “Right_IN1”设置为“low”
    Right_IN2 = 0; // “Right_IN2”设置为“high”
}

void motor_stop(void)
{
    // 停止电机
    duty_L = 0; duty_R = 0;
    Left_IN1 = 0; // “Left_IN1”设置为“low”
    Left_IN2 = 0; // “Left_IN2”设置为“low”
    Right_IN1 = 0; // “Right_IN1”设置为“low”
    Right_IN2 = 0; // “Right_IN2”设置为“low”
    
}
void motor_brake(void)              /* 定点停车用：ENA 常开 + 两端同高 = 短路制动 */
{
    duty_L = 50; duty_R = 50;
    Left_IN1 = 1; Left_IN2 = 1;
    Right_IN1 = 1; Right_IN2 = 1;
}

void motor_speed(unsigned char speed_Left, unsigned char speed_Right)
{
    if (speed_Left  > 100) speed_Left  = 100;
    if (speed_Right > 100) speed_Right = 100;
    duty_L = speed_Left  / 2;      /* 100 级 → 50 级，100 对应常开 */
    duty_R = speed_Right / 2;
}
