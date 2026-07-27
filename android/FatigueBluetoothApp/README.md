# Fatigue Bluetooth App

This Android app receives fatigue status frames from the RA8D1 board through an HC-05 classic Bluetooth SPP link.

Expected firmware frames:

```text
BT|BOOT|name=FatigueMonitor|baud=115200
BT|STATE|level=2|status=LIGHT|score=31|conf=74|cnn=1|state=0
BT|ALERT|level=4|status=DANGER|score=100|advice=Severe fatigue alarm
```

Build command:

```powershell
.\gradlew.bat assembleDebug
```

The debug APK is generated at:

```text
app\build\outputs\apk\debug\app-debug.apk
```
