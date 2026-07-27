from pathlib import Path
import subprocess
import wave


ROOT = Path(__file__).resolve().parents[1]
OUT_DIR = ROOT / "E18_usb_cdc_demo" / "code"
BUILD_DIR = ROOT / "tmp" / "audio_assets"
HEADER = OUT_DIR / "audio_samples.h"
SAMPLE_RATE = 8000

PROMPTS = [
    ("face_lost", "未检测到人脸，请回到摄像头前方。"),
    ("attention", "注意力下降，请保持专注。"),
    ("warning", "检测到疲劳趋势，请尽快休息。"),
    ("danger", "严重疲劳，请立即停车休息。"),
]


def run(command):
    subprocess.run(command, check=True)


def synthesize_wav(name, text):
    wav_path = BUILD_DIR / f"{name}.wav"
    ps_script = BUILD_DIR / f"synth_{name}.ps1"
    escaped_text = text.replace("'", "''")
    escaped_wav = str(wav_path).replace("'", "''")
    ps_script.write_text(
        "\n".join(
            [
                "Add-Type -AssemblyName System.Speech",
                "$synth = New-Object System.Speech.Synthesis.SpeechSynthesizer",
                "$synth.SelectVoice('Microsoft Huihui Desktop')",
                "$synth.Rate = 1",
                "$synth.Volume = 100",
                "$format = New-Object System.Speech.AudioFormat.SpeechAudioFormatInfo(16000, [System.Speech.AudioFormat.AudioBitsPerSample]::Sixteen, [System.Speech.AudioFormat.AudioChannel]::Mono)",
                f"$synth.SetOutputToWaveFile('{escaped_wav}', $format)",
                f"$synth.Speak('{escaped_text}')",
                "$synth.Dispose()",
            ]
        ),
        encoding="utf-8-sig",
    )
    run(["powershell", "-NoProfile", "-ExecutionPolicy", "Bypass", "-File", str(ps_script)])
    return wav_path


def convert_to_unsigned_pcm(wav_path, name):
    raw_path = BUILD_DIR / f"{name}.u8"
    run(
        [
            "ffmpeg",
            "-y",
            "-loglevel",
            "error",
            "-i",
            str(wav_path),
            "-ac",
            "1",
            "-ar",
            str(SAMPLE_RATE),
            "-af",
            "volume=1.7,alimiter=limit=0.95",
            "-f",
            "u8",
            str(raw_path),
        ]
    )
    return raw_path.read_bytes()


def format_array(name, data):
    lines = [f"#define AUDIO_{name.upper()}_LEN ({len(data)}u)", f"const uint8 audio_{name}[] ="]
    lines.append("{")
    for offset in range(0, len(data), 16):
        chunk = ", ".join(f"{value:3d}" for value in data[offset : offset + 16])
        comma = "," if offset + 16 < len(data) else ""
        lines.append(f"    {chunk}{comma}")
    lines.append("};")
    return "\n".join(lines)


def main():
    BUILD_DIR.mkdir(parents=True, exist_ok=True)
    OUT_DIR.mkdir(parents=True, exist_ok=True)

    arrays = []
    for name, text in PROMPTS:
        wav_path = synthesize_wav(name, text)
        data = convert_to_unsigned_pcm(wav_path, name)
        arrays.append((name, data, text))

    content = [
        "/*********************************************************************************************************************",
        "* Generated local voice prompts for RA8D1 DAC speaker playback.",
        "* Regenerate with: python tools/generate_audio_assets.py",
        "********************************************************************************************************************/",
        "#ifndef _AUDIO_SAMPLES_H_",
        "#define _AUDIO_SAMPLES_H_",
        "",
        '#include "zf_common_typedef.h"',
        "",
        "#define AUDIO_SAMPLE_RATE_HZ 8000u",
        "",
    ]
    for name, data, text in arrays:
        content.append(f"/* {name}: {text} */")
        content.append(format_array(name, data))
        content.append("")
    content.append("#endif")
    content.append("")

    HEADER.write_text("\n".join(content), encoding="utf-8")

    for name, data, _ in arrays:
        duration_ms = len(data) * 1000 // SAMPLE_RATE
        print(f"{name}: {len(data)} bytes, {duration_ms} ms")
    print(f"wrote {HEADER}")


if __name__ == "__main__":
    main()
