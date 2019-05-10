/* 
 * @name   Sora_lib
 * @group  Profiterole
 * @author Afisora
 * @brief  DMA Driver
 */
   
#include "sora_system.h"


/**
 * @name        EDMA_Transport_Init
 * @brief       EDMA数据传输初始化
 * @clock       System (CPU) clock
 * @param CHn   DMA通道号
 * @param SADDR 源地址
 * @param byten 传输数据大小
 * @return      无
 * @example     EDMA_Transport_Init(DMA_CH5, (uint32)img_buff, DMA_BYTE32);
 * @note        每次副循环都要手动触发
 */
void EDMA_Transport_Init(DMA_CHn CHn, uint32 SADDR, DMA_BYTEn byten)
{
  uint8 BYTEs;

  switch(byten)
  {
   case DMA_BYTE1 :BYTEs = 1 ;break;
   case DMA_BYTE2 :BYTEs = 2 ;break;
   case DMA_BYTE4 :BYTEs = 4 ;break;
   case DMA_BYTE16:BYTEs = 16;break;
   case DMA_BYTE32:BYTEs = 32;break;
   default:return;break;
  }
  
  //开启时钟
  CLOCK_EnableClock(kCLOCK_Dma0);
  
  //TCD源地址设置
  DMA0->TCD[CHn].SADDR     = DMA_SADDR_SADDR(SADDR);
  //TCD源地址偏移设置：无偏移
  DMA0->TCD[CHn].SOFF      = DMA_SOFF_SOFF(0);
  //TCD目标地址偏移设置：按传输数据大小偏移
  DMA0->TCD[CHn].DOFF      = DMA_DOFF_DOFF(BYTEs);
  //TCD最终源地址调整：无调整
  DMA0->TCD[CHn].SLAST     = DMA_SLAST_SLAST(0);
  //TCD最终目的地址调整/分散聚集地址：无调整
  DMA0->TCD[CHn].DLAST_SGA = DMA_DLAST_SGA_DLASTSGA(0);
  //TCD传输属性
  DMA0->TCD[CHn].ATTR  = (0
						  | DMA_ATTR_DSIZE(byten) //目标数据传输大小
						  | DMA_ATTR_DMOD (0)     //禁用目标地址取模
						  | DMA_ATTR_SSIZE(byten) //源数据传输大小
						  | DMA_ATTR_SMOD (0)     //禁用源地址取模
						 );
  //TCD有符号副循环偏移TCD：设置每次传输字节数，不启用副循环源地址偏移和目的地址偏移
  DMA0->TCD[CHn].NBYTES_MLNO = DMA_NBYTES_MLOFFYES_NBYTES(BYTEs);
  //TCD控制与状态寄存器TCD
  DMA0->TCD[CHn].CSR = (0
						//| DMA_CSR_DREQ(1)     //对应通道DMA硬件请求使能EQR在主循环传输后自动清0（屏蔽硬件DMA请求）
						| DMA_CSR_INTMAJOR(1) //主循环传输结束，即CITER为0时使能中断
					   );
  //DMA中断开启
  //DMA_IRQ_EN(CHn);                                
}

/**
 * @name        EDMA_Transport_StartOnce
 * @brief       EDMA对数据进行一次传输
 * @clock       System (CPU) clock
 * @param CHn   DMA通道号
 * @param DADDR 目的地址
 * @param count 主循环次数
 * @return      无
 * @example     EDMA_Transport_StartOnce(DMA_CH5, (uint32)img_fifo, Camera_Width * Camera_Height / 32);
 * @note        每次副循环都要手动触发
 */
void EDMA_Transport_StartOnce(DMA_CHn CHn, uint32 DADDR, uint32 count) 
{ 
  //TCD目的地址设置
  DMA0->TCD[CHn].DADDR         = DMA_DADDR_DADDR(DADDR);
  //TCD当前副循环链接，主循环计数
  DMA0->TCD[CHn].CITER_ELINKNO = (0 
								  | DMA_CITER_ELINKNO_ELINK(0)     //禁用副循环链接
								  | DMA_CITER_ELINKNO_CITER(count) //当前主循环计数值
								 );
  //TCD起始副循环链接，主循环计数
  DMA0->TCD[CHn].BITER_ELINKNO = (0 
								  | DMA_BITER_ELINKNO_ELINK(0)     //禁用副循环链接
								  | DMA_BITER_ELINKNO_BITER(count) //初始主循环计数值
								 );
  //使能DMA硬件请求
  DMA0->TCD[CHn].CSR |= DMA_CSR_START(1);
} 


/**
 * @name        EDMA_PORT_Init
 * @brief       EDMA对PORT采集初始化
 * @clock       System (CPU) clock
 * @param CHn   DMA通道号 必须用DMA Group0
 * @param SADDR 源地址
 * @param DADDR 目的地址
 * @param byten 传输数据大小
 * @param ptxn  触发源
 * @param count 主循环次数
 * @param ecfg  触发设置
 * @param pcfg  拉设置
 * @return      无
 * @example     EDMA_PORT_Init(DMA_CH4, (void*)&PTD_BYTE0_IN, (void*)Image_Buff, DMA_BYTE1, PTA11, Camera_Width, falling_DMA, pull_up);
 */
void EDMA_PORT_Init(DMA_CHn CHn, void *SADDR, void *DADDR, DMA_BYTEn byten, PTXn_e ptxn, uint32 count, exti_cfg ecfg, pull_cfg pcfg)
{
  uint8 BYTEs;
  uint8 ptx0, ptxm, n;

  switch(byten)
  {
   case DMA_BYTE1 :BYTEs = 1 ;break;
   case DMA_BYTE2 :BYTEs = 2 ;break;
   case DMA_BYTE4 :BYTEs = 4 ;break;
   case DMA_BYTE16:BYTEs = 16;break;
   case DMA_BYTE32:BYTEs = 32;break;
   default:return;break;
  }
  
  //开启时钟
  CLOCK_EnableClock(kCLOCK_Dmamux0);
  CLOCK_EnableClock(kCLOCK_Dma0);
  
  //TCD源地址设置
  DMA0->TCD[CHn].SADDR     = DMA_SADDR_SADDR(SADDR);
  //TCD目的地址设置
  DMA0->TCD[CHn].DADDR     = DMA_DADDR_DADDR(DADDR);
  //TCD源地址偏移设置：无偏移
  DMA0->TCD[CHn].SOFF      = DMA_SOFF_SOFF(0);
  //TCD目标地址偏移设置：按传输数据大小偏移
  DMA0->TCD[CHn].DOFF      = DMA_DOFF_DOFF(BYTEs);
  //TCD最终源地址调整：无调整
  DMA0->TCD[CHn].SLAST     = DMA_SLAST_SLAST(0);
  //TCD最终目的地址调整/分散聚集地址：无调整
  DMA0->TCD[CHn].DLAST_SGA = DMA_DLAST_SGA_DLASTSGA(0);
  //TCD传输属性
  DMA0->TCD[CHn].ATTR  = (0
						  | DMA_ATTR_DSIZE(byten) //目标数据传输大小
						  | DMA_ATTR_DMOD (0)     //禁用目标地址取模
						  | DMA_ATTR_SSIZE(byten) //源数据传输大小
						  | DMA_ATTR_SMOD (0)     //禁用源地址取模
						 );
  //TCD当前副循环链接，主循环计数
  DMA0->TCD[CHn].CITER_ELINKNO = (0 
								  | DMA_CITER_ELINKNO_ELINK(0)     //禁用副循环链接
								  | DMA_CITER_ELINKNO_CITER(count) //当前主循环计数值
								 );
  //TCD起始副循环链接，主循环计数
  DMA0->TCD[CHn].BITER_ELINKNO = (0 
								  | DMA_BITER_ELINKNO_ELINK(0)     //禁用副循环链接
								  | DMA_BITER_ELINKNO_BITER(count) //初始主循环计数值
								 );
  //TCD有符号副循环偏移TCD：设置每次传输字节数，不启用副循环源地址偏移和目的地址偏移
  DMA0->TCD[CHn].NBYTES_MLNO = DMA_NBYTES_MLOFFYES_NBYTES(BYTEs);
  //TCD控制与状态寄存器TCD
  DMA0->TCD[CHn].CSR = (0
						| DMA_CSR_DREQ(1)     //对应通道DMA硬件请求使能EQR在主循环传输后自动清0（屏蔽硬件DMA请求）
						| DMA_CSR_INTMAJOR(1) //主循环传输结束，即CITER为0时使能中断
					   );
  //DMAMUX配置：设置触发源为PORTX
  DMAMUX->CHCFG[CHn] = (0
						| DMAMUX_CHCFG_ENBL(1)
						| DMAMUX_CHCFG_SOURCE(PTX(ptxn) + DMA_Port_A)
					   );
  //PORT设置DMA触发
  EXTI_Init(ptxn, ecfg, pcfg);
  //输入PORT初始化
  //SADDR 实际上就是 GPIO的 输入寄存器 PDIR 的地址
  //GPIOA、GPIOB、GPIOC、GPIOD、GPIOE 的地址 分别是 0x400FF000u 、0x400FF040u 、0x400FF080u、 0x400FF0C0u、0x400FF100u
  //sizeof(GPIO_MemMap) = 0x18
  //每个GPIO地址 &0x1C0 后，得到 0x000 , 0x040 , 0x080 ,0x0C0 ,0x100
  //再 /0x40 后得到 0 、 1 、 2、 3、4 ，刚好就是 PTA、PTB、PTC 、PTD 、PTD 、PTE
  ptx0 = ((((uint32)SADDR) & 0x1C0) / 0x40) * 32;
  
  //每个GPIO 对应的寄存器地址， &0x 3F 后得到的值都是相同的。
  //PTA_B0_IN 即 GPIOA 的 输入寄存器 PDIR 的 地址
  //(SADDR & 0x3f - PTA_B0_IN & 0x3f) 等效于 (SADDR - PTA_B0_IN) & 0x3f
  //假设需要采集的位 为 0~7、8~15、16~23、24~31 ，则 上面式子对应的值 为 0、1、2、3
  //刚好是  0~7、8~15、16~23、24~31 位的地址偏移，再 * 8 就变成 0、8、16、24
  n = (uint8)(((uint32)SADDR - ((uint32)(&PTA_BYTE0_IN))) & 0x3f) * 8;  //最小的引脚号
  
  ptx0 += n;
  ptxm = ptx0 + (BYTEs * 8) - 1;  //最大的引脚号
  
  while(ptx0 <= ptxm)
  {
	GPIO_Init((PTXn_e)ptx0, GPI, 0);       //设置为输入
	PORT_Init((PTXn_e)ptx0, 1, pull_down); //输入源应该下拉，默认读取到的是0
	ptx0++;
  }
  //使能DMA硬件请求
  //DMA_EN(CHn);
  //DMA中断开启
  //DMA_IRQ_EN(CHn);                                
}

/**
 * @name        EDMA_PORT_StartOnce
 * @brief       EDMA对PORT进行一次采集
 * @clock       System (CPU) clock
 * @param CHn   DMA通道号
 * @param DADDR 目的地址
 * @param count 主循环次数
 * @return      无
 * @example     EDMA_PORT_StartOnce(DMA_CH4,(uint32)(&Image_Buff[Line_Count][0]),Camera_Width);
 */
void EDMA_PORT_StartOnce(DMA_CHn CHn, uint32 DADDR, uint32 count) 
{ 
  //TCD目的地址设置
  DMA0->TCD[CHn].DADDR         = DMA_DADDR_DADDR(DADDR);
  //TCD当前副循环链接，主循环计数
  DMA0->TCD[CHn].CITER_ELINKNO = (0 
								  | DMA_CITER_ELINKNO_ELINK(0)     //禁用副循环链接
								  | DMA_CITER_ELINKNO_CITER(count) //当前主循环计数值
								 );
  //TCD起始副循环链接，主循环计数
  DMA0->TCD[CHn].BITER_ELINKNO = (0 
								  | DMA_BITER_ELINKNO_ELINK(0)     //禁用副循环链接
								  | DMA_BITER_ELINKNO_BITER(count) //初始主循环计数值
								 );
  //使能DMA硬件请求
  DMA_EN(CHn);
} 

/**
 * @name        EDMA_UART_RX_Init
 * @brief       EDMA对UART接收初始化
 * @clock       System (CPU) clock
 * @param CHn   DMA通道号 UART0-UART1必须用DMA Group0，UART2-UART5必须用DMA Group1
 * @param uartn UART源
 * @return      无
 * @example     EDMA_UART_RX_Init(DMA_CH20, UART_2);
 */
void EDMA_UART_RX_Init(DMA_CHn CHn, UARTn_e uartn)
{
  DMA_BYTEn byten = DMA_BYTE1;
  uint8 BYTEs = 1;
  
  //开启时钟
  CLOCK_EnableClock(kCLOCK_Dmamux0);
  CLOCK_EnableClock(kCLOCK_Dma0);
  
  //TCD源地址设置
  DMA0->TCD[CHn].SADDR     = DMA_SADDR_SADDR((uint32)&(UARTX[uartn]->D));
  //TCD源地址偏移设置：无偏移
  DMA0->TCD[CHn].SOFF      = DMA_SOFF_SOFF(0);
  //TCD目标地址偏移设置：按传输数据大小偏移
  DMA0->TCD[CHn].DOFF      = DMA_DOFF_DOFF(BYTEs);
  //TCD最终源地址调整：无调整
  DMA0->TCD[CHn].SLAST     = DMA_SLAST_SLAST(0);
  //TCD传输属性
  DMA0->TCD[CHn].ATTR  = (0
						  | DMA_ATTR_DSIZE(byten) //目标数据传输大小
						  | DMA_ATTR_DMOD (0)     //禁用目标地址取模
						  | DMA_ATTR_SSIZE(byten) //源数据传输大小
						  | DMA_ATTR_SMOD (0)     //禁用源地址取模
						 );
  //TCD有符号副循环偏移TCD：设置每次传输字节数，不启用副循环源地址偏移和目的地址偏移
  DMA0->TCD[CHn].NBYTES_MLNO = DMA_NBYTES_MLOFFYES_NBYTES(BYTEs);
  //TCD控制与状态寄存器TCD
  DMA0->TCD[CHn].CSR = (0
						| DMA_CSR_DREQ(1)     //对应通道DMA硬件请求使能EQR在主循环传输后自动清0（屏蔽硬件DMA请求）
						| DMA_CSR_INTMAJOR(0) //主循环传输结束，即CITER为0时使能中断
					   );
  //DMAMUX配置：设置触发源为PORTX
  switch(uartn)
  {
   case UART_0:
	DMAMUX->CHCFG[CHn] = (0
						  | DMAMUX_CHCFG_ENBL(1)
						  | DMAMUX_CHCFG_SOURCE(DMA_UART0_Rx)
						 );
	break;
   case UART_1:
	DMAMUX->CHCFG[CHn] = (0
						  | DMAMUX_CHCFG_ENBL(1)
						  | DMAMUX_CHCFG_SOURCE(DMA_UART1_Rx)
						 );
	break;
   case UART_2:
	DMAMUX->CHCFG[CHn] = (0
						  | DMAMUX_CHCFG_ENBL(1)
						  | DMAMUX_CHCFG_SOURCE(DMA_UART2_Rx)
						 );
	break;
   case UART_3:
	DMAMUX->CHCFG[CHn] = (0
						  | DMAMUX_CHCFG_ENBL(1)
						  | DMAMUX_CHCFG_SOURCE(DMA_UART3_Rx)
						 );
	break;
   case UART_4:
	DMAMUX->CHCFG[CHn] = (0
						  | DMAMUX_CHCFG_ENBL(1)
						  | DMAMUX_CHCFG_SOURCE(DMA_UART4_Rx)
						 );
	break;
   case UART_5:
	DMAMUX->CHCFG[CHn] = (0
						  | DMAMUX_CHCFG_ENBL(1)
						  | DMAMUX_CHCFG_SOURCE(DMA_UART5_Rx)
						 );
	break;
   default:
	break;
  }
  
  //使能DMA硬件请求
  //DMA_EN(CHn);
  //DMA中断开启
  //DMA_IRQ_EN(CHn);                                
}

/**
 * @name        EDMA_UART_RX_Stop
 * @brief       停止EDMA对UART接收缓存区传输
 * @clock       System (CPU) clock
 * @param CHn   DMA通道号
 * @return      当前地址与起始地址偏差（可用于数据量判断）
 * @example     buffrx = EDMA_UART_RX_Stop(DMA_CH20, (uint32)str);
 */
uint32 EDMA_UART_RX_Stop(DMA_CHn CHn, uint32 address) 
{
  DMA_DIS(CHn);
  uint32 DADDR = DMA0->TCD[CHn].DADDR;
  //计算上次传输数据量
  return DADDR - address;
}

/**
 * @name        EDMA_UART_RX_Start
 * @brief       EDMA对UART接收缓存区启动一次传输
 * @clock       System (CPU) clock
 * @param CHn   DMA通道号
 * @param DADDR 目的地址
 * @param count 主循环次数
 * @return      无
 * @example     EDMA_UART_RX_Start(DMA_CH20, (uint32)str, 10);
 */
void EDMA_UART_RX_Start(DMA_CHn CHn, uint32 DADDR, uint32 count) 
{ 
  //TCD目的地址设置
  DMA0->TCD[CHn].DADDR         = DMA_DADDR_DADDR(DADDR);
  //TCD最终目的地址调整/分散聚集地址：
  DMA0->TCD[CHn].DLAST_SGA     = DMA_DLAST_SGA_DLASTSGA(0);
  //TCD当前副循环链接，主循环计数
  DMA0->TCD[CHn].CITER_ELINKNO = (0 
								  | DMA_CITER_ELINKNO_ELINK(0)     //禁用副循环链接
								  | DMA_CITER_ELINKNO_CITER(count) //当前主循环计数值
								 );
  //TCD起始副循环链接，主循环计数
  DMA0->TCD[CHn].BITER_ELINKNO = (0 
								  | DMA_BITER_ELINKNO_ELINK(0)     //禁用副循环链接
								  | DMA_BITER_ELINKNO_BITER(count) //初始主循环计数值
								 );
  //使能DMA硬件请求
  DMA_EN(CHn);
} 

/**
 * @name        EDMA_FlexPWM_Init
 * @brief       初始化FlexPWM的DMA服务
 * @clock       System (CPU) clock
 * @param ch	PWM通道号
 * @param CHn	DMA通道号
 * @param SADDR DMA源地址
 * @return      无
 * @example     EDMA_FlexPWM_Init(PWM0_SM1_CHA, DMA_CH7, (uint32_t)ValueL);
 * @note		PWM的对齐模式只可选择无符号边缘对齐
 */
void EDMA_FlexPWM_Init(PWM_CHn ch, DMA_CHn CHn, uint32_t SADDR)
{
	uint8_t		BYTEs = 2;
	DMA_BYTEn	byten = DMA_BYTE2;
	PWM_Type*	base;
	PWM_SMn		subModule = (PWM_SMn)((ch % 8U) / 2U);
	uint32_t	DADDR;

	//初始配置读取
	if (ch / 8U == 0) base = PWM0;
	else base = PWM1;

	if (ch % 2U == 0)
	{
		DADDR = (uint32)& base->SM[subModule].VAL3;
	}
	else
	{
		DADDR = (uint32)& base->SM[subModule].VAL5;
	}

	//开启时钟
	CLOCK_EnableClock(kCLOCK_Dmamux0);
	CLOCK_EnableClock(kCLOCK_Dma0);

	//TCD源地址设置
	DMA0->TCD[CHn].SADDR = DMA_SADDR_SADDR(SADDR);
	//TCD目的地址设置
	DMA0->TCD[CHn].DADDR = DMA_DADDR_DADDR(DADDR);
	//TCD源地址偏移设置：按传输数据大小偏移
	DMA0->TCD[CHn].SOFF = DMA_SOFF_SOFF(BYTEs);
	//TCD目标地址偏移设置：按传输数据大小偏移
	DMA0->TCD[CHn].DOFF = DMA_DOFF_DOFF(0);
	//TCD最终目的地址调整/分散聚集地址：调整至初始位置
	DMA0->TCD[CHn].DLAST_SGA = DMA_DLAST_SGA_DLASTSGA(0);
	//TCD传输属性
	DMA0->TCD[CHn].ATTR = (0
		| DMA_ATTR_DSIZE(byten)	//目标数据传输大小
		| DMA_ATTR_DMOD(0)		//禁用目标地址取模
		| DMA_ATTR_SSIZE(byten)	//源数据传输大小
		| DMA_ATTR_SMOD(0)		//禁用源地址取模
		);

	//TCD有符号副循环偏移TCD：设置每次传输字节数，不启用副循环源地址偏移和目的地址偏移
	DMA0->TCD[CHn].NBYTES_MLNO = DMA_NBYTES_MLOFFYES_NBYTES(BYTEs);

	//TCD控制与状态寄存器TCD
	DMA0->TCD[CHn].CSR = (0
		| DMA_CSR_MAJORLINKCH(0)	//主循环链接通道
		| DMA_CSR_MAJORELINK(0)		//禁用主循环链接
		| DMA_CSR_DREQ(1)			//对应通道DMA硬件请求使能EQR在主循环传输后自动清0（屏蔽硬件DMA请求）
		| DMA_CSR_INTMAJOR(1)		//主循环传输结束，即CITER为0时使能中断
		);

	//DMAMUX配置：设置触发源为flexPWM_WR
	if (base == PWM0)
	{
		DMAMUX->CHCFG[CHn] = (0
			| DMAMUX_CHCFG_ENBL(1)
			| DMAMUX_CHCFG_SOURCE(subModule + DMA_flexPWM0_WR0)
			);
	}
	else
	{
		DMAMUX->CHCFG[CHn] = (0
			| DMAMUX_CHCFG_ENBL(1)
			| DMAMUX_CHCFG_SOURCE(subModule + DMA_flexPWM1_WR0)
			);
	}

	//DMA中断开启
	DMA_IRQ_EN(CHn);                                
}


/**
 * @name        EDMA_FlexPWM_StartOnce
 * @brief		启动一次PWM传输
 * @clock       System (CPU) clock
 * @param CHn	DMA通道号
 * @param count	数据传输次数
 * @return      无
 * @example		EDMA_FlexPWM_StartOnce(DMA_CH7, 10);
 */
void EDMA_FlexPWM_StartOnce(DMA_CHn CHn, uint32 count)
{

	//TCD最终源地址调整：调整至初始位置
	DMA0->TCD[CHn].SLAST = DMA_SLAST_SLAST(-count * 2);
	//TCD当前副循环链接，主循环计数
	DMA0->TCD[CHn].CITER_ELINKNO = (0
		| DMA_CITER_ELINKNO_ELINK(0)     //禁用副循环链接
		| DMA_CITER_ELINKNO_CITER(count) //当前主循环计数值
		);
	//TCD起始副循环链接，主循环计数
	DMA0->TCD[CHn].BITER_ELINKNO = (0
		| DMA_BITER_ELINKNO_ELINK(0)     //禁用副循环链接
		| DMA_BITER_ELINKNO_BITER(count) //初始主循环计数值
		);
	//使能DMA硬件请求
	DMA_EN(CHn);
}




