/*********************************************************************************************************************
* RA8D1 图像导出模块
* 从 mt9v03x_image[] 裁剪 ROI，最近邻缩放到 64x32，串口发送
********************************************************************************************************************/
#include "image_export.h"

#define HEADER_SIZE 5
#define FOOTER_SIZE 2

static uint8  export_buf[HEADER_SIZE + EXPORT_OUT_SIZE + FOOTER_SIZE];
static uint16 export_seq = 0;
static uint16 frame_skip_cnt = 0;

// 最近邻缩放：将 ROI 区域缩放到 EXPORT_OUT_W x EXPORT_OUT_H
static void crop_and_resize(void)
{
    uint16 roi_w = EXPORT_ROI_RIGHT - EXPORT_ROI_LEFT;   // 160
    uint16 roi_h = EXPORT_ROI_BOTTOM - EXPORT_ROI_TOP;   // 100

    for (uint16 dy = 0; dy < EXPORT_OUT_H; dy++)
    {
        // 源行号
        uint16 src_row = EXPORT_ROI_TOP + (dy * roi_h / EXPORT_OUT_H);
        uint16 row_offset = src_row * MT9V03X_W;

        for (uint16 dx = 0; dx < EXPORT_OUT_W; dx++)
        {
            // 源列号
            uint16 src_col = EXPORT_ROI_LEFT + (dx * roi_w / EXPORT_OUT_W);
            export_buf[dy * EXPORT_OUT_W + dx] = mt9v03x_image[row_offset + src_col];
        }
    }
}

void image_export_init(void)
{
    export_seq = 0;
    frame_skip_cnt = 0;

    printf("Image Export Mode\r\n");
    printf("ROI: (%d,%d)-(%d,%d) -> %dx%d\r\n",
           EXPORT_ROI_LEFT, EXPORT_ROI_TOP,
           EXPORT_ROI_RIGHT, EXPORT_ROI_BOTTOM,
           EXPORT_OUT_W, EXPORT_OUT_H);
    printf("Label: %d (%s)\r\n",
           EXPORT_LABEL, EXPORT_LABEL ? "FATIGUED" : "NORMAL");
    printf("Exporting...\r\n");
}

void image_export_send_frame(void)
{
    if (!mt9v03x_finish_flag)
        return;
    mt9v03x_finish_flag = 0;

    g_ceu_mt9v03x.p_api->captureStart(g_ceu_mt9v03x.p_ctrl, (uint8*)mt9v03x_image);
    R_CEU->CAPCR = R_CEU_CAPCR_CTNCP_Msk;

    // 帧率限制
    frame_skip_cnt++;
    if (frame_skip_cnt < EXPORT_FRAME_SKIP)
        return;
    frame_skip_cnt = 0;

    // 裁剪+缩放到缓冲区偏移 HEADER_SIZE 处
    // 先填充帧头
    export_buf[0] = 0xAA;
    export_buf[1] = 0x55;
    export_buf[2] = (uint8)(export_seq >> 8);
    export_buf[3] = (uint8)(export_seq & 0xFF);
    export_buf[4] = EXPORT_LABEL;

    // 缩放图像到 export_buf + 5
    uint16 roi_w = EXPORT_ROI_RIGHT - EXPORT_ROI_LEFT;
    uint16 roi_h = EXPORT_ROI_BOTTOM - EXPORT_ROI_TOP;
    for (uint16 dy = 0; dy < EXPORT_OUT_H; dy++)
    {
        uint16 src_row = EXPORT_ROI_TOP + (dy * roi_h / EXPORT_OUT_H);
        uint16 row_offset = src_row * MT9V03X_W;
        for (uint16 dx = 0; dx < EXPORT_OUT_W; dx++)
        {
            uint16 src_col = EXPORT_ROI_LEFT + (dx * roi_w / EXPORT_OUT_W);
            export_buf[HEADER_SIZE + dy * EXPORT_OUT_W + dx] = mt9v03x_image[row_offset + src_col];
        }
    }

    // 帧尾
    export_buf[HEADER_SIZE + EXPORT_OUT_SIZE] = 0x0D;
    export_buf[HEADER_SIZE + EXPORT_OUT_SIZE + 1] = 0x0A;

    export_seq++;

    // 一次性发送整帧
    debug_write_buffer(export_buf, HEADER_SIZE + EXPORT_OUT_SIZE + FOOTER_SIZE);

    // 等待发送完成（2055字节 @ 115200bps ≈ 180ms）
    system_delay_ms(200);
}
