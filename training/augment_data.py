"""
数据增强脚本
对采集的 .npy 图像进行增强，扩充训练数据集

用法：
  python augment_data.py --input training_data --output augmented_data --multiply 5
"""
import numpy as np
import argparse
import os
import random

def augment_image(img):
    """对单张 32x64 灰度图像进行随机增强"""
    result = img.copy().astype(np.float32)

    # 随机亮度偏移
    brightness = random.uniform(-30, 30)
    result += brightness

    # 50% 概率水平翻转
    if random.random() > 0.5:
        result = result[:, ::-1]

    # 随机高斯噪声
    noise = np.random.normal(0, 5, result.shape)
    result += noise

    # Clip 到 0-255
    result = np.clip(result, 0, 255).astype(np.uint8)

    # 随机小幅平移（±4像素）
    dx = random.randint(-4, 4)
    dy = random.randint(-2, 2)
    shifted = np.zeros_like(result)
    h, w = result.shape
    src_y_start = max(0, -dy)
    src_y_end = min(h, h - dy)
    src_x_start = max(0, -dx)
    src_x_end = min(w, w - dx)
    dst_y_start = max(0, dy)
    dst_x_start = max(0, dx)
    shifted[dst_y_start:dst_y_start + (src_y_end - src_y_start),
            dst_x_start:dst_x_start + (src_x_end - src_x_start)] = \
        result[src_y_start:src_y_end, src_x_start:src_x_end]
    result = shifted

    return result

def main():
    parser = argparse.ArgumentParser(description="Augment training data")
    parser.add_argument("--input", default="training_data", help="Input directory")
    parser.add_argument("--output", default="augmented_data", help="Output directory")
    parser.add_argument("--multiply", type=int, default=5, help="Augmentation multiplier")
    args = parser.parse_args()

    for label in ["normal", "fatigued"]:
        src_dir = os.path.join(args.input, label)
        dst_dir = os.path.join(args.output, label)
        os.makedirs(dst_dir, exist_ok=True)

        if not os.path.exists(src_dir):
            print(f"Skipping {label}: directory not found")
            continue

        files = [f for f in os.listdir(src_dir) if f.endswith(".npy")]
        print(f"{label}: {len(files)} original files -> {len(files) * args.multiply} augmented")

        count = 0
        for fname in files:
            img = np.load(os.path.join(src_dir, fname))

            # 保存原图
            np.save(os.path.join(dst_dir, f"orig_{count:05d}.npy"), img)
            count += 1

            # 增强副本
            for i in range(args.multiply - 1):
                aug = augment_image(img)
                np.save(os.path.join(dst_dir, f"aug_{count:05d}.npy"), aug)
                count += 1

        print(f"  Saved {count} files to {dst_dir}/")

    print("\nDone!")

if __name__ == "__main__":
    main()
