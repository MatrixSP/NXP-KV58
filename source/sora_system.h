/* 
 * @date   2019年02月24日最后修改
 * @name   Sora_lib
 * @group  Profiterole
 * @author Afisora
 * @brief  System
 */

#ifndef __SORA_SYSTEM_H__
#define __SORA_SYSTEM_H__
   
#include "include.h"

/*
 * 时钟频率（hz）
 */
#define Core_Clock              CLOCK_GetFreq(kCLOCK_CoreSysClk)
#define Fast_Peripheral_Clock   CLOCK_GetFreq(kCLOCK_FastPeriphClk)
#define FlexBus_Clock           CLOCK_GetFreq(kCLOCK_FlexBusClk)
#define Bus_Clock               CLOCK_GetFreq(kCLOCK_BusClk)
#define System_clock            Core_Clock
#define Flash_Clock             Bus_Clock
#define MCGIRCLK                CLOCK_GetFreq(kCLOCK_McgInternalRefClk)
#define MCGFFCLK                CLOCK_GetFreq(kCLOCK_McgFixedFreqClk)
#define OSCERCLK                CLOCK_GetFreq(kCLOCK_Osc0ErClk)
#define OSCERCLK_Undiv          CLOCK_GetFreq(kCLOCK_Osc0ErClkUndiv)
#define ERCLK32K                CLOCK_GetFreq(kCLOCK_Er32kClk)
#define LPO_Clock               CLOCK_GetFreq(kCLOCK_LpoClk)

/*
 * 时钟频率（khz）
 */
#if 0   
#define Core_Clock              237500
#define Fast_Peripheral_Clock   118750
#define FlexBus_Clock           59375
#define Bus_Clock               23750  
#elif   0
#define Core_Clock              250000
#define Fast_Peripheral_Clock   125000
#define FlexBus_Clock           62500
#define Bus_Clock               31250
#elif   0
#define Core_Clock              256250
#define Fast_Peripheral_Clock   128125
#define FlexBus_Clock           64062.5
#define Bus_Clock               32031.25
#elif   0   
#define Core_Clock              262500
#define Fast_Peripheral_Clock   131250
#define FlexBus_Clock           65625
#define Bus_Clock               32812.5
#elif   0   
#define Core_Clock              268750
#define Fast_Peripheral_Clock   134375
#define FlexBus_Clock           67187.5
#define Bus_Clock               26875
#endif

//#define System_clock            Core_Clock
//#define Flash_Clock             Bus_Clock
//#define MCGIRCLK                32768
//#define MCGFFCLK                1562.5
//#define OSCERCLK                50000
//#define OSCERCLK_Undiv          50000
//#define ERCLK32K                1
//#define MCK_Clock               237500
//#define LPO_Clock               1

/*
 * 延时
 */
#if 1
#include "sora_dwt.h"                 
#define DELAY()         dwt_delay_ms(500)
#define DELAY_MS(ms)    dwt_delay_ms(ms)
#define DELAY_US(us)    dwt_delay_us(us)
#elif   0
#include "sora_lptmr.h"
#define DELAY()         lptmr_delay_ms(500)
#define DELAY_MS(ms)    lptmr_delay_ms(ms)
#define DELAY_US(us)    lptmr_delay_us(us)
#elif   0
#include "sora_pit.h"
#define DELAY()         pit_delay_ms(PIT3,500)
#define DELAY_MS(ms)    pit_delay_ms(PIT3,ms)
#define DELAY_US(us)    pit_delay_us(PIT3,us)
#else
#include "sora_SysTick.h"
#define DELAY()         systick_delay_ms(500)
#define DELAY_MS(ms)    systick_delay_ms(ms)
#define DELAY_US(us)    systick_delay_us(us)
#endif

#endif
