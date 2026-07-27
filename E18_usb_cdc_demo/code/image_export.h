/*********************************************************************************************************************
* RA8D1 图像导出模块
* 通过串口导出 ROI 裁剪+缩放后的 64x32 灰度图像，用于 PC 端训练数据采集
*
* 串口协议：
*   [0xAA, 0x55, seq_hi, seq_lo, label] + 2048字节像素 + [0x0D, 0x0A]
********************************************************************************************************************/
#ifndef _IMAGE_EXPORT_H_
#define _IMAGE_EXPORT_H_

#include "zf_common_headfile.h"

// ROI 裁剪区域（与检测区域一致）
#define EXPORT_ROI_LEFT     80
#define EXPORT_ROI_RIGHT    240
#define EXPORT_ROI_TOP      10
#define EXPORT_ROI_BOTTOM   110

// 输出图像尺寸
#define EXPORT_OUT_W        64
#define EXPORT_OUT_H        32
#define EXPORT_OUT_SIZE     (EXPORT_OUT_W * EXPORT_OUT_H)  // 2048 bytes

// 采集标签（修改这里切换：0=正常，1=疲劳）
#define EXPORT_LABEL        0

// 帧率限制（115200波特率下约 5fps）
#define EXPORT_FRAME_SKIP   16  // 每16帧导出1帧（80fps/16=5fps）

// 接口函数
void image_export_init(void);
void image_export_send_frame(void);

#endif
