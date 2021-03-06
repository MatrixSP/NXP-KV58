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

/* DEBUG */
float Debug[4] = 0;
/* 
 * Debug 0 ESC PWM Width 
 * Debug 1 Speed Car  //Error
 * Debug 2 Speed CMD  //Controller P
 * Debug 3 Controller U  //Controller I
 */

/* 速度控制 */
#ifndef Controller_Buffer
#define CTRL_Timer				20
#else
#define CTRL_Timer				1
#endif
#define BASE_POINT				1500
#define Kp						30.0
#define Ki						2.5
#define SPEED_CONTROL_COUNT		2
#define DIRECTION_CONTROL_COUNT 2
float u = 0;  // 便于显示屏显示
float u_p = 0;
float u_i = 0;

/* 执行机构 */
uint16_t steer_ppm;
uint16_t steer_ppm_old;
uint16_t esc_ppm;
uint16_t esc_ppm_old;

/* 编码器比例 */
#define ENCODER_SCALE	14089.8

/* ENC配置 */
#define ENC_PIT			PIT3
#define ENC_Cycle		50

/* FlexPWM配置 */
#define Movement_SM		PWM_SM2
#define Movement_Freq	200
#define ESC_CH			PWM0_SM2_CHA
#define ESC_PPM_MIN		1400  //1420
#define ESC_PPM_MAX		1680  //1680
#define STEER_CH		PWM0_SM2_CHB
#define STEER_PPM_MIN	500
#define STEER_PPM_MAX	2500

/* UART配置 */
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

/* 通信配置 */
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

/* 函数声明 */
uint8_t UART_Create_MSG(uint8_t buff[], MSG_TAG tag, void* value, uint8_t size);
void UART_Deal_MSG(uint8_t buff[]);
float HAL_Get_Speed();
float HAL_Get_Meter();
float HAL_Get_Time();
void HAL_Start_Control();
void HAL_Action(uint16_t steer, uint16_t esc);

int main(void)
{
	char text[10];	//LCD字符串缓存
	uint32_t i = 0;

	/* LCD 初始化 */
	LCD_Init();
	/* LCD 初始化完成 */

	/* PWM初始化 */
	FlexPWM_Independent_Submodule_Init(PWM0, Movement_SM, PWM_Signed_EdgeAligned, Movement_Freq);	//舵机和电调均采用200Hz信号
	FlexPWM_Independent_Channel_Init(ESC_CH);		//电调初始化
	FlexPWM_Independent_Channel_Duty(ESC_CH, 0);	//电调初始占空比
	FlexPWM_Independent_Channel_Init(STEER_CH);		//舵机初始化
	FlexPWM_Independent_Channel_Duty(STEER_CH, 0);	//舵机初始占空比
	FlexPWM_Independent_Channel_LDOK(PWM0, PWM_SM2);
	/*
		此处应为遥控器输入信号检测初始化
	*/
	//FlexPWM_Independent_Submodule_Init(PWM1, PWM_SM0, PWM_Unsigned_EdgeAligned, 700000);	 //WS2812所需800kHz通讯信号，降频以保证通讯稳定
	//FlexPWM_Independent_Channel_Init(PWM1_SM0_CHA);		//WS2812初始化
	//FlexPWM_Independent_Channel_Duty(PWM1_SM0_CHA, 0);	//WS2812初始占空比
	/* PWM初始化完成 */

	/* ENC采集初始化 */
	ENC_Init_Period(ENC_PIT, ENC_Cycle);
	/* ENC采集初始化完成 */

	/* UART初始化 */
	UART_Com_Init(PC_UART, PC_Baud);	//连接车载PC
	UART_IRQ_EN(PC_UART, UART_RX_DMA);	
	UART_IRQ_EN(PC_UART, UART_IDLE);
	UART_IRQ_EN(PC_UART, UART_OVERRUN_ERROR);
	EDMA_UART_RX_Init(PC_RX_DMA, PC_UART);
	EDMA_UART_TX_Init(PC_TX_DMA, PC_UART);
	EDMA_UART_RX_Start(PC_RX_DMA, (uint32)PC_RX, PC_RX_Size);
	//UART_Com_Init(UART_1, 115200);	//板载调试口
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

	/* PIT初始化 */
	PIT_IRQ_Init(PIT0, MSG_Cycle);  // 中断
	PIT_Timer_Init(PIT1);           // 用作秒表
	PIT_Timer_Restart(PIT1);
	DisableInterrupts;
	PIT_IRQ_Init(PIT2, CTRL_Timer);  // 启动速度和方向控制中断
	PIT_IQR_Disable(PIT2);
	EnableInterrupts;
	/* PIT初始化完成 */

	/* DMA初始化 */
	/*
		此处应为DMA初始化
	*/
	/* DMA初始化完成 */

	/* 外设初始化 */
	Scope_Init(UART_1, 115200);
	/* 外设初始化完成 */

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

		/* MSG 处理 */
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

/* 速度和方向控制中断 */
#ifndef Controller_Buffer
void PIT2_IRQHandler()
{
#define BUFFER_LENGTH 64
	PIT_Flag_Clear(PIT2);
	static float e_prev = 0;
	static float u_buff[BUFFER_LENGTH] = 0;
	static int u_buffer_cursor = 0;
	float pi_buff = 0;
	float e = MSG.TWIST.SPEED - HAL_Get_Speed();

	//if (fabs(e) <= 0.1)e = 0;

	u_p = Kp * (e - e_prev);
	u_i = Ki * e;
	u += u_p + u_i;

	//pi_buff = u_p + u_i;

	//u -= u_buff[u_buffer_cursor];
	//u_buff[u_buffer_cursor] = pi_buff;
	//u += u_buff[u_buffer_cursor];

	//u_buffer_cursor++;
	//u_buffer_cursor %= BUFFER_LENGTH;

	Debug[1] = HAL_Get_Speed();
	Debug[2] = MSG.TWIST.SPEED;
	Debug[3] = u;
	//Debug[1] = e;
	//Debug[2] = u_p;
	//Debug[3] = u_i;

	//if (u > ESC_PPM_MAX - BASE_POINT) u = ESC_PPM_MAX - BASE_POINT;
	//if (u < ESC_PPM_MIN - BASE_POINT) u = ESC_PPM_MIN - BASE_POINT;

	e_prev = e;

	if (u > 0.5)
	{
		esc_ppm = BASE_POINT + 33 + (int16_t)u;
	}
	else if (u < -0.5)
	{
		esc_ppm = BASE_POINT - 25 + (int16_t)u;
	}
	else
	{
		esc_ppm = BASE_POINT;
	}
	
	steer_ppm = MSG.TWIST.STEER_PPM;

	HAL_Action(steer_ppm, esc_ppm);

	//Scope_Send((Debug[0] - 1500) * 100, 10000 * Debug[1], 1000 * Debug[2], 1000 * Debug[3]);
	Scope_Send((Debug[0] - 1500) * 100, 10000 * Debug[1], 10000 * Debug[2], 1000 * Debug[3]);
}
#else
void PIT2_IRQHandler()
{
	PIT_Flag_Clear(PIT2);
	static uint8_t count_1ms = 0;
	static uint8_t nSpeedControlCount = 0;
	static uint8_t nSpeedControlPeriod = 0;
	static uint8_t nDirectionControlCount = 0;
	static uint8_t nDirectionControlPeriod = 0;
	static float e_prev = 0;

	count_1ms++;
	if (count_1ms >= 5)
	{
		count_1ms = 0;
	}
	else if (count_1ms == 2)
	{
		uint16_t steerx = 0;
		uint16_t escx = 0;

		nSpeedControlPeriod++;
		nDirectionControlPeriod++;

		escx = (esc_ppm - esc_ppm_old) * (nSpeedControlPeriod + 1) / SPEED_CONTROL_COUNT + esc_ppm_old;
		steerx = (steer_ppm - steer_ppm_old) * (nDirectionControlPeriod + 1) / DIRECTION_CONTROL_COUNT + steer_ppm_old;

		HAL_Action(steerx, escx);
	}
	else if (count_1ms == 3)
	{
		nSpeedControlCount++;
		if (nSpeedControlCount >= SPEED_CONTROL_COUNT) 
		{
			esc_ppm_old = esc_ppm;

			float e = MSG.TWIST.SPEED - HAL_Get_Speed();

			if (fabs(e) <= 0.1)e = 0;

			u += Kp * (e - e_prev) + Ki * e;

			if (u > ESC_PPM_MAX - BASE_POINT) u = ESC_PPM_MAX - BASE_POINT;
			if (u < ESC_PPM_MIN - BASE_POINT) u = ESC_PPM_MIN - BASE_POINT;

			e_prev = e;
			esc_ppm = BASE_POINT + (uint16_t)u;

			nSpeedControlCount = 0;
			nSpeedControlPeriod = 0;
		}
	}
	else if (count_1ms == 4)
	{
		nDirectionControlCount++;
		if (nDirectionControlCount >= DIRECTION_CONTROL_COUNT)
		{
			steer_ppm_old = steer_ppm;

			steer_ppm = MSG.TWIST.STEER_PPM;

			nDirectionControlCount = 0;
			nDirectionControlPeriod = 0;
		}
	}
}
#endif

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
	static float Speed = 0;
	Speed = -ENC_Get_Speed() / ENCODER_SCALE / (ENC_Cycle / 1000.0);
	return Speed;
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
		PIT_IQR_Enable(PIT2);
		init = true;
	}
}

void HAL_Action(uint16_t steer, uint16_t esc)
{
	if (steer > STEER_PPM_MAX) steer = STEER_PPM_MAX;
	if (steer < STEER_PPM_MIN) steer = STEER_PPM_MIN;
	if (esc > ESC_PPM_MAX) esc = ESC_PPM_MAX;
	if (esc < ESC_PPM_MIN) esc = ESC_PPM_MIN;
	//if (esc < BASE_POINT) esc -= 25;
	Debug[0] = esc;
	FlexPWM_Independent_Channel_Duty(STEER_CH, FlexPWM_PPMCal_us(Movement_Freq, steer));
	FlexPWM_Independent_Channel_Duty(ESC_CH, FlexPWM_PPMCal_us(Movement_Freq, esc));
	FlexPWM_Independent_Channel_LDOK(PWM0, PWM_SM2);
}
