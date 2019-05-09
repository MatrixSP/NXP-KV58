/* 
 * @date   2019年01月30日最后修改
 * @name   Sora_lib
 * @group  Profiterole
 * @author Afisora
 * @brief  Include
 */

#ifndef __INCLUDE_H__
#define __INCLUDE_H__

//*通用头文件*/
#include <stdio.h>    //printf
#include <string.h>   //memcpy
#include <stdlib.h>   //malloc
#include <stdint.h>
#include <stdarg.h>
#include <stdbool.h>

/*Cortex-M内核MCU寄存器头文件*/
#include "arm_common_tables.h"
#include "arm_const_structs.h"
#include "arm_math.h"
#include "fsl_device_registers.h"
#include "MKV58F24.h"  //寄存器映像头文件
#include "system_MKV58F24.h"
#include "stdio.h "

/*KV58驱动头文件*/
#include "fsl_adc16.h"
#include "fsl_aoi.h"
#include "fsl_clock.h"
#include "fsl_cmp.h"
#include "fsl_common.h"
#include "fsl_crc.h"
#include "fsl_dac.h"
#include "fsl_dmamux.h"
#include "fsl_dspi.h"
#include "fsl_dspi_edma.h"
#include "fsl_edma.h"
#include "fsl_enc.h"
#include "fsl_enet.h"
#include "fsl_ewm.h"
#include "fsl_flash.h"
#include "fsl_flexbus.h"
#include "fsl_flexcan.h"
#include "fsl_flexcan_edma.h"
#include "fsl_ftfx_adapter.h"
#include "fsl_ftfx_cache.h"
#include "fsl_ftfx_controller.h"
#include "fsl_ftfx_features.h"
#include "fsl_ftfx_flash.h"
#include "fsl_ftfx_flexnvm.h"
#include "fsl_ftfx_utilities.h"
#include "fsl_ftm.h"
#include "fsl_gpio.h"
#include "fsl_hsadc.h"
#include "fsl_i2c.h"
#include "fsl_i2c_edma.h"
#include "fsl_llwu.h"
#include "fsl_lptmr.h"
#include "fsl_pdb.h"
#include "fsl_pit.h"
#include "fsl_pmc.h"
#include "fsl_port.h"
#include "fsl_pwm.h"
#include "fsl_rcm.h"
#include "fsl_sim.h"
#include "fsl_smc.h"
#include "fsl_sysmpu.h"
#include "fsl_trng.h"
#include "fsl_uart.h"
#include "fsl_uart_edma.h"
#include "fsl_wdog.h"
#include "fsl_xbara.h"
#include "fsl_xbarb.h"

/*扩展库头文件*/
#include "fsl_dma_manager.h"

/*实用库*/  
#include "fsl_debug_console.h"
#include "fsl_debug_console_conf.h"
#include "fsl_io.h" 
#include "fsl_log.h" 
#include "fsl_shell.h"
#include "fsl_str.h"
#include "fsl_swo.h" 

/*工程驱动*/
#include "common.h"
#include "pin_mux.h"    
#include "clock_config.h"
#include "sora_system.h"
#include "sora_xbara_cfg.h"
#include "sora_dwt.h"
#include "sora_port.h"
//#include "sora_lptmr.h"
#include "sora_pit.h"
#include "sora_gpio.h"
#include "sora_gpio_cfg.h"
#include "sora_enc.h"
#include "sora_uart.h"
#include "sora_pwm.h"
#include "sora_edma.h"

/*外围设备驱动*/
#include "sora_lcd.h"
#include "sora_camera.h"
#include "sora_camera_mt9v032.h"

#endif