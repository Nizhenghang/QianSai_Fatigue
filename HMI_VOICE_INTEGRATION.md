# 串口屏与本地语音播报集成说明

## 当前实现

本项目现在采用“RA8D1 本地 DAC 语音播报”方案，不再把报警做成蜂鸣器方波，也不依赖 ESP8266 或云端 TTS 才能发声。

硬件链路来自 `RA8D1_DevBoard V1.0 原理图.pdf` 第 5 页：

```text
RA8D1 P014(DAC) -> C30 -> NS4150B INP -> VOP/VON -> P11 扬声器接口
RA8D1 P015(IO)  -> R49 -> NS4150B CTRL
```

## 新增文件

| 文件 | 作用 |
| --- | --- |
| `E18_usb_cdc_demo/code/audio_player.c/.h` | 初始化 DAC，按 8kHz 采样率播放语音 PCM，并按疲劳等级选择提示音 |
| `E18_usb_cdc_demo/code/audio_samples.h` | 由电脑 TTS 生成的 4 段中文语音数组，烧录后直接从 Flash 播放 |
| `tools/generate_audio_assets.py` | 调用 Windows `Microsoft Huihui Desktop` 生成中文语音，再用 `ffmpeg` 转成 8kHz unsigned PCM |
| `tools/check_audio_assets.py` | 检查语音数组是否存在、采样率是否正确、数组是否为空或静音 |

## 语音等级

| 等级 | 触发状态 | 播放内容 |
| --- | --- | --- |
| `0` | 未检测到人脸 | “未检测到人脸，请回到摄像头前方。” |
| `2` | 注意力下降 | “注意力下降，请保持专注。” |
| `3` | 疲劳预警 | “检测到疲劳趋势，请尽快休息。” |
| `4` | 严重疲劳 | “严重疲劳，请立即停车休息。” |

`HMI_LEVEL_NORMAL` 不播放语音，避免正常驾驶时反复打扰。

## 固件调用关系

```text
fatigue_detect.c
  -> hmi_update_status() / hmi_show_alert()
    -> voice_assistant_say(level, message)
      -> audio_player_play_level(level)
        -> dac_out(sample)
```

`voice_assistant.c` 仍保留 `VOICE|SAY|...` Debug 日志，方便你用串口观察当前播报事件；真正发声由 `audio_player.c` 完成。

## 串口屏功能

串口屏仍通过 `UART3` 接收 Nextion/TJC 风格指令：

| 控件 | 类型 | 内容 |
| --- | --- | --- |
| `t_status` | 文本 | 当前状态：正常、注意力下降、疲劳预警、严重疲劳、未检测到人脸 |
| `t_advice` | 文本 | 当前建议语 |
| `t_voice` | 文本 | 语音提醒状态 |
| `n_level` | 数值 | 疲劳等级 `0~4` |
| `n_ratio` | 数值 | 疲劳投票比例 |
| `n_conf` | 数值 | CNN 置信度百分比 |

## 重新生成语音

如果后续想换提示词或换声音，修改 `tools/generate_audio_assets.py` 里的 `PROMPTS`，然后运行：

```powershell
python tools\generate_audio_assets.py
python tools\check_audio_assets.py
```

生成依赖：

- Windows 已安装 `Microsoft Huihui Desktop` 中文语音。
- 本机已安装 `ffmpeg`。

## 注意事项

- 当前播放方式是阻塞式 DAC 播放，一句语音约 3~4 秒，播报期间会短暂停顿摄像头检测流程；比赛演示通常可以接受。
- 如果后续要求“边检测边播报”，下一步应改成定时器/PIT 中断或 DMA 播放。
- 语音数组源码约 600KB，实际 Flash 中 PCM 数据约 116KB，RA8D1 当前 Flash 空间可以承受。
