/* 
 * @name   Sora_lib
 * @group  Profiterole
 * @author Afisora
 * @brief  Camera Config
 */

#include "sora_system.h"

#ifdef __SORA_CAMERA_H__

volatile uint8 img_buff[Camera_Height][Camera_Width] = {0};
volatile uint8 img_fifo[Camera_Height][Camera_Width] = {0};
volatile uint8 img_process[Camera_Height][Camera_Width] = {0};
volatile IMG_STATUS img_flag = IMG_NOTINIT; //图像状态

#endif
