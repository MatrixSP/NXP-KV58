/* 
 * @date   2019年02月25日最后修改
 * @name   Sora_lib
 * @group  Akko
 * @author Afisora
 * @brief  PIT Driver
 */

#ifndef __SORA_PIT_H__
#define __SORA_PIT_H__

typedef enum PITn
{
    PIT0,
    PIT1,
    PIT2,
    PIT3
} PITn;


#define PIT_Flag_Clear(pitn)   PIT->CHANNEL[pitn].TFLG |=PIT_TFLG_TIF_MASK     //清中断标志
#define PIT_Timer_Start(pitn)  PIT->CHANNEL[pitn].TCTRL |= PIT_TCTRL_TEN_MASK
#define PIT_Timer_Stop(pitn)   PIT->CHANNEL[pitn].TCTRL &= ~PIT_TCTRL_TEN_MASK

void PIT_IRQ_Init(PITn pitn, uint32 cnt);
void PIT_Timer_Init(PITn pitn);
float PIT_Timer_Get(PITn pitn);
void PIT_Timer_Restart(PITn pitn);

#endif