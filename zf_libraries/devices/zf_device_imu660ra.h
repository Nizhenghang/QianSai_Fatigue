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
* 文件名称          zf_device_imu660ra
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
#ifndef _zf_device_imu963ra_h
#define _zf_device_imu963ra_h

#include "zf_common_headfile.h"

typedef enum
{
    IMU660RA_ACC_SAMPLE_SGN_2G ,                                                // 加速度计量程 ±2G  (ACC = Accelerometer 加速度计) (SGN = signum 带符号数 表示正负范围) (G = g 重力加速度 g≈9.80 m/s^2)
    IMU660RA_ACC_SAMPLE_SGN_4G ,                                                // 加速度计量程 ±4G  (ACC = Accelerometer 加速度计) (SGN = signum 带符号数 表示正负范围) (G = g 重力加速度 g≈9.80 m/s^2)
    IMU660RA_ACC_SAMPLE_SGN_8G ,                                                // 加速度计量程 ±8G  (ACC = Accelerometer 加速度计) (SGN = signum 带符号数 表示正负范围) (G = g 重力加速度 g≈9.80 m/s^2)
    IMU660RA_ACC_SAMPLE_SGN_16G,                                                // 加速度计量程 ±16G (ACC = Accelerometer 加速度计) (SGN = signum 带符号数 表示正负范围) (G = g 重力加速度 g≈9.80 m/s^2)
}imu660ra_acc_sample_config;

typedef enum
{
    IMU660RA_GYRO_SAMPLE_SGN_125DPS ,                                           // 陀螺仪量程 ±125DPS  (GYRO = Gyroscope 陀螺仪) (SGN = signum 带符号数 表示正负范围) (DPS = Degree Per Second 角速度单位 °/S)
    IMU660RA_GYRO_SAMPLE_SGN_250DPS ,                                           // 陀螺仪量程 ±250DPS  (GYRO = Gyroscope 陀螺仪) (SGN = signum 带符号数 表示正负范围) (DPS = Degree Per Second 角速度单位 °/S)
    IMU660RA_GYRO_SAMPLE_SGN_500DPS ,                                           // 陀螺仪量程 ±500DPS  (GYRO = Gyroscope 陀螺仪) (SGN = signum 带符号数 表示正负范围) (DPS = Degree Per Second 角速度单位 °/S)
    IMU660RA_GYRO_SAMPLE_SGN_1000DPS,                                           // 陀螺仪量程 ±1000DPS (GYRO = Gyroscope 陀螺仪) (SGN = signum 带符号数 表示正负范围) (DPS = Degree Per Second 角速度单位 °/S)
    IMU660RA_GYRO_SAMPLE_SGN_2000DPS,                                           // 陀螺仪量程 ±2000DPS (GYRO = Gyroscope 陀螺仪) (SGN = signum 带符号数 表示正负范围) (DPS = Degree Per Second 角速度单位 °/S)
}imu660ra_gyro_sample_config;

#define IMU660RA_CS_PIN                 (BSP_IO_PORT_04_PIN_13)                                       // CS 片选引脚
#define IMU660RA_CS(x)                  ((x) ? (gpio_set_level(IMU660RA_CS_PIN, 1)) : (gpio_set_level(IMU660RA_CS_PIN, 0)))

#define IMU660RA_ACC_SAMPLE_DEFAULT     ( IMU660RA_ACC_SAMPLE_SGN_8G )          // 在这设置默认的 加速度计 初始化量程
#define IMU660RA_GYRO_SAMPLE_DEFAULT    ( IMU660RA_GYRO_SAMPLE_SGN_2000DPS )    // 在这设置默认的 陀螺仪   初始化量程

#define IMU660RA_TIMEOUT_COUNT          ( 0x00FF )                                  // IMU660RA 超时计数

//================================================定义 IMU660RA 内部地址================================================
#define IMU660RA_DEV_ADDR               ( 0x69 )                                    // SA0接地：0x68 SA0上拉：0x69 模块默认上拉
#define IMU660RA_SPI_W                  ( 0x00 )
#define IMU660RA_SPI_R                  ( 0x80 )

#define IMU660RA_CHIP_ID                ( 0x00 )
#define IMU660RA_PWR_CONF               ( 0x7C )
#define IMU660RA_PWR_CTRL               ( 0x7D )
#define IMU660RA_INIT_CTRL              ( 0x59 )
#define IMU660RA_INIT_DATA              ( 0x5E )
#define IMU660RA_INT_STA                ( 0x21 )
#define IMU660RA_ACC_ADDRESS            ( 0x0C )
#define IMU660RA_GYRO_ADDRESS           ( 0x12 )
#define IMU660RA_ACC_CONF               ( 0x40 )
#define IMU660RA_ACC_RANGE              ( 0x41 )
#define IMU660RA_GYR_CONF               ( 0x42 )
#define IMU660RA_GYR_RANGE              ( 0x43 )
//================================================定义 IMU660RA 内部地址================================================

extern int16 imu660ra_gyro_x, imu660ra_gyro_y, imu660ra_gyro_z;                 // 三轴陀螺仪数据      gyro (陀螺仪)
extern int16 imu660ra_acc_x, imu660ra_acc_y, imu660ra_acc_z;                    // 三轴加速度计数据     acc (accelerometer 加速度计)

void  imu660ra_get_acc                  (void);                                     // 获取 IMU660RA 加速度计数据
void  imu660ra_get_gyro                 (void);                                     // 获取 IMU660RA 陀螺仪数据

uint8   imu660ra_init                   (void);

#endif
