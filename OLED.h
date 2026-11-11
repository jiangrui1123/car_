#ifndef __OLED_H
#define __OLED_H

/*==============================================================================
 * 0.96 英寸 SSD1306 OLED 驱动（I2C 接口）
 * 目标 MCU : STC89C52RC（8051 内核，Keil C51）
 * 显示方式 : 4 行 x 16 列，8x16 ASCII 字模
 *
 * 默认接线（可在 OLED.c 顶部修改 sbit 定义）：
 *   OLED VCC  -> 模块要求的电源（3.3V 或 5V，见模块说明）
 *   OLED GND  -> GND
 *   OLED SCL  -> P2.1
 *   OLED SDA  -> P2.0   ← 注意：蜂鸣器不能再用 P2.0！
 *============================================================================*/

#define OLED_LINE_MAX       4
#define OLED_COLUMN_MAX     16

void OLED_Init(void);

/* 清屏：只在初始化时用，运行中不要调用 */
void OLED_Clear(void);

/* 显示字符串（Line: 1~4，Column: 1~16） */
void OLED_ShowString(unsigned char Line, unsigned char Column, char *String);

/* 显示一个字符（Line: 1~4，Column: 1~16） */
void OLED_ShowChar(unsigned char Line, unsigned char Column, char Char);
void OLED_ShowNum(unsigned char Line, unsigned char Column,
                 unsigned long Number, unsigned char Length);
void OLED_ShowHexNum(unsigned char Line, unsigned char Column,
                     unsigned long Number, unsigned char Length);

/* 赛道显示：先调一次 OLED_RegionInit() 画 "Region:"，
 * 之后每次 OLED_ShowRegion(2/3/4) 只重画最后那一个数字，约 4ms 不阻塞。 */
void OLED_RegionInit(void);
void OLED_ShowRegion(unsigned char n);

/* 底层接口，一般不需要在应用层调用 */
void OLED_SetCursor(unsigned char Y, unsigned char X);
void OLED_WriteCommand(unsigned char Command);
void OLED_WriteData(unsigned char Data);

#endif
