"""
CNN 训练脚本
训练轻量级疲劳检测 CNN 模型

用法：
  python train_cnn.py --data augmented_data --epochs 30
  python train_cnn.py --data training_data --epochs 50   # 不增强直接训练
"""
import numpy as np
import tensorflow as tf
from tensorflow import keras
import argparse
import os

def load_data(data_dir):
    """加载 .npy 图像数据"""
    images = []
    labels = []

    for label_val, label_name in enumerate(["normal", "fatigued"]):
        dir_path = os.path.join(data_dir, label_name)
        if not os.path.exists(dir_path):
            print(f"Warning: {dir_path} not found, skipping")
            continue

        files = sorted([f for f in os.listdir(dir_path) if f.endswith(".npy")])
        print(f"  {label_name}: {len(files)} samples")

        for fname in files:
            img = np.load(os.path.join(dir_path, fname))
            images.append(img)
            labels.append(label_val)

    images = np.array(images, dtype=np.float32)
    labels = np.array(labels, dtype=np.int32)

    # 归一化到 [0, 1]
    images = images / 255.0

    # 添加通道维度: (N, 32, 64) -> (N, 32, 64, 1)
    images = images[..., np.newaxis]

    # 打乱
    indices = np.arange(len(images))
    np.random.shuffle(indices)
    images = images[indices]
    labels = labels[indices]

    return images, labels

def build_model():
    """构建轻量级 CNN"""
    model = keras.Sequential([
        keras.layers.InputLayer(input_shape=(32, 64, 1)),

        # Conv1: 5x5, 8 filters, stride 2 -> 14x30x8
        keras.layers.Conv2D(8, (5, 5), strides=(2, 2), activation='relu', padding='valid'),

        # MaxPool: 2x2 -> 7x15x8
        keras.layers.MaxPooling2D((2, 2)),

        # Conv2: 3x3, 16 filters -> 5x13x16
        keras.layers.Conv2D(16, (3, 3), activation='relu', padding='valid'),

        # MaxPool: 2x2 -> 2x6x16
        keras.layers.MaxPooling2D((2, 2)),

        keras.layers.Flatten(),

        # FC1: 192 -> 32
        keras.layers.Dense(32, activation='relu'),

        # FC2: 32 -> 2
        keras.layers.Dense(2, activation='softmax')
    ])

    model.compile(
        optimizer='adam',
        loss='sparse_categorical_crossentropy',
        metrics=['accuracy']
    )

    return model

def main():
    parser = argparse.ArgumentParser(description="Train fatigue detection CNN")
    parser.add_argument("--data", default="augmented_data", help="Data directory")
    parser.add_argument("--epochs", type=int, default=30, help="Training epochs")
    parser.add_argument("--batch", type=int, default=32, help="Batch size")
    parser.add_argument("--output", default="fatigue_model.h5", help="Output model file")
    args = parser.parse_args()

    print(f"Loading data from {args.data}/...")
    images, labels = load_data(args.data)
    print(f"Total: {len(images)} samples ({np.sum(labels==0)} normal, {np.sum(labels==1)} fatigued)")

    # 划分训练/验证集 (80/20)
    split = int(len(images) * 0.8)
    x_train, x_val = images[:split], images[split:]
    y_train, y_val = labels[:split], labels[split:]
    print(f"Train: {len(x_train)}, Val: {len(x_val)}")

    model = build_model()
    model.summary()

    print(f"\nTraining for {args.epochs} epochs...")
    history = model.fit(
        x_train, y_train,
        validation_data=(x_val, y_val),
        epochs=args.epochs,
        batch_size=args.batch,
        verbose=1
    )

    # 评估
    val_loss, val_acc = model.evaluate(x_val, y_val, verbose=0)
    print(f"\nFinal validation accuracy: {val_acc:.4f}")

    if val_acc < 0.85:
        print("WARNING: Accuracy below 85%. Consider:")
        print("  - Collecting more training data")
        print("  - More augmentation")
        print("  - Adjusting model architecture")

    model.save(args.output)
    print(f"Model saved to {args.output}")

if __name__ == "__main__":
    main()
