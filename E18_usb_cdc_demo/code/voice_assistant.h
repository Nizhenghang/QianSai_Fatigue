/*********************************************************************************************************************
* Voice assistant bridge
********************************************************************************************************************/
#ifndef _VOICE_ASSISTANT_H_
#define _VOICE_ASSISTANT_H_

#include "hmi_display.h"

void voice_assistant_init(void);
void voice_assistant_say(hmi_fatigue_level_enum level, const char *message);
void voice_assistant_send_status(hmi_fatigue_level_enum level, const fatigue_diag_struct *diag);
void voice_assistant_silence(void);

#endif
