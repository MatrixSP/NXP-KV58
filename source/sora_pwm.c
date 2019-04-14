/* 
 * @date   2019年04月14日最后修改
 * @name   Sora_lib
 * @group  Akko
 * @author Afisora
 * @brief  eFlexPWM Driver
 */

#include "sora_system.h"

/**
 * @name			FlexPWM_Independent_Init
 * @brief			初始化PWM，各子模块独立工作模式
 * @clock			Fast Peripheral clock
 * @param base		PWM模块号(PWM0/PWM1)
 * @param subModule	子模块号
 * @return			无
 * @example			FlexPWM_Independent_Init(PWM0, PWM_SM0);
 */
void FlexPWM_Independent_Init(PWM_Type* base, PWM_SMn subModule)
{
	//开启时钟
	//子模块0的时钟不能选择“跟随子模块0的时钟”
	//子模块0的重载源不能选择“以子模块0为重载源”
	if (base == PWM0)
	{
		switch (subModule)
		{
		case PWM_SM0:CLOCK_EnableClock(kCLOCK_Pwm0_Sm0); break;
		case PWM_SM1:CLOCK_EnableClock(kCLOCK_Pwm0_Sm1); break;
		case PWM_SM2:CLOCK_EnableClock(kCLOCK_Pwm0_Sm2); break;
		case PWM_SM3:CLOCK_EnableClock(kCLOCK_Pwm0_Sm3); break;
		default:
			return;
		}
	}
	else if (base == PWM1)
	{
		switch (subModule)
		{
		case PWM_SM0:CLOCK_EnableClock(kCLOCK_Pwm1_Sm0); break;
		case PWM_SM1:CLOCK_EnableClock(kCLOCK_Pwm1_Sm1); break;
		case PWM_SM2:CLOCK_EnableClock(kCLOCK_Pwm1_Sm2); break;
		case PWM_SM3:CLOCK_EnableClock(kCLOCK_Pwm1_Sm3); break;
		default:
			return;
		}
	}
	else
		return;

	//清除错误状态标志
	base->FSTS |= PWM_FSTS_FFLAG_MASK;

	//配置控制寄存器2
	//若需要互补PWM则不应当为INDEP置位，而应当进一步配置MCTRL寄存器
	base->SM[subModule].CTRL2 = (0U
		| PWM_CTRL2_CLK_SEL(0U)		//使用内部时钟
		| PWM_CTRL2_RELOAD_SEL(0U)	//使用本地重载
		| PWM_CTRL2_FORCE_SEL(0U)	//使用FORCE寄存器触发重载
		| PWM_CTRL2_INIT_SEL(0U)	//使用本地初始化同步信号
		| PWM_CTRL2_INDEP(1U)		//通道A与通道B使用独立模式
		| PWM_CTRL2_WAITEN(1U)		//当CPU处于Wait模式时仍启用PWM
		| PWM_CTRL2_DBGEN(1U)		//当CPU处于Debug模式时仍启用PWM
		);

	//配置控制寄存器
	base->SM[subModule].CTRL = (0U
		| PWM_CTRL_PRSC(0U)		//预分频使用1分频
		| PWM_CTRL_LDFQ(0U)		//PWM重载频率使用每个周期重载
		| PWM_CTRL_LDMOD(1U)	//缓存立即重载至寄存器
		);

	//配置错误寄存器
	if (base->FFILT & PWM_FFILT_FILT_PER_MASK)
	{
		/* When changing values for fault period from a non-zero value, first write a value of 0
		 * to clear the filter
		 */
		base->FFILT &= ~(PWM_FFILT_FILT_PER_MASK);
	}
	base->FFILT = (PWM_FFILT_FILT_CNT(0U) | PWM_FFILT_FILT_PER(0U));

	//触发Force事件以强制重载寄存器
	base->SM[subModule].CTRL2 |= PWM_CTRL2_FORCE(1U);
}