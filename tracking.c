#include "tracking.h"
#include <REGX52.H>
#include "motor.h"
#include "Timer0.h"
#include "Buzzer_LED.h"

#define TURN_MS   200      /* 继续保持转向的时间 */

sbit tracking_left_2 = P2^3;  /* 左侧传感器2引脚 */
sbit tracking_left_1 = P2^4;  /* 左侧传感器1引脚 */
sbit tracking_middle = P2^5;    /* 中间传感器引脚 */
sbit tracking_right_1 = P2^6; /* 右侧传感器1引脚 */
sbit tracking_right_2 = P2^7; /* 右侧传感器2引脚 */

unsigned char move_flag = 0;
unsigned char cross_count = 0;
unsigned char stop_flag = 1;
unsigned char lost_line = 0; /* 丢线标志，0表示未丢线，1表示丢线 */
static unsigned int t_lost = 0;
static unsigned char turn_dir = 0;   /* 0=不转 1=左 2=右 */
static unsigned int  t_turn   = 0;   /* 转弯开始的时刻 */


void tracking_Init(void)
{
    tracking_left_2 = 1;  /* 设置为输入模式 */
    tracking_left_1 = 1;  /* 设置为输入模式 */
    tracking_middle = 1;    /* 设置为输入模式 */
    tracking_right_1 = 1; /* 设置为输入模式 */
    tracking_right_2 = 1; /* 设置为输入模式 */
}

void tracking(void)
{
    static unsigned char last_count = 0;
    static unsigned int t_brake = 0;
    unsigned char now_count = 0;

    if (stop_flag == 0)
    {
        if ((unsigned int)(now_ms() - t_brake) < 200) motor_brake();  /* 先短路制动 */
        else                                          motor_stop();   /* 再断电 */
        return;
    }
    /* ================= 转弯状态优先 ================= */
    if (turn_dir)
    {
                /* 在转弯状态的括号里，时间判断之前 */
        if ((unsigned int)(now_ms() - t_turn) >= 120 &&
            tracking_middle == 0 &&
            tracking_left_1 == 1 && tracking_right_1 == 1)
        {
            turn_dir = 0;                 /* 已经回到线上，提前结束 */
        }

        else if ((unsigned int)(now_ms() - t_turn) < TURN_MS)
        {
            if (turn_dir == 1)  motor_speed(20, 100);   /* 左转：左慢右快 */
            else                motor_speed(100, 20);   /* 右转：左快右慢 */
            motor_left_forward();
            motor_right_forward();
            return;                                     /* 转弯期间跳过循迹判定 */
        }
        turn_dir = 0;                                   /* 时间到，交回循迹 */
    }

    if(tracking_left_2 == 1 && tracking_left_1 == 1 && tracking_middle ==0 && tracking_right_1 == 1 && tracking_right_2 == 1) // 中间传感器检测到黑线
    {
        move_flag = 1;
        motor_left_forward();
        motor_right_forward();
        motor_speed(80, 80); // 设置电机速度为80
    }

    else if(tracking_left_2 == 1 && tracking_left_1 == 0 && tracking_right_1 == 1 && tracking_right_2 == 1) // 左侧传感器1检测到黑线
    {
        move_flag = 2;
        motor_left_forward();
        motor_right_forward();
        motor_speed(60, 80); // 设置电机速度为左轮60，右轮80
    }

    else if(tracking_left_2 == 1 && tracking_left_1 == 1 && tracking_right_1 == 0 && tracking_right_2 == 1) // 右侧传感器1检测到黑线
    {
        move_flag = 3;
        motor_right_forward();
        motor_left_forward();
        motor_speed(80, 60); // 设置电机速度为左轮80，
    }

    else if(tracking_left_2 == 0 && tracking_left_1 == 1 && tracking_middle == 1 && tracking_right_1 == 1 && tracking_right_2 == 1) // 左侧传感器2检测到黑线
    {
        move_flag = 4;
        motor_left_forward();
        motor_right_forward();
        motor_speed(40, 80); // 设置电机速度为左轮40，右轮80
    }

    else if(tracking_left_2 == 1 && tracking_left_1 == 1 && tracking_middle == 1 && tracking_right_1 == 1 && tracking_right_2 == 0) // 右侧传感器2检测到黑线
    {
        move_flag = 5;
        motor_right_forward();
        motor_left_forward();
        motor_speed(80, 40); // 设置电机速度为左轮80，右轮40
    }

    else if(tracking_left_2 == 0 && tracking_left_1 == 0 && tracking_middle == 0 && tracking_right_1 == 1 &&tracking_right_2 == 1) // 右侧和中侧检测到黑线
    {
        move_flag = 7;
        turn_dir = 1;                 /* 启动左转 */
        t_turn   = now_ms();
        buzzer_state = 1;// 打开蜂鸣器
    }

    else if(tracking_left_2 == 1 && tracking_left_1 == 1 && tracking_middle == 0 && tracking_right_1 == 0 && tracking_right_2 == 0) // 左侧和中侧检测到黑线
    {
        move_flag = 8;
        turn_dir = 2;                 /* 启动右转 */
        t_turn   = now_ms();
        buzzer_state = 1;// 打开蜂鸣器
    }

    else if (tracking_left_1==0&&tracking_middle==0&&tracking_right_1==0)
    {
        move_flag = 6;
        now_count=1;
        /* 函数末尾：只在"刚进入"时加一，压着线不会重复加 */
        if(!last_count) 
        {
            cross_count++;
            switch (cross_count)
            {
                case 2:               /* 第 2 次压线：左转 */
                    turn_dir = 1;
                    t_turn   = now_ms();
                    buzzer_state = 1;
                    break;

                case 4:               /* 第 4 次压线：右转 */
                    turn_dir = 2;
                    t_turn   = now_ms();
                    buzzer_state = 1;
                    break;

                case 5:               /* 第 5 次压线：终点 */
                    buzzer_state = 1;
                    break;

                default:
                    break;
            }
        }
        
    }

    else
    {
        move_flag = 9;                          
        if (t_lost == 0) t_lost = now_ms();

        {
            unsigned int dt = (unsigned int)(now_ms() - t_lost);

            if (dt < 400)                       /* 0~0.4s：慢速直行找线 */
            {
                motor_speed(30, 30);
                motor_left_forward();
                motor_right_forward();
            }
            else if (dt < 1500)                 /* 0.4~1.5s：原地左转扫线 */
            {
                motor_speed(20, 60);
                motor_left_backward();
                motor_right_forward();
            }
            else                                /* 1.5s 还找不到才认输 */
            {
                t_brake   = now_ms();           
                stop_flag = 0;
            }
        }
    }
    if (move_flag != 9) t_lost = 0;// 只要没丢线就清零计时器
    last_count = now_count;
    if (cross_count >= 5)          /* 一到 5 就切进停车序列，下一帧开始刹车 */
    {
        t_brake   = now_ms();
        stop_flag = 0;
    }
}
