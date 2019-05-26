/* 
 * @name   Sora_lib
 * @group  Profiterole
 * @author Afisora
 * @brief  UART Driver
 */

#include "sora_system.h"

UART_MemMapPtr UARTX[6] = {UART0, UART1, UART2, UART3, UART4, UART5};

/**
 * @name        UART_Com_Init
 * @brief       UART端口初始化
 * @clock       Fast Peripheral clock
 * @param uartn UART通道号
 * @param baud  波特率
 * @return      无
 * @example     UART_Com_Init(UART_0, 115200);
 */
void UART_Com_Init(UARTn_e uartn, uint32 baud)
{
  uint16 SBR, BRFA;
  uint8 temp;
  
  switch(uartn)
  {
   case UART_0:
	CLOCK_EnableClock(kCLOCK_Uart0);
	
	if(UART0_RX == PTA1)
	{           
	  PORT_Init(UART0_RX, 2, pull_disable);
	}
	else if(UART0_RX == PTA15)
	{
	  PORT_Init(UART0_RX, 3, pull_disable);
	}
	else if(UART0_RX == PTB16)
	{
	  PORT_Init(UART0_RX, 3, pull_disable);
	}
	else if(UART0_RX == PTC6)
	{
	  PORT_Init(UART0_RX, 6, pull_disable);
	}
	else if(UART0_RX == PTD6)
	{
	  PORT_Init(UART0_RX, 3, pull_disable);
	}
	else if(UART0_RX == PTE21)
	{
	  PORT_Init(UART0_RX, 4, pull_disable);
	}
	else if(UART0_RX == PTB1)
	{
	  PORT_Init(UART0_RX, 7, pull_disable);
	}
	else
	{
	  break;
	}
	
	if(UART0_TX == PTA2)
	{
	  PORT_Init(UART0_TX, 2, pull_disable);  
	}
	else if(UART0_TX == PTA14)
	{
	  PORT_Init(UART0_TX, 3, pull_disable);      
	}
	else if(UART0_TX == PTB17)
	{
	  PORT_Init(UART0_TX, 3, pull_disable);       
	}
	else if(UART0_TX == PTD7)
	{
	  PORT_Init(UART0_TX, 3, pull_disable);       
	}
	else if(UART0_TX == PTC7)
	{
	  PORT_Init(UART0_TX, 5, pull_disable);       
	}
	else if(UART0_TX == PTE20)
	{
	  PORT_Init(UART0_TX, 4, pull_disable);       
	}
	else if(UART0_TX == PTB0)
	{
	  PORT_Init(UART0_TX, 7, pull_disable);       
	}
	else
	{
	  break;
	}
	
	break;
	
   case UART_1:
	CLOCK_EnableClock(kCLOCK_Uart1);
	PORT_Init(UART1_RX, 3, pull_disable);
	PORT_Init(UART1_TX, 3, pull_disable);
	break;
	
   case UART_2:
	CLOCK_EnableClock(kCLOCK_Uart2);
	PORT_Init(UART2_RX, 3, pull_disable);
	PORT_Init(UART2_TX, 3, pull_disable);
	break;
	
   case UART_3:
	CLOCK_EnableClock(kCLOCK_Uart3);
	PORT_Init(UART3_RX, 3, pull_disable);
	PORT_Init(UART3_TX, 3, pull_disable);
	break;
	
   case UART_4:
	
	CLOCK_EnableClock(kCLOCK_Uart4);
	
	if(UART4_RX == PTC14)
	{
	  PORT_Init(UART4_RX, 9, pull_disable);
	}
	else if(UART4_RX == PTE25)
	{
	  PORT_Init(UART4_RX, 8, pull_disable);
	}
	else
	{
	  break;
	}
	
	if(UART4_TX == PTC15)
	{
	  PORT_Init(UART4_TX, 9, pull_disable);
	}
	else if(UART4_TX == PTE24)
	{
	  PORT_Init(UART4_TX, 8, pull_disable);
	}
	else
	{
	  break;
	}
	
	break;
	
   case UART_5:
	CLOCK_EnableClock(kCLOCK_Uart5);
	PORT_Init(UART5_RX, 3, pull_disable);
	PORT_Init(UART5_TX, 3, pull_disable);
	break;
	
   default:
	break;
  }
  //关闭UART发射器和接收器               
  UARTX[uartn]->C2 &= ~(0
						| UART_C2_TE(1) 
						| UART_C2_RE(1) 
					   );  
  //奇偶校验、数据格式配置
  UARTX[uartn]->C1 |= (0
					   | UART_C1_M(0)   //0为8位，1为9位
					   | UART_C1_PE(0)  //奇偶校验使能
					   | UART_C1_PT(0)  //0为偶校验，1为奇校验
					  );
  //计算并配置波特率
  SBR = Fast_Peripheral_Clock / (baud * 16);
  BRFA = (uint16)(((float)Fast_Peripheral_Clock / baud - 16 * SBR) * 2);
  //写入波特率SBR
  temp = UARTX[uartn]->BDH & (~UART_BDH_SBR_MASK);
  UARTX[uartn]->BDH = temp | UART_BDH_SBR(SBR >> 8);
  UARTX[uartn]->BDL = UART_BDL_SBR(SBR);
  //写入波特率修正BRFA
  temp = UARTX[uartn]->C4 & (~UART_C4_BRFA_MASK);
  UARTX[uartn]->C4 = temp | UART_C4_BRFA(BRFA);
  //启用FIFO
  UARTX[uartn]->PFIFO |= (0
						  | UART_PFIFO_TXFE(1)
						  | UART_PFIFO_RXFE(1)
						 );
  //清空FIFO
  UARTX[uartn]->CFIFO |= UART_CFIFO_TXFLUSH(1);
  UARTX[uartn]->CFIFO |= UART_CFIFO_RXFLUSH(1);
  //开启UART发射器和接收器               
  UARTX[uartn]->C2 |= (0
					   | UART_C2_TE(1) 
					   | UART_C2_RE(1) 
					  );
}

/**
 * @name        UART_Put_Char
 * @brief       UART发送1字节
 * @clock       Fast Peripheral clock
 * @param uartn UART通道号
 * @param ch    发送数据
 * @return      无
 * @example     UART_Put_Char(UART_0, 'A');
 */
inline void UART_Put_Char(UARTn_e uartn, uint8 ch)
{
  while(!((UARTX[uartn]->S1) & UART_S1_TDRE_MASK));
  UARTX[uartn]->D = (uint8)ch;
}

/**
 * @name        UART_Put_Buff
 * @brief       UART发送数据包
 * @clock       Fast Peripheral clock
 * @param uartn UART通道号
 * @param buff  数据包数组
 * @param len   数据包长度
 * @return      无
 * @example     UART_Put_Buff(UART_0, buff, 4);
 */
void UART_Put_Buff(UARTn_e uartn, const uint8 *buff, uint32 len)
{
  while(len--)
  {
	UART_Put_Char(uartn, *buff);
	buff++;
  }
}

/**
 * @name        UART_Put_Str
 * @brief       UART发送字符串
 * @clock       Fast Peripheral clock
 * @param uartn UART通道号
 * @param str   发送数据
 * @return      无
 * @example     UART_Put_Str(UART_0, "123456789");
 */
void UART_Put_Str(UARTn_e uartn, const uint8 *str)
{
  while(*str)
  {
	UART_Put_Char(uartn, *str++);
  }
}

/**
 * @name        UART_Get_Char
 * @brief       UART接收1字节（阻塞）
 * @clock       Fast Peripheral clock
 * @param uartn UART通道号
 * @return      接收到的字节
 * @example     buff = UART_Get_Char(UART_0);
 */
inline char UART_Get_Char(UARTn_e uartn)
{
  while(!((UARTX[uartn]->S1) & UART_S1_RDRF_MASK));  
  return(UARTX[uartn]->D);
}

/**
 * @name        UART_Query_Char
 * @brief       UART接收1字节（查询）
 * @clock       Fast Peripheral clock
 * @param uartn UART通道号
 * @param ch    保存字节的地址
 * @return      是否接收到数据，1为成功，0为失败
 * @example     buff = UART_Query_Char(UART_0, data);
 */
inline char UART_Query_Char(UARTn_e uartn, uint8 *ch)
{
  if(UARTX[uartn]->RCFIFO)     
  {
	*ch = UARTX[uartn]->D;      
	return  1;                            
  }
  *ch = 0;                                    
  return 0;                            
}

/**
 * !BUG           无法正常工作，请勿使用，接收数据尽量用中断处理
 * @name          UART_Query_Str
 * @brief         UART接收字符串（查询）
 * @clock         Fast Peripheral clock
 * @param uartn   UART通道号
 * @param str     保存字符串的地址
 * @param max_len 字符串最大长度
 * @return        查询接收的次数
 * @example       buff = UART_Query_Str(UART_0, str, 10);
 */
uint32 UART_Query_Str(UARTn_e uartn, uint8 *str, uint32 max_len)
{
  uint32 i = 0;
  while(UART_Query_Char(uartn, str + i))
  {
	if(*(str + i) == NULL)    
	{
	  return i;
	}
	i++;
	if(i >= max_len)         
	{
	  return i;
	}
  };
  return i;
}

/**
 * @name           UART_IRQ_EN
 * @brief          UART接收中断使能
 * @clock          Fast Peripheral clock
 * @param uartn    UART通道号
 * @param uart_irq UART中断类型
 * @return         无
 * @example        UART_IRQ_EN(UART_0,UART_RX);
 */
void UART_IRQ_EN(UARTn_e uartn, uart_irq cfg)
{  
  //使能中断请求
  switch(cfg)
  {
   case UART_TX_IRQ:      
	UARTX[uartn]->C2 |= UART_C2_TIE(1); 
	UARTX[uartn]->C5 |= UART_C5_TDMAS(0); 
	break;
   case UART_TX_DMA:      
	UARTX[uartn]->C2 |= UART_C2_TIE(1); 
	UARTX[uartn]->C5 |= UART_C5_TDMAS(1); 
	break;
   case UART_COMPLETE:
	UARTX[uartn]->C2 |= UART_C2_TCIE(1);
	break;
   case UART_RX_IRQ:      
	UARTX[uartn]->C2 |= UART_C2_RIE(1); 
	UARTX[uartn]->C5 |= UART_C5_RDMAS(0); 
	break;
   case UART_RX_DMA:      
	UARTX[uartn]->C2 |= UART_C2_RIE(1); 
	UARTX[uartn]->C5 |= UART_C5_RDMAS(1); 
	break;
   case UART_IDLE:    
	UARTX[uartn]->C1 |= UART_C1_ILT(1);
	UARTX[uartn]->C2 |= UART_C2_ILIE(1); 
	break;
   case UART_OVERRUN_ERROR:
	UARTX[uartn]->C3 |= UART_C3_ORIE(1);
	break;
   case UART_NOISE_ERROR:
	UARTX[uartn]->C3 |= UART_C3_NEIE(1);
	break;
   case UART_FRAMING_ERROR:
	UARTX[uartn]->C3 |= UART_C3_FEIE(1);
	break;
   case UART_PARITY_ERROR:
	UARTX[uartn]->C3 |= UART_C3_PEIE(1);
	break;
   case UART_FIFO_RX_OVERFLOW:
	UARTX[uartn]->CFIFO |= UART_CFIFO_RXOFE(1);
	break; 
   case UART_FIFO_TX_OVERFLOW:
	UARTX[uartn]->CFIFO |= UART_CFIFO_TXOFE(1);
	break; 
   case UART_FIFO_RX_UNDERFLOW:
	UARTX[uartn]->CFIFO |= UART_CFIFO_RXUFE(1);
	break; 
   default:
	break;
  }
  //使能中断
  switch(uartn)
  {
   case UART_0:
	NVIC_EnableIRQ(UART0_RX_TX_IRQn);
	break;
   case UART_1:      
	NVIC_EnableIRQ(UART1_RX_TX_IRQn);
	break;
   case UART_2:        
	NVIC_EnableIRQ(UART2_RX_TX_IRQn);
	break;
   case UART_3:       
	NVIC_EnableIRQ(UART3_RX_TX_IRQn);
	break;
   case UART_4:    
	NVIC_EnableIRQ(UART4_RX_TX_IRQn);
	break;
   case UART_5:   
	NVIC_EnableIRQ(UART5_RX_TX_IRQn);
	break;
   default:
	break;
  }
}

/**
 * @name        UART_IRQ_DIS_ALL
 * @brief       UART接收中断关闭
 * @clock       Fast Peripheral clock
 * @param uartn UART通道号
 * @return      无
 * @example     UART_IRQ_DIS_ALL(UART_0);
 */
void UART_IRQ_DIS_ALL(UARTn_e uartn)
{ 
  //禁用所有中断请求
  UARTX[uartn]->C2    &= ~UART_C2_TIE(1); 
  UARTX[uartn]->C2    &= ~UART_C2_TCIE(1);
  UARTX[uartn]->C2    &= ~UART_C2_RIE(1); 
  UARTX[uartn]->C2    &= ~UART_C2_ILIE(1);
  UARTX[uartn]->C3    &= ~UART_C3_ORIE(1);
  UARTX[uartn]->C3    &= ~UART_C3_NEIE(1);
  UARTX[uartn]->C3    &= ~UART_C3_FEIE(1);
  UARTX[uartn]->C3    &= ~UART_C3_PEIE(1);
  UARTX[uartn]->CFIFO &= ~UART_CFIFO_RXOFE(1);
  UARTX[uartn]->CFIFO &= ~UART_CFIFO_TXOFE(1);
  UARTX[uartn]->CFIFO &= ~UART_CFIFO_RXUFE(1);
  //关闭中断
  switch(uartn)
  {
   case UART_0:
	NVIC_DisableIRQ(UART0_RX_TX_IRQn);
	break;
   case UART_1:      
	NVIC_DisableIRQ(UART1_RX_TX_IRQn);
	break;
   case UART_2:        
	NVIC_DisableIRQ(UART2_RX_TX_IRQn);
	break;
   case UART_3:       
	NVIC_DisableIRQ(UART3_RX_TX_IRQn);
	break;
   case UART_4:    
	NVIC_DisableIRQ(UART4_RX_TX_IRQn);
	break;
   case UART_5:   
	NVIC_DisableIRQ(UART5_RX_TX_IRQn);
	break;
   default:
	break;
  }
}

/**
 * @name           UART_IRQ_DIS
 * @brief          UART关闭中断请求
 * @clock          Fast Peripheral clock
 * @param uartn    UART通道号
 * @param uart_irq UART请求类型
 * @return         无
 * @example        UART_IRQ_DIS(PC_UART, UART_TX_DMA);
 */
void UART_IRQ_DIS(UARTn_e uartn, uart_irq cfg)
{
	switch (cfg)
	{
	case UART_TX_IRQ:
		UARTX[uartn]->C2 -= UART_C2_TIE(1);
		UARTX[uartn]->C5 -= UART_C5_TDMAS(0);
		break;
	case UART_TX_DMA:
		UARTX[uartn]->C2 -= UART_C2_TIE(1);
		UARTX[uartn]->C5 -= UART_C5_TDMAS(1);
		break;
	case UART_COMPLETE:
		UARTX[uartn]->C2 -= UART_C2_TCIE(1);
		break;
	case UART_RX_IRQ:
		UARTX[uartn]->C2 -= UART_C2_RIE(1);
		UARTX[uartn]->C5 -= UART_C5_RDMAS(0);
		break;
	case UART_RX_DMA:
		UARTX[uartn]->C2 -= UART_C2_RIE(1);
		UARTX[uartn]->C5 -= UART_C5_RDMAS(1);
		break;
	case UART_IDLE:
		UARTX[uartn]->C1 -= UART_C1_ILT(1);
		UARTX[uartn]->C2 -= UART_C2_ILIE(1);
		break;
	case UART_OVERRUN_ERROR:
		UARTX[uartn]->C3 -= UART_C3_ORIE(1);
		break;
	case UART_NOISE_ERROR:
		UARTX[uartn]->C3 -= UART_C3_NEIE(1);
		break;
	case UART_FRAMING_ERROR:
		UARTX[uartn]->C3 -= UART_C3_FEIE(1);
		break;
	case UART_PARITY_ERROR:
		UARTX[uartn]->C3 -= UART_C3_PEIE(1);
		break;
	case UART_FIFO_RX_OVERFLOW:
		UARTX[uartn]->CFIFO -= UART_CFIFO_RXOFE(1);
		break;
	case UART_FIFO_TX_OVERFLOW:
		UARTX[uartn]->CFIFO -= UART_CFIFO_TXOFE(1);
		break;
	case UART_FIFO_RX_UNDERFLOW:
		UARTX[uartn]->CFIFO -= UART_CFIFO_RXUFE(1);
		break;
	default:
		break;
	}
	//使能中断
	switch (uartn)
	{
	case UART_0:
		NVIC_EnableIRQ(UART0_RX_TX_IRQn);
		break;
	case UART_1:
		NVIC_EnableIRQ(UART1_RX_TX_IRQn);
		break;
	case UART_2:
		NVIC_EnableIRQ(UART2_RX_TX_IRQn);
		break;
	case UART_3:
		NVIC_EnableIRQ(UART3_RX_TX_IRQn);
		break;
	case UART_4:
		NVIC_EnableIRQ(UART4_RX_TX_IRQn);
		break;
	case UART_5:
		NVIC_EnableIRQ(UART5_RX_TX_IRQn);
		break;
	default:
		break;
	}
}