from pathlib import Path
from PIL import Image, ImageDraw, ImageFont, ImageFilter
import math

ROOT = Path(__file__).resolve().parents[1]
OUT_DIR = ROOT / "outputs"
OUT_DIR.mkdir(exist_ok=True)
PNG_PATH = OUT_DIR / "RA8D1_Training_Deployment_Flowchart_CN.png"

SCALE = 2
WIDTH, HEIGHT = 1800 * SCALE, 1040 * SCALE


def scale(value):
    return int(round(value * SCALE))


FONT_PATH = next(
    path
    for path in [
        Path(r"C:\Windows\Fonts\msyh.ttc"),
        Path(r"C:\Windows\Fonts\simhei.ttf"),
        Path(r"C:\Windows\Fonts\simsun.ttc"),
    ]
    if path.exists()
)
BOLD_FONT_PATH = Path(r"C:\Windows\Fonts\msyhbd.ttc")


def font(size, bold=False):
    path = BOLD_FONT_PATH if bold and BOLD_FONT_PATH.exists() else FONT_PATH
    return ImageFont.truetype(str(path), scale(size))


image = Image.new("RGB", (WIDTH, HEIGHT), "#f7fbff")
draw = ImageDraw.Draw(image)

for y in range(HEIGHT):
    t = y / HEIGHT
    red = int(239 * (1 - t) + 247 * t)
    green = int(246 * (1 - t) + 251 * t)
    blue = int(255 * (1 - t) + 249 * t)
    draw.line([(0, y), (WIDTH, y)], fill=(red, green, blue))

for x in range(0, WIDTH, scale(48)):
    draw.line([(x, 0), (x, HEIGHT)], fill="#e8f0fb", width=1)
for y in range(0, HEIGHT, scale(48)):
    draw.line([(0, y), (WIDTH, y)], fill="#e8f0fb", width=1)


def text_center(x, y, content, font_obj, fill="#172b4d"):
    bbox = draw.textbbox((0, 0), content, font=font_obj)
    draw.text(
        (x - (bbox[2] - bbox[0]) / 2, y - (bbox[3] - bbox[1]) / 2),
        content,
        font=font_obj,
        fill=fill,
    )


def text_left(x, y, content, font_obj, fill="#172b4d"):
    draw.text((x, y), content, font=font_obj, fill=fill)


def rounded(rect, radius, fill, outline=None, width=1):
    draw.rounded_rectangle(
        [scale(v) for v in rect],
        radius=scale(radius),
        fill=fill,
        outline=outline,
        width=scale(width),
    )


def shadow_round(rect, radius, shadow=14, offset=(0, 12), opacity=35):
    layer = Image.new("RGBA", (WIDTH, HEIGHT), (0, 0, 0, 0))
    layer_draw = ImageDraw.Draw(layer)
    x1, y1, x2, y2 = [scale(v) for v in rect]
    ox, oy = [scale(v) for v in offset]
    layer_draw.rounded_rectangle(
        [x1 + ox, y1 + oy, x2 + ox, y2 + oy],
        radius=scale(radius),
        fill=(25, 52, 95, opacity),
    )
    layer = layer.filter(ImageFilter.GaussianBlur(scale(shadow)))
    image.paste(layer, (0, 0), layer)


def draw_lines_center(x, y, lines, size=16, gap=25, fill="#52627a", bold=False):
    fnt = font(size, bold)
    for idx, line in enumerate(lines):
        text_center(scale(x), scale(y + idx * gap), line, fnt, fill)


def card(x, y, width, height, title, lines, accent, badge=None):
    shadow_round((x, y, x + width, y + height), 24)
    rounded((x, y, x + width, y + height), 24, "#ffffff", "#dce6f5", 2)
    rounded((x, y, x + width, y + 12), 6, accent)
    draw.ellipse([scale(x + 28), scale(y + 30), scale(x + 62), scale(y + 64)], fill=accent)
    if badge:
        text_center(scale(x + 45), scale(y + 48), badge, font(16, True), "#ffffff")
    text_center(scale(x + width / 2 + 18), scale(y + 48), title, font(23, True), "#172b4d")
    body_start = y + 88 if len(lines) >= 4 else y + 92
    body_gap = 23 if len(lines) >= 4 else 24
    draw_lines_center(x + width / 2, body_start, lines, 15, body_gap)


def artifact_card(x, y, width, height, title, lines):
    shadow_round((x, y, x + width, y + height), 30, shadow=18, offset=(0, 14), opacity=46)
    rounded((x, y, x + width, y + height), 30, "#13213b", "#13213b", 2)
    rounded((x + 30, y + 30, x + width - 30, y + height - 30), 20, "#213a6b", "#355aa3", 2)
    text_center(scale(x + width / 2), scale(y + 58), title, font(25, True), "#ffffff")
    draw_lines_center(x + width / 2, y + 102, lines, 15, 24, "#dce8ff")


def arrow(x1, y1, x2, y2, color="#4f7cff", width=5):
    x1, y1, x2, y2 = map(scale, [x1, y1, x2, y2])
    draw.line([(x1, y1), (x2, y2)], fill=color, width=scale(width))
    angle = math.atan2(y2 - y1, x2 - x1)
    length = scale(20)
    a1 = angle + math.pi * 0.82
    a2 = angle - math.pi * 0.82
    draw.polygon(
        [
            (x2, y2),
            (x2 + length * math.cos(a1), y2 + length * math.sin(a1)),
            (x2 + length * math.cos(a2), y2 + length * math.sin(a2)),
        ],
        fill=color,
    )


def down_arrow(x, y1, y2, color="#12a594"):
    arrow(x, y1, x, y2, color, 5)


def curved_feedback(points, color="#ff9f43", width=5):
    scaled = [(scale(x), scale(y)) for x, y in points]
    draw.line(scaled, fill=color, width=scale(width), joint="curve")
    x1, y1 = scaled[-2]
    x2, y2 = scaled[-1]
    angle = math.atan2(y2 - y1, x2 - x1)
    length = scale(20)
    a1 = angle + math.pi * 0.82
    a2 = angle - math.pi * 0.82
    draw.polygon(
        [
            (x2, y2),
            (x2 + length * math.cos(a1), y2 + length * math.sin(a1)),
            (x2 + length * math.cos(a2), y2 + length * math.sin(a2)),
        ],
        fill=color,
    )


text_left(scale(90), scale(48), "模型训练与单片机部署流程图", font(44, True), "#13213b")
text_left(
    scale(90),
    scale(118),
    "Data Collection → CNN Training → Weight Export → Firmware Build → RA8D1 Runtime Validation",
    font(22),
    "#5c6f8c",
)

rounded((78, 174, 1722, 498), 36, "#ffffff", "#d9e5f6", 2)
text_left(scale(118), scale(213), "PC 端训练流水线", font(28, True), "#2457ff")

top_y = 270
card_w = 235
card_h = 155
xs = [118, 390, 662, 934, 1206]
card(xs[0], top_y, card_w, card_h, "样本采集", ["RA8D1 开启", "EXPORT_MODE", "导出 64×32 ROI", "按类别保存"], "#4f7cff", "1")
card(xs[1], top_y, card_w, card_h, "数据整理", ["检查样本质量", "剔除模糊/异常帧", "类别数量尽量均衡"], "#6c63ff", "2")
card(xs[2], top_y, card_w, card_h, "数据增强", ["亮度 / 平移 / 噪声", "扩充姿态场景", "生成增强数据"], "#12b3a8", "3")
card(xs[3], top_y, card_w, card_h, "CNN 训练", ["Keras 轻量 CNN", "输入 64×32 灰度图", "输出二分类结果"], "#ff9f43", "4")
card(xs[4], top_y, card_w, card_h, "模型评估", ["验证集准确率", "观察误判样本", "调整阈值与数据"], "#ff5d73", "5")

for i in range(4):
    arrow(xs[i] + card_w, top_y + card_h / 2, xs[i + 1], top_y + card_h / 2, "#4f7cff")

artifact_card(1500, 252, 170, 190, "训练产物", ["fatigue_model.h5", "训练日志", "样本统计"])
arrow(xs[4] + card_w, top_y + card_h / 2, 1500, 347, "#ff5d73")

rounded((78, 560, 1722, 900), 36, "#ffffff", "#d9e5f6", 2)
text_left(scale(118), scale(602), "嵌入式部署与实机验证", font(28, True), "#12a594")

artifact_card(150, 650, 250, 160, "模型导出", ["export_model.py", "生成权重头文件"])
card(485, 650, 250, 160, "固件集成", ["替换 cnn_weights.h", "C 语言手写推理", "疲劳检测调用分类"], "#4f7cff", "6")
card(820, 650, 250, 160, "编译烧录", ["Keil / RA 编译", "烧录到 RA8D1", "初始化摄像头外设"], "#6c63ff", "7")
card(1155, 650, 250, 160, "实时运行", ["ROI 预处理", "单片机端 CNN 推理", "连续帧等级判断"], "#ff9f43", "8")
card(1490, 650, 190, 160, "结果输出", ["语音报警", "HC-05 蓝牙", "App 显示"], "#12b3a8", "9")

curved_feedback([(1585, 442), (1585, 520), (275, 520), (275, 650)], "#12a594", 5)
arrow(400, 730, 485, 730, "#12a594")
arrow(735, 730, 820, 730, "#12a594")
arrow(1070, 730, 1155, 730, "#12a594")
arrow(1405, 730, 1490, 730, "#12a594")

curved_feedback([(1600, 650), (1665, 580), (1670, 510), (1324, 510), (1324, 425)], "#ff9f43", 5)
text_center(scale(1465), scale(535), "误判样本回流：补采数据 → 重训模型 → 更新权重", font(16, True), "#c26b00")

rounded((116, 930, 1684, 990), 22, "#f6f9ff", "#d8e4f7", 2)
text_center(
    scale(900),
    scale(962),
    "核心闭环：采集真实驾驶状态样本 → 训练轻量 CNN → 导出 C 权重 → RA8D1 本地推理 → 实机反馈继续优化数据集",
    font(21, True),
    "#172b4d",
)

image.save(PNG_PATH, quality=96)
print(PNG_PATH)
print(FONT_PATH)
