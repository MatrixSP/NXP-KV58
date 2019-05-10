/*
 * @date   2019年05月06日最后修改
 * @name   Sora_lib
 * @group  Profiterole
 * @author Afisora
 * @brief  eFlexPWM Driver
 */

#include "sora_system.h"

extern const PTXn_e PWM_Pin[16];

//PWM对齐模式保存区
PWM_Align PWM_Alignedmode_status[8] = 
{
	PWM_Signed_CenterAligned,	//pwm0sm0
	PWM_Signed_CenterAligned,	//pwm0sm1
	PWM_Signed_CenterAligned,	//pwm0sm2
	PWM_Signed_CenterAligned,	//pwm0sm3
	PWM_Signed_CenterAligned,	//pwm1sm0
	PWM_Signed_CenterAligned,	//pwm1sm1
	PWM_Signed_CenterAligned,	//pwm1sm2
	PWM_Signed_CenterAligned,	//pwm1sm3
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
	uint16_t	pulseCnt = 0;
	uint16_t	pwmHighPulse = 0;
	int16_t		modulo = 0;
	static bool	config = false;
	uint32_t	pwmClock;
	uint8_t		div = 0;

	//XBARA初始化，桥接错误通道
	//Fault通道直接控制了PWM子模块的运行
	if (!config)
	{
		XBARA_Init(XBARA);
		XBARA_SetSignalsConnection(XBARA, kXBARA_InputVdd, kXBARA_OutputPwm0Fault0);
		XBARA_SetSignalsConnection(XBARA, kXBARA_InputVdd, kXBARA_OutputPwm0Fault1);
		XBARA_SetSignalsConnection(XBARA, kXBARA_InputVdd, kXBARA_OutputPwm0Fault2);
		XBARA_SetSignalsConnection(XBARA, kXBARA_InputVdd, kXBARA_OutputPwm0Fault3);
		config = true;
	}

	//开启时钟
	//子模块0的时钟不能选择“跟随子模块0的时钟”
	//子模块0的重载源不能选择“以子模块0为重载源”
	if (base == PWM0)
	{
		switch (subModule)
		{
		case PWM_SM0:CLOCK_EnableClock(kCLOCK_Pwm0_Sm0);break;
		case PWM_SM1:CLOCK_EnableClock(kCLOCK_Pwm0_Sm1);break;
		case PWM_SM2:CLOCK_EnableClock(kCLOCK_Pwm0_Sm2);break;
		case PWM_SM3:CLOCK_EnableClock(kCLOCK_Pwm0_Sm3);break;
		default:
			return;
		}
		//记录PWM对齐设置
		PWM_Alignedmode_status[subModule] = mode;
	}
	else if (base == PWM1)
	{
		switch (subModule)
		{
		case PWM_SM0:CLOCK_EnableClock(kCLOCK_Pwm1_Sm0);break;
		case PWM_SM1:CLOCK_EnableClock(kCLOCK_Pwm1_Sm1);break;
		case PWM_SM2:CLOCK_EnableClock(kCLOCK_Pwm1_Sm2);break;
		case PWM_SM3:CLOCK_EnableClock(kCLOCK_Pwm1_Sm3);break;
		default:
			return;
		}
		//记录PWM对齐设置
		PWM_Alignedmode_status[4 + subModule] = mode;
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

	//计算分频因子
	for (div = 0; div < 8; div++)
	{
		if (((Fast_Peripheral_Clock / (1U << div)) / freq) <= 0xFFFF)
			break;
	}

	//配置控制寄存器
	base->SM[subModule].CTRL = (0U
		| PWM_CTRL_PRSC(div)	//分频
		| PWM_CTRL_LDFQ(0U)		//PWM重载频率使用每个周期重载
		| PWM_CTRL_LDMOD(0U)	//禁用缓存立即重载至寄存器
		| PWM_CTRL_FULL(1U)		//每个PWM周期重载一次寄存器
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
	pwmClock = Fast_Peripheral_Clock / (1U << ((base->SM[subModule].CTRL & PWM_CTRL_PRSC_MASK) >> PWM_CTRL_PRSC_SHIFT));
	pulseCnt = (pwmClock / freq);
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
 * @name		FlexPWM_Independent_Channel_Init
 * @brief		PWM通道初始化并开启输出
 * @clock		Fast Peripheral clock
 * @param ch	通道号
 * @return		无
 * @example		FlexPWM_Independent_Channel_Init(PWM0_SM0_CHA);
 */
void FlexPWM_Independent_Channel_Init(PWM_CHn ch)
{
	PTXn_e		pin = PWM_Pin[ch];
	PWM_Type*	base;
	PWM_SMn		subModule = (PWM_SMn)((ch % 8U) / 2U);
	uint8_t		outputEnableShift = 0;

	//初始配置读取
	if (ch / 8U == 0) base = PWM0;
	else base = PWM1;
	if(ch % 2U == 0) outputEnableShift = PWM_OUTEN_PWMA_EN_SHIFT;
	else outputEnableShift = PWM_OUTEN_PWMB_EN_SHIFT;

	//设置管脚复用
	switch (PTX(pin))
	{
	case 0:PORT_Init(pin, 6, pull_up); break;	//PTA
	case 1:PORT_Init(pin, 5, pull_up); break;	//PTB
	case 2:PORT_Init(pin, 5, pull_up); break;	//PTC
	case 3:PORT_Init(pin, 6, pull_up); break;	//PTD
	case 4:PORT_Init(pin, 5, pull_up); break;	//PTE
	default: 
		return;
	}

	//禁用子模块
	base->MCTRL &= ~PWM_MCTRL_RUN(1U << subModule);

	//开启管脚PWM输出
	base->OUTEN |= (1U << (outputEnableShift + subModule));

	//加载寄存器缓冲
	base->MCTRL |= PWM_MCTRL_LDOK(1U << subModule);

	//启用子模块
	base->MCTRL |= PWM_MCTRL_RUN(1U << subModule);
}

/**
 * @name					FlexPWM_Independent_Channel_Duty
 * @brief					PWM通道设定占空比
 * @clock					Fast Peripheral clock
 * @param ch				通道号
 * @param dutyCyclePercent	占空比
 * @return					无
 * @example		
 */
void FlexPWM_Independent_Channel_Duty(PWM_CHn ch, float dutyCyclePercent)
{
	PWM_Type*	base;
	PWM_SMn		subModule = (PWM_SMn)((ch % 8U) / 2U);
	PWM_Align	mode;
	uint16_t	pulseCnt = 0;
	uint16_t	pwmHighPulse = 0;
	int16_t		modulo = 0;

	//初始配置读取
	if (ch / 8U == 0) base = PWM0;
	else base = PWM1;
	mode = PWM_Alignedmode_status[ch / 2U];

	//配置占空比
	switch (mode)
	{
	case PWM_Signed_CenterAligned:
	{
		//有符号中心对齐
		modulo = base->SM[subModule].VAL1;
		pulseCnt = modulo * 2;
		/* Calculate pulse width */
		pwmHighPulse = (uint16_t)((pulseCnt * dutyCyclePercent) / 100);
		/* Calculate pulse width */
		if (ch % 2U == 0)
		{
			//Channel A
			base->SM[subModule].VAL2 = (-(pwmHighPulse / 2));
			base->SM[subModule].VAL3 = (pwmHighPulse / 2);
		}
		else
		{
			//Channel B
			base->SM[subModule].VAL4 = (-(pwmHighPulse / 2));
			base->SM[subModule].VAL5 = (pwmHighPulse / 2);
		}
		break;
	}
	case PWM_Unsigned_CenterAligned:
	{
		//无符号中心对齐
		pulseCnt = base->SM[subModule].VAL1;
		/* Calculate pulse width */
		pwmHighPulse = (uint16_t)((pulseCnt * dutyCyclePercent) / 100);
		if (ch % 2U == 0)
		{
			//Channel A
			base->SM[subModule].VAL2 = ((pulseCnt - pwmHighPulse) / 2);
			base->SM[subModule].VAL3 = ((pulseCnt + pwmHighPulse) / 2);
		}
		else
		{
			//Channel B
			base->SM[subModule].VAL4 = ((pulseCnt - pwmHighPulse) / 2);
			base->SM[subModule].VAL5 = ((pulseCnt + pwmHighPulse) / 2);
		}
		break;
	}
	case PWM_Signed_EdgeAligned:
	{
		//有符号边缘对齐
		modulo = base->SM[subModule].VAL1;
		pulseCnt = modulo * 2;
		/* Calculate pulse width */
		pwmHighPulse = (uint16_t)((pulseCnt * dutyCyclePercent) / 100);
		if (ch % 2U == 0)
		{
			//Channel A
			base->SM[subModule].VAL2 = (-modulo);
			base->SM[subModule].VAL3 = (-modulo + pwmHighPulse);
		}
		else
		{
			//Channel B
			base->SM[subModule].VAL4 = (-modulo);
			base->SM[subModule].VAL5 = (-modulo + pwmHighPulse);
		}
		break;
	}
	case PWM_Unsigned_EdgeAligned:
	{
		//无符号边缘对齐
		pulseCnt = base->SM[subModule].VAL1;
		/* Calculate pulse width */
		pwmHighPulse = (uint16_t)((pulseCnt * dutyCyclePercent) / 100);
		if (ch % 2U == 0)
		{
			//Channel A
			base->SM[subModule].VAL2 = 0;
			base->SM[subModule].VAL3 = pwmHighPulse;
		}
		else
		{
			//Channel B
			base->SM[subModule].VAL4 = 0;
			base->SM[subModule].VAL5 = pwmHighPulse;
		}
		break;
	}
	default:
		break;
	}

	//加载寄存器缓冲
	base->MCTRL |= PWM_MCTRL_LDOK(1U << subModule);
}


/**
 * @name					FlexPWM_Independent_Channel_Duty_Buff
 * @brief					PWM通道设定占空比，寄存器数据缓存
 * @clock					Fast Peripheral clock
 * @param ch				通道号
 * @param dutyCyclePercent	占空比
 * @return					无
 * @example					FlexPWM_Independent_Channel_Duty_Buff(ch, j, &VH[i], &VL[i]);
 */
void FlexPWM_Independent_Channel_Duty_Buff(PWM_CHn ch, float dutyCyclePercent, int16_t* VALH, int16_t* VALL)
{
	PWM_Type*	base;
	PWM_SMn		subModule = (PWM_SMn)((ch % 8U) / 2U);
	PWM_Align	mode;
	uint16_t	pulseCnt = 0;
	uint16_t	pwmHighPulse = 0;
	int16_t		modulo = 0;

	//初始配置读取
	if (ch / 8U == 0) base = PWM0;
	else base = PWM1;
	mode = PWM_Alignedmode_status[ch / 2U];

	//配置占空比
	switch (mode)
	{
	case PWM_Signed_CenterAligned:
	{
		//有符号中心对齐
		modulo = base->SM[subModule].VAL1;
		pulseCnt = modulo * 2;
		/* Calculate pulse width */
		pwmHighPulse = (uint16_t)((pulseCnt * dutyCyclePercent) / 100);
		/* Calculate pulse width */
		if (ch % 2U == 0)
		{
			//Channel A
			*VALH = (-(pwmHighPulse / 2));
			*VALL = (pwmHighPulse / 2);
		}
		else
		{
			//Channel B
			*VALH = (-(pwmHighPulse / 2));
			*VALL = (pwmHighPulse / 2);
		}
		break;
	}
	case PWM_Unsigned_CenterAligned:
	{
		//无符号中心对齐
		pulseCnt = base->SM[subModule].VAL1;
		/* Calculate pulse width */
		pwmHighPulse = (uint16_t)((pulseCnt * dutyCyclePercent) / 100);
		if (ch % 2U == 0)
		{
			//Channel A
			*VALH = ((pulseCnt - pwmHighPulse) / 2);
			*VALL = ((pulseCnt + pwmHighPulse) / 2);
		}
		else
		{
			//Channel B
			*VALH = ((pulseCnt - pwmHighPulse) / 2);
			*VALL = ((pulseCnt + pwmHighPulse) / 2);
		}
		break;
	}
	case PWM_Signed_EdgeAligned:
	{
		//有符号边缘对齐
		modulo = base->SM[subModule].VAL1;
		pulseCnt = modulo * 2;
		/* Calculate pulse width */
		pwmHighPulse = (uint16_t)((pulseCnt * dutyCyclePercent) / 100);
		if (ch % 2U == 0)
		{
			//Channel A
			*VALH = (-modulo);
			*VALL = (-modulo + pwmHighPulse);
		}
		else
		{
			//Channel B
			*VALH = (-modulo);
			*VALL = (-modulo + pwmHighPulse);
		}
		break;
	}
	case PWM_Unsigned_EdgeAligned:
	{
		//无符号边缘对齐
		pulseCnt = base->SM[subModule].VAL1;
		/* Calculate pulse width */
		pwmHighPulse = (uint16_t)((pulseCnt * dutyCyclePercent) / 100);
		if (ch % 2U == 0)
		{
			//Channel A
			*VALH = 0;
			*VALL = pwmHighPulse;
		}
		else
		{
			//Channel B
			*VALH = 0;
			*VALL = pwmHighPulse;
		}
		break;
	}
	default:
		break;
	}
}

/**
 * @name					FlexPWM_VALDE_Control
 * @brief					Value寄存器重载DMA使能控制
 * @clock					Fast Peripheral clock
 * @param ch				通道号
 * @param able				使能控制
 * @return					无
 * @example					FlexPWM_VALDE_Control(PWM0_SM1_CHA, true);
 */
void FlexPWM_VALDE_Control(PWM_CHn ch, bool able)
{
	PWM_Type*	base;
	PWM_SMn		subModule = (PWM_SMn)((ch % 8U) / 2U);

	//初始配置读取
	if (ch / 8U == 0) base = PWM0;
	else base = PWM1;

	//设置Value装载DMA控制器
	if (able)
	{
		base->SM[subModule].DMAEN |= PWM_DMAEN_VALDE(1);
	}
	else
	{
		base->SM[subModule].DMAEN &= ~PWM_DMAEN_VALDE(1);
	}
}