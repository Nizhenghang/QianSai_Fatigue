"""
串口图像采集脚本
从 RA8D1 接收 64x32 灰度帧并保存为 .npy 文件

用法：
  python collect_frames.py --port COM3 --label normal
  python collect_frames.py --port COM3 --label fatigued

协议：[0xAA, 0x55, seq_hi, seq_lo, label] + 2048字节 + [0x0D, 0x0A]
"""
import serial
import numpy as np
import argparse
import os
import time

FRAME_W = 64
FRAME_H = 32
FRAME_SIZE = FRAME_W * FRAME_H  # 2048
HEADER_SIZE = 5
FOOTER_SIZE = 2
MAGIC = bytes([0xAA, 0x55])

def collect(port, label, output_dir, baudrate=115200, max_frames=500):
    save_dir = os.path.join(output_dir, label)
    os.makedirs(save_dir, exist_ok=True)

    ser = serial.Serial(port, baudrate, timeout=5)
    print(f"Connected to {port} @ {baudrate}")
    print(f"Saving {label} frames to {save_dir}/")
    print(f"Target: {max_frames} frames. Press Ctrl+C to stop early.\n")

    count = 0
    buf = bytearray()

    try:
        while count < max_frames:
            # 读取数据
            chunk = ser.read(4096)
            if not chunk:
                print("Timeout - no data received")
                continue
            buf.extend(chunk)

            # 查找帧头
            while len(buf) >= HEADER_SIZE + FRAME_SIZE + FOOTER_SIZE:
                # 查找 magic
                idx = buf.find(MAGIC)
                if idx < 0:
                    buf = buf[-4:]  # 保留末尾防止 magic 被截断
                    break

                # 丢弃 magic 之前的数据
                if idx > 0:
                    buf = buf[idx:]

                # 检查是否有完整帧
                if len(buf) < HEADER_SIZE + FRAME_SIZE + FOOTER_SIZE:
                    break

                # 解析帧头
                frame_label = buf[4]
                seq = (buf[2] << 8) | buf[3]

                # 提取像素数据
                pixels = bytes(buf[HEADER_SIZE:HEADER_SIZE + FRAME_SIZE])

                # 验证帧尾
                footer = buf[HEADER_SIZE + FRAME_SIZE:HEADER_SIZE + FRAME_SIZE + FOOTER_SIZE]
                if footer != bytes([0x0D, 0x0A]):
                    # 帧尾不匹配，跳过这个 magic
                    buf = buf[2:]
                    continue

                # 保存为 numpy 数组
                img = np.frombuffer(pixels, dtype=np.uint8).reshape(FRAME_H, FRAME_W)
                filename = os.path.join(save_dir, f"{label}_{count:04d}.npy")
                np.save(filename, img)

                count += 1
                if count % 50 == 0:
                    print(f"  [{label}] Collected {count}/{max_frames} frames (seq={seq})")

                # 移除已处理的数据
                buf = buf[HEADER_SIZE + FRAME_SIZE + FOOTER_SIZE:]

    except KeyboardInterrupt:
        print(f"\nStopped by user.")

    ser.close()
    print(f"\nDone! Collected {count} frames saved to {save_dir}/")

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Collect training frames from RA8D1")
    parser.add_argument("--port", required=True, help="Serial port (e.g. COM3)")
    parser.add_argument("--label", required=True, choices=["normal", "fatigued"],
                        help="Frame label")
    parser.add_argument("--output", default="training_data", help="Output directory")
    parser.add_argument("--max", type=int, default=500, help="Max frames to collect")
    args = parser.parse_args()

    collect(args.port, args.label, args.output, max_frames=args.max)
