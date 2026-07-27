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
* 文件名称          zf_driver_encoder
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
#include "zf_driver_encoder.h"

//----------------------------------------------------------------------------------------
// 函数简介     初始化计时器
// 参数说明     g_timer        计数器timer外设
// 返回参数     void
// 使用示例     encoder_init(g_timer1);
// 备注信息     
//----------------------------------------------------------------------------------------
void encoder_init (const timer_instance_t *g_timer)
{
    g_timer->p_api->open(g_timer->p_ctrl, g_timer->p_cfg);
    g_timer->p_api->start(g_timer->p_ctrl);
    //初始化完成后先清空一次计数值
    g_timer->p_api->reset(g_timer->p_ctrl);
}

//----------------------------------------------------------------------------------------
// 函数简介     获取计时器值
// 参数说明     g_timer        计数器timer外设
// 参数说明     dir_pin         计数器方向引脚
// 返回参数     int32_t         返回计数器值
// 使用示例     encoder_get_count(g_timer1, pin1);
// 备注信息     
//----------------------------------------------------------------------------------------
int32_t encoder_get_count (const timer_instance_t *g_timer, bsp_io_port_pin_t dir_pin)
{
    bsp_io_level_t _level;
    timer_status_t now_count = {0};

    //获取当前计数值
    g_timer->p_api->statusGet(g_timer->p_ctrl, &now_count);

    //获取方向引脚电平
    g_ioport.p_api->pinRead(g_ioport.p_ctrl, dir_pin, &_level);

    //判断方向引脚电平返回对应计数值
    if(_level)
    {
        return (int32_t)now_count.counter;
    }
    else
    {
        return -(int32_t)now_count.counter;
    }
}

//----------------------------------------------------------------------------------------
// 函数简介     清空计时器
// 参数说明     g_timer        计数器timer外设
// 返回参数     void
// 使用示例     encoder_clear_count(g_timer1);
// 备注信息     
//----------------------------------------------------------------------------------------
void encoder_clear_count (const timer_instance_t *g_timer)
{
    g_timer->p_api->reset(g_timer->p_ctrl);
}
