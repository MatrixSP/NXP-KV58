/*
 * @date   2019年05月09日最后修改
 * @name   Sora_lib
 * @group  Profiterole
 * @author Afisora
 * @brief  ENC Driver
 */


#include "sora_system.h"

 /**
  * @name			ENC_Init_Period
  * @brief			初始化ENC，配置PIT刷新缓冲
  * @clock			Fast Peripheral clock
  * @param pitn		PIT模块号(PWM0/PWM1)
  * @param time		PIT触发周期(ms)
  * @return			无
  * @example		ENC_Init_Period(PIT3, 200);
  */
void ENC_Init_Period(PITn pitn, uint32_t time)
{
	xbar_input_signal_t xpit;

	CLOCK_EnableClock(kCLOCK_Enc0);

	XBARA_Init(XBARA);
	//Phase A 
	PORT_Init(ENC_PhA_PTXn, ENC_PhA_ALT, pull_up);
	XBARA_SetSignalsConnection(XBARA, ENC_PhA_Xbar, kXBARA_OutputEncPhA);
	//Phase B 
	PORT_Init(ENC_PhB_PTXn, ENC_PhB_ALT, pull_up);
	XBARA_SetSignalsConnection(XBARA, ENC_PhB_Xbar, kXBARA_OutputEncPhB);

	//配置周期触发源
	switch (pitn)
	{
	case PIT0: xpit = kXBARA_InputPitTrigger0; break;
	case PIT1: xpit = kXBARA_InputPitTrigger1; break;
	case PIT2: xpit = kXBARA_InputPitTrigger2; break;
	case PIT3: xpit = kXBARA_InputPitTrigger3; break;
	default:
		return;
	}
	XBARA_SetSignalsConnection(XBARA, xpit, kXBARA_OutputEncCapTrigger);

	//配置PIT时钟
	PIT_IRQ_Init(pitn, time);

	//顺时针旋转计数增加，逆时针旋转计数减少
	ENC->CTRL = (0
		| ENC_CTRL_REV(0)	//是否对计数反向
		| ENC_CTRL_PH1(1)	//Phase B 电平表示正反转
		);
	ENC->CTRL2 = (0
		| ENC_CTRL2_UPDPOS(1)	//通过触发信号清除POSD、REV、UPOS、LPOS
		| ENC_CTRL2_UPDHLD(1)	//通过触发信号更新POSDH。REVH、UPOSH、LPOSH
		);
}