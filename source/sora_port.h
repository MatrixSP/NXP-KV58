/* 
 * @date   2019年02月23日最后修改
 * @name   Sora_lib
 * @group  Akko
 * @author Afisora
 * @brief  PORT Driver
 */

#ifndef __SORA_PORT_H_
#define __SORA_PORT_H_

extern PORT_MemMapPtr PORTX[5];

typedef enum PORT_IRQ
{
  PORTA_IRQ,
  PORTB_IRQ,
  PORTC_IRQ,
  PORTD_IRQ,
  PORTE_IRQ,
} PORT_IRQ;

typedef enum exti_cfg
{
  exti_disable = 0x00,     //禁用外部中断
  
  rising_DMA   = 0x01,     //上升沿触发DMA中断
  falling_DMA  = 0x02,     //下降沿触发DMA中断
  either_DMA   = 0x03,     //跳变沿触发DMA中断
  
  zero         = 0x08,     //低电平触发
  rising       = 0x09,     //上升沿触发
  falling      = 0x0A,     //下降沿触发
  either       = 0x0B,     //跳变沿触发
  one          = 0x0C,     //高电平触发
} exti_cfg;

typedef enum pull_cfg
{
  pull_disable = 0x00,
  pull_down    = 0x02,
  pull_up      = 0x03,
} pull_cfg;

#define PORTX_BASE(PTxn)    PORTX[PTX(PTxn)]    //PORT模块的地址

#define PTX(PTxn)           ((PTxn)>>5)         //模块号 A~E
#define PTn(PTxn)           ((PTxn)&0x1f)       //管脚号 0~31

#define PORT_IRQ_EN(PORT_IRQ)  NVIC_EnableIRQ((IRQn_Type)((PORT_IRQ) + (int8)PORTA_IRQn))   //启用PORT中断
#define PORT_IRQ_DIS(PORT_IRQ) NVIC_DisableIRQ((IRQn_Type)((PORT_IRQ) + (int8)PORTA_IRQn))  //禁用PORT中断
#define PORT_IRQ_GET(PTxn)     PORTX_BASE(PTxn)->ISFR & (1u<<PTn(PTxn))    //获取指定管脚外部中断标志
#define PORT_IRQ_CLEAN(PTxn)   PORTX_BASE(PTxn)->ISFR |= (1u<<PTn(PTxn))   //清除指定管脚外部中断标志

void PORT_Init(PTXn_e ptxn, uint8 ALTn, pull_cfg pcfg);
void EXTI_Init(PTXn_e ptxn, exti_cfg ecfg, pull_cfg pcfg);
void EXTI_Switch(PTXn_e ptxn, exti_cfg ecfg);

#endif