/*********************************************************************************************************************
* RA8D1 疲劳驾驶检测系统
* Copyright (c) 2025 SEEKFREE 逐飞科技
*
* 编译开关：
*   默认 - AI 疲劳检测模式
*   #define EXPORT_MODE - 串口图像导出模式（用于采集训练数据）
*   #define EXPORT_LABEL 1 - 导出模式下设置标签（0=正常，1=疲劳）
*
* 硬件连接：
*   - MT9V03X 摄像头通过转接板连接到拓展板摄像头接口
*   - 使用 DAP 下载调试器
********************************************************************************************************************/
#include "zf_common_headfile.h"

#ifdef EXPORT_MODE
#include "image_export.h"
#else
#include "fatigue_detect.h"
#endif

// **************************** 主函数 ****************************
void hal_entry(void)
{
    init_sdram();               // 初始化外部 SDRAM
    debug_init();               // 初始化 Debug 串口
    system_delay_ms(500);       // 等待摄像头电源稳定

#ifdef EXPORT_MODE
    printf("\r\n=== IMAGE EXPORT MODE ===\r\n");
#else
    printf("\r\n=== AI Fatigue Detection ===\r\n");
#endif
    printf("RA8D1 + MT9V03X (320x120 @ 80fps)\r\n");

    printf("Initializing camera...\r\n");
    if (mt9v03x_init())
    {
        printf("ERROR: Camera init failed!\r\n");
        while (1)
        {
            gpio_high(LED1);
            system_delay_ms(200);
            gpio_low(LED1);
            system_delay_ms(200);
        }
    }
    printf("Camera OK.\r\n");

#ifdef EXPORT_MODE
    image_export_init();
    printf("Exporting frames...\r\n");
    while (1)
    {
        image_export_send_frame();
    }
#else
    fatigue_init();
    printf("System running.\r\n\r\n");
    while (1)
    {
        fatigue_process_frame();
    }
#endif
}
