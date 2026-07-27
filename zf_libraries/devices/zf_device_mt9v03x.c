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
* 文件名称          zf_device_mt9v03x
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
#include "zf_device_mt9v03x.h"
#include "zf_device_config.h"

vuint8  mt9v03x_finish_flag;
uint8   mt9v03x_image[MT9V03X_W*MT9V03X_H] BSP_ALIGN_VARIABLE(64);// BSP_ALIGN_VARIABLE(64) BSP_PLACE_IN_SECTION(".sdram");

// 需要配置到摄像头的数据 不允许在这修改参数
static int16 mt9v03x_set_confing_buffer[MT9V03X_CONFIG_FINISH][2]=
{
    {MT9V03X_INIT,              0},                                             // 摄像头开始初始化

    {MT9V03X_AUTO_EXP,          MT9V03X_AUTO_EXP_DEF},                          // 自动曝光设置
    {MT9V03X_EXP_TIME,          MT9V03X_EXP_TIME_DEF},                          // 曝光时间
    {MT9V03X_FPS,               MT9V03X_FPS_DEF},                               // 图像帧率
    {MT9V03X_SET_COL,           MT9V03X_W},                                     // 图像列数量
    {MT9V03X_SET_ROW,           MT9V03X_H},                                     // 图像行数量
    {MT9V03X_LR_OFFSET,         MT9V03X_LR_OFFSET_DEF},                         // 图像左右偏移量
    {MT9V03X_UD_OFFSET,         MT9V03X_UD_OFFSET_DEF},                         // 图像上下偏移量
    {MT9V03X_GAIN,              MT9V03X_GAIN_DEF},                              // 图像增益
    {MT9V03X_PCLK_MODE,         MT9V03X_PCLK_MODE_DEF},                         // 像素时钟模式
};

// 从摄像头内部获取到的配置数据 不允许在这修改参数
static int16 mt9v03x_get_confing_buffer[MT9V03X_CONFIG_FINISH - 1][2]=
{
    {MT9V03X_AUTO_EXP,          0},                                             // 自动曝光设置
    {MT9V03X_EXP_TIME,          0},                                             // 曝光时间
    {MT9V03X_FPS,               0},                                             // 图像帧率
    {MT9V03X_SET_COL,           0},                                             // 图像列数量
    {MT9V03X_SET_ROW,           0},                                             // 图像行数量
    {MT9V03X_LR_OFFSET,         0},                                             // 图像左右偏移量
    {MT9V03X_UD_OFFSET,         0},                                             // 图像上下偏移量
    {MT9V03X_GAIN,              0},                                             // 图像增益
    {MT9V03X_PCLK_MODE,         0},                                             // 像素时钟模式命令 PCLK模式 < 仅总钻风 MT9V034 V1.5 以及以上版本支持该命令 >
};

void g_ceu_mt9v03x_callback (capture_callback_args_t* p_args)
{
    if((p_args->event & CEU_EVENT_FRAME_END) == CEU_EVENT_FRAME_END)
    {
        mt9v03x_finish_flag = 1;
    }
}

uint8 mt9v03x_init (void)
{
    system_delay_ms(100);
    soft_iic_info_struct mt9v03x_iic_struct;
    soft_iic_init(&mt9v03x_iic_struct, 0, MT9V03X_COF_IIC_DELAY, MT9V03X_COF_IIC_SCL, MT9V03X_COF_IIC_SDA);
    if(mt9v03x_set_config_sccb(&mt9v03x_iic_struct, mt9v03x_set_confing_buffer))
    {
        return 1;
    }
    g_ceu_mt9v03x.p_api->open(g_ceu_mt9v03x.p_ctrl, g_ceu_mt9v03x.p_cfg);
    g_ceu_mt9v03x.p_api->captureStart(g_ceu_mt9v03x.p_ctrl, (uint8*)mt9v03x_image);
	R_CEU->CAPCR = R_CEU_CAPCR_CTNCP_Msk;
    return 0;
}