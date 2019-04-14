/* 
 * @date   2019年02月23日最后修改
 * @name   Sora_lib
 * @group  Akko
 * @author Afisora
 * @brief  Camera Driver for MT9V032
 * @note
 * @{
 *    1.提高单片机主频能够改善采集能力
 *    2.MT9V032的51单片机含EEPROM，能够断电保存配置
 *    3.SDA对应51的RX，SCL对应51的TX，波特率为9600
 *    4.改变摄像头背后电阻配置可切换IIC直接控制摄像头
 *    5.分辨率752*480,376*240,188*120
 * @}
 */
   
#include "sora_system.h"

#ifdef MT9V032

int16 MT9V032_CFG_CMD[MT9V032_CONFIG_FINISH][2]=
{
  //自动曝光设置      范围1-63 0为关闭 如果自动曝光开启  EXP_TIME命令设置的数据将会变为最大曝光时间，也就是自动曝光时间的上限
  //                  一般情况是不需要开启这个功能，因为比赛场地光线一般都比较均匀，如果遇到光线非常不均匀的情况可以尝试设置该值，增加图像稳定性
  {MT9V032_CMD_AUTO_EXP,  Camera_Auto_EXP },   
  //曝光时间          摄像头收到后会自动计算出最大曝光时间，如果设置过大则设置为计算出来的最大曝光值
  {MT9V032_CMD_EXP_TIME,  Camera_EXP_Time }, 
  //图像帧率          摄像头收到后会自动计算出最大FPS，如果过大则设置为计算出来的最大FPS
  {MT9V032_CMD_FPS,       Camera_FPS      },  
  //图像列数量        范围1-752   
  {MT9V032_CMD_SET_COL,   Camera_Width    }, 
  //图像行数量        范围1-480
  {MT9V032_CMD_SET_ROW,   Camera_Height   },
  //图像左右偏移量    正值 右偏移   负值 左偏移  列为188 376 752时无法设置偏移    摄像头收偏移数据后会自动计算最大偏移，如果超出则设置计算出来的最大偏移
  {MT9V032_CMD_LR_OFFSET, Camera_Offset_LR},   
  //图像上下偏移量    正值 上偏移   负值 下偏移  行为120 240 480时无法设置偏移    摄像头收偏移数据后会自动计算最大偏移，如果超出则设置计算出来的最大偏移
  {MT9V032_CMD_UD_OFFSET, Camera_Offset_UD},  
  //图像增益          范围16-64     增益可以在曝光时间固定的情况下改变图像亮暗程度
  {MT9V032_CMD_GAIN,      Camera_Gain     },  
  //摄像头开始初始化
  {MT9V032_INIT,          0               }    
};

//从摄像头内部获取到的配置数据
int16 MT9V032_CFG_GET[MT9V032_CONFIG_FINISH - 1][2]=
{
  {MT9V032_CMD_AUTO_EXP,          0},   //自动曝光设置      
  {MT9V032_CMD_EXP_TIME,          0},   //曝光时间          
  {MT9V032_CMD_FPS,               0},   //图像帧率          
  {MT9V032_CMD_SET_COL,           0},   //图像列数量        
  {MT9V032_CMD_SET_ROW,           0},   //图像行数量        
  {MT9V032_CMD_LR_OFFSET,         0},   //图像左右偏移量    
  {MT9V032_CMD_UD_OFFSET,         0},   //图像上下偏移量    
  {MT9V032_CMD_GAIN,              0},   //图像增益          
};

uint8 MT9V032_UARTRX[3] = {0};
uint8 MT9V032_UARTTX[4] = {0};
uint8 MT9V032_UARTFLAG = 0;

void MT9V032_Init()
{
  uint16 i;
  UART_Com_Init(MT9V032_UART, 9600);
  UART_RX_IRQ_EN(MT9V032_UART, UART_RX_DMA);
  UART_RX_IRQ_EN(MT9V032_UART, UART_IDLE);
  EDMA_UART_RX_Init(MT9V032_UART_DMACHn, MT9V032_UART);
  //DELAY_MS(0);//等待摄像头启动完毕
  EDMA_UART_RX_Start(MT9V032_UART_DMACHn, (uint32)MT9V032_UARTRX, 3);
  
#ifdef MT9V032_CMD_SET
  //开始配置摄像头并重新初始化
  for(i = 0; i < MT9V032_CONFIG_FINISH; i++)
  {
    MT9V032_UARTTX[0] = 0xA5;
    MT9V032_UARTTX[1] = MT9V032_CFG_CMD[i][0];
    MT9V032_UARTTX[2] = MT9V032_CFG_CMD[i][1] >> 8;
    MT9V032_UARTTX[3] = (uint8)MT9V032_CFG_CMD[i][1];
    UART_Put_Buff(MT9V032_UART, MT9V032_UARTTX, 4);
    DELAY_MS(2);
  }
  LCD_Fill(0x00);
  while(!MT9V032_UARTFLAG)
  {
    LCD_P6x8Str(0, 0, "Camera Config Setting");
    //LCD_Show_Number(0, 1, UART2->C2);
    //LCD_Show_Number(0, 2, UART2->S1);
    //LCD_Show_Number(0, 3, UART2->SFIFO);
  }
  while((0xFF != MT9V032_UARTRX[1]) || (0xFF != MT9V032_UARTRX[2]));
#endif
  
#ifdef MT9V032_CMD_GET
  LCD_Fill(0x00);
  for(i = 0; i < MT9V032_CONFIG_FINISH - 1; i++)
  {
    MT9V032_UARTFLAG = 0;
    EDMA_UART_RX_Start(MT9V032_UART_DMACHn, (uint32)MT9V032_UARTRX, 3);
    MT9V032_UARTTX[0] = 0xA5;
    MT9V032_UARTTX[1] = MT9V032_GET_STATUS;
    MT9V032_UARTTX[2] = MT9V032_CFG_GET[i][0] >> 8;
    MT9V032_UARTTX[3] = (uint8)MT9V032_CFG_GET[i][0];
    UART_Put_Buff(MT9V032_UART, MT9V032_UARTTX, 4);
    while(!MT9V032_UARTFLAG)
    {
      LCD_P6x8Str(0, 0, "Camera Config Reading");
      //LCD_Show_Number(0, 1, UART2->C2);
      //LCD_Show_Number(0, 2, UART2->S1);
      //LCD_Show_Number(0, 3, UART2->SFIFO);
    }
    MT9V032_CFG_GET[i][1] = MT9V032_UARTRX[1] << 8 | MT9V032_UARTRX[2];
  }
#endif
}
void UART2_RX_TX_IRQHandler(void)
{
  uint8 buff;
  if(UART2->S1 & UART_S1_IDLE_MASK)
  {
    buff = UART2->D;
    EDMA_UART_RX_Stop(MT9V032_UART_DMACHn, (uint32)MT9V032_UARTRX);
    UART2->CFIFO |= UART_CFIFO_RXFLUSH(1);
    MT9V032_UARTFLAG = 1;
  }
}



#endif