/* 
 * @date   2019年02月25日最后修改
 * @name   Sora_lib
 * @group  Profiterole
 * @author Afisora
 * @brief  Camera Config
 */

#ifndef __SORA_CAMERA_H__
#define __SORA_CAMERA_H__

typedef enum IMG_STATUS
{
  IMG_NOTINIT,
  IMG_FINISH,             //图像采集完毕
  IMG_FAIL,               //图像采集失败
  IMG_GATHER,             //图像采集中
  IMG_START,              //开始采集图像
  IMG_STOP,               //禁止图像采集
  IMG_PROCESS             //图像处理中
} IMG_STATUS;

#define MT9V032
#define Camera_Width      188                     //图像宽度
#define Camera_Height     120                     //图像高度
#define Camera_FPS        100
#define Camera_Auto_EXP   0
#define Camera_EXP_Time   300
#define Camera_Offset_LR  0
#define Camera_Offset_UD  0
#define Camera_Gain       48
#define Camera_DMA_SADDR  (void*)&PTB_BYTE0_IN    //数据接口地址
#define Camera_DMA_CHn    DMA_CH4                 //DMA传输通道
#define Camera_DMA_Mode   falling_DMA             //DMA触发模式
#define Camera_VS         PTB8                    //场信号端口
#define Camera_VS_Mode    falling                 //场中断触发模式
#define Camera_HS         PTB9                    //行信号端口
#define Camera_HS_Mode    falling                 //行中断触发模式
#define Camera_PS         PTC1                    //像素信号端口

//图像状态指示标志
extern volatile IMG_STATUS img_flag;
//图像缓冲区
extern volatile uint8 img_buff[Camera_Height][Camera_Width];
//图像吞吐区
extern volatile uint8 img_fifo[Camera_Height][Camera_Width];
//图像处理区
extern volatile uint8 img_process[Camera_Height][Camera_Width];

#endif