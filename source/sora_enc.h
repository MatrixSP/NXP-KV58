/*
 * @name   Sora_lib
 * @group  Profiterole
 * @author Afisora
 * @brief  ENC Driver
 */


#ifndef __SORA_ENC_H__
#define __SORA_ENC_H__

#define ENC_PhA_PTXn PTA25
#define ENC_PhA_ALT  2
#define ENC_PhA_Xbar kXBARA_InputXbarIn5

#define ENC_PhB_PTXn PTA24
#define ENC_PhB_ALT  2
#define ENC_PhB_Xbar kXBARA_InputXbarIn4

 /**
  * @name			ENC_Get_Speed
  * @brief			获取ENC测速
  * @clock			Fast Peripheral clock
  * @return			POSDH寄存器值
  * @example		ENC_Speed = ENC_Get_Speed();
  */
static inline int16_t ENC_Get_Speed()
{
	return (int16_t)ENC->POSDH;
}

void ENC_Init_Period(PITn pitn, uint32_t time);


#endif