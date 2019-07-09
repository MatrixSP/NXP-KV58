/*
 * @name   Sora_lib
 * @group  Profiterole
 * @author Afisora
 * @brief  UART Virtual Scope
 */

#include "sora_system.h"

float OutData[4] = { 0 };
float temp = 0.0;
UARTn_e SCOPE_COM = UART_NULL;

void Scope_Init(UARTn_e uartn, uint32 baud)
{
	SCOPE_COM = uartn;
	UART_Com_Init(uartn, baud);
}

unsigned short Scope_CRC_Check(unsigned char* Buf, unsigned char CRC_CNT)
{
	unsigned short CRC_Temp;
	unsigned char i, j;
	CRC_Temp = 0xffff;

	for (i = 0; i < CRC_CNT; i++) {
		CRC_Temp ^= Buf[i];
		for (j = 0; j < 8; j++) {
			if (CRC_Temp & 0x01)
				CRC_Temp = (CRC_Temp >> 1) ^ 0xa001;
			else
				CRC_Temp = CRC_Temp >> 1;
		}
	}
	return(CRC_Temp);
}

void Scope_Output_Data(void)
{
	int temp[4] = { 0 };
	unsigned int temp1[4] = { 0 };
	unsigned char databuf[10] = { 0 };
	unsigned char i;
	unsigned short CRC16 = 0;
	for (i = 0; i < 4; i++)
	{
		temp[i] = (int)OutData[i];
		temp1[i] = (unsigned int)temp[i];

	}

	for (i = 0; i < 4; i++)
	{
		databuf[i * 2] = (unsigned char)(temp1[i] % 256);
		databuf[i * 2 + 1] = (unsigned char)(temp1[i] / 256);
	}

	CRC16 = Scope_CRC_Check(databuf, 8);
	databuf[8] = CRC16 % 256;
	databuf[9] = CRC16 / 256;

	for (i = 0; i < 10; i++)
		UART_Put_Char(SCOPE_COM, databuf[i]);
}

void Scope_Send(float a, float b, float c, float d)
{
	if (SCOPE_COM == UART_NULL)
		return;
	OutData[0] = a;
	OutData[1] = b;
	OutData[2] = c;
	OutData[3] = d;
	Scope_Output_Data();
}