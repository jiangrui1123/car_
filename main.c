#include <REGX52.H>
#include "tracking.h"
#include "motor.h"
#include "Timer0.h"
#include "OLED.h"
#include "UART.h"
#include "Buzzer_LED.h"

void main(void)
{
    unsigned int now;
    unsigned int last_1 = 0; // 上次执行 5ms 任务的时间戳
    unsigned int last_2 = 0; // 上次执行 100ms 任务的时间戳

    // 初始化
    tracking_Init();
    motor_Init();
    OLED_Init();
    UART_Init();
    Buzzer_LED_init();

    OLED_ShowString(1, 1, "mode:"); // OLED 显示赛道区域提示

    while(1)
    {
         // 获取当前系统毫秒数 now 1毫秒加1
        now = now_ms();

        /*================= 5ms：控制 =================*/
        if ((unsigned int)(now - last_1) >= 5)
        {
            last_1 = now;
            // 5ms 执行的任务
            /**
             * 根据 Mode 的值执行相应的操作
             * 0x01: 循迹模式
             * 0x10: 设置速度为 100
             * 0x20: 设置速度为 60
             * 0x30: 设置速度为 30
             * 0x11: 前进
             * 0x12: 后退
             * 0x13: 左转
             * 0x14: 右转
             * 0x00: 停止
             */
            switch(Mode)
            {
                case 0x01: // 循迹模式
                    tracking();
                    break;
                case 0x10: // 
                    speed = 100; // 设置速度为 100
                    break;
                case 0x20: // 
                    speed = 60; // 设置速度为 60
                    break;
                case 0x30: // 
                    speed = 30; // 设置速度为 30
                    break;
                case 0x11: // 
                    UART_forward(speed); // 前进
                    break;
                case 0x12: // 
                    UART_backward(speed); // 后退
                    break;
                case 0x13: // 
                    UART_left(speed); // 左转
                    break;
                case 0x14: // 
                    UART_right(speed); // 右转
                    break;
                case 0x00: // 停止模式
                    motor_stop(); // 停止电机
                    break;
            }
            if(buzzer_state == 1) // 如果蜂鸣器状态为开启
            {
                Buzzer_Task(); // 5ms 调用一次蜂鸣器任务
            }
        }
         //100ms：OLED显示赛道区域
        if ((unsigned int)(now - last_2) >= 100)
        {
            last_2 = now;
            OLED_ShowHexNum(1, 7, Mode, 2); // OLED 显示模式
            // 100ms 执行的任务
            OLED_ShowNum(1, 9, cross_count, 2); // OLED 显示计次次数
        }
    }
}

void Timer0_Routine() interrupt 1
{
    static unsigned char pwm = 0, ms_div = 0;

    TL0 = 0x48;     /* 200us @11.0592MHz */
    TH0 = 0xFF;

    if (++pwm >= 50) pwm = 0;
    motor_left_PWM  = (pwm < duty_L);
    motor_right_PWM = (pwm < duty_R);

    //200us * 5 = 1ms 此处代码用于产生系统毫秒时基 g_ms，供主循环调用 now_ms() 获取当前系统毫秒数
    if (++ms_div >= 5) { ms_div = 0; g_ms++; }
    //g_ms++ /* 1ms 节拍 */
}

void UART_Routine() interrupt 4
{
    unsigned char temp;
    if (RI == 1)
    {
        temp = SBUF;
        switch(temp)
        {
            case 0x01:
            case 0x10:
            case 0x20:
            case 0x30:
            case 0x11:
            case 0x12:
            case 0x13:
            case 0x14:
            case 0x00:
                Mode = temp; // 更新全局变量 Mode
                break;
            default:
                Mode = 0x00; // 收到无效字节时，设置 Mode 为 0x00，表示停止模式
                break;
        }
        if(TI)TI=0; // 清除中断标志位
        RI = 0;
    }
}
