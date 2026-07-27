/*********************************************************************************************************************
* Local DAC audio player for fatigue voice prompts.
*
* Hardware path from the RA8D1 AI KIT schematic:
* P014 DAC -> NS4150B INP, P015 CTRL -> NS4150B enable, VOP/VON -> speaker.
********************************************************************************************************************/
#include "audio_player.h"
#include "audio_samples.h"
#include "zf_common_headfile.h"

#define AUDIO_DAC_CENTER       2048u
#define AUDIO_DAC_GAIN         14
#define AUDIO_SAMPLE_PERIOD_US (1000000u / AUDIO_SAMPLE_RATE_HZ)

static uint8 audio_ready = 0;

static void audio_player_play_pcm(const uint8 *samples, uint32 length)
{
    if (!audio_ready || 0 == samples || 0 == length)
        return;

    for (uint32 i = 0; i < length; i++)
    {
        int32 centered = ((int32)samples[i] - 128) * AUDIO_DAC_GAIN;
        int32 value = (int32)AUDIO_DAC_CENTER + centered;

        if (value < 0)
            value = 0;
        else if (value > 4095)
            value = 4095;

        dac_out((uint16)value);
        system_delay_us(AUDIO_SAMPLE_PERIOD_US);
    }

    audio_player_silence();
}

void audio_player_init(void)
{
    dac_init();
    audio_ready = 1;
    audio_player_silence();
}

void audio_player_silence(void)
{
    dac_out(AUDIO_DAC_CENTER);
}

void audio_player_play_level(hmi_fatigue_level_enum level)
{
    switch (level)
    {
    case HMI_FACE_LOST:
        audio_player_play_pcm(audio_face_lost, AUDIO_FACE_LOST_LEN);
        break;

    case HMI_LEVEL_ATTENTION:
        audio_player_play_pcm(audio_attention, AUDIO_ATTENTION_LEN);
        break;

    case HMI_LEVEL_WARNING:
        audio_player_play_pcm(audio_warning, AUDIO_WARNING_LEN);
        break;

    case HMI_LEVEL_DANGER:
        audio_player_play_pcm(audio_danger, AUDIO_DANGER_LEN);
        break;

    case HMI_LEVEL_NORMAL:
    default:
        audio_player_silence();
        break;
    }
}
