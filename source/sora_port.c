/* 
 * @name   Sora_lib
 * @group  Profiterole
 * @author Afisora
 * @brief  PORT Driver
 */

#include "sora_system.h"

PORT_MemMapPtr PORTX[5] = {PORTA, PORTB, PORTC, PORTD, PORTE};

/**
 * @name        PORT_Init
 * @brief       PORT管脚初始化
 * @param ptxn  管脚号
 * @param ALTn  复用设置
 * @param pcfg  拉设置
 * @return      无
 * @example     PORT_Init(PTA11, 0);
 */
void PORT_Init(PTXn_e ptxn, uint8 ALTn, pull_cfg pcfg)
{
  CLOCK_EnableClock((clock_ip_name_t)(kCLOCK_PortA + PTX(ptxn)));
  //设置管脚复用
  PORTX_BASE(ptxn)->PCR[PTn(ptxn)] = (PORTX_BASE(ptxn)->PCR[PTn(ptxn)] & ~PORT_PCR_MUX_MASK) | PORT_PCR_MUX(ALTn);
  //拉使能
  PORTX_BASE(ptxn)->PCR[PTn(ptxn)] = (PORTX_BASE(ptxn)->PCR[PTn(ptxn)] & ~PORT_PCR_PE_MASK)  | PORT_PCR_PE(pcfg/2); 
  //拉设置
  PORTX_BASE(ptxn)->PCR[PTn(ptxn)] = (PORTX_BASE(ptxn)->PCR[PTn(ptxn)] & ~PORT_PCR_PS_MASK)  | PORT_PCR_PS(pcfg%2);
}


/**
 * @name        EXTI_Init
 * @brief       外部中断初始化
 * @param ptxn  管脚号
 * @param ecfg  触发设置
 * @param pcfg  拉设置
 * @return      无
 * @example     EXTI_Init(PTA11, rising, pull_up);
 */
void EXTI_Init(PTXn_e ptxn, exti_cfg ecfg, pull_cfg pcfg)
{
  //开启时钟
  CLOCK_EnableClock((clock_ip_name_t)(kCLOCK_PortA + PTX(ptxn)));
  if(ecfg > 0x03)
  {
    PORT_IRQ_EN((PORT_IRQ)PTX(ptxn));
  }
  //设置管脚复用GPIO
  PORTX_BASE(ptxn)->PCR[PTn(ptxn)] = (PORTX_BASE(ptxn)->PCR[PTn(ptxn)] & ~PORT_PCR_MUX_MASK) | PORT_PCR_MUX(1);
  //中断设置
  PORTX_BASE(ptxn)->PCR[PTn(ptxn)] = (PORTX_BASE(ptxn)->PCR[PTn(ptxn)] & ~PORT_PCR_IRQC_MASK)| PORT_PCR_IRQC(ecfg);
  //拉使能
  PORTX_BASE(ptxn)->PCR[PTn(ptxn)] = (PORTX_BASE(ptxn)->PCR[PTn(ptxn)] & ~PORT_PCR_PE_MASK)  | PORT_PCR_PE(pcfg/2); 
  //拉设置
  PORTX_BASE(ptxn)->PCR[PTn(ptxn)] = (PORTX_BASE(ptxn)->PCR[PTn(ptxn)] & ~PORT_PCR_PS_MASK)  | PORT_PCR_PS(pcfg%2);
  //清除中断标志
  PORT_IRQ_CLEAN(ptxn);
}

/**
 * @name        EXTI_Switch
 * @brief       外部中断模式切换
 * @param ptxn  管脚号
 * @param ecfg  触发设置
 * @return      无
 * @example     EXTI_Switch(PTA11, exti_disable);
 */
inline void EXTI_Switch(PTXn_e ptxn, exti_cfg ecfg)
{
  //中断设置
  PORTX_BASE(ptxn)->PCR[PTn(ptxn)] = (PORTX_BASE(ptxn)->PCR[PTn(ptxn)] & ~PORT_PCR_IRQC_MASK)| PORT_PCR_IRQC(ecfg);
}


