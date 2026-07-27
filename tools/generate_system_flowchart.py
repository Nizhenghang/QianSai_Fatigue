from pathlib import Path
from PIL import Image, ImageDraw, ImageFont, ImageFilter
import math

ROOT = Path(__file__).resolve().parents[1]
OUT_DIR = ROOT / "outputs"
OUT_DIR.mkdir(exist_ok=True)
PNG_PATH = OUT_DIR / "RA8D1_Fatigue_System_Design_Flowchart_CN.png"

SCALE = 2
WIDTH, HEIGHT = 1800 * SCALE, 1120 * SCALE


def scale(value):
    return int(round(value * SCALE))


FONT_CANDIDATES = [
    Path(r"C:\Windows\Fonts\msyh.ttc"),
    Path(r"C:\Windows\Fonts\msyhbd.ttc"),
    Path(r"C:\Windows\Fonts\simhei.ttf"),
    Path(r"C:\Windows\Fonts\simsun.ttc"),
]

FONT_PATH = next((path for path in FONT_CANDIDATES if path.exists()), None)
BOLD_FONT_PATH = Path(r"C:\Windows\Fonts\msyhbd.ttc")

if FONT_PATH is None:
    raise RuntimeError("No Chinese font found")


def font(size, bold=False):
    path = BOLD_FONT_PATH if bold and BOLD_FONT_PATH.exists() else FONT_PATH
    return ImageFont.truetype(str(path), scale(size))


image = Image.new("RGB", (WIDTH, HEIGHT), "#f5f9ff")
draw = ImageDraw.Draw(image)

for y in range(HEIGHT):
    t = y / HEIGHT
    red = int(239 * (1 - t) + 242 * t)
    green = int(246 * (1 - t) + 250 * t)
    blue = int(255 * (1 - t) + 248 * t)
    draw.line([(0, y), (WIDTH, y)], fill=(red, green, blue))

for x in range(0, WIDTH, scale(48)):
    draw.line([(x, 0), (x, HEIGHT)], fill="#e6eefb", width=1)
for y in range(0, HEIGHT, scale(48)):
    draw.line([(0, y), (WIDTH, y)], fill="#e6eefb", width=1)


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


def shadow_round(rect, radius, shadow=18, offset=(0, 14), opacity=42):
    layer = Image.new("RGBA", (WIDTH, HEIGHT), (0, 0, 0, 0))
    layer_draw = ImageDraw.Draw(layer)
    x1, y1, x2, y2 = [scale(value) for value in rect]
    ox, oy = [scale(value) for value in offset]
    layer_draw.rounded_rectangle(
        [x1 + ox, y1 + oy, x2 + ox, y2 + oy],
        radius=scale(radius),
        fill=(25, 52, 95, opacity),
    )
    layer = layer.filter(ImageFilter.GaussianBlur(scale(shadow)))
    image.paste(layer, (0, 0), layer)


def rounded(rect, radius, fill, outline=None, width=1):
    draw.rounded_rectangle(
        [scale(value) for value in rect],
        radius=scale(radius),
        fill=fill,
        outline=outline,
        width=scale(width),
    )


def draw_multiline_center(x, y, lines, size=21, fill="#43536b", gap=31, bold=False):
    font_obj = font(size, bold)
    for index, line in enumerate(lines):
        text_center(scale(x), scale(y + index * gap), line, font_obj, fill)


def draw_card(x, y, width, height, title, lines, accent):
    shadow_round((x, y, x + width, y + height), 28)
    rounded((x, y, x + width, y + height), 28, "#ffffff", "#d9e2f2", 2)
    rounded((x, y, x + width, y + 14), 7, accent)
    draw.ellipse([scale(x + 26), scale(y + 28), scale(x + 58), scale(y + 60)], fill=accent + "22")
    draw.ellipse([scale(x + 34), scale(y + 36), scale(x + 50), scale(y + 52)], fill=accent)
    text_center(scale(x + width / 2 + 14), scale(y + 47), title, font(26, True), "#172b4d")
    draw_multiline_center(x + width / 2, y + 102, lines, 21, "#43536b", 34)


def draw_mini_card(x, y, width, height, title, lines, accent):
    shadow_round((x, y, x + width, y + height), 22, shadow=12, offset=(0, 10), opacity=34)
    rounded((x, y, x + width, y + height), 22, "#ffffff", "#dce6f5", 2)
    rounded((x + 22, y + 22, x + 30, y + height - 22), 4, accent)
    text_center(scale(x + width / 2 + 10), scale(y + 40), title, font(23, True), "#172b4d")
    draw_multiline_center(x + width / 2 + 10, y + 82, lines, 17, "#52627a", 28)


def arrow_line(x1, y1, x2, y2, color="#4f7cff", width=5):
    x1, y1, x2, y2 = map(scale, [x1, y1, x2, y2])
    draw.line([(x1, y1), (x2, y2)], fill=color, width=scale(width))
    angle = math.atan2(y2 - y1, x2 - x1)
    length = scale(22)
    point_a = angle + math.pi * 0.82
    point_b = angle - math.pi * 0.82
    draw.polygon(
        [
            (x2, y2),
            (x2 + length * math.cos(point_a), y2 + length * math.sin(point_a)),
            (x2 + length * math.cos(point_b), y2 + length * math.sin(point_b)),
        ],
        fill=color,
    )


def curve_arrow(points, color="#4f7cff", width=5):
    scaled = [(scale(x), scale(y)) for x, y in points]
    draw.line(scaled, fill=color, width=scale(width), joint="curve")
    x1, y1 = scaled[-2]
    x2, y2 = scaled[-1]
    angle = math.atan2(y2 - y1, x2 - x1)
    length = scale(22)
    point_a = angle + math.pi * 0.82
    point_b = angle - math.pi * 0.82
    draw.polygon(
        [
            (x2, y2),
            (x2 + length * math.cos(point_a), y2 + length * math.sin(point_a)),
            (x2 + length * math.cos(point_b), y2 + length * math.sin(point_b)),
        ],
        fill=color,
    )


text_left(scale(90), scale(42), "RA8D1 驾驶员疲劳检测系统设计流程图", font(42, True), "#13213b")
text_left(
    scale(90),
    scale(112),
    "Camera → ROI → Lightweight CNN → Temporal Decision → Voice / Bluetooth Output",
    font(22),
    "#5c6f8c",
)

rounded((86, 165, 1714, 750), 36, "#ffffff", "#d9e5f6", 2)
text_left(scale(122), scale(196), "实时检测链路", font(28, True), "#2457ff")

draw_card(120, 260, 285, 205, "图像采集层", ["MT9V03X 摄像头", "320×120 灰度图像", "帧同步 / 图像缓冲"], "#4f7cff")
draw_card(455, 260, 285, 205, "预处理层", ["裁剪驾驶员 ROI", "缩放到 64×32", "灰度归一化 /255"], "#6c63ff")

shadow_round((790, 236, 1120, 488), 34)
mask = Image.new("L", (WIDTH, HEIGHT), 0)
mask_draw = ImageDraw.Draw(mask)
mask_draw.rounded_rectangle([scale(790), scale(236), scale(1120), scale(488)], radius=scale(34), fill=255)
gradient = Image.new("RGBA", (WIDTH, HEIGHT), (0, 0, 0, 0))
pixels = gradient.load()
for xx in range(scale(790), scale(1120)):
    t = (xx - scale(790)) / scale(330)
    color = (int(36 * (1 - t) + 18 * t), int(87 * (1 - t) + 179 * t), int(255 * (1 - t) + 168 * t), 255)
    for yy in range(scale(236), scale(488)):
        pixels[xx, yy] = color
layer = Image.composite(gradient, Image.new("RGBA", (WIDTH, HEIGHT), (0, 0, 0, 0)), mask)
image.paste(layer, (0, 0), layer)
text_center(scale(955), scale(286), "RA8D1 核心推理", font(30, True), "#ffffff")
text_center(scale(955), scale(330), "轻量级 CNN 二分类模型", font(21, True), "#e9f2ff")
rounded((825, 356, 1085, 444), 18, "#3fc7d6")
text_center(scale(955), scale(390), "Conv → Pool → Conv → Pool", font(18, True), "#ffffff")
text_center(scale(955), scale(420), "Dense → normal / fatigued", font(18, True), "#ffffff")

draw_card(1170, 260, 285, 205, "状态判定层", ["连续帧投票 / 置信度", "疲劳分数累计", "轻度 / 严重疲劳分级"], "#ff9f43")
draw_card(1490, 224, 190, 148, "语音报警", ["扬声器模块", "分级语音提醒"], "#ff5d73")
draw_card(1490, 420, 190, 148, "手机显示", ["HC-05 蓝牙", "Android App"], "#12b3a8")

arrow_line(405, 362, 455, 362)
arrow_line(740, 362, 790, 362)
arrow_line(1120, 362, 1170, 362)
curve_arrow([(1455, 332), (1475, 320), (1490, 298)], "#ff5d73")
curve_arrow([(1455, 408), (1475, 450), (1490, 494)], "#12b3a8")

rounded((128, 570, 583, 688), 26, "#f6f9ff", "#d8e4f7", 2)
text_center(scale(355), scale(614), "硬件支撑", font(23, True), "#172b4d")
text_center(scale(355), scale(652), "外部 SDRAM · UART · DAC/音频 · FSP/逐飞驱动库", font(19), "#52627a")

rounded((645, 570, 1155, 688), 26, "#f6fffb", "#cfeee7", 2)
text_center(scale(900), scale(614), "嵌入式部署特点", font(23, True), "#172b4d")
text_center(scale(900), scale(652), "不依赖 TFLite，C 语言手写前向推理，权重数组本地运行", font(19), "#52627a")

rounded((1218, 570, 1648, 688), 26, "#fff8f2", "#ffe0bf", 2)
text_center(scale(1433), scale(614), "输出结果", font(23, True), "#172b4d")
text_center(scale(1433), scale(652), "正常 · 轻度疲劳 · 严重疲劳 · 未检测到人脸", font(19), "#52627a")

rounded((86, 795, 1714, 1058), 36, "#ffffff", "#d9e5f6", 2)
text_left(scale(122), scale(826), "训练与模型部署闭环", font(28, True), "#12a594")

card_y = 882
card_w = 270
card_h = 126
card_gap = 44
card_xs = [128 + index * (card_w + card_gap) for index in range(5)]

draw_mini_card(card_xs[0], card_y, card_w, card_h, "数据采集", ["EXPORT_MODE", "串口导出 ROI 样本"], "#4f7cff")
draw_mini_card(card_xs[1], card_y, card_w, card_h, "数据处理", ["normal / fatigued", "增强与均衡"], "#6c63ff")
draw_mini_card(card_xs[2], card_y, card_w, card_h, "模型训练", ["Keras 轻量 CNN", "二分类训练"], "#ff9f43")
draw_mini_card(card_xs[3], card_y, card_w, card_h, "模型导出", ["fatigue_model.h5", "生成 cnn_weights.h"], "#ff5d73")
draw_mini_card(card_xs[4], card_y, card_w, card_h, "固件烧录运行", ["Keil / RA 工程编译", "RA8D1 端实时推理"], "#12b3a8")

arrow_y = card_y + card_h / 2
for index in range(4):
    arrow_line(card_xs[index] + card_w, arrow_y, card_xs[index + 1], arrow_y, "#12a594")

text_left(scale(90), scale(1072), "关键代码链路：hal_entry.c → fatigue_detect.c → cnn_inference.c → cnn_weights.h", font(15), "#718096")
text_left(scale(90), scale(1097), "训练部署链路：collect_frames.py → augment_data.py → train_cnn.py → export_model.py → cnn_weights.h", font(15), "#718096")

image.save(PNG_PATH, quality=96)
print(PNG_PATH)
print(FONT_PATH)
