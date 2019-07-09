/* 
 * @name   Sora_lib
 * @group  Profiterole
 * @author Afisora
 * @brief  UART Driver
 */
   
#ifndef __SORA_UART_H__
#define __SORA_UART_H__

extern UART_MemMapPtr UARTX[6];

typedef enum UARTn_e
{
  UART_0,
  UART_1,
  UART_2,
  UART_3,
  UART_4,
  UART_5,
  UART_NULL,
} UARTn_e;

typedef enum uart_irq
{
  UART_TX_IRQ,            //发射器中断
  UART_TX_DMA,            //发射器DMA传输请求
  UART_COMPLETE,          //传输完成中断
  UART_RX_IRQ,            //接收器中断
  UART_RX_DMA,            //接收器DMA传输请求
  UART_IDLE,              //UART总线空闲中断
  UART_OVERRUN_ERROR,     //过载触发中断
  UART_NOISE_ERROR,       //噪声触发中断
  UART_FRAMING_ERROR,     //数据帧错误中断
  UART_PARITY_ERROR,      //奇偶校验错误中断
  UART_FIFO_RX_OVERFLOW,  //接收缓冲区溢出触发中断
  UART_FIFO_TX_OVERFLOW,  //发送缓冲区溢出触发中断
  UART_FIFO_RX_UNDERFLOW, //读取数据量超出接收缓冲区现有数据量（下溢出）触发中断
} uart_irq;

//      Channel     Pin           MUX                    
#define UART0_RX    PTA15       //PTA1、PTA15、PTB16、PTD6、PTE21、PTB0、PTC6   PTA1与J-LINK冲突
#define UART0_TX    PTA14       //PTA2、PTA14、PTB17、PTD7、PTE20、PTB1、PTC7   PTA2与J-LINK冲突

#define UART1_RX    PTE1        //PTC3、PTE1
#define UART1_TX    PTE0        //PTC4、PTE0

#define UART2_RX    PTE17       //PTD2、PTE17
#define UART2_TX    PTE16       //PTD3、PTE16

#define UART3_RX    PTE5		//PTB10、PTC16、PTE5
#define UART3_TX    PTE4		//PTB11、PTC17、PTE4

#define UART4_RX    PTE25       //PTC14、PTE25
#define UART4_TX    PTE24       //PTC15、PTE24

#define UART5_RX    PTE9        //PTD8、PTE9 
#define UART5_TX    PTE8        //PTD9、PTE8 

void UART_Com_Init(UARTn_e uartn, uint32 baud);
void UART_Put_Char(UARTn_e uartn, uint8 ch);
void UART_Put_Buff(UARTn_e uartn, const uint8 *buff, uint32 len);
void UART_Put_Str(UARTn_e uartn, const uint8 *str);
char UART_Get_Char(UARTn_e uartn);
char UART_Query_Char(UARTn_e uartn, uint8 *ch);
uint32 UART_Query_Str(UARTn_e uartn, uint8 *str, uint32 max_len);
void UART_IRQ_EN(UARTn_e uartn, uart_irq cfg);
void UART_IRQ_DIS_ALL(UARTn_e uartn);
void UART_IRQ_DIS(UARTn_e uartn, uart_irq cfg);

#endif