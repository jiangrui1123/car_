#ifndef __TRACKING_H__
#define __TRACKING_H__

extern unsigned char move_flag; // 运动标志，0表示停止，1表示前进，2表示左转，3表示右转
extern unsigned char cross_count; // 计数标志，记录经过的交叉点
extern unsigned char stop_flag; // 停止标志，0表示停止，1表示继续运动
extern unsigned char lost_line; // 丢线标志，0表示未丢线，1表示丢线

void tracking_Init(void);
void tracking(void);


#endif