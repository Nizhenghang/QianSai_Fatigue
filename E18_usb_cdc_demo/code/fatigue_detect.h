/*********************************************************************************************************************
* RA8D1 AI fatigue detection module
* CNN inference + temporal smoothing + alarm output
********************************************************************************************************************/
#ifndef _FATIGUE_DETECT_H_
#define _FATIGUE_DETECT_H_

#include "zf_common_headfile.h"

#define FATIGUE_ROI_LEFT     80
#define FATIGUE_ROI_RIGHT    240
#define FATIGUE_ROI_TOP      10
#define FATIGUE_ROI_BOTTOM   110

#define CNN_SMOOTH_N         40
#define CNN_ALARM_RATIO      65
#define CNN_FATIGUE_CONF_THRESH 0.65f
#define RAW_FATIGUE_STREAK_THR  32
#define FATIGUE_SCORE_MAX      240
#define FATIGUE_LIGHT_SCORE    60
#define FATIGUE_WARNING_SCORE  90
#define FATIGUE_DANGER_SCORE   120
#define FATIGUE_SCORE_RISE     2
#define FATIGUE_SCORE_DECAY    2
#define FATIGUE_LIGHT_RATIO    ((FATIGUE_LIGHT_SCORE * 100) / FATIGUE_SCORE_MAX)
#define FATIGUE_WARNING_RATIO  ((FATIGUE_WARNING_SCORE * 100) / FATIGUE_SCORE_MAX)
#define FATIGUE_DANGER_RATIO   ((FATIGUE_DANGER_SCORE * 100) / FATIGUE_SCORE_MAX)
#define FATIGUE_WARMUP_FRAMES  (FATIGUE_FPS / 2)
#define TARGET_LOST_FRAME_THR  (FATIGUE_FPS / 2)
#define TARGET_MIN_MEAN        8
#define TARGET_MAX_MEAN        245
#define TARGET_MIN_RANGE       18
#define FATIGUE_ALARM_MS       5000
#define FATIGUE_FPS            80

#define ALARM_FRAME_THR        (FATIGUE_ALARM_MS * FATIGUE_FPS / 1000)

typedef enum
{
    FATIGUE_STATE_MONITORING = 0,
    FATIGUE_STATE_ALARM
} fatigue_state_enum;

typedef struct
{
    uint16 roi_w, roi_h;
    uint8  cnn_result;
    float  cnn_confidence;
    uint8  fatigued_count;
    uint8  fatigued_ratio;
} fatigue_diag_struct;

void                fatigue_init            (void);
void                fatigue_process_frame   (void);
fatigue_state_enum  fatigue_get_state       (void);
const fatigue_diag_struct* fatigue_get_diag (void);

#endif