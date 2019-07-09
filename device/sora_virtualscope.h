/*
 * @name   Sora_lib
 * @group  Profiterole
 * @author Afisora
 * @brief  UART Virtual Scope
 */

#ifndef __SORA_VIRTUALSCOPE_H__
#define __SORA_VIRTUALSCOPE_H__

void Scope_Init(UARTn_e uartn, uint32 baud);
extern unsigned short Scope_CRC_Check(unsigned char* Buf, unsigned char CRC_CNT);
extern void Scope_Output_Data(void);
extern void Scope_Send(float a, float b, float c, float d);

#endif