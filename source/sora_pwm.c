/*
 * @date   2019年04月14日最后修改
 * @name   Sora_lib
 * @group  Akko
 * @author Afisora
 * @brief  eFlexPWM Driver
 */

#include "sora_system.h"

struct PWM_Alignedmode_status
{
	PWM_Align pwm0sm0;
	PWM_Align pwm0sm1;
	PWM_Align pwm0sm2;
	PWM_Align pwm0sm3;
	PWM_Align pwm1sm0;
	PWM_Align pwm1sm1;
	PWM_Align pwm1sm2;
	PWM_Align pwm1sm3;
};

struct PWM_Alignedmode_status pwm_alignedmode_status =
{
	.pwm0sm0 = PWM_Signed_CenterAligned,
	.pwm0sm1 = PWM_Signed_CenterAligned,
	.pwm0sm2 = PWM_Signed_CenterAligned,
	.pwm0sm3 = PWM_Signed_CenterAligned,
	.pwm1sm0 = PWM_Signed_CenterAligned,
	.pwm1sm1 = PWM_Signed_CenterAligned,
	.pwm1sm2 = PWM_Signed_CenterAligned,
	.pwm1sm3 = PWM_Signed_CenterAligned,
};

/**
 * @name			FlexPWM_Independent_Submodule_Init
 * @brief			初始化PWM，配置子模块为独立工作模式，设定通道对齐方式与频率
 * @clock			Fast Peripheral clock
 * @param base		PWM模块号(PWM0/PWM1)
 * @param subModule	子模块号
 * @param mode		PWM对齐模式
 * @param freq		PWM频率
 * @return			无
 * @example			FlexPWM_Independent_Submodule_Init(PWM0, PWM_SM0, PWM_Signed_CenterAligned, 1000);
 */
void FlexPWM_Independent_Submodule_Init(PWM_Type* base, PWM_SMn subModule, PWM_Align mode, uint32_t freq)
{
	uint16_t pulseCnt = 0;
	uint16_t pwmHighPulse = 0;
	int16_t modulo = 0;
	//开启时钟并记录PWM对齐设置
	//子模块0的时钟不能选择“跟随子模块0的时钟”
	//子模块0的重载源不能选择“以子模块0为重载源”
	if (base == PWM0)
	{
		switch (subModule)
		{
		case PWM_SM0:
		{
			CLOCK_EnableClock(kCLOCK_Pwm0_Sm0);
			pwm_alignedmode_status.pwm0sm0 = mode;
			break;
		}
		case PWM_SM1:
		{
			CLOCK_EnableClock(kCLOCK_Pwm0_Sm1);
			pwm_alignedmode_status.pwm0sm1 = mode;
			break;
		}
		case PWM_SM2:
		{
			CLOCK_EnableClock(kCLOCK_Pwm0_Sm2);
			pwm_alignedmode_status.pwm0sm2 = mode;
			break;
		}
		case PWM_SM3:
		{
			CLOCK_EnableClock(kCLOCK_Pwm0_Sm3);
			pwm_alignedmode_status.pwm0sm3 = mode;
			break;
		}
		default:
			return;
		}
	}
	else if (base == PWM1)
	{
		switch (subModule)
		{
		case PWM_SM0:
		{
			CLOCK_EnableClock(kCLOCK_Pwm1_Sm0);
			pwm_alignedmode_status.pwm1sm0 = mode;
			break;
		}
		case PWM_SM1:
		{
			CLOCK_EnableClock(kCLOCK_Pwm1_Sm1);
			pwm_alignedmode_status.pwm1sm1 = mode;
			break;
		}
		case PWM_SM2:
		{
			CLOCK_EnableClock(kCLOCK_Pwm1_Sm2);
			pwm_alignedmode_status.pwm1sm2 = mode;
			break;
		}
		case PWM_SM3:
		{
			CLOCK_EnableClock(kCLOCK_Pwm1_Sm3);
			pwm_alignedmode_status.pwm1sm3 = mode;
			break;
		}
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

	//配置频率并初始化占空比为0
	pulseCnt = (Fast_Peripheral_Clock * 1000 / freq);
	switch (mode)
	{
	case PWM_Signed_CenterAligned:
	{
		//有符号中心对齐
		modulo = pulseCnt >> 1;
		base->SM[subModule].INIT = (-modulo);
		base->SM[subModule].VAL0 = 0;
		base->SM[subModule].VAL1 = modulo;
		base->SM[subModule].VAL2 = (-(pwmHighPulse / 2));
		base->SM[subModule].VAL3 = (pwmHighPulse / 2);
		base->SM[subModule].VAL4 = (-(pwmHighPulse / 2));
		base->SM[subModule].VAL5 = (pwmHighPulse / 2);
		break;
	}
	case PWM_Unsigned_CenterAligned:
	{
		//无符号中心对齐
		base->SM[subModule].INIT = 0;
		base->SM[subModule].VAL0 = (pulseCnt / 2);
		base->SM[subModule].VAL1 = pulseCnt;
		base->SM[subModule].VAL2 = ((pulseCnt - pwmHighPulse) / 2);
		base->SM[subModule].VAL3 = ((pulseCnt + pwmHighPulse) / 2);
		base->SM[subModule].VAL4 = ((pulseCnt - pwmHighPulse) / 2);
		base->SM[subModule].VAL5 = ((pulseCnt + pwmHighPulse) / 2);
		break;
	}
	case PWM_Signed_EdgeAligned:
	{
		//有符号边缘对齐
		modulo = pulseCnt >> 1;
		base->SM[subModule].INIT = (-modulo);
		base->SM[subModule].VAL0 = 0;
		base->SM[subModule].VAL1 = modulo;
		base->SM[subModule].VAL2 = (-modulo);
		base->SM[subModule].VAL3 = (-modulo + pwmHighPulse);
		base->SM[subModule].VAL4 = (-modulo);
		base->SM[subModule].VAL5 = (-modulo + pwmHighPulse);
		break;
	}
	case PWM_Unsigned_EdgeAligned:
	{
		//无符号边缘对齐
		base->SM[subModule].INIT = 0;
		base->SM[subModule].VAL0 = (pulseCnt / 2);
		base->SM[subModule].VAL1 = pulseCnt;
		base->SM[subModule].VAL2 = 0;
		base->SM[subModule].VAL3 = pwmHighPulse;
		base->SM[subModule].VAL4 = 0;
		base->SM[subModule].VAL5 = pwmHighPulse;
		break;
	}
	default:
		break;
	}
}

/**
 * @name			FlexPWM_Independent_SetupPwm
 * @brief
 * @clock			Fast Peripheral clock
 */
void FlexPWM_Independent_Channel_Init(PWM_Type * base, PWM_SMn subModule, PWM_CHn channel)
{
	uint32_t pwmClock;
	uint16_t pulseCnt = 0;
}