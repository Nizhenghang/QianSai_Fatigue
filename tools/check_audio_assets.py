from pathlib import Path
import re


ROOT = Path(__file__).resolve().parents[1]
HEADER = ROOT / "E18_usb_cdc_demo" / "code" / "audio_samples.h"


def require(condition, message):
    if not condition:
        raise SystemExit(message)


def main():
    require(HEADER.exists(), f"missing generated header: {HEADER}")
    text = HEADER.read_text(encoding="utf-8")

    require("#define AUDIO_SAMPLE_RATE_HZ 8000u" in text, "sample rate must be 8000 Hz")

    for name in [
        "audio_face_lost",
        "audio_attention",
        "audio_warning",
        "audio_danger",
    ]:
        length_match = re.search(rf"#define {name.upper()}_LEN\s+\((\d+)u\)", text)
        data_match = re.search(
            rf"const uint8 {name}\[\] =\s*\{{(.*?)\}};",
            text,
            re.DOTALL,
        )
        require(length_match, f"missing length macro for {name}")
        require(data_match, f"missing data array for {name}")

        length = int(length_match.group(1))
        values = [int(item) for item in re.findall(r"\b\d+\b", data_match.group(1))]
        require(length == len(values), f"{name} length macro does not match data")
        require(length > 1000, f"{name} is too short to be a voice prompt")
        require(min(values) >= 0 and max(values) <= 255, f"{name} values must be unsigned 8-bit PCM")
        require(len(set(values[: min(length, 2000)])) > 8, f"{name} looks constant or silent")

    print("audio asset checks passed")


if __name__ == "__main__":
    main()
