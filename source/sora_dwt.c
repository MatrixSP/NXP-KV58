/* 
 * @date   2019年1月31日最后修改
 * @name   Sora_lib
 * @group  Profiterole
 * @author Afisora
 * @brief  DWT Driver
 */

#include "sora_system.h"

//0xE000EDFC DEMCR RW Debug Exception and Monitor Control Register.
#define DEMCR              ( *(unsigned int *)0xE000EDFC )
#define DHCSR_TRCENA       ( 0x01 << 24) //DEMCR的DWT使能位
//0xE0001000 DWT_CTRL RW The Debug Watchpoint and Trace (DWT) unit
#define DWT_CTRL           ( *(unsigned int *)0xE0001000 )
#define DWT_CTRL_CYCCNTENA ( 0x01 << 0 ) //DWT的SYCCNT使能位
//0xE0001004 DWT_CYCCNT RW Cycle Count register, 
#define DWT_CYCCNT         ( *(unsigned int *)0xE0001004 ) //显示或设置处理器的周期计数值

//DWT_LAR address is 0xe0010fb0 and the unlock value is 0xc5acce55
#define DWT_LAR            ( *(unsigned int *)0xE0001FB0 ) //Software Lock Access Register
#define DWT_LAR_UNLOCK     ( 0xC5ACCE55 )

void dwt_init()
{
  DEMCR |= DHCSR_TRCENA;          // enable trace for DWT features
  DWT_LAR = DWT_LAR_UNLOCK;       // unlock access to DWT registers
  DWT_CYCCNT = 0;                 // reset the cycle count value
  DWT_CTRL |= DWT_CTRL_CYCCNTENA; // enable the cycle counter
}

void dwt_delay_us(uint32 uSec)
{
  int ticks_start, ticks_end, ticks_delay;
  dwt_init();
  ticks_start = DWT_CYCCNT;
  ticks_delay = (uSec * (Core_Clock / 1000000)); //将微秒数换算成滴答数          
  ticks_end = ticks_start + ticks_delay;
  if(ticks_end < ticks_start)
  {
    //计数溢出，翻转
    while( DWT_CYCCNT > ticks_end ); //等待DWT_CTRL_CYCCNTENA溢出 返回0
  }
  while( DWT_CYCCNT < ticks_end );
}

void dwt_delay_ms(uint32 mSec)
{
    while(mSec--)
    {
        dwt_delay_us(1000);
    }
}