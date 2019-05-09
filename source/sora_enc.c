/*
 * @date   2019��05��09������޸�
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

	ENC->CTRL = (0
		| ENC_CTRL_REV(0)	//˳ʱ����ת�������ӣ���ʱ����ת��������
		| ENC_CTRL_PH1(1)	//Phase B ��ƽ��ʾ����ת
		);
	ENC->CTRL2 = (0
		| ENC_CTRL2_UPDPOS(1)	//�Ƿ�ͨ�������ź����POSD��REV��UPOS��LPOS
		| ENC_CTRL2_UPDHLD(1)	//�Ƿ�ͨ�������źŸ���POSDH��REVH��UPOSH��LPOSH
		);
}