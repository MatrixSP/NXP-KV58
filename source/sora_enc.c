/*
 * @date   2019��05��09������޸�
 * @name   Sora_lib
 * @group  Profiterole
 * @author Afisora
 * @brief  ENC Driver
 */


#include "sora_system.h"

 /**
  * @name			ENC_Init_Period
  * @brief			��ʼ��ENC������PITˢ�»���
  * @clock			Fast Peripheral clock
  * @param pitn		PITģ���(PWM0/PWM1)
  * @param time		PIT��������(ms)
  * @return			��
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

	//�������ڴ���Դ
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

	//����PITʱ��
	PIT_IRQ_Init(pitn, time);

	//˳ʱ����ת�������ӣ���ʱ����ת��������
	ENC->CTRL = (0
		| ENC_CTRL_REV(0)	//�Ƿ�Լ�������
		| ENC_CTRL_PH1(1)	//Phase B ��ƽ��ʾ����ת
		);
	ENC->CTRL2 = (0
		| ENC_CTRL2_UPDPOS(1)	//ͨ�������ź����POSD��REV��UPOS��LPOS
		| ENC_CTRL2_UPDHLD(1)	//ͨ�������źŸ���POSDH��REVH��UPOSH��LPOSH
		);
}