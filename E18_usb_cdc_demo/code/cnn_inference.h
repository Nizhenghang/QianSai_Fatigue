/*********************************************************************************************************************
* RA8D1 CNN 推理引擎
* 轻量级 2-Conv + 2-FC 网络前向推理
* 输入 64x32 灰度图，输出 2 类（normal / fatigued）
********************************************************************************************************************/
#ifndef _CNN_INFERENCE_H_
#define _CNN_INFERENCE_H_

#include "zf_common_headfile.h"

#define CNN_INPUT_W     64
#define CNN_INPUT_H     32
#define CNN_INPUT_SIZE  (CNN_INPUT_W * CNN_INPUT_H)

// 分类结果
#define CNN_NORMAL      0
#define CNN_FATIGUED    1

// 接口
void    cnn_init(void);
uint8   cnn_classify(const uint8 *input, float *confidence);

#endif
