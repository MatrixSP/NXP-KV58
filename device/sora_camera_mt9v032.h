/* 
 * @date   2019年02月14日最后修改
 * @name   Sora_lib
 * @group  Profiterole
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

#ifndef __SORA_CAMERA_MT9V032_H__
#define __SORA_CAMERA_MT9V032_H__

typedef enum MT9V032_CMD
{
    MT9V032_INIT           = 0x00,  //摄像头初始化命令
    MT9V032_CMD_AUTO_EXP,           //自动曝光命令
    MT9V032_CMD_EXP_TIME,           //曝光时间命令
    MT9V032_CMD_FPS,                //摄像头帧率命令
    MT9V032_CMD_SET_COL,            //图像列命令
    MT9V032_CMD_SET_ROW,            //图像行命令
    MT9V032_CMD_LR_OFFSET,          //图像左右偏移命令
    MT9V032_CMD_UD_OFFSET,          //图像上下偏移命令
    MT9V032_CMD_GAIN,               //图像偏移命令
    MT9V032_CONFIG_FINISH,          //非命令位，主要用来占位计数
    
    MT9V032_SET_EXP_TIME   = 0xF0,  //单独设置曝光时间命令（不存入EEPROM）
    MT9V032_GET_STATUS,             //获取摄像头配置命令
    MT9V032_GET_VERSION,            //固件版本号命令
	
    MT9V032_SET_ADDR       = 0xFE,  //寄存器地址命令
    MT9V032_SET_DATA                //寄存器数据命令
} MT9V032_CMD;

#define MT9V032_CMD_SET
#define MT9V032_CMD_GET
#define MT9V032_UART        UART_2
#define MT9V032_UART_DMACHn DMA_CH20

extern int16 MT9V032_CFG_GET[MT9V032_CONFIG_FINISH - 1][2];
extern int16 MT9V032_CFG_CMD[MT9V032_CONFIG_FINISH][2];
extern uint8 MT9V032_UARTRX[3];
extern uint8 MT9V032_UARTTX[4];
extern uint8 MT9V032_UARTFLAG;

void MT9V032_Init();

#endif