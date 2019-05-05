/* 
 * @date   2019年04月16日最后修改
 * @name   Sora_lib
 * @group  Akko
 * @author Afisora
 * @brief  eFlexPWM Driver
 */


#ifndef __SORA_PWM_H__
#define __SORA_PWM_H__

//定义PWM子模块号
typedef enum PWM_SMn
{
	PWM_SM0,
	PWM_SM1,
	PWM_SM2,
	PWM_SM3,
} PWM_SMn;

//定义PWM通道号
typedef enum PWM_CHn
{
	PWM0_SM0_CHA, 
	PWM0_SM0_CHB,
	PWM0_SM1_CHA,
	PWM0_SM1_CHB,
	PWM0_SM2_CHA,
	PWM0_SM2_CHB,
	PWM0_SM3_CHA,
	PWM0_SM3_CHB,

	PWM1_SM0_CHA,
	PWM1_SM0_CHB,
	PWM1_SM1_CHA,
	PWM1_SM1_CHB,
	PWM1_SM2_CHA,
	PWM1_SM2_CHB,
	PWM1_SM3_CHA,
	PWM1_SM3_CHB,
} PWM_CHn;

//定义PWM对齐模式
typedef enum PWM_Align
{
	PWM_Signed_CenterAligned,
	PWM_Unsigned_CenterAligned,
	PWM_Signed_EdgeAligned,
	PWM_Unsigned_EdgeAligned,
}PWM_Align;

//PWM管脚定义
extern const PTXn_e PWM_Pin[];

//PWM对齐模式保存区
extern PWM_Align PWM_Alignedmode_status[];

void FlexPWM_Independent_Submodule_Init(PWM_Type* base, PWM_SMn subModule, PWM_Align mode, uint32_t freq);
void FlexPWM_Independent_Channel_Init(PWM_CHn ch);
void FlexPWM_Independent_Channel_Duty(PWM_CHn ch, float dutyCyclePercent);
void FlexPWM_Independent_Channel_Duty_Buff(PWM_CHn ch, float dutyCyclePercent, int16_t* VALH, int16_t* VALL);
void FlexPWM_VALDE_Control(PWM_CHn ch, bool able);

#endif