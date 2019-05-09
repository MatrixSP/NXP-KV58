/*
 * @date   2019年05月09日最后修改
 * @name   Sora_lib
 * @group  Profiterole
 * @author Afisora
 * @brief  ENC Driver
 */


#include "sora_system.h"

void ENC_Init_Test()
{
	CLOCK_EnableClock(kCLOCK_Enc0);

	XBARA_Init(XBARA);

	//Phase A PTA25
	PORT_Init(PTA25, 2, pull_up);
	XBARA_SetSignalsConnection(XBARA, kXBARA_InputXbarIn5, kXBARA_OutputEncPhA);

	//Phase B PTA24
	PORT_Init(PTA24, 2, pull_up);
	XBARA_SetSignalsConnection(XBARA, kXBARA_InputXbarIn4, kXBARA_OutputEncPhB);

	//Trigger
	XBARA_SetSignalsConnection(XBARA, kXBARA_InputPitTrigger0, kXBARA_OutputEncCapTrigger);

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