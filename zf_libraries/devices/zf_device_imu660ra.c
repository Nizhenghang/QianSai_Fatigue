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
#include "zf_device_imu660ra.h"
#include "zf_device_config.h"

int16 imu660ra_gyro_x = 0, imu660ra_gyro_y = 0, imu660ra_gyro_z = 0;            // 三轴陀螺仪数据   gyro (陀螺仪)
int16 imu660ra_acc_x = 0, imu660ra_acc_y = 0, imu660ra_acc_z = 0;  

volatile bool write_flag = false;

void spi1_callback (spi_callback_args_t *p_args)
{
    if (SPI_EVENT_TRANSFER_COMPLETE == p_args->event)
    {
        write_flag = true;
    }
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     IMU660RA 写寄存器
// 参数说明     reg             寄存器地址
// 参数说明     data            数据
// 返回参数     void
// 使用示例     imu660ra_write_acc_gyro_register(IMU660RA_SLV0_CONFIG, 0x00);
// 备注信息     内部调用
//-------------------------------------------------------------------------------------------------------------------
static void imu660ra_write_register (uint8 reg, uint8 idata)
{
    IMU660RA_CS(0);
    uint8 command[2] = {reg | IMU660RA_SPI_W, idata};
    write_flag = false;
    g_spi1.p_api->write(g_spi1.p_ctrl, command, 2, SPI_BIT_WIDTH_8_BITS);
    while(!write_flag);

    IMU660RA_CS(1);
}

static volatile uint8 static_dat = 0;


static uint8 imu660ra_read_register (uint8 reg)
{
    uint8 p_reg[3] = {reg | IMU660RA_SPI_R, 0};
    uint8 idata[3] = {0};

    IMU660RA_CS(0);
    
    write_flag = false;
    g_spi1.p_api->writeRead(g_spi1.p_ctrl, p_reg, idata, 3, SPI_BIT_WIDTH_8_BITS);
    while(!write_flag);
    
    IMU660RA_CS(1);
    static_dat = idata[2];

    return idata[2];
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     IMU963RA 读数据 内部调用
// 参数说明     reg             寄存器地址
// 参数说明     data            数据缓冲区
// 参数说明     len             数据长度
// 返回参数     void
// 使用示例     imu963ra_read_acc_gyro_registers(IMU963RA_OUTX_L_A, dat, 6);
// 备注信息     内部调用
//-------------------------------------------------------------------------------------------------------------------
static void imu660ra_read_registers (uint8_t reg, uint8_t *idata, uint32_t len)
{
    uint8_t p_reg[8] = {reg | IMU660RA_SPI_R, 0, 0, 0, 0, 0, 0};

    IMU660RA_CS(0);
    
    write_flag = false;
    g_spi1.p_api->writeRead(g_spi1.p_ctrl, &p_reg, idata, len, SPI_BIT_WIDTH_8_BITS);
    while(!write_flag);

    IMU660RA_CS(1);
}


//-------------------------------------------------------------------------------------------------------------------
// 函数简介     IMU660RA 读数据 内部调用
// 参数说明     reg             寄存器地址
// 参数说明     data            数据缓冲区
// 参数说明     len             数据长度
// 返回参数     void
// 使用示例     imu660ra_read_acc_gyro_registers(IMU660RA_OUTX_L_A, dat, 6);
// 备注信息     内部调用
//-------------------------------------------------------------------------------------------------------------------
static void imu660ra_write_registers (uint8 reg, uint8 *idata, uint32_t len)
{
    IMU660RA_CS(0);
    
    uint8 command[2] = {reg | IMU660RA_SPI_W};
    write_flag = false;
    g_spi1.p_api->write(g_spi1.p_ctrl, command, 1, SPI_BIT_WIDTH_8_BITS);
    while(!write_flag);

    write_flag = false;
    g_spi1.p_api->write(g_spi1.p_ctrl, idata, len, SPI_BIT_WIDTH_8_BITS);
    while(!write_flag);
    
    IMU660RA_CS(1);
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     IMU660RA 六轴自检 内部调用
// 参数说明     void
// 返回参数     uint8           1-自检失败 0-自检成功
// 使用示例     imu660ra_acc_gyro_self_check();
// 备注信息     内部调用
//-------------------------------------------------------------------------------------------------------------------
static uint8 imu660ra_self_check (void)
{
    uint8 dat = 0, return_state = 0;
    uint16 timeout_count = 0;
    do
    {
        if(IMU660RA_TIMEOUT_COUNT < timeout_count ++)
        {
            return_state =  1;
            break;
        }
        dat = imu660ra_read_register(IMU660RA_CHIP_ID);
        system_delay_ms(1);
    }while(0x24 != dat);                                                        // 读取设备ID是否等于0X24，如果不是0X24则认为没检测到设备
    return return_state;
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     获取 IMU660RA 加速度计数据
// 参数说明     void
// 返回参数     void
// 使用示例     imu660ra_get_acc();
// 备注信息     执行该函数后，直接查看对应的变量即可
//-------------------------------------------------------------------------------------------------------------------
void imu660ra_get_acc (void)
{
    uint8 dat[7];

    imu660ra_read_registers(IMU660RA_ACC_ADDRESS, dat, 8);
    imu660ra_acc_x = (int16_t)(((uint16_t)dat[3] << 8 | dat[2]));
    imu660ra_acc_y = (int16_t)(((uint16_t)dat[5] << 8 | dat[4]));
    imu660ra_acc_z = (int16_t)(((uint16_t)dat[7] << 8 | dat[6]));
}


//-------------------------------------------------------------------------------------------------------------------
// 函数简介     获取IMU660RA陀螺仪数据
// 参数说明     void
// 返回参数     void
// 使用示例     imu660ra_get_gyro();
// 备注信息     执行该函数后，直接查看对应的变量即可
//-------------------------------------------------------------------------------------------------------------------
void imu660ra_get_gyro (void)
{
    uint8 dat[7];

    imu660ra_read_registers(IMU660RA_GYRO_ADDRESS, dat, 8);
    imu660ra_gyro_x = (int16_t)(((uint16_t)dat[3] << 8 | dat[2]));
    imu660ra_gyro_y = (int16_t)(((uint16_t)dat[5] << 8 | dat[4]));
    imu660ra_gyro_z = (int16_t)(((uint16_t)dat[7] << 8 | dat[6]));
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     初始化 IMU660RA
// 参数说明     void
// 返回参数     uint8           1-初始化失败 0-初始化成功
// 使用示例     imu660ra_init();
// 备注信息     
//-------------------------------------------------------------------------------------------------------------------
uint8 imu660ra_init (void)
{
    uint8 return_state = 0;
                                      
    system_delay_ms (10);
    g_spi1.p_api->open(g_spi1.p_ctrl, g_spi1.p_cfg);
    imu660ra_write_register(0x00, 0x00); 
    do
    {
        if(imu660ra_self_check())                                               // IMU660RA 自检
        {
            // 如果程序在输出了断言信息 并且提示出错位置在这里
            // 那么就是 IMU660RA 自检出错并超时退出了
            // 检查一下接线有没有问题 如果没问题可能就是坏了
            return_state = 1;
            break;
        }
        imu660ra_write_register(IMU660RA_PWR_CONF, 0x00);                       // 关闭高级省电模式
        system_delay_ms(1);
        imu660ra_write_register(IMU660RA_INIT_CTRL, 0x00);                      // 开始对模块进行初始化配置
        imu660ra_write_registers(IMU660RA_INIT_DATA, (uint8*)imu660ra_config_file, sizeof(imu660ra_config_file));   // 输出配置文件
        imu660ra_write_register(IMU660RA_INIT_CTRL, 0x01);                      // 初始化配置结束
        system_delay_ms(20);
        if(1 != imu660ra_read_register(IMU660RA_INT_STA))                       // 检查是否配置完成
        {
            // 如果程序在输出了断言信息 并且提示出错位置在这里
            // 那么就是 IMU660RA 配置初始化文件出错了
            // 检查一下接线有没有问题 如果没问题可能就是坏了
            return_state = 1;
            break;
        }
        imu660ra_write_register(IMU660RA_PWR_CTRL, 0x0E);                       // 开启性能模式  使能陀螺仪、加速度、温度传感器
        imu660ra_write_register(IMU660RA_ACC_CONF, 0xA7);                       // 加速度采集配置 性能模式 正常采集 50Hz  采样频率
        imu660ra_write_register(IMU660RA_GYR_CONF, 0xA9);                       // 陀螺仪采集配置 性能模式 正常采集 200Hz 采样频率

        // IMU660RA_ACC_SAMPLE 寄存器
        // 设置为 0x00 加速度计量程为 ±2  g   获取到的加速度计数据除以 16384  可以转化为带物理单位的数据 (g 代表重力加速度 物理学名词 一般情况下 g 取 9.8 m/s^2 为标准值)
        // 设置为 0x01 加速度计量程为 ±4  g   获取到的加速度计数据除以 8192   可以转化为带物理单位的数据 (g 代表重力加速度 物理学名词 一般情况下 g 取 9.8 m/s^2 为标准值)
        // 设置为 0x02 加速度计量程为 ±8  g   获取到的加速度计数据除以 4096   可以转化为带物理单位的数据 (g 代表重力加速度 物理学名词 一般情况下 g 取 9.8 m/s^2 为标准值)
        // 设置为 0x03 加速度计量程为 ±16 g   获取到的加速度计数据除以 2048   可以转化为带物理单位的数据 (g 代表重力加速度 物理学名词 一般情况下 g 取 9.8 m/s^2 为标准值)
        switch(IMU660RA_ACC_SAMPLE_DEFAULT)
        {
            default:
            {
                return_state = 1;
            }break;
            case IMU660RA_ACC_SAMPLE_SGN_2G:
            {
                imu660ra_write_register(IMU660RA_ACC_RANGE, 0x00);
            }break;
            case IMU660RA_ACC_SAMPLE_SGN_4G:
            {
                imu660ra_write_register(IMU660RA_ACC_RANGE, 0x01);
            }break;
            case IMU660RA_ACC_SAMPLE_SGN_8G:
            {
                imu660ra_write_register(IMU660RA_ACC_RANGE, 0x02);
            }break;
            case IMU660RA_ACC_SAMPLE_SGN_16G:
            {
                imu660ra_write_register(IMU660RA_ACC_RANGE, 0x03);
            }break;
        }
        if(1 == return_state)
        {
            break;
        }

        // IMU660RA_GYR_RANGE 寄存器
        // 设置为 0x04 陀螺仪量程为 ±125  dps    获取到的陀螺仪数据除以 262.4   可以转化为带物理单位的数据 单位为 °/s
        // 设置为 0x03 陀螺仪量程为 ±250  dps    获取到的陀螺仪数据除以 131.2   可以转化为带物理单位的数据 单位为 °/s
        // 设置为 0x02 陀螺仪量程为 ±500  dps    获取到的陀螺仪数据除以 65.6    可以转化为带物理单位的数据 单位为 °/s
        // 设置为 0x01 陀螺仪量程为 ±1000 dps    获取到的陀螺仪数据除以 32.8    可以转化为带物理单位的数据 单位为 °/s
        // 设置为 0x00 陀螺仪量程为 ±2000 dps    获取到的陀螺仪数据除以 16.4    可以转化为带物理单位的数据 单位为 °/s
        switch(IMU660RA_GYRO_SAMPLE_DEFAULT)
        {
            default:
            {
                return_state = 1;
            }break;
            case IMU660RA_GYRO_SAMPLE_SGN_125DPS:
            {
                imu660ra_write_register(IMU660RA_GYR_RANGE, 0x04);
            }break;
            case IMU660RA_GYRO_SAMPLE_SGN_250DPS:
            {
                imu660ra_write_register(IMU660RA_GYR_RANGE, 0x03);
            }break;
            case IMU660RA_GYRO_SAMPLE_SGN_500DPS:
            {
                imu660ra_write_register(IMU660RA_GYR_RANGE, 0x02);
            }break;
            case IMU660RA_GYRO_SAMPLE_SGN_1000DPS:
            {
                imu660ra_write_register(IMU660RA_GYR_RANGE, 0x01);
            }break;
            case IMU660RA_GYRO_SAMPLE_SGN_2000DPS:
            {
                imu660ra_write_register(IMU660RA_GYR_RANGE, 0x00);
            }break;
        }
        if(1 == return_state)
        {
            break;
        }
    }while(0);


    return return_state;

}
