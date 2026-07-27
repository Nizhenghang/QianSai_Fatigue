/*********************************************************************************************************************
* RA8D1 AI 疲劳驾驶检测模块
* CNN 推理 + 时序投票 + 报警输���
********************************************************************************************************************/
#include "fatigue_detect.h"
#include "cnn_inference.h"
#include "hmi_display.h"
#include "voice_assistant.h"

//==================================================================================================================
// 内部状态
//==================================================================================================================
static volatile fatigue_state_enum fatigue_state    = FATIGUE_STATE_MONITORING;
static          uint32             alarm_frame_cnt  = 0;
static          uint32             frame_counter    = 0;
static          uint8              led_toggle       = 0;
static          fatigue_diag_struct diag_info;

// CNN 输入缓冲（64x32 缩放后的图像）
static          uint8              cnn_input[CNN_INPUT_SIZE] BSP_PLACE_IN_SECTION(".bss.sdram");

// 时序投票窗口
static          uint8              vote_buf[CNN_SMOOTH_N];
static          uint8              vote_idx        = 0;
static          uint8              vote_filled     = 0;
static          uint8              fatigued_count  = 0;
static          uint8              raw_fatigued_streak = 0;
static          uint16             target_lost_count = 0;
static          uint8              target_lost_alerted = 0;
static          uint16             fatigue_score = 0;

//==================================================================================================================
// 内部函数
//==================================================================================================================

// 从 mt9v03x_image[] 裁剪 ROI 并缩放到 64x32
static void crop_and_resize(void)
{
    uint16 roi_w = FATIGUE_ROI_RIGHT - FATIGUE_ROI_LEFT;   // 160
    uint16 roi_h = FATIGUE_ROI_BOTTOM - FATIGUE_ROI_TOP;   // 100

    for (uint16 dy = 0; dy < CNN_INPUT_H; dy++)
    {
        uint16 src_row = FATIGUE_ROI_TOP + (dy * roi_h / CNN_INPUT_H);
        uint16 row_off = src_row * MT9V03X_W;
        for (uint16 dx = 0; dx < CNN_INPUT_W; dx++)
        {
            uint16 src_col = FATIGUE_ROI_LEFT + (dx * roi_w / CNN_INPUT_W);
            cnn_input[dy * CNN_INPUT_W + dx] = mt9v03x_image[row_off + src_col];
        }
    }
}

// 更新投票窗口
static void update_vote(uint8 is_fatigued)
{
    // 移出旧值
    if (vote_buf[vote_idx])
        fatigued_count--;

    // 写入新值
    vote_buf[vote_idx] = is_fatigued;
    if (is_fatigued)
        fatigued_count++;

    vote_idx++;
    if (vote_idx >= CNN_SMOOTH_N)
    {
        vote_idx = 0;
        vote_filled = 1;
    }
}

static void reset_vote(void)
{
    fatigued_count = 0;
    vote_idx = 0;
    vote_filled = 0;
    raw_fatigued_streak = 0;
    for (uint8 i = 0; i < CNN_SMOOTH_N; i++)
        vote_buf[i] = 0;
}

static void update_fatigue_score(uint8 risk_frame)
{
    if (risk_frame)
    {
        if (fatigue_score + FATIGUE_SCORE_RISE >= FATIGUE_SCORE_MAX)
            fatigue_score = FATIGUE_SCORE_MAX;
        else
            fatigue_score += FATIGUE_SCORE_RISE;
    }
    else
    {
        if (fatigue_score > FATIGUE_SCORE_DECAY)
            fatigue_score -= FATIGUE_SCORE_DECAY;
        else
            fatigue_score = 0;
    }
}

static uint8 is_target_valid(void)
{
    uint32 sum = 0;
    uint8 min_val = 255;
    uint8 max_val = 0;

    for (uint16 i = 0; i < CNN_INPUT_SIZE; i++)
    {
        uint8 pixel = cnn_input[i];
        sum += pixel;
        if (pixel < min_val) min_val = pixel;
        if (pixel > max_val) max_val = pixel;
    }

    uint8 mean = (uint8)(sum / CNN_INPUT_SIZE);
    uint8 range = max_val - min_val;
    return (mean >= TARGET_MIN_MEAN) && (mean <= TARGET_MAX_MEAN) && (range >= TARGET_MIN_RANGE);
}


//==================================================================================================================
// 接口
//==================================================================================================================
void fatigue_init(void)
{
    fatigue_state    = FATIGUE_STATE_MONITORING;
    alarm_frame_cnt  = 0;
    frame_counter    = 0;
    led_toggle       = 0;
    fatigued_count   = 0;
    vote_idx         = 0;
    vote_filled      = 0;
    target_lost_count = 0;
    target_lost_alerted = 0;
    fatigue_score    = 0;

    reset_vote();

    cnn_init();
    hmi_init();
    voice_assistant_init();

    gpio_low(LED1); gpio_low(LED2); gpio_low(LED3); gpio_low(LED4);

    printf("AI Fatigue Detection v8.0\r\n");
    printf("CNN: %dx%d input, 2-class output\r\n", CNN_INPUT_W, CNN_INPUT_H);
    printf("ROI: (%d,%d)-(%d,%d)\r\n",
           FATIGUE_ROI_LEFT, FATIGUE_ROI_TOP,
           FATIGUE_ROI_RIGHT, FATIGUE_ROI_BOTTOM);
    printf("Vote: %d frames, %d%% threshold\r\n",
           CNN_SMOOTH_N, CNN_ALARM_RATIO);
    printf("Monitoring...\r\n");
}

void fatigue_process_frame(void)
{
    if (!mt9v03x_finish_flag)
        return;
    mt9v03x_finish_flag = 0;

    g_ceu_mt9v03x.p_api->captureStart(g_ceu_mt9v03x.p_ctrl, (uint8*)mt9v03x_image);
    R_CEU->CAPCR = R_CEU_CAPCR_CTNCP_Msk;

    frame_counter++;

    // 裁剪缩放
    crop_and_resize();

    if (!is_target_valid())
    {
        target_lost_count++;
        reset_vote();
        fatigue_score = 0;
        if (FATIGUE_STATE_ALARM == fatigue_state)
        {
            voice_assistant_silence();
            alarm_frame_cnt = 0;
            fatigue_state = FATIGUE_STATE_MONITORING;
        }

        diag_info.roi_w             = 0;
        diag_info.roi_h             = 0;
        diag_info.cnn_result        = CNN_NORMAL;
        diag_info.cnn_confidence    = 0;
        diag_info.fatigued_count    = 0;
        diag_info.fatigued_ratio    = 0;

        if (target_lost_count >= TARGET_LOST_FRAME_THR)
        {
            if (frame_counter % (FATIGUE_FPS / 4) == 0)
            {
                led_toggle = !led_toggle;
                if (led_toggle)
                {
                    gpio_high(LED2); gpio_high(LED4);
                }
                else
                {
                    gpio_low(LED2); gpio_low(LED4);
                }
            }

            if (frame_counter % FATIGUE_FPS == 0)
            {
                printf("NO_TARGET lost=%d frames\r\n", target_lost_count);
            }

            if (!target_lost_alerted)
            {
                target_lost_alerted = 1;
                hmi_show_alert(HMI_FACE_LOST, "请调整摄像头或回到检测区域");
            }
        }
        hmi_update_status(&diag_info, fatigue_state);
        return;
    }

    if (target_lost_count >= TARGET_LOST_FRAME_THR)
    {
        printf("TARGET_BACK lost=%d frames\r\n", target_lost_count);
        gpio_low(LED2); gpio_low(LED4);
    }
    target_lost_count = 0;
    target_lost_alerted = 0;

    // CNN 推理
    float confidence = 0;
    uint8 result = cnn_classify(cnn_input, &confidence);
    uint8 vote_result = ((result == CNN_FATIGUED) && (confidence >= CNN_FATIGUE_CONF_THRESH)) ? 1 : 0;
    if (vote_result)
        raw_fatigued_streak++;
    else
        raw_fatigued_streak = 0;

    switch (fatigue_state)
    {
    case FATIGUE_STATE_MONITORING:
    {
        if (frame_counter <= FATIGUE_WARMUP_FRAMES)
        {
            diag_info.roi_w             = FATIGUE_ROI_RIGHT - FATIGUE_ROI_LEFT;
            diag_info.roi_h             = FATIGUE_ROI_BOTTOM - FATIGUE_ROI_TOP;
            diag_info.cnn_result        = vote_result ? CNN_FATIGUED : CNN_NORMAL;
            diag_info.cnn_confidence    = confidence;
            diag_info.fatigued_count    = 0;
            diag_info.fatigued_ratio    = 0;

            if (frame_counter % FATIGUE_FPS == 0)
            {
                printf("WARMUP CNN=%d conf=%.2f raw=%s\r\n",
                       result, confidence,
                       result ? "FATIGUED" : "normal");
            }
            break;
        }

        // 时序投票
        update_vote(vote_result);
        update_fatigue_score(vote_result);

        uint8 total = vote_filled ? CNN_SMOOTH_N : vote_idx;
        uint8 ratio = (total > 0) ? (fatigued_count * 100 / total) : 0;

        if ((fatigue_score >= FATIGUE_DANGER_SCORE) &&
            ((ratio >= CNN_ALARM_RATIO && total >= CNN_SMOOTH_N) ||
             (raw_fatigued_streak >= RAW_FATIGUE_STREAK_THR)))
        {
            fatigue_state   = FATIGUE_STATE_ALARM;
            alarm_frame_cnt = 0;
            printf("!!! FATIGUE !!! CNN=%d conf=%.2f vote=%d/%d=%d%% score=%d gate=%d streak=%d\r\n",
                   result, confidence, fatigued_count, total, ratio, fatigue_score, vote_result, raw_fatigued_streak);
            hmi_show_alert(HMI_LEVEL_DANGER, "严重疲劳，请立即停车休息");
        }

        // 诊断
        diag_info.roi_w             = FATIGUE_ROI_RIGHT - FATIGUE_ROI_LEFT;
        diag_info.roi_h             = FATIGUE_ROI_BOTTOM - FATIGUE_ROI_TOP;
        diag_info.cnn_result        = vote_result ? CNN_FATIGUED : CNN_NORMAL;
        diag_info.cnn_confidence    = confidence;
        diag_info.fatigued_count    = fatigued_count;
        diag_info.fatigued_ratio    = (uint8)((fatigue_score * 100u) / FATIGUE_SCORE_MAX);

        if (frame_counter % (FATIGUE_FPS / 4) == 0)
        {
            printf("CNN=%d conf=%.2f gate=%d streak=%d vote=%d/%d=%d%% score=%d %s\r\n",
                   result, confidence,
                   vote_result,
                   raw_fatigued_streak,
                   fatigued_count, total, ratio,
                   fatigue_score,
                   vote_result ? "FATIGUED" : "normal");
        }
        hmi_update_status(&diag_info, fatigue_state);
        hmi_process_command();
        break;
    }

    case FATIGUE_STATE_ALARM:
        alarm_frame_cnt++;

        led_toggle = !led_toggle;
        if (led_toggle)
        {
            gpio_high(LED1); gpio_high(LED3);
            gpio_low(LED2);  gpio_low(LED4);
        }
        else
        {
            gpio_low(LED1);  gpio_low(LED3);
            gpio_high(LED2); gpio_high(LED4);
        }

        if (alarm_frame_cnt >= ALARM_FRAME_THR)
        {
            voice_assistant_silence();
            gpio_low(LED1); gpio_low(LED2);
            gpio_low(LED3); gpio_low(LED4);
            reset_vote();
            fatigue_score = 0;
            fatigue_state = FATIGUE_STATE_MONITORING;
            printf("Alarm ended.\r\n");
        }
        hmi_update_status(&diag_info, fatigue_state);
        hmi_process_command();
        break;
    }
}

fatigue_state_enum fatigue_get_state(void)
{
    return fatigue_state;
}

const fatigue_diag_struct* fatigue_get_diag(void)
{
    return &diag_info;
}
