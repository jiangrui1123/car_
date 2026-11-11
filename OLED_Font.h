#ifndef __OLED_FONT_H
#define __OLED_FONT_H

/* OLED字模库，宽8像素，高16像素
 * Keil C51: 使用 code 关键字把字模放在 Flash 中，否则会占用宝贵的内部 RAM。
 * 注意：字模数据定义在 OLED_Font.c 中，本文件只做外部声明，
 *       可以被任意多个 .c 文件包含（extern 不会重复分配空间）。
 */
extern const unsigned char code OLED_F8x16[][16];

#endif
