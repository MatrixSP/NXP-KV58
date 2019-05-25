/*
 * @name   Sora_lib
 * @group  Profiterole
 * @author Afisora
 * @brief  ��ʽ����
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
	unsigned char text[10];	//LCD�ַ�������
	uint32_t i = 0;

	/* LCD ��ʼ�� */
	LCD_Init();
	/* LCD ��ʼ����� */

	/* PWM��ʼ�� */
	FlexPWM_Independent_Submodule_Init(PWM0, PWM_SM2, PWM_Signed_EdgeAligned, 200);	//����͵��������200Hz�ź�
	FlexPWM_Independent_Channel_Init(PWM0_SM2_CHA);		//�����ʼ��
	FlexPWM_Independent_Channel_Duty(PWM0_SM2_CHA, 10);	//�����ʼռ�ձ�
	FlexPWM_Independent_Channel_Init(PWM0_SM2_CHB);		//�����ʼ��
	FlexPWM_Independent_Channel_Duty(PWM0_SM2_CHB, 90);	//�����ʼռ�ձ�
	/*
		�˴�ӦΪң���������źż���ʼ��
	*/
	FlexPWM_Independent_Submodule_Init(PWM1, PWM_SM0, PWM_Unsigned_EdgeAligned, 700000);	 //WS2812����800kHzͨѶ�źţ���Ƶ�Ա�֤ͨѶ�ȶ�
	FlexPWM_Independent_Channel_Init(PWM1_SM0_CHA);		//WS2812��ʼ��
	FlexPWM_Independent_Channel_Duty(PWM1_SM0_CHA, 0);	//WS2812��ʼռ�ձ�
	/* PWM��ʼ����� */

	/* ENC�ɼ���ʼ�� */
	ENC_Init_Period(PIT3, 50);
	/* ENC�ɼ���ʼ����� */

	/* UART��ʼ�� */
	UART_Com_Init(UART_0, 115200);	//���ӳ���PC
	UART_Com_Init(UART_1, 115200);	//���ص��Կ�
	/* UART��ʼ����� */

	/* IIC��ʼ�� */
	/*
		�˴�ӦΪIIC��ʼ��
	*/
	/* IIC��ʼ����� */

	/* SPI��ʼ�� */
	/*
		�˴�ӦΪSPI��ʼ��
	*/
	/* SPI��ʼ����� */

	/* PIT�жϳ�ʼ�� */
	PIT_IRQ_Init(PIT0, 1); //1msһ���ж�
	/* PIT�жϳ�ʼ����� */

	/* DMA��ʼ�� */
	/*
		�˴�ӦΪDMA��ʼ��
	*/
	/* DMA��ʼ����� */

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