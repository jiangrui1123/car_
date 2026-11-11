#ifndef __BUZZER_LED_H__
#define __BUZZER_LED_H__

extern unsigned char buzzer_state; // 蜂鸣器状态标志，0表示关闭，1表示开启
extern unsigned int buzzer_time; // 蜂鸣器开启的时间戳

void Buzzer_LED_init(void);
void Buzzer_Task(void); // 5ms 调用一次

#endif