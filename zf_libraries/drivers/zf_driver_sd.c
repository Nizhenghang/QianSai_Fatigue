/*********************************************************************************************************************
* RA8D1 Opensourec Library 即（RA8D1 开源库）是一个基于官方 SDK 接口的第三方开源库
* Copyright (c) 2025 SEEKFREE 逐飞科技
* 
* 本文件是 RA8D1 开源库的一部分
* 
* RA8D1 开源库 是免费软件
* 您可以根据自由软件基金会发布的 GPL（GNU General Public License，即 GNU通用公共许可证）的条款
* 即 GPL 的第3版（即 GPL3.0）或（您选择的）任何后来的版本，重新发布和/或修改它
* 
* 本开源库的发布是希望它能发挥作用，但并未对其作任何的保证
* 甚至没有隐含的适销性或适合特定用途的保证
* 更多细节请参见 GPL
* 
* 您应该在收到本开源库的同时收到一份 GPL 的副本
* 如果没有，请参阅<https://www.gnu.org/licenses/>
* 
* 额外注明：
* 本开源库使用 GPL3.0 开源许可证协议 以上许可申明为译文版本
* 许可申明英文版在 libraries/doc 文件夹下的 GPL3_permission_statement.txt 文件中
* 许可证副本在 libraries 文件夹下 即该文件夹下的 LICENSE 文件
* 欢迎各位使用并传播本程序 但修改内容时必须保留逐飞科技的版权声明（即本声明）
* 
* 文件名称          zf_driver_sd
* 公司名称          成都逐飞科技有限公司
* 版本信息          查看 libraries/doc 文件夹内 version 文件 版本说明
* 开发环境          MDK 5.38
* 适用平台          RA8D1
* 店铺链接          https://seekfree.taobao.com/
* 
* 修改记录
* 日期              作者                备注
* 2025-03-25        ZSY            first version
********************************************************************************************************************/
#include "zf_driver_sd.h"

static uint8 g_transfer_complete = 0;
static uint8 g_card_erase_complete = 0;
static bool g_card_inserted = false;

uint8_t g_dest[SDHI_MAX_BLOCK_SIZE] BSP_ALIGN_VARIABLE(4);  //4字节对齐
uint8_t g_src[SDHI_MAX_BLOCK_SIZE]  BSP_ALIGN_VARIABLE(4);
 
void g_sdmmc1_callback (sdmmc_callback_args_t * p_args)
{
     if (SDMMC_EVENT_TRANSFER_COMPLETE == p_args->event)  //读取或写入完成
     {
         g_transfer_complete = 1;
     }
     if (SDMMC_EVENT_CARD_INSERTED == p_args->event)  //卡插入中断
     {
         g_card_inserted = true;
     }
     if (SDMMC_EVENT_CARD_REMOVED == p_args->event)   //卡拔出中断
     {
         g_card_inserted = false;
     }
     if (SDMMC_EVENT_ERASE_COMPLETE == p_args->event)  //擦除完成
     {
         g_card_erase_complete = 1;
     }
     if (SDMMC_EVENT_ERASE_BUSY == p_args->event)  //擦除超时
     {
         g_card_erase_complete = 2;
     }
}


void sd_read_write(void)
{
    sdmmc_status_t status;
    sdmmc_device_t my_sdmmc_device = {0};
    uint32_t i;

    for (i = 0; i < SDHI_MAX_BLOCK_SIZE; i++)
    {
        g_src[i] = (uint8_t)('A' + (uint8_t)(i % 26));
    }

    g_sdmmc1.p_api->statusGet(g_sdmmc1.p_ctrl, &status);
    if (!status.card_inserted)
    {
        while (!g_card_inserted)
        {
            system_delay_ms(1000);
        }
    }

    system_delay_ms(10);
    g_sdmmc1.p_api->mediaInit(g_sdmmc1.p_ctrl, &my_sdmmc_device);

    /* 写入数据 */
    g_transfer_complete = 0;
    g_sdmmc1.p_api->write(g_sdmmc1.p_ctrl, g_src, 1, 1);
    while (g_transfer_complete == 0);
    g_transfer_complete = 0;

    /* 读出数据 */
    g_transfer_complete = 0;
    g_sdmmc1.p_api->read(g_sdmmc1.p_ctrl, g_dest, 1, 1);
    while (g_transfer_complete == 0);
    g_transfer_complete = 0;

    /* 打印数据 */
    for (i = 0; i < SDHI_MAX_BLOCK_SIZE; i++)
    {
        if (i % 26 == 0 && i > 0)
        {
            printf(" ");
        }
        printf("%c", g_dest[i]);
    }

    /* 对比数据 */
    if (strncmp((char*)&g_dest[0], (char*)&g_src[0], SDHI_MAX_BLOCK_SIZE) == 0)
    {
        printf("\r\nSD write read success!\r\n");
    }
    else
    {
        printf("\r\nSD write read fault!\r\n");
    }
}
 
void sd_init()
{
    g_sdmmc1.p_api->open(g_sdmmc1.p_ctrl, g_sdmmc1.p_cfg);
}
