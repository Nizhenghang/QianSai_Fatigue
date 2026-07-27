/*********************************************************************************************************************
* Voice assistant bridge
*
* The RA8D1 plays local voice prompts through the onboard DAC speaker path.
* Debug text events are kept so the serial log still shows what was spoken.
********************************************************************************************************************/
#include "voice_assistant.h"
#include "audio_player.h"
#include "zf_common_headfile.h"

#define VOICE_WIFI_SSID      "YOUR_WIFI_SSID"
#define VOICE_WIFI_PASSWORD  "YOUR_WIFI_PASSWORD"
#define VOICE_USE_WIFI       0

static uint8 voice_ready = 0;

static void voice_send_line(const char *line)
{
#if VOICE_USE_WIFI
    if (voice_ready)
        wifi_uart_send_buffer((const uint8_t *)line, strlen(line));
#else
    printf("%s", line);
#endif
}

void voice_assistant_init(void)
{
    voice_ready = 0;
    audio_player_init();

#if VOICE_USE_WIFI
    if (0 == wifi_uart_init(VOICE_WIFI_SSID, VOICE_WIFI_PASSWORD, WIFI_UART_STATION))
    {
        voice_ready = 1;
        voice_send_line("VOICE|BOOT|wifi_ready\r\n");
    }
#else
    voice_ready = 1;
    voice_send_line("VOICE|BOOT|uart_bridge_ready\r\n");
#endif
}

void voice_assistant_say(hmi_fatigue_level_enum level, const char *message)
{
    char buffer[192];
    if (voice_ready)
    {
        zf_sprintf((int8 *)buffer, (const int8 *)"VOICE|SAY|level=%d|text=%s\r\n", (int)level, message);
        voice_send_line(buffer);
    }

    audio_player_play_level(level);
}

void voice_assistant_send_status(hmi_fatigue_level_enum level, const fatigue_diag_struct *diag)
{
    char buffer[160];
    if (!voice_ready)
        return;

    zf_sprintf((int8 *)buffer, (const int8 *)"VOICE|STATE|level=%d|ratio=%d|conf=%d|cnn=%d\r\n",
               (int)level,
               (int)diag->fatigued_ratio,
               (int)(diag->cnn_confidence * 100.0f),
               (int)diag->cnn_result);
    voice_send_line(buffer);
}

void voice_assistant_silence(void)
{
    audio_player_silence();
}
