/* 
 * @date   2019年02月09日最后修改
 * @name   Sora_lib
 * @group  Profiterole
 * @author Afisora
 * @brief  GPIO Driver
 */

#ifndef __SORA_GPIO_H__
#define __SORA_GPIO_H__

extern GPIO_MemMapPtr GPIOX[5];

/*
 * GPIO模式
 */
typedef enum GPIO_CFG
{
    GPI         = 0,                          //定义管脚输入方向   
    GPO         = 1,                          //定义管脚输出方向
} GPIO_CFG;  

#define GPIOX_BASE(PTxn)    GPIOX[PTX(PTxn)]    //GPIO模块的地址

void GPIO_Init(PTXn_e ptxn, GPIO_CFG dir,int data);
void GPIO_Ctrl(PTXn_e ptxn, int data);
void GPIO_Reverse(PTXn_e ptxn);
uint8 GPIO_Get(PTXn_e ptxn);

#endif
