/*
 * @name   Sora_lib
 * @group  Profiterole
 * @author Afisora
 * @brief  正式程序
 * @todo
 * @{
 *
 * @}
 * @note
 * @{
 *
 * @}
 */

#include "include.h"

int16_t	ENC_Speed = 0;

int main(void)
{
	unsigned char text[10];	//LCD字符串缓存
	uint32_t i = 0;

	/* LCD 初始化 */
	LCD_Init();
	/* LCD 初始化完成 */

	/* PWM初始化 */
	FlexPWM_Independent_Submodule_Init(PWM0, PWM_SM2, PWM_Signed_EdgeAligned, 200);	//舵机和电调均采用200Hz信号
	FlexPWM_Independent_Channel_Init(PWM0_SM2_CHA);		//电调初始化
	FlexPWM_Independent_Channel_Duty(PWM0_SM2_CHA, 10);	//电调初始占空比
	FlexPWM_Independent_Channel_Init(PWM0_SM2_CHB);		//舵机初始化
	FlexPWM_Independent_Channel_Duty(PWM0_SM2_CHB, 90);	//舵机初始占空比
	/*
		此处应为遥控器输入信号检测初始化
	*/
	FlexPWM_Independent_Submodule_Init(PWM1, PWM_SM0, PWM_Unsigned_EdgeAligned, 700000);	 //WS2812所需800kHz通讯信号，降频以保证通讯稳定
	FlexPWM_Independent_Channel_Init(PWM1_SM0_CHA);		//WS2812初始化
	FlexPWM_Independent_Channel_Duty(PWM1_SM0_CHA, 0);	//WS2812初始占空比
	/* PWM初始化完成 */

	/* ENC采集初始化 */
	ENC_Init_Period(PIT3, 50);
	/* ENC采集初始化完成 */

	/* UART初始化 */
	UART_Com_Init(UART_0, 115200);	//连接车载PC
	UART_Com_Init(UART_1, 115200);	//板载调试口
	/* UART初始化完成 */

	/* IIC初始化 */
	/*
		此处应为IIC初始化
	*/
	/* IIC初始化完成 */

	/* SPI初始化 */
	/*
		此处应为SPI初始化
	*/
	/* SPI初始化完成 */

	/* PIT中断初始化 */
	PIT_IRQ_Init(PIT0, 1); //1ms一次中断
	/* PIT中断初始化完成 */

	/* DMA初始化 */
	/*
		此处应为DMA初始化
	*/
	/* DMA初始化完成 */

	while (1U)
	{
		LCD_P6x8Str(0, 1, "Main");
		sprintf(text, "ENC: %05d", ENC_Speed);
		LCD_P6x8Str(0, 2, text);
	}
}

void PIT0_IRQHandler()
{
	PIT_Flag_Clear(PIT0);
}

void PIT3_IRQHandler()
{
	PIT_Flag_Clear(PIT3);
	ENC_Speed = ENC_Get_Speed();
}