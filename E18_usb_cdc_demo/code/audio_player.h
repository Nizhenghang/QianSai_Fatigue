/*********************************************************************************************************************
* Local DAC audio player for the RA8D1 speaker amplifier.
********************************************************************************************************************/
#ifndef _AUDIO_PLAYER_H_
#define _AUDIO_PLAYER_H_

#include "hmi_display.h"

void audio_player_init(void);
void audio_player_silence(void);
void audio_player_play_level(hmi_fatigue_level_enum level);

#endif
