#include <REGX52.H>
#include <INTRINS.H>
#include "OLED.h"
#include "OLED_Font.h"

/*==============================================================================
 * 引脚配置
 *   SDA = P2.0，SCL = P2.1（外接 4.7k 上拉；模块 3.3V 供电时上拉到 3.3V）
 *============================================================================*/
sbit OLED_SCL = P2^1;
sbit OLED_SDA = P2^0;

#define OLED_ADDR   0x78

/* I2C 位延时：1 个机器周期（≈1us@12MHz），SCL 约 60~70kHz。
 * 杜邦线较长、接线较乱导致偶发花屏时，改成两次 _nop_()。*/
#define OLED_I2C_DELAY()    _nop_()

/*==============================================================================
 * 内部：毫秒延时（只在上电初始化时用一次）
 *============================================================================*/
static void OLED_DelayMs(unsigned int ms)
{
    unsigned char i, j;

    while (ms--)
    {
        i = 2;
        j = 239;
        do
        {
            while (--j);
        } while (--i);
    }
}

/*==============================================================================
 * 内部：软件 I2C
 *   写操作全部"一次事务连续发多个字节"，不再每字节 Start/Stop，
 *   同样数据量的总线时间约为原来 1/3。
 *============================================================================*/

static void OLED_I2C_Start(void)
{
    OLED_SDA = 1;
    OLED_SCL = 1;
    OLED_I2C_DELAY();
    OLED_SDA = 0;
    OLED_I2C_DELAY();
    OLED_SCL = 0;
}

static void OLED_I2C_Stop(void)
{
    OLED_SDA = 0;
    OLED_SCL = 1;
    OLED_I2C_DELAY();
    OLED_SDA = 1;
    OLED_I2C_DELAY();
}

static void OLED_I2C_SendByte(unsigned char Byte)
{
    unsigned char i;

    for (i = 0; i < 8; i++)
    {
        if (Byte & 0x80)
        {
            OLED_SDA = 1;
        }
        else
        {
            OLED_SDA = 0;
        }
        Byte <<= 1;
        OLED_SCL = 1;
        OLED_I2C_DELAY();
        OLED_SCL = 0;
    }

    /* 第 9 拍：释放 SDA，给一个时钟，不检查应答 */
    OLED_SDA = 1;
    OLED_SCL = 1;
    OLED_I2C_DELAY();
    OLED_SCL = 0;
}

/* 总线恢复：MCU 在传输中途复位时，从机可能把 SDA 拉死，
 * 这时打 9 个时钟再发 Stop，把总线拉回空闲，否则屏永远不响应。*/
static void OLED_I2C_Recover(void)
{
    unsigned char i;

    OLED_SDA = 1;
    for (i = 0; i < 9; i++)
    {
        OLED_SCL = 1;
        OLED_I2C_DELAY();
        OLED_SCL = 0;
        OLED_I2C_DELAY();
    }
    OLED_I2C_Stop();
}

/* 一条事务里连续写 n 个命令（命令流） */
static void OLED_WriteCmdN(const unsigned char code *p, unsigned char n)
{
    unsigned char i;

    OLED_I2C_Start();
    OLED_I2C_SendByte(OLED_ADDR);
    OLED_I2C_SendByte(0x00);
    for (i = 0; i < n; i++)
    {
        OLED_I2C_SendByte(p[i]);
    }
    OLED_I2C_Stop();
}

/* 一条事务里连续写 n 个像素数据（字模在 code 区） */
static void OLED_WriteDataN(const unsigned char code *p, unsigned char n)
{
    unsigned char i;

    OLED_I2C_Start();
    OLED_I2C_SendByte(OLED_ADDR);
    OLED_I2C_SendByte(0x40);
    for (i = 0; i < n; i++)
    {
        OLED_I2C_SendByte(p[i]);
    }
    OLED_I2C_Stop();
}

/*==============================================================================
 * 对外接口
 *============================================================================*/

void OLED_WriteCommand(unsigned char Command)
{
    OLED_I2C_Start();
    OLED_I2C_SendByte(OLED_ADDR);
    OLED_I2C_SendByte(0x00);
    OLED_I2C_SendByte(Command);
    OLED_I2C_Stop();
}

void OLED_WriteData(unsigned char Data)
{
    OLED_I2C_Start();
    OLED_I2C_SendByte(OLED_ADDR);
    OLED_I2C_SendByte(0x40);
    OLED_I2C_SendByte(Data);
    OLED_I2C_Stop();
}

/* 设置光标：3 个命令合并成一次事务 */
void OLED_SetCursor(unsigned char Y, unsigned char X)
{
    if (Y > 7)
    {
        Y = 7;
    }
    if (X > 127)
    {
        X = 127;
    }

    OLED_I2C_Start();
    OLED_I2C_SendByte(OLED_ADDR);
    OLED_I2C_SendByte(0x00);
    OLED_I2C_SendByte(0xB0 | Y);
    OLED_I2C_SendByte(0x10 | (X >> 4));
    OLED_I2C_SendByte(0x00 | (X & 0x0F));
    OLED_I2C_Stop();
}

/* 清屏：整页 128 字节一次事务，8 页共 8 条事务（约 150ms@12MHz） */
void OLED_Clear(void)
{
    unsigned char page, i;

    for (page = 0; page < 8; page++)
    {
        OLED_SetCursor(page, 0);
        OLED_I2C_Start();
        OLED_I2C_SendByte(OLED_ADDR);
        OLED_I2C_SendByte(0x40);
        for (i = 0; i < 128; i++)
        {
            OLED_I2C_SendByte(0x00);
        }
        OLED_I2C_Stop();
    }
}

/* 显示一个字符：上下两页各 8 字节，各一条事务（约 4ms） */
void OLED_ShowChar(unsigned char Line, unsigned char Column, char Char)
{
    unsigned char Index;

    if (Line < 1 || Line > OLED_LINE_MAX || Column < 1 || Column > OLED_COLUMN_MAX)
    {
        return;
    }

    if ((unsigned char)Char < ' ' || (unsigned char)Char > '~')
    {
        Char = ' ';
    }
    Index = (unsigned char)Char - ' ';

    OLED_SetCursor((Line - 1) * 2, (Column - 1) * 8);
    OLED_WriteDataN(&OLED_F8x16[Index][0], 8);

    OLED_SetCursor((Line - 1) * 2 + 1, (Column - 1) * 8);
    OLED_WriteDataN(&OLED_F8x16[Index][8], 8);
}

void OLED_ShowString(unsigned char Line, unsigned char Column, char *String)
{
    unsigned char i;

    if (String == 0)
    {
        return;
    }
    if (Line < 1 || Line > OLED_LINE_MAX || Column < 1 || Column > OLED_COLUMN_MAX)
    {
        return;
    }

    for (i = 0; String[i] != '\0'; i++)
    {
        if ((Column + i) > OLED_COLUMN_MAX)
        {
            break;
        }
        OLED_ShowChar(Line, Column + i, String[i]);
    }
}
/**
  * @brief  OLED 次方函数
  * @retval 返回值等于 X 的 Y 次方
  */
static unsigned long OLED_Pow(unsigned char X, unsigned char Y)
{
    unsigned long Result = 1;

    while (Y > 0)
    {
        Result *= X;
        Y--;
    }

    return Result;
}

/**
  * @brief  OLED 显示无符号十进制数
  * @param  Line 起始行位置，范围：1~4
  * @param  Column 起始列位置，范围：1~16
  * @param  Number 要显示的数字，范围：0~4294967295
  * @param  Length 要显示数字的长度，范围：1~10
  */
void OLED_ShowNum(unsigned char Line, unsigned char Column,
                  unsigned long Number, unsigned char Length)
{
    unsigned char i;

    if (Length < 1 || Length > 10)
    {
        return;
    }

    for (i = 0; i < Length; i++)
    {
        OLED_ShowChar(Line, Column + i,
                      Number / OLED_Pow(10, Length - i - 1) % 10 + '0');
    }
}

/**
  * @brief  OLED 显示十六进制数
  * @param  Line 起始行位置，范围：1~4
  * @param  Column 起始列位置，范围：1~16
  * @param  Number 要显示的数字，范围：0~0xFFFFFFFF
  * @param  Length 要显示数字的长度，范围：1~8
  */
void OLED_ShowHexNum(unsigned char Line, unsigned char Column,
                     unsigned long Number, unsigned char Length)
{
    unsigned char i;
    unsigned char SingleNumber;

    if (Length < 1 || Length > 8)
    {
        return;
    }

    for (i = 0; i < Length; i++)
    {
        SingleNumber = (unsigned char)(Number / OLED_Pow(16, Length - i - 1) % 16);

        if (SingleNumber < 10)
        {
            OLED_ShowChar(Line, Column + i, SingleNumber + '0');
        }
        else
        {
            OLED_ShowChar(Line, Column + i, SingleNumber - 10 + 'A');
        }
    }
}


/* 上电画一次 "Region:"（第 1 行 1~7 列），之后数字只重画第 8 列 */
void OLED_RegionInit(void)
{
    OLED_ShowString(1, 1, "Region:");
}

void OLED_ShowRegion(unsigned char n)
{
    static unsigned char shown = 0xFF;

    if (n < 1 || n > 9 || n == shown)
    {
        return;
    }
    shown = n;

    OLED_ShowChar(1, 8, (char)('0' + n));   /* 只动 16 字节，约 4ms */
}

/*==============================================================================
 * 初始化
 *   所有初始化命令合并成一条事务，比逐条发快 3 倍。
 *============================================================================*/
static const unsigned char code OLED_InitSeq[] =
{
    0xAE,           /* 关显示 */
    0x20, 0x02,     /* 页寻址模式 */
    0xD5, 0x80,     /* 时钟分频 */
    0xA8, 0x3F,     /* 复用率 64 */
    0xD3, 0x00,     /* 显示偏移 0 */
    0x40,           /* 起始行 0 */
    0xA1,           /* 左右正常 */
    0xC8,           /* 上下正常 */
    0xDA, 0x12,     /* COM 硬件配置 */
    0x81, 0xCF,     /* 对比度 */
    0xD9, 0xF1,     /* 预充电 */
    0xDB, 0x30,     /* VCOMH */
    0xA4,           /* 按 RAM 内容显示 */
    0xA6,           /* 正常显示 */
    0x8D, 0x14,     /* 开电荷泵 */
    0xAF            /* 开显示 */
};

void OLED_Init(void)
{
    OLED_SCL = 1;
    OLED_SDA = 1;

    OLED_DelayMs(100);          /* 上电等待模块内部复位 */
    OLED_I2C_Recover();

    OLED_WriteCmdN(OLED_InitSeq, sizeof(OLED_InitSeq));
    OLED_Clear();
}