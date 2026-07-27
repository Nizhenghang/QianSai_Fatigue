from pathlib import Path
import re


ROOT = Path(__file__).resolve().parents[1]
FATIGUE_H = ROOT / "E18_usb_cdc_demo" / "code" / "fatigue_detect.h"
FATIGUE_C = ROOT / "E18_usb_cdc_demo" / "code" / "fatigue_detect.c"
HMI_C = ROOT / "E18_usb_cdc_demo" / "code" / "hmi_display.c"


def require(condition, message):
    if not condition:
        raise SystemExit(message)


def main():
    fatigue_h = FATIGUE_H.read_text(encoding="utf-8", errors="ignore")
    fatigue_c = FATIGUE_C.read_text(encoding="utf-8", errors="ignore")
    hmi_c = HMI_C.read_text(encoding="utf-8", errors="ignore")

    require("#define CNN_SMOOTH_N         40" in fatigue_h, "vote window should cover about 0.5s")
    require("#define CNN_ALARM_RATIO      65" in fatigue_h, "alarm candidate ratio should be 65 percent")
    require("#define CNN_FATIGUE_CONF_THRESH 0.65f" in fatigue_h, "fatigue confidence threshold should be 0.65")
    require("#define RAW_FATIGUE_STREAK_THR  32" in fatigue_h, "raw fatigue streak should require about 0.4s")
    require("#define FATIGUE_SCORE_MAX      240" in fatigue_h, "fatigue score max should be defined")
    require("#define FATIGUE_LIGHT_SCORE    45" in fatigue_h, "light fatigue score threshold should be defined")
    require("#define FATIGUE_WARNING_SCORE  72" in fatigue_h, "warning fatigue score threshold should be lowered")
    require("#define FATIGUE_DANGER_SCORE   120" in fatigue_h, "danger fatigue score threshold should be lowered")
    require("#define FATIGUE_SCORE_RISE     2" in fatigue_h, "fatigue score rise step should be defined")
    require("#define FATIGUE_SCORE_DECAY    2" in fatigue_h, "fatigue score decay step should be defined")

    require(
        "if (vote_result)\n        raw_fatigued_streak++;" in fatigue_c,
        "raw fatigue streak must count confidence-gated fatigue frames",
    )
    require("static          uint16             fatigue_score = 0;" in fatigue_c, "fatigue score state should exist")
    require("static void update_fatigue_score(uint8 risk_frame)" in fatigue_c, "fatigue score updater should exist")
    require(
        "fatigue_score += FATIGUE_SCORE_RISE;" in fatigue_c,
        "risk frames should raise fatigue score faster than before",
    )
    require(
        "fatigue_score -= FATIGUE_SCORE_DECAY;" in fatigue_c,
        "normal frames should decay fatigue score by a named step",
    )
    require(
        "diag_info.cnn_result        = vote_result ? CNN_FATIGUED : CNN_NORMAL;" in fatigue_c,
        "diagnostic cnn_result should expose gated result to HMI",
    )
    require(
        "diag_info.fatigued_ratio    = (uint8)((fatigue_score * 100u) / FATIGUE_SCORE_MAX);" in fatigue_c,
        "HMI fatigue ratio should come from temporal fatigue score",
    )
    require(
        "fatigue_score >= FATIGUE_DANGER_SCORE" in fatigue_c,
        "alarm should require danger fatigue score",
    )
    require(
        'hmi_show_alert(HMI_FACE_LOST, "请调整摄像头或回到检测区域");' in fatigue_c,
        "missing face should trigger a clear face-lost voice alert",
    )
    require(
        "if (FATIGUE_STATE_ALARM == fatigue_state)" in fatigue_c
        and "fatigue_state = FATIGUE_STATE_MONITORING;" in fatigue_c,
        "target loss should clear a stale severe-fatigue alarm state",
    )
    show_alert = re.search(
        r"void hmi_show_alert\(hmi_fatigue_level_enum level, const char \*message\)\s*\{(?P<body>.*?)\n\}",
        hmi_c,
        re.S,
    )
    require(show_alert is not None, "hmi_show_alert should exist")
    require(
        "last_sent_level = level;" in show_alert.group("body")
        and "alert_cooldown = HMI_ALERT_REPEAT_FRAMES;" in show_alert.group("body"),
        "explicit HMI alerts should start cooldown to avoid duplicate voice playback",
    )
    require(
        "diag->fatigued_ratio >= FATIGUE_DANGER_RATIO" in hmi_c,
        "HMI danger level should use named danger ratio",
    )

    print("fatigue logic checks passed")


if __name__ == "__main__":
    main()
