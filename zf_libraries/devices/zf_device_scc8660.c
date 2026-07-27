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
* 文件名称          zf_device_scc8660
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
#include "zf_device_scc8660.h"
#include "zf_device_config.h"

volatile uint8  scc8660_finish_flag;
uint16  scc8660_buff[SCC8660_W*SCC8660_H] BSP_ALIGN_VARIABLE(64);// BSP_ALIGN_VARIABLE(64) BSP_PLACE_IN_SECTION(".sdram");

// 需要配置到摄像头的数据 不允许在这修改参数
static int16 scc8660_set_confing_buffer[SCC8660_CONFIG_FINISH][2]=
{
    {SCC8660_INIT,              0},                                             // 摄像头开始初始化

    {SCC8660_AUTO_EXP,          SCC8660_AUTO_EXP_DEF},                          // 自动曝光
    {SCC8660_BRIGHT,            SCC8660_BRIGHT_DEF},                            // 亮度设置
    {SCC8660_FPS,               SCC8660_FPS_DEF},                               // 图像帧率
    {SCC8660_SET_COL,           SCC8660_W},                                     // 图像列数
    {SCC8660_SET_ROW,           SCC8660_H},                                     // 图像行数
    {SCC8660_PCLK_DIV,          SCC8660_PCLK_DIV_DEF},                          // PCLK分频系数
    {SCC8660_PCLK_MODE,         SCC8660_PCLK_MODE_DEF},                         // PCLK模式
    {SCC8660_COLOR_MODE,        SCC8660_COLOR_MODE_DEF},                        // 图像色彩模式
    {SCC8660_DATA_FORMAT,       SCC8660_DATA_FORMAT_DEF},                       // 输出数据格式
    {SCC8660_MANUAL_WB,         SCC8660_MANUAL_WB_DEF}                          // 手动白平衡
};

void g_ceu_scc8660_callback (capture_callback_args_t* p_args)
{
    if((p_args->event & CEU_EVENT_FRAME_END) == CEU_EVENT_FRAME_END)
    {
        scc8660_finish_flag = 1;
    }
}

uint8 scc8660_init (void)
{
    system_delay_ms(100);
    soft_iic_info_struct scc8660_iic_struct;
    soft_iic_init(&scc8660_iic_struct, 0, SCC8660_COF_IIC_DELAY, SCC8660_COF_IIC_SCL, SCC8660_COF_IIC_SDA);
    if(scc8660_set_config_sccb(&scc8660_iic_struct, scc8660_set_confing_buffer))
    {
        return 1;
    }
    g_ceu_scc8660.p_api->open(g_ceu_scc8660.p_ctrl, g_ceu_scc8660.p_cfg);
    g_ceu_scc8660.p_api->captureStart(g_ceu_scc8660.p_ctrl, (uint8*)scc8660_buff);
	R_CEU->CAPCR = R_CEU_CAPCR_CTNCP_Msk;
    return 0;
}