/* 
 * @date   2019年02月25日最后修改
 * @name   Sora_lib
 * @group  Profiterole
 * @author Afisora
 * @brief  LCD Driver
 */

#ifndef __SORA_LCD_H__
#define __SORA_LCD_H__

extern void LCD_WrDat(unsigned char data);
extern void LCD_WrCmd(unsigned char cmd);
extern void LCD_Set_Pos(unsigned char x, unsigned char y);
extern void LCD_Fill(unsigned char bmp_data);
extern void LCD_CLS(void);
extern void LCD_Init(void);

extern void LCD_PutPixel(unsigned char x,unsigned char y);
extern void LCD_Rectangle(unsigned char x1,unsigned char y1,unsigned char x2,unsigned char y2,unsigned char gif);
extern void LCD_P6x8Str(unsigned char x,unsigned char y,unsigned char ch[]);
extern void LCD_P8x16Str(unsigned char x,unsigned char y,unsigned char ch[]);
extern void LCD_Show_BMP(unsigned char x0,unsigned char y0,unsigned char x1,unsigned char y1,unsigned char * bmp);
extern void LCD_PrintU16(unsigned char x,unsigned char y,unsigned int num);
extern void LCD_PrintFloat(unsigned char x,unsigned char y,float num);
extern void LCD_ClrDot(unsigned char x);
extern void LCD_write_char(uint8 X, uint8 Y,uint16 c);
extern void LCD_Show_Number(uint8 X,uint8 Y,int16 number);

extern void LCD_Refresh_Gram(void);
extern void LCD_DrawPoint(uint8 x,uint8 y,uint8 t);
extern void LCD_Clear(void);


#endif