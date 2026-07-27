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
* 文件名称          zf_driver_usb_cdc
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

#include "zf_driver_usb_cdc.h"

extern uint8_t g_apl_device[];
extern uint8_t g_apl_configuration[];
extern uint8_t g_apl_hs_configuration[];
extern uint8_t g_apl_qualifier_descriptor[];
extern uint8_t* g_apl_string_table[];

const usb_descriptor_t usb_descriptor =
{
    g_apl_device,                   /* Pointer to the device descriptor */
    g_apl_configuration,            /* Pointer to the configuration descriptor for Full-speed */
    g_apl_hs_configuration,         /* Pointer to the configuration descriptor for Hi-speed */
    g_apl_qualifier_descriptor,     /* Pointer to the qualifier descriptor */
    g_apl_string_table,             /* Pointer to the string descriptor table */
    7
};

usb_status_t            usb_event;
uint8_t                 g_buf[READ_BUF_SIZE]    = {0};
usb_event_info_t        event_info              = {0};
usb_pcdc_linecoding_t   g_line_coding;

void usb_callback(usb_callback_args_t * arg)
{
}

void usb_cdc_event_loop()
{
    g_basic0.p_api->eventGet(&event_info, &usb_event);
    g_basic0.p_api->pullUp(g_basic0.p_ctrl, USB_ON);
    
    switch(usb_event)
    {
    case USB_STATUS_CONFIGURED:
    {
        g_basic0.p_api->read(&g_basic0_ctrl, g_buf, READ_BUF_SIZE, USB_CLASS_PCDC);
        break;
    }

    case USB_STATUS_READ_COMPLETE:
    {
        g_basic0.p_api->read(&g_basic0_ctrl, g_buf, READ_BUF_SIZE, USB_CLASS_PCDC);
        usb_cdc_send_buffer(g_buf); // 将接收到的数据发送回去
        memset(g_buf, 0, sizeof(g_buf));
        break;
    }

    case USB_STATUS_REQUEST : 
    {
        if(USB_PCDC_SET_LINE_CODING == (event_info.setup.request_type & USB_BREQUEST))
        {
             g_basic0.p_api->periControlDataGet(&g_basic0_ctrl, (uint8_t*) &g_line_coding, LINE_CODING_LENGTH);
        }
        else if(USB_PCDC_GET_LINE_CODING == (event_info.setup.request_type & USB_BREQUEST))
        {
             g_basic0.p_api->periControlDataSet(&g_basic0_ctrl, (uint8_t*) &g_line_coding, LINE_CODING_LENGTH);
        }
        else
        {
            g_basic0.p_api->periControlStatusSet(&g_basic0_ctrl, USB_SETUP_STATUS_ACK);
        }

        break;
    }
    case USB_STATUS_DETACH:
    {
        g_basic0.p_api->close(&g_basic0_ctrl);
    }
    case USB_STATUS_SUSPEND:
    {
        g_basic0.p_api->suspend(&g_basic0_ctrl);
        break;
    }
    case USB_STATUS_RESUME:
    {
        g_basic0.p_api->resume(&g_basic0_ctrl);
        break;
    }
    default:
    {
        break;
    }
    }
}

void usb_cdc_send_buffer(uint8_t * buffer)
{
    usb_status_t            send_usb_event;
    g_basic0.p_api->write(&g_basic0_ctrl, (uint8_t*)buffer, strlen((char*)buffer), USB_CLASS_PCDC);
    
    do
    {
        g_basic0.p_api->eventGet(&event_info, &send_usb_event);
    }
    while(USB_STATUS_WRITE_COMPLETE != send_usb_event);
}

void usb_cdc_init()
{
    g_basic0.p_api->open(g_basic0.p_ctrl, g_basic0.p_cfg);
    g_basic0.p_api->pullUp(g_basic0.p_ctrl, USB_ON);
}

