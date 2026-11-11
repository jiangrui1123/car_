#include <REGX52.H>
#include "Buzzer_LED.h"
#include "Timer0.h"

sbit LED = P3^7; // 定义LED引脚
sbit Buzzer = P2^2; // 定义Buzzer引脚

unsigned char buzzer_state = 0; // 蜂鸣器状态标志，0表示关闭，1表示开启
static unsigned char beep_on = 0;
static unsigned int  beep_t0 = 0;

void Buzzer_LED_init(void)
{
    LED = 0; // 初始化LED为关闭状态
    Buzzer = 0; // 初始化Buzzer为关闭状态
}

void Buzzer_Task(void)                 /* 5ms 调用一次 */
{
    unsigned int now = now_ms();

    if (buzzer_state && !beep_on)      /* 进入沿：刚被触发 */
    {
        beep_on = 1;
        beep_t0 = now;
        LED = 1; Buzzer = 1;
    }
    if (beep_on && (unsigned int)(now - beep_t0) >= 200)
    {
        beep_on = 0;
        buzzer_state = 0;
        LED = 0; Buzzer = 0;
    }
}