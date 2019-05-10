/* 
 * @name   Sora_lib
 * @group  Profiterole
 * @author Afisora
 * @brief  DMA Driver
 */

#ifndef __SORA_EDMA_H__
#define __SORA_EDMA_H__

//Group 1 contains channels 31-16
//Group 0 contains channels 15-0

typedef enum DMA_sources
{
    Channel_Disabled        = 0,
    
    //Group0 START
    DMA_UART0_Rx            = 2,
    DMA_UART0_Tx            = 3,
    DMA_UART1_Rx            = 4,
    DMA_UART1_Tx            = 5,
    
    DMA_flexPWM0_WR0        = 6,
    DMA_flexPWM0_WR1        = 7,
    DMA_flexPWM0_WR2        = 8,
    DMA_flexPWM0_WR3        = 9,
    DMA_flexPWM0_CP0        = 10,
    DMA_flexPWM0_CP1        = 11,
    DMA_flexPWM0_CP2        = 12,
    DMA_flexPWM0_CP3        = 13,
    
    DMA_FlexCAN0            = 14,
    DMA_FlexCAN1            = 15,

    DMA_SPI0_Rx             = 16,
    DMA_SPI0_Tx             = 17,
    
    DMA_XBARA_OUT0          = 18,
    DMA_XBARA_OUT1          = 19,
    DMA_XBARA_OUT2          = 20,
    DMA_XBARA_OUT3          = 21,

    DMA_I2C0                = 22,

    DMA_FTM0_CH0            = 24,
    DMA_FTM0_CH1            = 25,
    DMA_FTM0_CH2            = 26,
    DMA_FTM0_CH3            = 27,
    DMA_FTM0_CH4            = 28,
    DMA_FTM0_CH5            = 29,
    DMA_FTM0_CH6            = 30,
    DMA_FTM0_CH7            = 31,
    DMA_FTM1_CH0            = 32,
    DMA_FTM1_CH1            = 33,
    
    DMA_CMP3                = 34,
    
    DMA_FTM3_CH0            = 36,
    DMA_FTM3_CH1            = 37,
    DMA_FTM3_CH2            = 38,
    DMA_FTM3_CH3            = 39,

    DMA_HSADC0A             = 40,//HSADC0A Scan complete
    DMA_HSADC0B             = 41,//HSADC0B Scan complete
    
    DMA_CMP0                = 42,
    DMA_CMP1                = 43,
    DMA_CMP2                = 44,
    
    DMA_DAC0                = 45,

    DMA_PDB1                = 47,
    DMA_PDB0                = 48,

    DMA_Port_A              = 49,
    DMA_Port_B              = 50,
    DMA_Port_C              = 51,
    DMA_Port_D              = 52,
    DMA_Port_E              = 53,

    DMA_FTM3_CH4            = 54,
    DMA_FTM3_CH5            = 55,
    DMA_FTM3_CH6            = 56,
    DMA_FTM3_CH7            = 57,

    DMA_Always_EN0          = 60,//DMAX MUX always enabled
    DMA_Always_EN1          = 61,//DMAX MUX always enabled
    DMA_Always_EN2          = 62,//DMAX MUX always enabled
    DMA_Always_EN3          = 63,//DMAX MUX always enabled
    //#Group0 END
    
    //Group1 START
    DMA_UART2_Rx            = 2,
    DMA_UART2_Tx            = 3,
    DMA_UART3_Rx            = 4,
    DMA_UART3_Tx            = 5,
    
    DMA_flexPWM1_WR0        = 6,
    DMA_flexPWM1_WR1        = 7,
    DMA_flexPWM1_WR2        = 8,
    DMA_flexPWM1_WR3        = 9,
    DMA_flexPWM1_CP0        = 10,
    DMA_flexPWM1_CP1        = 11,
    DMA_flexPWM1_CP2        = 12,
    DMA_flexPWM1_CP3        = 13,
    
    DMA_FlexCAN2            = 14,

    DMA_SPI1_Rx             = 16,
    DMA_SPI1_Tx             = 17,

    DMA_I2C1                = 22,

    DMA_FTM2_CH0            = 32,
    DMA_FTM2_CH1            = 33,
    
    DMA_SPI2_Rx             = 34,
    DMA_SPI2_Tx             = 35,
    
    DMA_Timer0              = 36,//IEEE 1588 Timer 0
    DMA_Timer1              = 37,//IEEE 1588 Timer 0
    DMA_Timer2              = 38,//IEEE 1588 Timer 0
    DMA_Timer3              = 39,//IEEE 1588 Timer 0

    DMA_HSADC1A             = 40,//HSADC0A Scan complete
    DMA_HSADC1B             = 41,//HSADC0B Scan complete

    DMA_ADC0                = 45,

    DMA_UART4_Rx            = 54,
    DMA_UART4_Tx            = 55,
    DMA_UART5_Rx            = 56,
    DMA_UART5_Tx            = 57,

    DMA_Always_EN4          = 60,//DMAX MUX always enabled
    DMA_Always_EN5          = 61,//DMAX MUX always enabled
    DMA_Always_EN6          = 62,//DMAX MUX always enabled
    DMA_Always_EN7          = 63,//DMAX MUX always enabled
    //#Group1 END
    
} DMA_sources;

typedef enum DMA_BYTEn
{
    DMA_BYTE1  = 0,
    DMA_BYTE2  = 1,
    DMA_BYTE4  = 2,
    DMA_BYTE16 = 4,
    DMA_BYTE32 = 5,
} DMA_BYTEn;

typedef enum DMA_CHn
{
    DMA_CH0,
    DMA_CH1,
    DMA_CH2,
    DMA_CH3,
    DMA_CH4, 
    DMA_CH5, 
    DMA_CH6, 
    DMA_CH7,
    DMA_CH8,
    DMA_CH9,
    DMA_CH10,
    DMA_CH11,
    DMA_CH12,
    DMA_CH13,
    DMA_CH14,
    DMA_CH15,
    DMA_CH16,
    DMA_CH17,
    DMA_CH18,
    DMA_CH19,
    DMA_CH20,
    DMA_CH21,
    DMA_CH22,
    DMA_CH23,
    DMA_CH24,
    DMA_CH25,
    DMA_CH26,
    DMA_CH27,
    DMA_CH28,
    DMA_CH29,
    DMA_CH30,
    DMA_CH31,
} DMA_CHn;

typedef enum DMA_STATUS
{
  DMA_NOTINIT, //DMA未初始化完毕
  DMA_IDLE,    //DMA空闲
  DMA_PROCESS, //DMA传输中
  DMA_FINISH,  //DMA传输完毕
  DMA_REQUEST, //DMA传输请求
} DMA_STATUS;

#define  DMA_IRQ_EN(DMA_CHn)    NVIC_EnableIRQ((IRQn_Type)((DMA_CHn) & 0x0F))  //启用DMA中断
#define  DMA_IRQ_DIS(DMA_CHn)   NVIC_DisableIRQ((IRQn_Type)((DMA_CHn) & 0x0F)) //禁止DMA中断
#define  DMA_IRQ_CLEAN(DMA_CHn) DMA0->INT |= (DMA_INT_INT0_MASK<<(DMA_CHn))    //清除DMA中断标志位

#define  DMA_EN(DMA_CHn)        DMA0->ERQ |= (DMA_ERQ_ERQ0_MASK<<(DMA_CHn))    //使能DMA硬件请求
#define  DMA_DIS(DMA_CHn)       DMA0->ERQ &=~(DMA_ERQ_ERQ0_MASK<<(DMA_CHn))    //禁止DMA硬件请求

void EDMA_Transport_Init(DMA_CHn CHn, uint32 SADDR, DMA_BYTEn byten);
void EDMA_Transport_StartOnce(DMA_CHn CHn, uint32 DADDR, uint32 count);
void EDMA_PORT_Init(DMA_CHn CHn, void *SADDR, void *DADDR, DMA_BYTEn byten, PTXn_e ptxn, uint32 count, exti_cfg ecfg, pull_cfg pcfg);
void EDMA_PORT_StartOnce(DMA_CHn CHn, uint32 DADDR, uint32 count);
void EDMA_UART_RX_Init(DMA_CHn CHn, UARTn_e uartn);
uint32 EDMA_UART_RX_Stop(DMA_CHn CHn, uint32 DADDR);
void EDMA_UART_RX_Start(DMA_CHn CHn, uint32 DADDR, uint32 count);
void EDMA_FlexPWM_Init(PWM_CHn ch, DMA_CHn CHn, uint32_t SADDR);
void EDMA_FlexPWM_StartOnce(DMA_CHn CHn, uint32 count);

#endif
