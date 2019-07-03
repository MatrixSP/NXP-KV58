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


/* �ٶȿ��� */
#define CTRL_Cycle		20
#define BASE_POINT		1530
#define Kp				30.0
#define Ki				5.0
float u = 0;  // ������ʾ����ʾ

/* ִ�л��� */
uint16_t steer_ppm;
uint16_t esc_ppm;

/* ���������� */
#define ENCODER_SCALE	14089.8

/* ENC���� */
#define ENC_PIT			PIT3
#define ENC_Cycle		50

/* FlexPWM���� */
#define Movement_SM		PWM_SM2
#define Movement_Freq	200
#define ESC_CH			PWM0_SM2_CHA
#define ESC_PPM_MIN		1420
#define ESC_PPM_MAX		1680
#define STEER_CH		PWM0_SM2_CHB
#define STEER_PPM_MIN	1000
#define STEER_PPM_MAX	2000

/* UART���� */
#define PC_UART			UART_0
#define PC_Baud			115200//921600
#define PC_RX_Size		64
uint8_t PC_RX[PC_RX_Size] = { 0 };
bool	PC_RX_Flag = false;
uint8_t PC_RX_Counter = 0;
#define PC_RX_DMA		DMA_CH5
#define PC_TX_Size		64
uint8_t PC_TX[PC_TX_Size] = { 0 };
bool	PC_TX_Flag = false;
#define PC_TX_DMA		DMA_CH6

/* ͨ������ */
typedef enum
{
	MSG_HEARTBEAT,
	MSG_WHEEL_ENCODER,
	MSG_PANIC,
	MSG_TWIST,
	MSG_STOP_IMMEDIATELY
} MSG_TAG;

#define MSG_Cycle		50
typedef struct
{
	struct
	{
		float SPEED;
		uint16_t STEER_PPM;
	} TWIST;
} MSG_Type;

MSG_Type MSG;

/* �������� */
uint8_t UART_Create_MSG(uint8_t buff[], MSG_TAG tag, void* value, uint8_t size);
void UART_Deal_MSG(uint8_t buff[]);
void Movement_Action();


float HAL_Get_Speed();
float HAL_Get_Meter();
float HAL_Get_Time();
void HAL_Start_Control();
void HAL_Action();

int main(void)
{
	char text[10];	//LCD�ַ�������
	uint32_t i = 0;

	/* LCD ��ʼ�� */
	LCD_Init();
	/* LCD ��ʼ����� */

	/* PWM��ʼ�� */
	FlexPWM_Independent_Submodule_Init(PWM0, Movement_SM, PWM_Signed_EdgeAligned, Movement_Freq);	//����͵��������200Hz�ź�
	FlexPWM_Independent_Channel_Init(ESC_CH);		//�����ʼ��
	FlexPWM_Independent_Channel_Duty(ESC_CH, 0);	//�����ʼռ�ձ�
	FlexPWM_Independent_Channel_Init(STEER_CH);		//�����ʼ��
	FlexPWM_Independent_Channel_Duty(STEER_CH, 0);	//�����ʼռ�ձ�
	FlexPWM_Independent_Channel_LDOK(PWM0, PWM_SM2);
	/*
		�˴�ӦΪң���������źż���ʼ��
	*/
	//FlexPWM_Independent_Submodule_Init(PWM1, PWM_SM0, PWM_Unsigned_EdgeAligned, 700000);	 //WS2812����800kHzͨѶ�źţ���Ƶ�Ա�֤ͨѶ�ȶ�
	//FlexPWM_Independent_Channel_Init(PWM1_SM0_CHA);		//WS2812��ʼ��
	//FlexPWM_Independent_Channel_Duty(PWM1_SM0_CHA, 0);	//WS2812��ʼռ�ձ�
	/* PWM��ʼ����� */

	/* ENC�ɼ���ʼ�� */
	ENC_Init_Period(ENC_PIT, ENC_Cycle);
	/* ENC�ɼ���ʼ����� */

	/* UART��ʼ�� */
	UART_Com_Init(PC_UART, PC_Baud);	//���ӳ���PC
	UART_IRQ_EN(PC_UART, UART_RX_DMA);	
	UART_IRQ_EN(PC_UART, UART_IDLE);
	UART_IRQ_EN(PC_UART, UART_OVERRUN_ERROR);
	EDMA_UART_RX_Init(PC_RX_DMA, PC_UART);
	EDMA_UART_TX_Init(PC_TX_DMA, PC_UART);
	EDMA_UART_RX_Start(PC_RX_DMA, (uint32)PC_RX, PC_RX_Size);
	//UART_Com_Init(UART_1, 115200);	//���ص��Կ�
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

	/* PIT��ʼ�� */
	PIT_IRQ_Init(PIT0, MSG_Cycle);  // �ж�
	PIT_Timer_Init(PIT1);           // �������
	PIT_Timer_Restart(PIT1);
	/* PIT��ʼ����� */

	/* DMA��ʼ�� */
	/*
		�˴�ӦΪDMA��ʼ��
	*/
	/* DMA��ʼ����� */
	while (1U)
	{
		/* LCD Display*/
		LCD_P6x8Str(0, 1, "Main");
		sprintf(text, "  TIME  %.2f s", HAL_Get_Time());
		LCD_P6x8Str(0, 2, text);
		sprintf(text, "   ENC  %.2f m", HAL_Get_Meter());
		LCD_P6x8Str(0, 3, text);
		sprintf(text, "  SPED  %.1f", HAL_Get_Speed());
		LCD_P6x8Str(0, 4, text);
		sprintf(text, "  CTRL  %.1f", u);
		LCD_P6x8Str(0, 6, text);

		/* MSG ���� */
		if (PC_RX_Flag)
		{
			UART_Deal_MSG(PC_RX);
			PC_RX_Flag = false;
			EDMA_UART_RX_Start(PC_RX_DMA, (uint32)PC_RX, PC_RX_Size);
		}
	}
}

void PIT0_IRQHandler()
{
	PIT_Flag_Clear(PIT0);
	float encoder_meter = HAL_Get_Meter();
	uint8_t length = 0;
	length = UART_Create_MSG(PC_TX, MSG_WHEEL_ENCODER, &encoder_meter, sizeof(encoder_meter));
	UART_IRQ_EN(PC_UART, UART_TX_DMA);
	EDMA_UART_TX_Start(PC_TX_DMA, (uint32)PC_TX, length);
}


/* �ٶȺͷ�������ж� */
void PIT2_IRQHandler()
{
	PIT_Flag_Clear(PIT2);
	static float e_prev = 0;
	float e = MSG.TWIST.SPEED - HAL_Get_Speed();

	if (fabs(e) <= 0.1)e = 0;

	u += Kp * (e - e_prev) + Ki * e;

	if (u > ESC_PPM_MAX - BASE_POINT) u = ESC_PPM_MAX - BASE_POINT;
	if (u < ESC_PPM_MIN - BASE_POINT) u = ESC_PPM_MIN - BASE_POINT;

	e_prev = e;

	esc_ppm = BASE_POINT + (int16_t)u;
	steer_ppm = MSG.TWIST.STEER_PPM;

	HAL_Action();
}

void UART0_RX_TX_IRQHandler(void)
{
	uint8 buff;
	if (UART0->S1 & UART_S1_IDLE_MASK)
	{
		buff = UART0->D;
		PC_RX_Counter = EDMA_UART_RX_Stop(PC_RX_DMA, (uint32)PC_RX);
		UART0->CFIFO |= UART_CFIFO_RXFLUSH(1);
		PC_RX_Flag = true;
	}

	if (UART0->S1 & UART_S1_OR_MASK)
	{
		buff = UART0->S1;
		buff = UART0->D;
		UART0->CFIFO |= UART_CFIFO_RXFLUSH(1);
	}
}

void DMA6_DMA22_IRQHandler(void)
{
	if (DMA0->INT & (1 << 6))
	{
		DMA0->CINT |= DMA_CINT_CINT(6);
		UART_IRQ_DIS(PC_UART, UART_TX_DMA);
	}
}

uint8_t UART_Create_MSG(uint8_t buff[], MSG_TAG tag, void* value, uint8_t size)
{
	uint8_t i = 0;
	uint8_t check = 0;
	buff[0] = 0x55;
	buff[1] = 0xAA;
	buff[2] = tag;
	buff[3] = size;
	memcpy(buff + 4, value, size);
	for (i = 0; i < size; i++)
	{
		check += buff[4 + i];
	}
	buff[4 + size] = check;
	return 5 + size;
}

void UART_Deal_MSG(uint8_t buff[])
{
	uint8_t i = 0;
	uint8_t check = 0;
	if (buff[0] != 0x55)
		return;
	if (buff[1] != 0xAA)
		return;
	for (i = 0; i < buff[3]; i++)
	{
		check += buff[4 + i];
	}
	if (buff[4 + buff[3]] != check)
		return;
	switch (buff[2])
	{
	case MSG_TWIST:
		memcpy(&MSG.TWIST, buff + 4, buff[3]);
		HAL_Start_Control();
		break;
	default: break;
	}
}
float HAL_Get_Speed()
{
	return -ENC_Get_Speed() / ENCODER_SCALE / (ENC_Cycle / 1000.0);
}

float HAL_Get_Meter()
{
	return -ENC_Get_Position() / ENCODER_SCALE;
}

float HAL_Get_Time()
{
	static float PIT_Time = -1;
	PIT_Time = PIT_Timer_Continuous(PIT1, PIT_Time);
	return PIT_Time / 1000.0;
}

void HAL_Start_Control()
{
	static bool init = false;
	if (!init)
	{
		PIT_IRQ_Init(PIT2, CTRL_Cycle);  // �����ٶȺͷ�������ж�
		init = true;
	}
}

void HAL_Action()
{
	if (steer_ppm > STEER_PPM_MAX) steer_ppm = STEER_PPM_MAX;
	if (steer_ppm < STEER_PPM_MIN) steer_ppm = STEER_PPM_MIN;
	if (esc_ppm > ESC_PPM_MAX) esc_ppm = ESC_PPM_MAX;
	if (esc_ppm < ESC_PPM_MIN) esc_ppm = ESC_PPM_MIN;
	FlexPWM_Independent_Channel_Duty(STEER_CH, FlexPWM_PPMCal_us(Movement_Freq, steer_ppm));
	FlexPWM_Independent_Channel_Duty(ESC_CH, FlexPWM_PPMCal_us(Movement_Freq, esc_ppm));
	FlexPWM_Independent_Channel_LDOK(PWM0, PWM_SM2);
}
