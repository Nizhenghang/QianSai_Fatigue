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
* 文件名称          zf_driver_adc
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
#include "zf_driver_adc.h"

volatile bool adc_complete_flag = false;

void adc_callback (adc_callback_args_t* p_args)
{
    FSP_PARAMETER_NOT_USED(p_args);
    adc_complete_flag = true;
    g_adc0.p_api->scanStop(g_adc0.p_ctrl);
}

//----------------------------------------------------------------------------------------
// 函数简介     获取ADC的值
// 参数说明     void
// 返回参数     void
// 使用示例     adc_get_voltage();
// 备注信息     
//----------------------------------------------------------------------------------------
double adc_get_voltage (void)
{
    uint16_t adc_data = 0;
    double voltage_out;
    g_adc0.p_api->scanStart(g_adc0.p_ctrl);
    while(!adc_complete_flag);                                      // 等待转换完成标志
    adc_complete_flag = false;                                      // 重新清除标志位
                                                                       
    g_adc0.p_api->read(g_adc0.p_ctrl, ADC_CHANNEL_0, &adc_data);    // 读取通道0数据
    voltage_out = (double)(adc_data * 3.3 / 4095) * 11;             // ADC原始数据转换为电压值（ADC参考电压为3.3V）
    return voltage_out;
}

//----------------------------------------------------------------------------------------
// 函数简介     获取ADC的值
// 参数说明     void
// 返回参数     void
// 使用示例     adc_get_voltage();
// 备注信息     
//----------------------------------------------------------------------------------------
uint16_t adc_get_mic (void)
{
    uint16_t adc_data = 0;

    g_adc0.p_api->scanStart(g_adc0.p_ctrl);
    while(!adc_complete_flag);                                      // 等待转换完成标志
    adc_complete_flag = false;                                      // 重新清除标志位
                                                                       
    g_adc0.p_api->read(g_adc0.p_ctrl, ADC_CHANNEL_5, &adc_data);    // 读取通道0数据
    return adc_data;
}

//----------------------------------------------------------------------------------------
// 函数简介     获取ADC的值
// 参数说明     void
// 返回参数     void
// 使用示例     adc_get_random();
// 备注信息     
//----------------------------------------------------------------------------------------
uint16_t adc_get_random (void)
{
    uint16_t adc_data = 0;

    g_adc0.p_api->scanStart(g_adc0.p_ctrl);
    while(!adc_complete_flag);                                      // 等待转换完成标志
    adc_complete_flag = false;                                      // 重新清除标志位
                                                                       
    g_adc0.p_api->read(g_adc0.p_ctrl, ADC_CHANNEL_6, &adc_data);    // 读取通道0数据
    return adc_data;
}

//----------------------------------------------------------------------------------------
// 函数简介     ADC初始化
// 参数说明     void
// 返回参数     void
// 使用示例     adc_init();
// 备注信息     
//----------------------------------------------------------------------------------------
void adc_init (void)
{
    g_adc0.p_api->open(g_adc0.p_ctrl, g_adc0.p_cfg);
    g_adc0.p_api->scanCfg(g_adc0.p_ctrl, g_adc0.p_channel_cfg);
}

