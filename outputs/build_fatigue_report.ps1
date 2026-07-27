$ErrorActionPreference = 'Stop'
$outDocx = 'D:\Nizhenghang\QianSai\outputs\RA8D1_驾驶员疲劳检测系统_实验报告.docx'
$outPdf = 'D:\Nizhenghang\QianSai\outputs\RA8D1_驾驶员疲劳检测系统_实验报告_预览.pdf'

function RGB($r,$g,$b) { return [int]($r + ($g * 256) + ($b * 65536)) }

$wdStory = 6
$wdCollapseEnd = 0
$wdPageBreak = 7
$wdLineStyleSingle = 1
$wdAlignLeft = 0
$wdAlignCenter = 1
$wdAlignRight = 2
$wdPreferredWidthPoints = 3
$wdExportFormatPDF = 17

$word = New-Object -ComObject Word.Application
$word.Visible = $false
$doc = $word.Documents.Add()
$sel = $word.Selection

# Page setup
$sec = $doc.Sections.Item(1)
$sec.PageSetup.TopMargin = 72
$sec.PageSetup.BottomMargin = 72
$sec.PageSetup.LeftMargin = 72
$sec.PageSetup.RightMargin = 72
$sec.PageSetup.HeaderDistance = 36
$sec.PageSetup.FooterDistance = 36

# Base styles
$doc.Styles.Item('Normal').Font.Name = '宋体'
$doc.Styles.Item('Normal').Font.Size = 11
$doc.Styles.Item('Normal').ParagraphFormat.LineSpacingRule = 0
$doc.Styles.Item('Normal').ParagraphFormat.SpaceAfter = 6

foreach($name in @('标题 1','Heading 1')) { try { $doc.Styles.Item($name).Font.Name='黑体'; $doc.Styles.Item($name).Font.Size=16; $doc.Styles.Item($name).Font.Bold=$true; $doc.Styles.Item($name).Font.Color=RGB 31 78 121; $doc.Styles.Item($name).ParagraphFormat.SpaceBefore=14; $doc.Styles.Item($name).ParagraphFormat.SpaceAfter=8 } catch {} }
foreach($name in @('标题 2','Heading 2')) { try { $doc.Styles.Item($name).Font.Name='黑体'; $doc.Styles.Item($name).Font.Size=13; $doc.Styles.Item($name).Font.Bold=$true; $doc.Styles.Item($name).Font.Color=RGB 46 116 181; $doc.Styles.Item($name).ParagraphFormat.SpaceBefore=10; $doc.Styles.Item($name).ParagraphFormat.SpaceAfter=6 } catch {} }
foreach($name in @('标题','Title')) { try { $doc.Styles.Item($name).Font.Name='黑体'; $doc.Styles.Item($name).Font.Size=20; $doc.Styles.Item($name).Font.Bold=$true } catch {} }

function WritePara($text, $style='Normal', $bold=$false, $align=0) {
    $script:sel.Style = $style
    $script:sel.ParagraphFormat.Alignment = $align
    $script:sel.Font.Name = '宋体'
    $script:sel.Font.Size = 11
    $script:sel.Font.Bold = $bold
    $script:sel.TypeText($text)
    $script:sel.TypeParagraph()
    $script:sel.Font.Bold = $false
    $script:sel.ParagraphFormat.Alignment = 0
}

function WriteHeading1($text) { WritePara $text '标题 1' $true 0 }
function WriteHeading2($text) { WritePara $text '标题 2' $true 0 }

function WriteBullets($items) {
    foreach($item in $items){
        $script:sel.Style = 'Normal'
        $script:sel.Font.Name = '宋体'
        $script:sel.Font.Size = 11
        $script:sel.TypeText('· ' + $item)
        $script:sel.TypeParagraph()
    }
}

function WriteFigurePlaceholder($caption, $hint, $height=155) {
    $script:sel.TypeParagraph()
    $range = $script:sel.Range
    $shape = $script:doc.Shapes.AddShape(1, 0, 0, 420, $height, $range)
    $shape.RelativeHorizontalPosition = 0
    $shape.Left = 0
    $shape.RelativeVerticalPosition = 0
    $shape.Top = 0
    $shape.WrapFormat.Type = 3
    $shape.Fill.ForeColor.RGB = RGB 242 246 250
    $shape.Line.ForeColor.RGB = RGB 91 155 213
    $shape.Line.Weight = 1.25
    $shape.TextFrame.TextRange.Text = "[图片预留] `r" + $caption + "`r" + $hint
    $shape.TextFrame.TextRange.Font.Name = '微软雅黑'
    $shape.TextFrame.TextRange.Font.Size = 11
    $shape.TextFrame.TextRange.Font.Color = RGB 80 80 80
    $shape.TextFrame.TextRange.ParagraphFormat.Alignment = 1
    $script:sel.MoveDown() | Out-Null
    WritePara ('图：' + $caption) 'Normal' $false 1
}

function AddTable($headers, $rows, $widths) {
    $table = $script:doc.Tables.Add($script:sel.Range, $rows.Count + 1, $headers.Count)
    $table.Borders.Enable = $true
    $table.Range.Font.Name = '宋体'
    $table.Range.Font.Size = 10
    $table.Rows.Item(1).Range.Font.Bold = $true
    $table.Rows.Item(1).Shading.BackgroundPatternColor = RGB 232 238 245
    for($c=1;$c -le $headers.Count;$c++){
        $table.Cell(1,$c).Range.Text = $headers[$c-1]
        $table.Cell(1,$c).Width = $widths[$c-1]
    }
    for($r=0;$r -lt $rows.Count;$r++){
        for($c=0;$c -lt $headers.Count;$c++){
            $table.Cell($r+2,$c+1).Range.Text = [string]$rows[$r][$c]
            $table.Cell($r+2,$c+1).Width = $widths[$c]
        }
    }
    $table.Range.ParagraphFormat.SpaceAfter = 2
    $script:sel.SetRange($table.Range.End, $table.Range.End)
    $script:sel.TypeParagraph()
}

# Header/footer
$header = $sec.Headers.Item(1).Range
$header.Text = 'RA8D1 端侧视觉 AI 驾驶员疲劳检测系统实验报告'
$header.Font.Name = '宋体'
$header.Font.Size = 9
$header.Font.Color = RGB 100 116 139
$footer = $sec.Footers.Item(1).Range
$footer.ParagraphFormat.Alignment = 1
$footer.Text = '第  页'
$footer.Font.Name = '宋体'
$footer.Font.Size = 9

# Title block
$sel.Style='标题'
$sel.ParagraphFormat.Alignment = 1
$sel.Font.Name='黑体'; $sel.Font.Size=20; $sel.Font.Bold=$true
$sel.TypeText('驾驶员疲劳检测系统实验报告')
$sel.TypeParagraph()
$sel.Font.Bold=$false
$sel.Font.Name='宋体'; $sel.Font.Size=11
$sel.ParagraphFormat.Alignment = 1
$sel.TypeText('作品名称：RA8D1 端侧视觉 AI 驾驶员疲劳检测系统')
$sel.TypeParagraph()
$sel.TypeText('（本报告不包含学校名称、指导老师等信息，相关信息可由作者后续自行填写）')
$sel.TypeParagraph()
$sel.TypeParagraph()

WriteHeading1 '摘要'
WritePara '本作品面向驾驶安全场景，设计并实现了一套基于 RA8D1 单片机的驾驶员疲劳检测系统。系统以 MT9V03X 摄像头作为图像采集入口，在单片机端完成驾驶员面部区域的裁剪、缩放和轻量级 CNN 推理，并通过连续疲劳分数累积机制判断当前状态是否处于正常、轻度疲劳、警告疲劳、严重疲劳或无人脸状态。与仅依赖单次动作触发的方案相比，本作品将单帧图像识别结果和时间维度上的状态变化结合起来，降低了短暂眨眼、低头或偶发误判带来的误报风险。'
WritePara '系统硬件部分主要由 RA8D1 开发板、MT9V03X 摄像头、外接扬声器模块、HC-05 蓝牙模块及手机端显示 APP 组成。软件部分包含图像采集、ROI 预处理、CNN 前向推理、疲劳等级判断、语音播报、蓝牙数据发送和 Android 手机端显示等模块。模型训练流程由 PC 端采集脚本、数据增强脚本、训练脚本和模型导出脚本组成，可将训练得到的权重导出为 C 语言数组并部署到单片机固件中。'
WritePara '目前系统已经实现端侧实时检测、疲劳等级显示、严重疲劳语音报警、无人脸提醒以及手机端状态同步等功能。该作品具有端侧部署、低成本硬件、交互方式直观和便于后续扩展等特点，可用于智能驾驶辅助、疲劳驾驶预警、嵌入式 AI 教学展示及低成本安全监测实验平台。'

WriteHeading1 '第一部分  作品概述'
WriteHeading2 '1.1 功能与特性'
WritePara '本作品的核心功能是对驾驶员状态进行实时监测，并根据疲劳程度进行分级提示。系统启动后，摄像头持续采集驾驶员前方图像，固件从图像中裁剪出固定 ROI 区域并缩放为 64×32 灰度输入，随后调用轻量级 CNN 模型完成单帧正常/疲劳分类。后处理模块不会直接依据一次分类结果报警，而是将连续帧的疲劳判断转化为疲劳分数，并根据阈值映射为正常、轻度、警告和严重疲劳等级。'
WriteBullets @('端侧实时推理：不依赖云端传输图像，隐私性和独立性较好。','疲劳等级分级：输出 NORMAL、LIGHT、WARN、DANGER 和 NO_FACE 状态。','语音播报提醒：通过外接扬声器模块播放不同疲劳等级的提示语音。','蓝牙手机显示：通过 HC-05 模块将状态数据发送到 Android APP。','数据闭环：支持图像采集、数据增强、模型训练和权重导出。')
WriteFigurePlaceholder '作品整体实物图预留' '建议放置正面或斜 45°拍摄的整机照片。'

WriteHeading2 '1.2 应用领域'
WritePara '该系统主要应用于驾驶安全与嵌入式智能感知场景。在实际交通环境中，疲劳驾驶容易导致反应迟缓、注意力下降甚至交通事故，因此低成本、实时、可独立运行的疲劳检测装置具有较强的应用价值。本作品也适合作为嵌入式 AI 教学平台，用于展示从数据采集、模型训练到 MCU 部署的完整流程。'
WritePara '除车载疲劳检测外，该方案还可扩展到学习状态监测、值班人员精神状态提醒、工业岗位安全监测等场景。在这些场景中，系统可以结合不同摄像头安装角度和阈值策略，对长时间低头、闭眼、离岗等状态进行提示。'
WriteFigurePlaceholder '典型应用场景图预留' '可放置车内安装位置、驾驶员测试场景或应用示意图。' 135

WriteHeading2 '1.3 主要技术特点'
WritePara '本作品采用“端侧视觉 AI + 时序状态判断 + 多通道提醒”的设计思路。AI 模型不是部署在 PC 或云端，而是以 C 数组和手写前向推理的形式集成在 RA8D1 固件中。模型结构为 64×32×1 输入，经两层卷积、两层池化和两层全连接输出二分类结果。疲劳检测模块进一步结合置信度阈值、连续疲劳帧、疲劳分数累积和目标丢失检测完成最终状态判断。'
WriteBullets @('手写 CNN 前向推理，便于在无通用推理框架的 MCU 上运行。','中间特征图放置于 SDRAM，减轻片上存储压力。','采用固定 ROI 和灰度图输入，降低计算量。','通过蓝牙串口协议输出状态帧，便于手机端显示和调试。','语音提醒替代单一蜂鸣器，使报警信息更直观。')

WriteHeading2 '1.4 主要性能指标'
WritePara '系统当前以功能实现和现场演示为主，部分量化指标需要结合后续统一测试数据进一步完善。已确认的固件构建信息和关键参数如下。'
$headers=@('指标项','当前值或说明')
$rows=@(
    @('主控平台','RA8D1 开发板'),
    @('图像输入','MT9V03X 摄像头，ROI 裁剪后缩放为 64×32 灰度图'),
    @('模型结构','Conv(5×5,8,stride2) + Pool + Conv(3×3,16) + Pool + FC(32) + FC(2)'),
    @('状态等级','NORMAL / LIGHT / WARN / DANGER / NO_FACE'),
    @('蓝牙通信','HC-05 经典蓝牙 SPP，UART3 9600 baud'),
    @('固件构建','Keil 构建 0 Error(s), 0 Warning(s)'),
    @('准确率/误报率','预留：待统一测试集统计后填写')
)
AddTable $headers $rows @(150, 330)

WriteHeading2 '1.5 主要创新点'
WriteBullets @('将轻量 CNN 模型部署到单片机端，实现不依赖 PC/云端的疲劳识别。','采用连续疲劳分数而非单帧触发，能够更符合真实疲劳检测的时间连续性。','将语音播报和手机 APP 显示结合，使报警结果更易理解和展示。','形成自采数据、训练、导出、固件部署的完整嵌入式 AI 闭环。')

WriteHeading2 '1.6 设计流程'
WritePara '设计流程从需求分析开始，首先确定系统需要完成驾驶员状态采集、疲劳识别、报警提醒和手机显示。随后完成硬件选型与连接，构建 RA8D1、摄像头、扬声器和蓝牙模块组成的实验平台。软件层面先实现图像采集和导出，再采集正常与疲劳样本，使用训练脚本得到 CNN 模型并导出为固件权重。最后在固件中实现推理、连续状态判断、语音播报和蓝牙数据发送，并通过实测不断调整疲劳阈值。'
WriteFigurePlaceholder '系统设计流程图预留' '可放置从需求分析到模型部署的流程图或手绘图。' 145

$sel.InsertBreak($wdPageBreak)
WriteHeading1 '第二部分  系统组成及功能说明'
WritePara '本部分从整体结构、硬件系统和软件系统三个层次说明作品的设计细节。系统由图像采集层、端侧推理层、状态判断层和交互输出层组成，各层之间通过固定数据流连接，最终完成从驾驶员图像到疲劳等级提示的闭环。'

WriteHeading2 '2.1 整体介绍'
WritePara '系统整体框图可概括为：MT9V03X 摄像头采集图像，RA8D1 读取图像缓冲区并裁剪 ROI，CNN 推理模块输出正常/疲劳分类结果，疲劳检测模块进行连续分数累积和状态判断，最后由语音播报模块与 HC-05 蓝牙模块分别完成本地提醒和手机端显示。'
WriteFigurePlaceholder '系统整体框图预留' '建议放置“摄像头→RA8D1→CNN→状态判断→语音/蓝牙”的框图。' 170

WriteHeading2 '2.2 硬件系统介绍'
WritePara '硬件系统以 RA8D1 开发板为核心。MT9V03X 摄像头负责采集驾驶员图像，外接扬声器模块用于播放语音提示，HC-05 蓝牙模块通过 UART3 与主控通信，将疲劳等级和分数发送到手机端。系统供电和信号连接需要保证公共地可靠，蓝牙模块 RXD 接 RA8D1 UART3 TX，引脚电平保持 3.3 V TTL 兼容。'
WriteFigurePlaceholder '硬件整体连接照片预留' '建议放置 RA8D1、摄像头、扬声器、HC-05 的整体连接照片。' 155
WritePara '机械结构方面，当前作品以实验平台形式实现，摄像头需要固定在驾驶员正前方或略偏上的位置，以保证 ROI 区域能够覆盖眼部、口部和低头动作特征。后续可增加固定支架或外壳，提高比赛现场演示的稳定性。'
WriteFigurePlaceholder '摄像头固定方式或外壳结构图预留' '可放置支架、安装位置、外壳草图或实物照片。' 130
WritePara '电路模块方面，重点包括摄像头接口、扬声器输出模块和蓝牙串口通信模块。摄像头数据进入 RA8D1 的图像采集路径，语音模块通过本地音频播放逻辑输出不同等级提醒，蓝牙模块则发送 ASCII 状态帧，例如 BT|STATE|level=...|status=...|score=...。'
WriteFigurePlaceholder '关键电路或接线图预留' '建议放置 HC-05 接线、扬声器模块接线或开发板局部原理图。' 150

WriteHeading2 '2.3 软件系统介绍'
WritePara '软件系统包括固件端和手机端两部分。固件端运行在 RA8D1 上，主入口完成 SDRAM、摄像头、调试串口和相关外设初始化，然后在主循环中调用 fatigue_process_frame() 处理每一帧图像。手机端为 Android 蓝牙 APP，用于连接 HC-05 并解析来自单片机的状态帧。'
WriteFigurePlaceholder '软件整体流程图预留' '建议放置固件主循环、模型推理、蓝牙发送、APP 显示的流程图。' 160
WritePara '固件核心函数流程如下：首先判断摄像头帧是否完成；随后裁剪 FATIGUE_ROI_LEFT 至 FATIGUE_ROI_RIGHT、FATIGUE_ROI_TOP 至 FATIGUE_ROI_BOTTOM 区域，并缩放到 CNN_INPUT_W=64、CNN_INPUT_H=32；接着调用 cnn_classify() 得到疲劳分类和置信度；最后更新投票窗口、疲劳分数、报警状态和 HMI/蓝牙显示。'
WritePara '模型训练流程位于 training 目录。collect_frames.py 用于串口采集样本，augment_data.py 用于数据增强，train_cnn.py 用于训练 Keras CNN，export_model.py 将模型权重导出到 E18_usb_cdc_demo/code/cnn_weights.h。固件编译后即可在单片机端运行新的模型。'
WriteFigurePlaceholder '训练与部署流程图预留' '可放置 collect_frames → train_cnn → export_model → Keil 固件的流程图。' 150

$sel.InsertBreak($wdPageBreak)
WriteHeading1 '第三部分  完成情况及性能参数'
WriteHeading2 '3.1 整体介绍'
WritePara '目前作品已经完成从硬件搭建、模型训练导出、固件推理、疲劳等级判断、语音报警到蓝牙手机显示的主要功能闭环。系统能够在检测到无人脸时给出提示，在疲劳分数达到严重等级时触发语音报警，并将当前状态同步发送至手机 APP。'
WriteFigurePlaceholder '系统整体实物正面照片预留' '请放置整个系统正面照片。' 155
WriteFigurePlaceholder '系统整体实物斜 45°照片预留' '请放置整体斜 45°照片，用于展示装配效果。' 155

WriteHeading2 '3.2 工程成果'
WritePara '硬件成果包括 RA8D1 主控平台、摄像头采集模块、扬声器语音提醒模块和 HC-05 蓝牙通信模块。软件成果包括嵌入式固件、训练脚本、模型权重文件和 Android 手机端 APP。'
WriteFigurePlaceholder '硬件实物局部照片预留' '建议展示摄像头、蓝牙模块、扬声器模块等局部。' 145
WriteFigurePlaceholder '手机 APP 界面照片预留' '建议展示 NORMAL、LIGHT、WARN、DANGER 或 NO_FACE 状态界面。' 145
WriteFigurePlaceholder '语音报警或现场演示照片预留' '建议放置严重疲劳报警时的现场照片。' 145

WriteHeading2 '3.3 特性成果'
WritePara '系统已经实现的主要特性包括端侧图像推理、连续疲劳等级判断、无人脸检测提醒、语音播报和蓝牙状态同步。当前固件最近一次构建结果为 0 Error(s)、0 Warning(s)，生成 HEX 文件可直接烧录到目标板进行演示。'
$headers=@('测试项目','现象或结果','备注')
$rows=@(
    @('正常状态','手机端显示 NORMAL，语音不报警','需放置正常状态测试照片'),
    @('轻度疲劳','疲劳分数上升后显示 LIGHT','短暂动作不应频繁触发'),
    @('严重疲劳','达到阈值后语音播报严重疲劳提醒','需放置报警照片或视频截图'),
    @('无人脸状态','长时间无有效目标时进入 NO_FACE','可展示离开摄像头区域'),
    @('蓝牙通信','HC-05 向 APP 发送状态帧','APP 截图待插入'),
    @('模型准确率','待统一测试集统计','后续补充混淆矩阵')
)
AddTable $headers $rows @(105, 220, 155)
WriteFigurePlaceholder '性能测试或串口日志截图预留' '可放置构建日志、串口输出、APP 数据变化或测试记录。' 135

WriteHeading1 '第四部分  总结'
WriteHeading2 '4.1 可扩展之处'
WritePara '后续可以从四个方向继续优化：第一，扩充自采数据集，覆盖不同光照、姿态、距离和驾驶员个体差异；第二，建立独立测试集，统计准确率、召回率、误报率和混淆矩阵；第三，优化手机端界面，将疲劳分数曲线、历史报警记录和连接状态可视化；第四，设计固定支架和外壳，提高系统在比赛现场演示时的稳定性和美观度。'

WriteHeading2 '4.2 心得体会'
WritePara '本作品的实现过程体现了嵌入式 AI 项目从算法到工程落地的完整链路。最初的难点并不只是训练一个能够区分正常与疲劳图像的模型，而是如何让模型在单片机上稳定运行，并将单帧识别结果转化为符合真实场景的连续疲劳判断。实际调试中发现，单次打哈欠、短暂低头和摄像头范围丢失都可能影响判断结果，因此系统逐步加入了置信度阈值、连续分数累积、无人脸检测和报警冷却等机制。'
WritePara '在硬件调试方面，串口屏方案曾受供电和通信细节影响，最终改用 HC-05 蓝牙与手机 APP 显示，使作品展示更加稳定。语音报警部分也从单纯蜂鸣器输出升级为扬声器语音提示，提升了交互效果。通过这些调整可以体会到，比赛作品不仅要“能识别”，还要“能稳定演示、能解释清楚、能让观众理解”。'
WritePara '通过本项目，我进一步理解了数据采集、模型训练、模型导出、嵌入式部署和现场验证之间的关系。后续如果继续完善，应重点补足数据统计和实验验证部分，让作品不仅具备功能完整性，也具备更充分的工程证据。'

WriteHeading1 '第五部分  参考文献'
$refs=@(
    '[1] Renesas Electronics. RA8D1 Group User Manual: Hardware.',
    '[2] Renesas Electronics. Flexible Software Package Documentation.',
    '[3] SEEKFREE. RA8D1 开源库与逐飞相关外设驱动资料.',
    '[4] TensorFlow/Keras Documentation. Convolutional Neural Network Model Training.',
    '[5] Android Developers. Bluetooth Classic and Serial Communication Documentation.',
    '[6] HC-05 Bluetooth Module User Manual.'
)
foreach($r in $refs){ WritePara $r }

# Save and export
if(Test-Path $outDocx){ Remove-Item $outDocx -Force }
if(Test-Path $outPdf){ Remove-Item $outPdf -Force }
$doc.SaveAs2($outDocx, 16)
$doc.ExportAsFixedFormat($outPdf, $wdExportFormatPDF)
$doc.Close($false)
$word.Quit()
Write-Output $outDocx
Write-Output $outPdf

