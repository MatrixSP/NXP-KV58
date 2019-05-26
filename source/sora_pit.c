/* 
 * @name   Sora_lib
 * @group  Profiterole
 * @author Afisora
 * @brief  PIT Driver
 */

#include "sora_system.h"

/**
 * @name        PIT_IRQ_Init
 * @brief       PIT中断初始化
 * @clock       Bus/Flash clock
 * @param pitn  PIT模块号
 * @param cnt   触发周期（毫秒）
 * @return      无     
 * @example     PIT_IRQ_Init(PIT0, 500);
 */
void PIT_IRQ_Init(PITn pitn, uint32 cnt)
{
  /* 开启时钟*/
  CLOCK_EnableClock(kCLOCK_Pit0);
  
  /* PIT模块控制 PIT Module Control Register (PIT_MCR) */
  //使能PIT定时器时钟 ，调试模式下继续运行
  PIT->MCR &= ~(PIT_MCR_MDIS_MASK | PIT_MCR_FRZ_MASK );      
  
  /* 定时器加载值设置 Timer Load Value Register (PIT_LDVALn) */
  //设置溢出中断时间
  //LDVAL trigger = (period / clock period) - 1
  PIT->CHANNEL[pitn].LDVAL = (uint32_t)(cnt * (Bus_Clock / 1000)) - 1;  //ms * kHz - 1
  
  //定时时间到了后，TIF置1。写1的时候就会清0
  //清中断标志位
  PIT_Flag_Clear(pitn);                                             
  
  /* 定时器控制寄存器 Timer Control Register (PIT_TCTRL0) */
  //使能 PITn定时器,并开PITn中断
  PIT->CHANNEL[pitn].TCTRL |= ( PIT_TCTRL_TEN_MASK | PIT_TCTRL_TIE_MASK );
  //开接收引脚的IRQ中断
  NVIC_EnableIRQ((IRQn_Type)(pitn + 48));		   
}

/**
 * @name        PIT_Trigger_Init
 * @brief       PIT触发初始化
 * @clock       Bus/Flash clock
 * @param pitn  PIT模块号
 * @param cnt   触发周期（毫秒）
 * @return      无
 * @example     PIT_Trigger_Init(PIT0, 500);
 */
void PIT_Trigger_Init(PITn pitn, uint32 cnt)
{
	/* 开启时钟*/
	CLOCK_EnableClock(kCLOCK_Pit0);

	/* PIT模块控制 PIT Module Control Register (PIT_MCR) */
	//使能PIT定时器时钟 ，调试模式下继续运行
	PIT->MCR &= ~(PIT_MCR_MDIS_MASK | PIT_MCR_FRZ_MASK);

	/* 定时器加载值设置 Timer Load Value Register (PIT_LDVALn) */
	//设置溢出中断时间
	//LDVAL trigger = (period / clock period) - 1
	PIT->CHANNEL[pitn].LDVAL = (uint32_t)(cnt * (Bus_Clock / 1000)) - 1;  //ms * kHz - 1

	//定时时间到了后，TIF置1。写1的时候就会清0
	//清中断标志位
	PIT_Flag_Clear(pitn);

	/* 定时器控制寄存器 Timer Control Register (PIT_TCTRL0) */
	//使能 PITn定时器
	PIT->CHANNEL[pitn].TCTRL |= PIT_TCTRL_TEN_MASK;
}

/**
 * @name        PIT_Timer_Init
 * @brief       PIT计时初始化
 * @clock       Bus/Flash clock
 * @param pitn  PIT模块号
 * @return      无     
 * @example     PIT_Timer_Init(PIT3);
 */
void PIT_Timer_Init(PITn pitn)
{
  /* 开启时钟*/
  CLOCK_EnableClock(kCLOCK_Pit0);
  
  /* PIT模块控制 PIT Module Control Register (PIT_MCR) */
  //使能PIT定时器时钟 ，调试模式下继续运行
  PIT->MCR &= ~(PIT_MCR_MDIS_MASK | PIT_MCR_FRZ_MASK );      
  
  /* 定时器加载值设置 Timer Load Value Register (PIT_LDVALn) */
  //设置溢出中断时间
  PIT->CHANNEL[pitn].LDVAL = 0xFFFFFFFF;
}

/**
 * @name        PIT_Timer_Get
 * @brief       获取PIT计时
 * @clock       Bus/Flash clock
 * @param pitn  PIT模块号
 * @return      时间（ms）
 * @example     PIT_Timer_Get(PIT3);
 */
inline float PIT_Timer_Get(PITn pitn)
{
  uint32 cnt = 0xFFFFFFFF - PIT->CHANNEL[pitn].CVAL;
  return (float)cnt / (Bus_Clock / 1000);
}

/**
 * @name        PIT_Timer_Restart
 * @brief       重启PIT计时
 * @clock       Bus/Flash clock
 * @param pitn  PIT模块号
 * @return      无     
 * @example     PIT_Timer_Restart(PIT3);
 */
inline void PIT_Timer_Restart(PITn pitn)
{
  PIT_Timer_Stop(pitn);
  PIT_Timer_Start(pitn); 
}
