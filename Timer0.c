#include <REGX52.H>

volatile unsigned int g_ms=0;      /* 系统毫秒时基 */

/**
  * @brief  定时器0初始化，200us@11.0592MHz
  * @param  无
  * @retval 无
  */
void Timer0_Init(void)
{
	TMOD &= 0xF0;		//设置定时器模式
	TMOD |= 0x01;		//设置定时器模式
	TL0 = 0x48;		//设置定时初值
	TH0 = 0xFF;		//设置定时初值
	TF0 = 0;		//清除TF0标志
	TR0 = 1;		//定时器0开始计时
	ET0=1;
	EA=1;
	PT0=0;
}

/**
  * @brief  获取当前系统毫秒数
  * @param  无
  * @retval 当前系统毫秒数
  */
unsigned int now_ms(void)
{
    unsigned int t;
    // 关定时器中断再读，防止 16 位撕裂
    ET0 = 0; t = g_ms; ET0 = 1;
    return t;
}

/*定时器中断函数模板
void Timer0_Routine() interrupt 1
{
	static unsigned int T0Count;
	TL0 = 0x78;		//设置定时初值
	TH0 = 0xFF;		//设置定时初值
	T0Count++;
	if(T0Count>=1000)
	{
		T0Count=0;
		
	}
}
*/
