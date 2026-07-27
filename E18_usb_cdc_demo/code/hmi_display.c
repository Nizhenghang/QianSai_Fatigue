/*********************************************************************************************************************
* Fatigue Bluetooth display bridge
*
* UART3 is used as a one-way HC-05 serial link. The phone app receives ASCII
* text frames and renders the current driver state.
********************************************************************************************************************/
#include "hmi_display.h"
#include "cnn_inference.h"
#include "voice_assistant.h"

#define HMI_UPDATE_DIVIDER       20
#define HMI_ALERT_REPEAT_FRAMES  (FATIGUE_FPS * 3)
#define HMI_DEBUG_TEXT_PROTOCOL  1

static hmi_fatigue_level_enum current_level = HMI_LEVEL_NORMAL;
static hmi_fatigue_level_enum last_sent_level = HMI_FACE_LOST;
static uint32 update_counter = 0;
static uint32 alert_cooldown = 0;

static void hmi_send_buffer(const char *buf, uint32 len)
{
    uart3_write_buffer((const uint8 *)buf, len);
}

static void hmi_send_boot(void)
{
    char buffer[64];
    uint32 len = zf_sprintf((int8 *)buffer, (const int8 *)"BT|BOOT|name=FatigueMonitor|baud=9600\r\n");
    hmi_send_buffer(buffer, len);
}

static hmi_fatigue_level_enum calc_level(const fatigue_diag_struct *diag, fatigue_state_enum state)
{
    if ((0 == diag->roi_w) || (0 == diag->roi_h))
        return HMI_FACE_LOST;

    if (FATIGUE_STATE_ALARM == state)
        return HMI_LEVEL_DANGER;

    if (diag->fatigued_ratio >= FATIGUE_DANGER_RATIO)
        return HMI_LEVEL_DANGER;
    if (diag->fatigued_ratio >= FATIGUE_WARNING_RATIO)
        return HMI_LEVEL_WARNING;
    if (diag->fatigued_ratio >= FATIGUE_LIGHT_RATIO)
        return HMI_LEVEL_ATTENTION;
    return HMI_LEVEL_NORMAL;
}

const char *hmi_get_level_text(hmi_fatigue_level_enum level)
{
    switch (level)
    {
    case HMI_FACE_LOST:       return "NO_FACE";
    case HMI_LEVEL_NORMAL:    return "NORMAL";
    case HMI_LEVEL_ATTENTION: return "LIGHT";
    case HMI_LEVEL_WARNING:   return "WARN";
    case HMI_LEVEL_DANGER:    return "DANGER";
    default:                  return "UNKNOWN";
    }
}

const char *hmi_get_advice_text(hmi_fatigue_level_enum level)
{
    switch (level)
    {
    case HMI_FACE_LOST:       return "Face lost, align camera";
    case HMI_LEVEL_NORMAL:    return "Driver state is stable";
    case HMI_LEVEL_ATTENTION: return "Attention dropping";
    case HMI_LEVEL_WARNING:   return "Fatigue trend detected";
    case HMI_LEVEL_DANGER:    return "Severe fatigue alarm";
    default:                  return "Check driver status";
    }
}

static void hmi_push_status(hmi_fatigue_level_enum level, const fatigue_diag_struct *diag, fatigue_state_enum state)
{
    char buffer[128];
    uint32 len = zf_sprintf((int8 *)buffer,
                            (const int8 *)"BT|STATE|level=%d|status=%s|score=%d|conf=%d|cnn=%d|state=%d\r\n",
                            (int)level,
                            hmi_get_level_text(level),
                            (int)diag->fatigued_ratio,
                            (int)(diag->cnn_confidence * 100.0f),
                            (int)diag->cnn_result,
                            (int)state);
    hmi_send_buffer(buffer, len);

#if HMI_DEBUG_TEXT_PROTOCOL
    buffer[len] = '\0';
    printf("%s", buffer);
#endif
}

void hmi_init(void)
{
    uart3_init();
    current_level = HMI_LEVEL_NORMAL;
    last_sent_level = HMI_FACE_LOST;
    update_counter = 0;
    alert_cooldown = 0;

    hmi_send_boot();

#if HMI_DEBUG_TEXT_PROTOCOL
    printf("BT|BOOT|hc05_display_ready\r\n");
#endif
}

void hmi_show_alert(hmi_fatigue_level_enum level, const char *message)
{
    char buffer[128];
    uint32 len;

    current_level = level;
    last_sent_level = level;
    alert_cooldown = HMI_ALERT_REPEAT_FRAMES;

    len = zf_sprintf((int8 *)buffer,
                     (const int8 *)"BT|ALERT|level=%d|status=%s|score=%d|advice=%s\r\n",
                     (int)level,
                     hmi_get_level_text(level),
                     HMI_LEVEL_DANGER == level ? 100 : 0,
                     hmi_get_advice_text(level));
    hmi_send_buffer(buffer, len);

#if HMI_DEBUG_TEXT_PROTOCOL
    buffer[len] = '\0';
    printf("%s", buffer);
#endif
    voice_assistant_say(level, message);
}

void hmi_update_status(const fatigue_diag_struct *diag, fatigue_state_enum state)
{
    hmi_fatigue_level_enum level = calc_level(diag, state);

    current_level = level;
    update_counter++;
    if (alert_cooldown > 0)
        alert_cooldown--;

    if ((0 != (update_counter % HMI_UPDATE_DIVIDER)) && (level == last_sent_level))
        return;

    last_sent_level = level;
    hmi_push_status(level, diag, state);

    if (((HMI_LEVEL_ATTENTION == level) || (HMI_LEVEL_WARNING == level) ||
         (HMI_LEVEL_DANGER == level) || (HMI_FACE_LOST == level)) &&
        (0 == alert_cooldown))
    {
        alert_cooldown = HMI_ALERT_REPEAT_FRAMES;
        voice_assistant_say(level, hmi_get_advice_text(level));
    }
}

void hmi_process_command(void)
{
    /* The current serial screen is display-only. */
}

hmi_fatigue_level_enum hmi_get_level(void)
{
    return current_level;
}
