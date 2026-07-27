/*********************************************************************************************************************
* Fatigue HMI display and command interface
********************************************************************************************************************/
#ifndef _HMI_DISPLAY_H_
#define _HMI_DISPLAY_H_

#include "fatigue_detect.h"

typedef enum
{
    HMI_FACE_LOST = 0,
    HMI_LEVEL_NORMAL = 1,
    HMI_LEVEL_ATTENTION = 2,
    HMI_LEVEL_WARNING = 3,
    HMI_LEVEL_DANGER = 4
} hmi_fatigue_level_enum;

void hmi_init(void);
void hmi_update_status(const fatigue_diag_struct *diag, fatigue_state_enum state);
void hmi_show_alert(hmi_fatigue_level_enum level, const char *message);
void hmi_process_command(void);
hmi_fatigue_level_enum hmi_get_level(void);
const char *hmi_get_level_text(hmi_fatigue_level_enum level);
const char *hmi_get_advice_text(hmi_fatigue_level_enum level);

#endif
