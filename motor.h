#ifndef __MOTOR_H__
#define __MOTOR_H__

sbit motor_left_PWM = P1^0;
sbit motor_right_PWM = P1^5;
sbit Left_IN1 = P1^1;
sbit Left_IN2 = P1^2;
sbit Right_IN1 = P1^3;
sbit Right_IN2 = P1^4;

extern volatile unsigned char duty_L, duty_R;   /* 0~50，给中断用 */

void motor_Init(void);
void motor_left_forward(void);
void motor_left_backward(void);
void motor_right_forward(void);
void motor_right_backward(void);
void motor_stop(void);
void motor_speed(unsigned char speed_Left, unsigned char speed_Right);
void motor_brake(void);

#endif