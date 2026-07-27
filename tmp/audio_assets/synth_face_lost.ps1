Add-Type -AssemblyName System.Speech
$synth = New-Object System.Speech.Synthesis.SpeechSynthesizer
$synth.SelectVoice('Microsoft Huihui Desktop')
$synth.Rate = 1
$synth.Volume = 100
$format = New-Object System.Speech.AudioFormat.SpeechAudioFormatInfo(16000, [System.Speech.AudioFormat.AudioBitsPerSample]::Sixteen, [System.Speech.AudioFormat.AudioChannel]::Mono)
$synth.SetOutputToWaveFile('D:\Nizhenghang\QianSai\tmp\audio_assets\face_lost.wav', $format)
$synth.Speak('未检测到人脸，请回到摄像头前方。')
$synth.Dispose()