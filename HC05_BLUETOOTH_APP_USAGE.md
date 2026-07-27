# HC-05 Bluetooth App Usage

## Files

- Android APK: `android/FatigueBluetoothApp/app/build/outputs/apk/debug/app-debug.apk`
- Firmware HEX: `E18_usb_cdc_demo/Objects/zf_ra_motherboard_demo.hex`
- Android source: `android/FatigueBluetoothApp`
- Firmware Bluetooth output: `E18_usb_cdc_demo/code/hmi_display.c`

## Wiring

- RA8D1 UART3 TX / P409 -> HC-05 RX
- HC-05 VCC -> module required power input
- HC-05 GND -> RA8D1 GND
- First version does not require HC-05 TX -> RA8D1 RX
- Firmware UART3 is set to the HC-05 default baud rate: `9600`.

If the HC-05 breakout RX pin is not 3.3 V tolerant, add a resistor divider on RA8D1 TX -> HC-05 RX.

## Phone Setup

1. Install `app-debug.apk` on the Android phone.
2. Pair HC-05 in Android system Bluetooth settings.
3. Open `疲劳监测蓝牙助手`.
4. Grant Bluetooth permission if Android asks.
5. Select HC-05 and tap `连接 HC-05`.

## Firmware Frames

```text
BT|BOOT|name=FatigueMonitor|baud=9600
BT|STATE|level=2|status=LIGHT|score=31|conf=74|cnn=1|state=0
BT|ALERT|level=4|status=DANGER|score=100|advice=Severe fatigue alarm
```

## Status Mapping

- `NORMAL`: normal
- `LIGHT`: light fatigue
- `WARN`: warning fatigue
- `DANGER`: severe fatigue alarm
- `NO_FACE`: face not detected
