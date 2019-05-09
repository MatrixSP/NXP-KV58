/* 
 * @date   2019年02月12日最后修改
 * @name   Sora_lib
 * @group  Profiterole
 * @author Afisora
 * @brief  GPIO Driver
 */

#include "sora_system.h"

GPIO_MemMapPtr GPIOX[5] = {GPIOA, GPIOB, GPIOC, GPIOD, GPIOE};

/**
 * @name        GPIO_Init
 * @brief       GPIO初始化
 * @clock       System (CPU) clock
 * @param ptnx  管脚号
 * @param dir   GPIO模式
 * @param data  逻辑电平
 * @return      无     
 * @example     GPIO_Init(PTC0, GPO, 0);
 */
void GPIO_Init(PTXn_e ptxn, GPIO_CFG dir,int data)
{
  //开启时钟
  CLOCK_EnableClock((clock_ip_name_t)(kCLOCK_PortA + PTX(ptxn)));
  //设置管脚复用GPIO
  PORTX_BASE(ptxn)->PCR[PTn(ptxn)] = (PORTX_BASE(ptxn)->PCR[PTn(ptxn)] & ~PORT_PCR_MUX_MASK) | PORT_PCR_MUX(1);
  //设置GPIO模式
  if(dir == 1)//GPO
  {
    GPIOX_BASE(ptxn)->PDDR |= (1U << PTn(ptxn));
    if(data == 1)//logic 1
      GPIO_PinWrite(GPIOX_BASE(ptxn), PTn(ptxn), 1);
    else//logic 0
      GPIO_PinWrite(GPIOX_BASE(ptxn), PTn(ptxn), 0);
  }
  else//GPI
    GPIOX_BASE(ptxn)->PDDR &= ~(1U << PTn(ptxn));
}

/**
 * @name        GPIO_Ctrl
 * @brief       设置管脚输出电平
 * @clock       System (CPU) clock
 * @param ptnx  管脚号
 * @param data  逻辑电平
 * @return      无     
 * @example     GPIO_Ctrl(PTC0, 0);
 */
inline void GPIO_Ctrl(PTXn_e ptxn, int data)
{
  if(data == 1)//logic 1
    GPIO_PinWrite(GPIOX_BASE(ptxn), PTn(ptxn), 1);
  else//logic 0
    GPIO_PinWrite(GPIOX_BASE(ptxn), PTn(ptxn), 0);
}

/**
 * @name        GPIO_Reverse
 * @brief       反转管脚输出电平
 * @clock       System (CPU) clock
 * @param ptnx  管脚号
 * @return      无     
 * @example     GPIO_Reverse(PTC0);
 */
inline void GPIO_Reverse(PTXn_e ptxn)
{
  GPIOX_BASE(ptxn)->PDOR ^= (1U << PTn(ptxn));
}

/**
 * @name        GPIO_Get
 * @brief       读取管脚输入电平
 * @clock       System (CPU) clock
 * @param ptxn  GPIO模块序号
 * @return      逻辑电平值     
 * @example     x = GPIO_Get(PTC0);
 */
inline uint8 GPIO_Get(PTXn_e ptxn)
{
  return GPIO_PinRead(GPIOX_BASE(ptxn),PTn(ptxn));    
}

