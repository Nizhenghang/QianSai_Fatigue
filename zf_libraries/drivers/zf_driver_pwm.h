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
* 文件名称          zf_driver_pwm
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
#ifndef _ZF_DRIVER_PWM_H_
#define _ZF_DRIVER_PWM_H_

#include "hal_data.h"
#include "zf_common_typedef.h"

#define MOTOR1_DIR              BSP_IO_PORT_04_PIN_14
#define MOTOR1_TIMER            &motor_timer0
#define MOTOR1_GTIOC            GPT_IO_PIN_GTIOCA
#define MOTOR2_DIR              BSP_IO_PORT_07_PIN_12
#define MOTOR2_TIMER            &motor_timer2
#define MOTOR2_GTIOC            GPT_IO_PIN_GTIOCA
#define MOTOR3_DIR              BSP_IO_PORT_09_PIN_11
#define MOTOR3_TIMER            &motor_timer3
#define MOTOR3_GTIOC            GPT_IO_PIN_GTIOCA
#define MOTOR4_DIR              BSP_IO_PORT_09_PIN_14
#define MOTOR4_TIMER            &motor_timer5
#define MOTOR4_GTIOC            GPT_IO_PIN_GTIOCA

#define SERVO1_TIMER            &servo_timer6
#define SERVO1_GTIOC            GPT_IO_PIN_GTIOCA
#define SERVO2_TIMER            &servo_timer6
#define SERVO2_GTIOC            GPT_IO_PIN_GTIOCB
#define SERVO3_TIMER            &servo_timer9
#define SERVO3_GTIOC            GPT_IO_PIN_GTIOCA
#define SERVO4_TIMER            &servo_timer9
#define SERVO4_GTIOC            GPT_IO_PIN_GTIOCB

void pwm_init   (const timer_instance_t *g_timer);
void pwm_set_duty   (const timer_instance_t *g_timer, gpt_io_pin_t pin_t, uint32_t duty);

#endif /* _ZF_DRIVER_PWM_H_ */
