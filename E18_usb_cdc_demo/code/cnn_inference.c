/*********************************************************************************************************************
* RA8D1 CNN 推理引擎
*
* 网络结构（与 train_cnn.py 对应）：
*   Input:  64x32x1 (HxWxC)
*   Conv1:  5x5, 8 filters, stride 2 -> 14x30x8 + ReLU
*   Pool1:  2x2 -> 7x15x8
*   Conv2:  3x3, 16 filters -> 5x13x16 + ReLU
*   Pool2:  2x2 -> 2x6x16
*   Flatten: 192
*   FC1:    192->32 + ReLU
*   FC2:    32->2 + Softmax
*
* 权重在 cnn_weights.h 中（由 export_model.py 生成）
********************************************************************************************************************/
#include "cnn_inference.h"
#include "cnn_weights_v2.h"

// SDRAM 段放置宏（兼容 FSP 和非 FSP 环境）
#ifndef BSP_PLACE_IN_SECTION
#define BSP_PLACE_IN_SECTION(x) __attribute__((section(x))) __attribute__((__used__))
#endif

//==================================================================================================================
// 各层输出尺寸
//==================================================================================================================
// Conv1: input 32x64, kernel 5x5, stride 2, valid
#define CONV1_OUT_H     ((CNN_INPUT_H - CNN_L0_KH) / 2 + 1)    // 14
#define CONV1_OUT_W     ((CNN_INPUT_W - CNN_L0_KW) / 2 + 1)    // 30
#define CONV1_OUT_C     CNN_L0_COUT                              // 8
#define CONV1_OUT_SIZE  (CONV1_OUT_H * CONV1_OUT_W * CONV1_OUT_C)

// Pool1: 2x2
#define POOL1_OUT_H     (CONV1_OUT_H / 2)    // 7
#define POOL1_OUT_W     (CONV1_OUT_W / 2)    // 15
#define POOL1_OUT_SIZE  (POOL1_OUT_H * POOL1_OUT_W * CONV1_OUT_C)

// Conv2: input 7x15x8, kernel 3x3, stride 1, valid
#define CONV2_OUT_H     (POOL1_OUT_H - CNN_L1_KH + 1)    // 5
#define CONV2_OUT_W     (POOL1_OUT_W - CNN_L1_KW + 1)    // 13
#define CONV2_OUT_C     CNN_L1_COUT                        // 16
#define CONV2_OUT_SIZE  (CONV2_OUT_H * CONV2_OUT_W * CONV2_OUT_C)

// Pool2: 2x2
#define POOL2_OUT_H     (CONV2_OUT_H / 2)    // 2
#define POOL2_OUT_W     (CONV2_OUT_W / 2)    // 6
#define POOL2_OUT_SIZE  (POOL2_OUT_H * POOL2_OUT_W * CONV2_OUT_C)

// FC1 输出
#define FC1_OUT_SIZE    CNN_L2_DOUT    // 32

// FC2 输出
#define FC2_OUT_SIZE    CNN_L3_DOUT    // 2

//==================================================================================================================
// 中间缓冲区（SDRAM）
//==================================================================================================================
static float conv1_out[CONV1_OUT_SIZE]  BSP_PLACE_IN_SECTION(".bss.sdram");
static float pool1_out[POOL1_OUT_SIZE]  BSP_PLACE_IN_SECTION(".bss.sdram");
static float conv2_out[CONV2_OUT_SIZE]  BSP_PLACE_IN_SECTION(".bss.sdram");
static float pool2_out[POOL2_OUT_SIZE]  BSP_PLACE_IN_SECTION(".bss.sdram");
static float fc1_out[FC1_OUT_SIZE]      BSP_PLACE_IN_SECTION(".bss.sdram");
static float fc2_out[FC2_OUT_SIZE];

// 归一化后的输入
static float input_float[CNN_INPUT_SIZE] BSP_PLACE_IN_SECTION(".bss.sdram");

//==================================================================================================================
// 内部函数
//==================================================================================================================

// uint8 输入归一化到 [0,1] float
static void normalize_input(const uint8 *src)
{
    for (int i = 0; i < CNN_INPUT_SIZE; i++)
        input_float[i] = (float)src[i] / 255.0f;
}

// Conv2D: stride 版本
// input: [in_h, in_w, in_c], output: [out_h, out_w, out_c]
// weight: [kh, kw, in_c, out_c], bias: [out_c]
static void conv2d_stride(const float *input, int in_h, int in_w, int in_c,
                          float *output, int out_h, int out_w, int out_c,
                          const float *weight, const float *bias,
                          int kh, int kw, int stride)
{
    for (int oh = 0; oh < out_h; oh++)
    {
        for (int ow = 0; ow < out_w; ow++)
        {
            for (int oc = 0; oc < out_c; oc++)
            {
                float sum = bias[oc];
                for (int kh_idx = 0; kh_idx < kh; kh_idx++)
                {
                    for (int kw_idx = 0; kw_idx < kw; kw_idx++)
                    {
                        int ih = oh * stride + kh_idx;
                        int iw = ow * stride + kw_idx;
                        for (int ic = 0; ic < in_c; ic++)
                        {
                            float iv = input[ih * in_w * in_c + iw * in_c + ic];
                            float wv = weight[kh_idx * kw * in_c * out_c
                                            + kw_idx * in_c * out_c
                                            + ic * out_c + oc];
                            sum += iv * wv;
                        }
                    }
                }
                // ReLU
                output[oh * out_w * out_c + ow * out_c + oc] = sum > 0 ? sum : 0;
            }
        }
    }
}

// Conv2D: stride=1 版本
static void conv2d(const float *input, int in_h, int in_w, int in_c,
                   float *output, int out_h, int out_w, int out_c,
                   const float *weight, const float *bias,
                   int kh, int kw)
{
    conv2d_stride(input, in_h, in_w, in_c, output, out_h, out_w, out_c,
                  weight, bias, kh, kw, 1);
}

// MaxPool 2x2, stride 2
static void maxpool2x2(const float *input, int in_h, int in_w, int channels,
                       float *output, int out_h, int out_w)
{
    for (int oh = 0; oh < out_h; oh++)
    {
        for (int ow = 0; ow < out_w; ow++)
        {
            for (int c = 0; c < channels; c++)
            {
                float max_val = -1e30f;
                for (int dh = 0; dh < 2; dh++)
                {
                    for (int dw = 0; dw < 2; dw++)
                    {
                        int ih = oh * 2 + dh;
                        int iw = ow * 2 + dw;
                        float v = input[ih * in_w * channels + iw * channels + c];
                        if (v > max_val) max_val = v;
                    }
                }
                output[oh * out_w * channels + ow * channels + c] = max_val;
            }
        }
    }
}

// 全连接层
static void dense(const float *input, int in_dim, float *output, int out_dim,
                  const float *weight, const float *bias, int relu)
{
    for (int o = 0; o < out_dim; o++)
    {
        float sum = bias[o];
        for (int i = 0; i < in_dim; i++)
        {
            sum += input[i] * weight[i * out_dim + o];
        }
        if (relu && sum < 0) sum = 0;
        output[o] = sum;
    }
}

// Softmax
static void softmax(float *x, int n)
{
    float max_val = x[0];
    for (int i = 1; i < n; i++)
        if (x[i] > max_val) max_val = x[i];

    float sum = 0;
    for (int i = 0; i < n; i++)
    {
        x[i] = x[i] - max_val;
        float e = 1.0f;
        // 简易 exp 近似（足够用于 2 分类）
        float v = x[i];
        for (int j = 0; j < 8; j++)
            e = e * (1.0f + v / (1 << (j + 1)));
        x[i] = e;
        sum += e;
    }
    for (int i = 0; i < n; i++)
        x[i] /= sum;
}

//==================================================================================================================
// 接口
//==================================================================================================================
void cnn_init(void)
{
    // 清零中间缓冲
    for (int i = 0; i < CONV1_OUT_SIZE; i++) conv1_out[i] = 0;
    for (int i = 0; i < POOL1_OUT_SIZE; i++) pool1_out[i] = 0;
    for (int i = 0; i < CONV2_OUT_SIZE; i++) conv2_out[i] = 0;
    for (int i = 0; i < POOL2_OUT_SIZE; i++) pool2_out[i] = 0;
    for (int i = 0; i < FC1_OUT_SIZE; i++) fc1_out[i] = 0;
    for (int i = 0; i < FC2_OUT_SIZE; i++) fc2_out[i] = 0;
}

uint8 cnn_classify(const uint8 *input, float *confidence)
{
    // 1. 归一化输入
    normalize_input(input);

    // 2. Conv1: 32x64x1 -> 14x30x8 (stride 2, 5x5 kernel)
    conv2d_stride(input_float, CNN_INPUT_H, CNN_INPUT_W, 1,
                  conv1_out, CONV1_OUT_H, CONV1_OUT_W, CONV1_OUT_C,
                  (const float *)cnn_w0, cnn_b0, CNN_L0_KH, CNN_L0_KW, 2);

    // 3. Pool1: 14x30x8 -> 7x15x8
    maxpool2x2(conv1_out, CONV1_OUT_H, CONV1_OUT_W, CONV1_OUT_C,
               pool1_out, POOL1_OUT_H, POOL1_OUT_W);

    // 4. Conv2: 7x15x8 -> 5x13x16 (stride 1, 3x3 kernel)
    conv2d(pool1_out, POOL1_OUT_H, POOL1_OUT_W, CONV1_OUT_C,
           conv2_out, CONV2_OUT_H, CONV2_OUT_W, CONV2_OUT_C,
           (const float *)cnn_w1, cnn_b1, CNN_L1_KH, CNN_L1_KW);

    // 5. Pool2: 5x13x16 -> 2x6x16
    maxpool2x2(conv2_out, CONV2_OUT_H, CONV2_OUT_W, CONV2_OUT_C,
               pool2_out, POOL2_OUT_H, POOL2_OUT_W);

    // 6. FC1: 192 -> 32 + ReLU
    dense(pool2_out, POOL2_OUT_SIZE, fc1_out, FC1_OUT_SIZE,
          (const float *)cnn_w2, cnn_b2, 1);

    // 7. FC2: 32 -> 2
    dense(fc1_out, FC1_OUT_SIZE, fc2_out, FC2_OUT_SIZE,
          (const float *)cnn_w3, cnn_b3, 0);

    // 8. Return by logit margin. This avoids approximate softmax producing invalid probabilities.
    float margin = fc2_out[1] - fc2_out[0];
    float abs_margin = (margin >= 0.0f) ? margin : -margin;
    uint8 result = (margin > 0.0f) ? CNN_FATIGUED : CNN_NORMAL;
    *confidence = 0.5f + abs_margin / (2.0f * (1.0f + abs_margin));
    return result;
}
