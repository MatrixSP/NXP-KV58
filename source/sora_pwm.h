/* 
 * @name   Sora_lib
 * @group  Profiterole
 * @author Afisora
 * @brief  eFlexPWM Driver
 */


#ifndef __SORA_PWM_H__
#define __SORA_PWM_H__

 //PWM管脚定义
 //ALT:A6 B5 C5 D6 E5
const static PTXn_e PWM_Pin[16] =
{
	//Pin	Channel    		MUX  
	PTD0,	//pwm0sm0chA	//PTD0  PTA3  PTD14	PTA3与J-Link冲突
	PTD1,	//pwm0sm0chB	//PTD1  PTA4  PTD15
	PTD2,	//pwm0sm1chA	//PTD2  PTD12	
	PTD3,	//pwm0sm1chB	//PTD3  PTD13
	PTD4,	//pwm0sm2chA	//PTD4  PTD10
	PTD5,	//pwm0sm2chB	//PTD5  PTD11
	PTD8,	//pwm0sm3chA	//PTC1  PTD8
	PTD9,	//pwm0sm3chB	//PTC2  PTD9
	PTC14,	//pwm1sm0chA	//PTE5  PTC14 PTD0
	PTC15,	//pwm1sm0chB	//PTE6  PTC15 PTD1
	PTE7,	//pwm1sm1chA	//PTE7  PTB18 PTC12 PTD2
	PTE8,	//pwm1sm1chB	//PTE8  PTB19 PTC13 PTD3
	PTE9,	//pwm1sm2chA	//PTE9  PTC8  PTC16
	PTE10,	//pwm1sm2chB	//PTE10 PTC9  PTC17
	PTE11,	//pwm1sm3chA	//PTE11 PTC10 PTC18
	PTE12,	//pwm1sm3chB	//PTE12 PTC11 PTC19
};

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
extern inline void FlexPWM_Independent_Channel_LDOK(PWM_Type* base, PWM_SMn sm);
void FlexPWM_Independent_Channel_Duty_Buff(PWM_CHn ch, float dutyCyclePercent, int16_t* VALH, int16_t* VALL);
void FlexPWM_VALDE_Control(PWM_CHn ch, bool able);
extern inline float FlexPWM_PPMCal_us(uint32_t feq, uint32_t width);

#endif