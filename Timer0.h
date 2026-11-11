#ifndef __TIMER0_H__
#define __TIMER0_H__

extern volatile unsigned int g_ms;      /* 系统毫秒时基 */
void Timer0_Init(void);
unsigned int now_ms(void);

#endif
