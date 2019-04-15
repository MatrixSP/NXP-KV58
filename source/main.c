/*
 * @date   2019年04月16日最后修改
 * @name   Sora_lib
 * @group  Akko
 * @author Afisora
 * @brief  基于官方库的工作环境搭建
 * @todo
 * @{
 *    1.FPU功能测试
 *    2.DSP功能测试
 *    3.内核system库编写
 *    4.DMA与UART IDLE结合的LOG信息发送
 *    5.IIC、SPI、FTM、ADC、LCD
 *    6.DWT、PIT时钟或许有误
 * @}
 * @note
 * @{
 *    1.FPU设置   __FPU_PRESENT，__FPU_USED
 *    2.DSP设置   ARM_MATH_CM7，ARM_MATH_MATRIX_CHECK，ARM_MATH_ROUNDING
 *    3.PTA6的ALT5复用功能是FlexBus频率，配合分频比可计算出实际核心频率
 *    4.PTA6的ALT7复用功能使用的是JLINK模块，输出的TRACE_CLKOUT为主频的一半，可用于检查时钟频率
 *    5.debug模式下没设置编译优化
 *    6.头文件按sora_system->include.h->...顺序展开
 * @}
 */

#define main_8

#include "include.h"

 /*
  * @date   2019年04月16日备份
  * @brief  测试主程序#8
  * @mode   PWM
  * @done
  * @note
  */
#ifdef main_8
uint32_t a;
int main(void)
{
	LCD_Init();
	FlexPWM_Independent_Submodule_Init(PWM0, PWM_SM1, PWM_Signed_CenterAligned, 1000);
	FlexPWM_Independent_Channel_Init(PWM0_SM1_CHA);
	FlexPWM_Independent_Channel_Duty(PWM0_SM1_CHA, 90);
	a = CLOCK_GetFreq(kCLOCK_BusClk);
	while (1U)
	{
		LCD_P6x8Str(0, 1, "PWM");
	}
}

#endif

/*
 * @date   2019年02月26日备份
 * @brief  测试主程序#7
 * @mode   摄像头采集
 * @done   行场中断采集
 * @note   Bus/Flash时钟需要超频才可完整采集一行
 *         利用一级缓存解决处理周期与采集周期的异步匹配
 *         缓存（188*120 bytes）复制时间（当262.5Mhz时）约30us
 *         LCD刷新阈值使用的是大津法
 */
#ifdef main_7
//#define Console_UART
#define Console_LCD

float timer_copy = 0;       //总周期时间
volatile uint8 fps_cnt = 0;
volatile uint8 fps = 0;
volatile bool img_fifo_flag = true;
volatile uint16 Line_Count = 0;

extern volatile uint8 img_buff[Camera_Height][Camera_Width];
extern volatile uint8 img_fifo[Camera_Height][Camera_Width];
extern volatile uint8 img_process[Camera_Height][Camera_Width];
extern volatile IMG_STATUS img_flag;

uint8 T = 0;
void OTSU_Init(uint8 imgbuff[][Camera_Width]);
void OTSU(uint8 imgbuff[][Camera_Width]);

int main(void)
{
	__NVIC_SetPriority(PORTB_IRQn, 1);
	__NVIC_SetPriority(PIT0_IRQn, 0);
	BOARD_InitBootClocks();
	BOARD_InitBootPins();
	//PORT_Init(PTA6, 7, pull_disable);//Trace clock output from the ARM CoreSight debug block
	//PORT_Init(PTA6, 5, pull_disable);//FlexBus Clock Output
	LCD_Init();
	MT9V032_Init();
	__disable_irq();
	PIT_IRQ_Init(PIT0, 1000);
	PIT_Timer_Init(PIT3);
	GPIO_Init(PTD14, GPO, 0);
	GPIO_Init(PTC0, GPO, 0);
#ifdef Console_UART
	UART_Com_Init(UART_0, 256000);
#endif
	EXTI_Init(Camera_VS, Camera_VS_Mode, pull_up);  //VS 场信号
	EXTI_Init(Camera_HS, Camera_HS_Mode, pull_up);  //HS 行信号
	EDMA_PORT_Init(Camera_DMA_CHn, Camera_DMA_SADDR, (void*)img_buff, DMA_BYTE1, Camera_PS, Camera_Width, Camera_DMA_Mode, pull_down);
	img_flag = IMG_START;
	__enable_irq();
	LCD_Fill(0x00);
	DELAY_MS(1000);
	while (img_fifo_flag);
	memcpy((void*)img_process, (void*)img_fifo, Camera_Width * Camera_Height);
	OTSU_Init((uint8(*)[Camera_Width])img_process);
	while (1)
	{
		while (img_fifo_flag);
		PIT_Timer_Start(PIT3);
		memcpy((void*)img_process, (void*)img_fifo, Camera_Width * Camera_Height);
		timer_copy = PIT_Timer_Get(PIT3);
		PIT_Timer_Stop(PIT3);
		LCD_Show_Number(100, 0, 1000 * timer_copy);
		LCD_Show_Number(100, 1, fps);
		LCD_Show_Number(100, 2, T);
		OTSU((uint8(*)[Camera_Width])img_process);
#ifdef Console_LCD
		//绘制缩略图
		for (int j = 0; j < Camera_Height; j += 2)
		{
			for (int i = Camera_Width - 1; i > 0; i -= 2)
			{
				if (img_process[j][i] < T)
					LCD_DrawPoint(i / 2, 0.5 * Camera_Height - j / 2, 1);
				else
					LCD_DrawPoint(i / 2, 0.5 * Camera_Height - j / 2, 0);
			}
		}
		LCD_Refresh_Gram();
#endif

#ifdef Console_UART
		//逐飞上位机
		UART_Put_Char(UART_0, 0x00);
		UART_Put_Char(UART_0, 0xFF);
		UART_Put_Char(UART_0, 0x01);
		UART_Put_Char(UART_0, 0x01);
		UART_Put_Buff(UART_0, (uint8*)img_process, Camera_Width * Camera_Height);
#endif
		fps_cnt++;
	}
}

void PIT0_IRQHandler()
{
	PIT_Flag_Clear(PIT0);
	GPIO_Reverse(PTC0);
	fps = fps_cnt;
	fps_cnt = 0;
}

void PORTB_IRQHandler()
{
	if (PORT_IRQ_GET(Camera_VS))//场中断
	{
		PORT_IRQ_CLEAN(Camera_VS);
		if (img_flag == IMG_START)
		{
			img_flag = IMG_GATHER;
			EDMA_PORT_StartOnce(DMA_CH4, (uint32)(&img_buff[Line_Count][0]), Camera_Width);
			Line_Count++;
		}
		return;
	}
	if (PORT_IRQ_GET(Camera_HS))//行中断
	{
		PORT_IRQ_CLEAN(Camera_HS);
		PTD14_OUT = 1;
		if (img_flag == IMG_GATHER)
		{
			EDMA_PORT_StartOnce(DMA_CH4, (uint32)(&img_buff[Line_Count][0]), Camera_Width);
			Line_Count++;
			if (Line_Count >= Camera_Height)
			{
				img_flag = IMG_START;
				Line_Count = 0;
				PTD14_OUT = 0;
				img_fifo_flag = true;
				memcpy((void*)img_fifo, (void*)img_buff, Camera_Width * Camera_Height);
				img_fifo_flag = false;
				return;
			}
		}
		return;
	}
}

void OTSU_Init(uint8 imgbuff[][Camera_Width])
{
	uint8 T_test, i, j;
	float G = 0, G_test;
	float W = Camera_Height * Camera_Width, W_Front, W_Back;
	float W0, W1;
	float Sum_Front, Sum_Back;
	float U0 = 0, U1 = 0;
	for (T_test = 0; T_test < 255; T_test++)
	{
		W_Front = 0; W_Back = 0; Sum_Front = 0; Sum_Back = 0;
		for (i = 0; i < Camera_Height; i++)
		{
			for (j = 0; j < Camera_Width; j++)
			{
				if (imgbuff[i][j] >= T_test)
				{
					W_Back++;
					Sum_Back += imgbuff[i][j];
				}
				else
				{
					W_Front++;
					Sum_Front += imgbuff[i][j];
				}
			}
		}
		W0 = W_Front / W;
		W1 = W_Back / W;
		if (W_Front == 0) U0 = 0;
		else U0 = Sum_Front / W_Front;
		if (W_Back == 0) U1 = 0;
		else U1 = Sum_Back / W_Back;
		G_test = W0 * W1 * (U0 - U1) * (U0 - U1);
		if (G_test == 0)
			T_test = T_test + 20;
		if (G_test >= G)
		{
			G = G_test;
			T = T_test;
		}
		else
			break;
	}
}

void OTSU(uint8 imgbuff[][Camera_Width])
{
	uint8 T_test, T0 = T, i, j;
	float G = 0, G_test;
	float W = Camera_Height * Camera_Width, W_Front, W_Back;
	float W0, W1;
	float Sum_Front, Sum_Back;
	float U0 = 0, U1 = 0;
	for (T_test = T0 - 5; T_test < (T0 + 5); T_test = T_test + 1)
	{
		W_Front = 0; W_Back = 0; Sum_Front = 0; Sum_Back = 0;
		for (i = 0; i < Camera_Height; i = i + 5)
		{
			for (j = 0; j < Camera_Width; j = j + 10)
			{
				if (imgbuff[i][j] >= T_test)
				{
					W_Back++;
					Sum_Back += imgbuff[i][j];
				}
				else
				{
					W_Front++;
					Sum_Front += imgbuff[i][j];
				}
			}
		}
		W0 = W_Front / W;
		W1 = W_Back / W;
		if (W_Front == 0) U0 = 0;
		else U0 = Sum_Front / W_Front;
		if (W_Back == 0) U1 = 0;
		else U1 = Sum_Back / W_Back;
		G_test = W0 * W1 * (U0 - U1) * (U0 - U1);
		if (G_test == 0)
			T_test = T_test + 20;
		if (G_test >= G)
		{
			G = G_test;
			T = T_test;
		}
		else
			break;
	}
}

#endif

/*
 * @date   2019年02月25日备份
 * @brief  测试主程序#6
 * @mode   摄像头采集
 * @done   行场中断采集
 * @note   Bus/Flash时钟需要超频才可完整采集一行
 *         时序问题导致每两帧图像丢失其中一帧
 */
#ifdef main_6
volatile uint16 Line_Count = 0;
extern volatile uint8 img_buff[Camera_Height][Camera_Width];
extern volatile IMG_STATUS img_flag;

int main(void)
{
	BOARD_InitBootClocks();
	BOARD_InitBootPins();
	//PORT_Init(PTA6, 7, pull_disable);//Trace clock output from the ARM CoreSight debug block
	//PORT_Init(PTA6, 5, pull_disable);//FlexBus Clock Output
	LCD_Init();
	MT9V032_Init();
	DisableInterrupts;
	GPIO_Init(PTD14, GPO, 0);
	//UART_Com_Init(UART_0, 256000);
	EXTI_Init(Camera_VS, Camera_VS_Mode, pull_up);  //VS 场信号
	EXTI_Init(Camera_HS, Camera_HS_Mode, pull_up);  //HS 行信号
	EDMA_PORT_Init(Camera_DMA_CHn, Camera_DMA_SADDR, (void*)img_buff, DMA_BYTE1, Camera_PS, Camera_Width, Camera_DMA_Mode, pull_down);
	EnableInterrupts;
	img_flag = IMG_START;

	LCD_Fill(0x00);
	//LCD_PutPixel(0, 0);
	//LCD_PutPixel(127, 63);
	while (1)
	{
		if (img_flag == IMG_FINISH)
		{
			img_flag = IMG_PROCESS;

			//绘制缩略图
			for (int j = 0; j < Camera_Height; j += 2)
			{
				for (int i = Camera_Width - 1; i > 0; i -= 2)
				{
					if (img_buff[j][i] < 50)
					{
						LCD_DrawPoint(i / 2, 0.5 * Camera_Height - j / 2, 1);
					}
					else
					{
						LCD_DrawPoint(i / 2, 0.5 * Camera_Height - j / 2, 0);
					}
				}
			}
			LCD_Refresh_Gram();

			//串口发送图片
			//红树上位机
			//UART_Put_Char(UART_0, 0xFF);
			//for(int j = 0;j < Camera_Height;j++)
			//{
			//  for(int i = 0;i < Camera_Width;i++)
			//  {
			//    if(img_buff[j][i] == 0xFF)
			//    {
			//      img_buff[j][i] = 0xFE;
			//    }
			//    UART_Put_Char(UART_0, img_buff[j][i]);
			//  }
			//}
			//逐飞上位机
			//UART_Put_Char(UART_0, 0x00);
			//UART_Put_Char(UART_0, 0xFF);
			//UART_Put_Char(UART_0, 0x01);
			//UART_Put_Char(UART_0, 0x01);
			//UART_Put_Buff(UART_0, (uint8 *)img_buff, Camera_Width * Camera_Height);

			img_flag = IMG_START;
			PORT_IRQ_CLEAN(Camera_VS);
			EXTI_Switch(Camera_VS, Camera_VS_Mode);
			PORT_IRQ_EN(PORTB_IRQ);
		}
	}
}

void PORTB_IRQHandler()
{
	if (PORT_IRQ_GET(Camera_VS))//场中断
	{
		PORT_IRQ_CLEAN(Camera_VS);
		if (img_flag == IMG_START)
		{
			PTD14_OUT = 1;
			img_flag = IMG_GATHER;
			EDMA_PORT_StartOnce(DMA_CH4, (uint32)(&img_buff[Line_Count][0]), Camera_Width);
			Line_Count++;
			EXTI_Switch(Camera_VS, exti_disable);
			EXTI_Switch(Camera_HS, Camera_HS_Mode);
		}
		return;
	}
	if (PORT_IRQ_GET(Camera_HS))//行中断
	{
		PORT_IRQ_CLEAN(Camera_HS);
		if (img_flag == IMG_GATHER)
		{
			EDMA_PORT_StartOnce(DMA_CH4, (uint32)(&img_buff[Line_Count][0]), Camera_Width);
			Line_Count++;
			if (Line_Count >= Camera_Height)
			{
				img_flag = IMG_FINISH;
				Line_Count = 0;
				EXTI_Switch(Camera_HS, exti_disable);
				PORT_IRQ_DIS(PORTB_IRQ);
				PTD14_OUT = 0;
				return;
			}
		}
		return;
	}
}
#endif
/*
 * @date   2019年02月25日备份
 * @brief  测试主程序#5
 * @mode   摄像头采集
 * @done   DMA中断，场连续采集
 * @note   DMA初始化需开启DMA中断响应
 *         时序问题导致每两帧图像丢失其中一帧
 */
#ifdef main_5
volatile uint16 Line_Count = 0;
extern volatile uint8 img_buff[Camera_Height][Camera_Width];
extern volatile IMG_STATUS img_flag;

int main(void)
{
	BOARD_InitBootClocks();
	BOARD_InitBootPins();
	//PORT_Init(PTA6, 7, pull_disable);//输出System Clock
	//PORT_Init(PTA6, 5, pull_disable);//输出Flexbus Clock
	LCD_Init();
	MT9V032_Init();
	DisableInterrupts;
	UART_Com_Init(UART_0, 256000);
	EXTI_Init(Camera_VS, Camera_VS_Mode, pull_down);  //VS 场信号
	//EXTI_Init(Camera_HS, Camera_HS_Mode, pull_down);  //HS 行信号
	EDMA_PORT_Init(Camera_DMA_CHn, Camera_DMA_SADDR, (void*)img_buff, DMA_BYTE1, Camera_PS, Camera_Width * Camera_Height, Camera_DMA_Mode, pull_down);
	DMA_IRQ_EN(Camera_DMA_CHn);
	EnableInterrupts;
	img_flag = IMG_START;

	LCD_Fill(0x00);
	//LCD_PutPixel(0, 0);
	//LCD_PutPixel(127, 63);
	while (1)
	{
		LCD_P6x8Str(50, 1, "Gather");
		if (img_flag == IMG_FINISH)
		{
			img_flag = IMG_PROCESS;

			//清屏
			LCD_Fill(0x00);
			LCD_P6x8Str(50, 1, "Finish");
			//绘制缩略图
			//for(int j = 0;j < Camera_Height;j += 2)
			//{
			//  for(int i = Camera_Width - 1;i > 0;i -= 2)
			//  {
			//    if(img_buff[j][i] > 20)
			//    {
			//      LCD_PutPixel(i/2, j/2);
			//    }
			//  }
			//}
			//串口发送图片
			//UART_Put_Char(UART_0, 0xFF);
			//for(int j = 0;j < Camera_Height;j++)
			//{
			//  for(int i = 0;i < Camera_Width;i++)
			//  {
			//    if(img_buff[j][i] == 0xFF)
			//    {
			//      img_buff[j][i] = 0xFE;
			//    }
			//    UART_Put_Char(UART_0, img_buff[j][i]);
			//  }
			//}
			UART_Put_Char(UART_0, 0x00);
			UART_Put_Char(UART_0, 0xFF);
			UART_Put_Char(UART_0, 0x01);
			UART_Put_Char(UART_0, 0x01);
			UART_Put_Buff(UART_0, (uint8*)img_buff, Camera_Width * Camera_Height);


			img_flag = IMG_START;

			PORT_IRQ_EN(PORTB_IRQ);
		}
	}
}

void PORTB_IRQHandler()
{
	if (PORT_IRQ_GET(Camera_VS))//场中断
	{
		PORT_IRQ_CLEAN(Camera_VS);
		if (img_flag == IMG_START)
		{
			img_flag = IMG_GATHER;
			PORT_IRQ_DIS(PORTB_IRQ);
			EDMA_PORT_StartOnce(DMA_CH4, (uint32)(&img_buff[Line_Count][0]), Camera_Width * Camera_Height);
		}
		return;
	}
}

void DMA4_DMA20_IRQHandler()
{
	if (DMA0->INT & (1 << 4))
	{
		DMA0->CINT |= DMA_CINT_CINT(4);
		img_flag = IMG_FINISH;
		return;
	}
}
#endif

/*
 * @date   2019年02月23日备份
 * @brief  测试主程序#4
 * @mode   UART
 * @done   UART配置摄像头
 */
#ifdef main_4
int main(void)
{
	BOARD_InitBootClocks();
	BOARD_InitBootPins();

	//DisableInterrupts;
	LCD_Init();
	MT9V032_Init();
	//EnableInterrupts;

	LCD_Fill(0x00);
	while (1)
	{
		LCD_Show_Number(0, 0, MT9V032_CFG_GET[0][1]);
		LCD_Show_Number(0, 1, MT9V032_CFG_GET[1][1]);
		LCD_Show_Number(0, 2, MT9V032_CFG_GET[2][1]);
		LCD_Show_Number(0, 3, MT9V032_CFG_GET[3][1]);
		LCD_Show_Number(0, 4, MT9V032_CFG_GET[4][1]);
		LCD_Show_Number(0, 5, MT9V032_CFG_GET[5][1]);
		LCD_Show_Number(0, 6, MT9V032_CFG_GET[6][1]);
		LCD_Show_Number(0, 7, MT9V032_CFG_GET[7][1]);
	}
}
#endif

/*
 * @date   2019年02月13日备份
 * @brief  测试主程序#3
 * @mode   UART、DMA
 * @done   UART在总线空闲时配置DMA，在接收时由DMA传输缓冲区数据
 */
#ifdef main_3
uint8 str[10] = "0000000000";
uint8 buffidle;
uint8 buffrx;
__IO uint8 buff1 = 0;
int32 status = 1;
__IO uint32 address = 0;

int main(void)
{
	BOARD_InitBootClocks();
	BOARD_InitBootPins();

	DisableInterrupts;
	LCD_Init();
	GPIO_Init(PTC0, GPO, 0);
	UART_Com_Init(UART_2, 9600);
	//UART_Com_Init(UART_2, 115200);
	UART_RX_IRQ_EN(UART_2, UART_RX_DMA);
	UART_RX_IRQ_EN(UART_2, UART_IDLE);
	EDMA_UART_RX_Init(DMA_CH20, UART_2);
	address = (uint32)str;
	EDMA_UART_RX_Start(DMA_CH20, (uint32)str, 10);
	status = 0;
	EnableInterrupts;

	while (1)
	{
		if (status == 1)
		{
			LCD_P6x8Str(0, 1, str);
			status = 0;
			address = DMA0->TCD[DMA_CH20].DADDR;
			EDMA_UART_RX_Start(DMA_CH20, (uint32)str, 10);
		}
		LCD_Show_Number(0, 2, buff1);
		LCD_Show_Number(0, 3, UART2->S1);
		LCD_Show_Number(0, 4, UART2->SFIFO);
		LCD_Show_Number(0, 5, buffidle);
		LCD_Show_Number(0, 6, buffrx);
	}
}

void UART2_RX_TX_IRQHandler(void)
{
	if (UART2->S1 & UART_S1_IDLE_MASK)
	{
		buffidle = UART2->D;
		buff1++;
		buffrx = EDMA_UART_RX_Stop(DMA_CH20, (uint32)str);
		UART2->CFIFO |= UART_CFIFO_RXFLUSH(1);
		status = 1;
	}
}
#endif

/*
 * @date   2019年02月9日备份
 * @brief  测试主程序#2
 * @mode   DMA、PORT
 * @done   SONY CCD图像采集测试
 */
#ifdef main_2
 //定义图像采集状态
typedef enum
{
	IMG_NOTINIT,
	IMG_FINISH,             //图像采集完毕
	IMG_FAIL,               //图像采集失败(采集行数少了)
	IMG_GATHER,             //图像采集中
	IMG_START,              //开始采集图像
	IMG_STOP,               //禁止图像采集
	IMG_PROCESS
} IMG_STATUS;
#define Camera_Width  200
#define Camera_Height 200
volatile uint16 Line_Count = 0;
volatile uint8 Image_Buff[Camera_Height][Camera_Width];
volatile IMG_STATUS img_flag = IMG_START;        //图像状态
uint8 data_fromportb = 0;

int main(void)
{
	BOARD_InitBootClocks();
	BOARD_InitBootPins();

	DisableInterrupts;
	LCD_Init();
	GPIO_Init(PTA17, GPO, 0);
	GPIO_Init(PTE29, GPO, 0);
	EXTI_Init(PTB8, rising, pull_up);  //VS 场信号
	EXTI_Init(PTB9, rising, pull_up);  //HS 行信号
	EDMA_PORT_Init(DMA_CH4, (void*)& PTB_BYTE0_IN, (void*)Image_Buff, DMA_BYTE1, PTB11, Camera_Width, falling_DMA, pull_up);
	EnableInterrupts;

	while (1)
	{
		LCD_P6x8Str(50, 1, "Gather");
		//data_fromportb = PTB_BYTE0_IN;
		if (img_flag == IMG_FINISH)
		{
			img_flag = IMG_PROCESS;

			//清屏
			LCD_Fill(0x00);
			LCD_P6x8Str(50, 1, "Finish");
			//绘制缩略图
			for (int j = 0; j < Camera_Height; j += 4)
			{
				for (int i = Camera_Width - 1; i > 0; i -= 4)
				{
					if (Image_Buff[j][i] > 100)
					{
						LCD_PutPixel(i / 4, (64 - j / 4));
					}
				}
			}

			img_flag = IMG_START;
			PORT_IRQ_CLEAN(PTB8);
			PORT_IRQ_CLEAN(PTB9);
			PORT_IRQ_EN(PORTA_IRQ);
		}
	}
}

void PORTB_IRQHandler()
{
	if (PORT_IRQ_GET(PTB8))//场中断
	{
		PORT_IRQ_CLEAN(PTB8);
		//GPIO_Reverse(PTA17);
		if (img_flag == IMG_START)
		{
			Line_Count = 0;
			img_flag = IMG_GATHER;
		}
		return;
	}
	if (PORT_IRQ_GET(PTB9))//行中断
	{
		PORT_IRQ_CLEAN(PTB9);
		//GPIO_Reverse(PTE29);
		if (img_flag == IMG_GATHER)
		{
			if (Line_Count >= Camera_Height)
			{
				img_flag = IMG_FINISH;
				PORT_IRQ_DIS(PORTA_IRQ);
				return;
			}
			EDMA_PORT_StartOnce(DMA_CH4, (uint32)(&Image_Buff[Line_Count][0]), Camera_Width);
			Line_Count++;
		}
		return;
	}
}
#endif

/*
 * @date   2019年01月31日备份
 * @brief  测试主程序#1
 * @mode   GPIO、PIT、DWT、LCD、PORT
 * @done   LED闪烁测试（分别以延时、PIT定时中断、外部中断实现）
 */
#ifdef main_1
 //void main()
 //{
 //  BOARD_InitBootClocks();
 //  BOARD_InitBootPins();
 //  
 //  DisableInterrupts;
 //  
 //  LCD_Init(); 
 //  GPIO_Init(PTC0 , GPO, 0);
 //  GPIO_Init(PTA17, GPO, 0);
 //  GPIO_Init(PTE29 ,GPO, 0);
 //  LCD_Show_Number(20, 1, 123);
 //  LCD_P6x8Str(20, 2, "abc");
 //  PITn_Init(PIT0, 500);
 //  EXTI_Init(PTA11, rising, pull_up);
 //  
 //  EnableInterrupts;
 //  
 //  while (1)
 //  {
 //    PTA17_OUT = 0;
 //    DELAY_MS(500);
 //    PTA17_OUT = 1;
 //    DELAY_MS(500);
 //  }
 //}
 //
 //void PIT0_IRQHandler()
 //{
 //  PIT_Flag_Clear(PIT0);
 //  GPIO_Reverse(PTC0);
 //}
 //
 //void PORTA_IRQHandler()
 //{
 //  if(PORT_IRQ_GET(PTA11))
 //  {
 //    GPIO_Reverse(PTE29);
 //    PORT_IRQ_CLEAN(PTA11);
 //  }
 //}
#endif