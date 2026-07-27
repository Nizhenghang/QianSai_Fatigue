"""
Train an RA8D1-compatible fatigue CNN.

This keeps the deployed network topology unchanged so the generated weights
remain compatible with code/cnn_inference.c.
"""
import argparse
import os
import random

import numpy as np
import tensorflow as tf
from tensorflow import keras


LABELS = ["normal", "fatigued"]
IMAGE_H = 32
IMAGE_W = 64


def set_seed(seed):
    random.seed(seed)
    np.random.seed(seed)
    tf.random.set_seed(seed)


def load_split(data_dir, val_ratio, seed):
    train_images, train_labels = [], []
    val_images, val_labels = [], []
    rng = np.random.default_rng(seed)

    for label_value, label_name in enumerate(LABELS):
        label_dir = os.path.join(data_dir, label_name)
        files = sorted(
            os.path.join(label_dir, name)
            for name in os.listdir(label_dir)
            if name.endswith(".npy")
        )
        if not files:
            raise RuntimeError(f"No .npy files found in {label_dir}")

        indices = rng.permutation(len(files))
        val_count = max(1, int(round(len(files) * val_ratio)))
        val_index_set = set(indices[:val_count].tolist())

        for index, path in enumerate(files):
            image = np.load(path)
            if image.shape != (IMAGE_H, IMAGE_W):
                raise RuntimeError(f"Unexpected image shape {image.shape}: {path}")

            target_images = val_images if index in val_index_set else train_images
            target_labels = val_labels if index in val_index_set else train_labels
            target_images.append(image)
            target_labels.append(label_value)

        print(
            f"{label_name}: total={len(files)}, "
            f"train={len(files) - val_count}, val={val_count}"
        )

    x_train = np.asarray(train_images, dtype=np.float32) / 255.0
    y_train = np.asarray(train_labels, dtype=np.int32)
    x_val = np.asarray(val_images, dtype=np.float32) / 255.0
    y_val = np.asarray(val_labels, dtype=np.int32)

    x_train = x_train[..., np.newaxis]
    x_val = x_val[..., np.newaxis]

    train_order = rng.permutation(len(x_train))
    val_order = rng.permutation(len(x_val))
    return x_train[train_order], y_train[train_order], x_val[val_order], y_val[val_order]


def augment_batch(images, labels, multiply, seed):
    rng = np.random.default_rng(seed)
    augmented_images = [images]
    augmented_labels = [labels]

    for _ in range(max(0, multiply - 1)):
        result = images.copy()

        brightness = rng.uniform(-0.12, 0.12, size=(len(images), 1, 1, 1))
        contrast = rng.uniform(0.85, 1.15, size=(len(images), 1, 1, 1))
        noise = rng.normal(0.0, 0.025, size=result.shape)

        result = (result - 0.5) * contrast + 0.5 + brightness + noise

        for idx in range(len(result)):
            if rng.random() > 0.5:
                result[idx] = result[idx, :, ::-1, :]

            dx = int(rng.integers(-4, 5))
            dy = int(rng.integers(-2, 3))
            shifted = np.zeros_like(result[idx])

            src_y_start = max(0, -dy)
            src_y_end = min(IMAGE_H, IMAGE_H - dy)
            src_x_start = max(0, -dx)
            src_x_end = min(IMAGE_W, IMAGE_W - dx)
            dst_y_start = max(0, dy)
            dst_x_start = max(0, dx)

            shifted[
                dst_y_start : dst_y_start + (src_y_end - src_y_start),
                dst_x_start : dst_x_start + (src_x_end - src_x_start),
                :,
            ] = result[idx, src_y_start:src_y_end, src_x_start:src_x_end, :]
            result[idx] = shifted

        augmented_images.append(np.clip(result, 0.0, 1.0).astype(np.float32))
        augmented_labels.append(labels.copy())

    x_aug = np.concatenate(augmented_images, axis=0)
    y_aug = np.concatenate(augmented_labels, axis=0)
    order = rng.permutation(len(x_aug))
    return x_aug[order], y_aug[order]


def build_model():
    model = keras.Sequential(
        [
            keras.layers.InputLayer(input_shape=(IMAGE_H, IMAGE_W, 1)),
            keras.layers.Conv2D(8, (5, 5), strides=(2, 2), activation="relu", padding="valid"),
            keras.layers.MaxPooling2D((2, 2)),
            keras.layers.Conv2D(16, (3, 3), activation="relu", padding="valid"),
            keras.layers.MaxPooling2D((2, 2)),
            keras.layers.Flatten(),
            keras.layers.Dense(32, activation="relu", kernel_regularizer=keras.regularizers.l2(1e-4)),
            keras.layers.Dropout(0.15),
            keras.layers.Dense(2, activation="softmax"),
        ]
    )

    model.compile(
        optimizer=keras.optimizers.Adam(learning_rate=1e-3),
        loss="sparse_categorical_crossentropy",
        metrics=["accuracy"],
    )
    return model


def evaluate_model(model, x_val, y_val):
    probs = model.predict(x_val, verbose=0)
    pred = np.argmax(probs, axis=1)
    confusion = np.zeros((2, 2), dtype=np.int32)
    for truth, guess in zip(y_val, pred):
        confusion[truth, guess] += 1

    accuracy = float(np.mean(pred == y_val))
    fatigued_probs = probs[:, 1]
    print("\nValidation results")
    print(f"accuracy={accuracy:.4f}")
    print("confusion rows=true, cols=pred")
    print(confusion)
    print(
        "fatigued probability: "
        f"min={fatigued_probs.min():.4f}, "
        f"mean={fatigued_probs.mean():.4f}, "
        f"max={fatigued_probs.max():.4f}"
    )
    return accuracy, confusion


def main():
    parser = argparse.ArgumentParser(description="Train RA8D1-compatible fatigue CNN v2")
    parser.add_argument("--data", default="training_data", help="Raw training data directory")
    parser.add_argument("--output", default="fatigue_model_ra8d1_v2.h5", help="Output .h5 model")
    parser.add_argument("--epochs", type=int, default=80)
    parser.add_argument("--batch", type=int, default=32)
    parser.add_argument("--augment", type=int, default=6, help="Train-only augmentation multiplier")
    parser.add_argument("--val-ratio", type=float, default=0.2)
    parser.add_argument("--seed", type=int, default=20260626)
    args = parser.parse_args()

    set_seed(args.seed)
    x_train, y_train, x_val, y_val = load_split(args.data, args.val_ratio, args.seed)
    x_train, y_train = augment_batch(x_train, y_train, args.augment, args.seed + 1)

    print(f"\nTrain augmented: {x_train.shape}, Val raw: {x_val.shape}")

    model = build_model()
    model.summary()

    callbacks = [
        keras.callbacks.EarlyStopping(
            monitor="val_accuracy",
            patience=15,
            restore_best_weights=True,
        ),
        keras.callbacks.ReduceLROnPlateau(
            monitor="val_loss",
            factor=0.5,
            patience=6,
            min_lr=1e-5,
        ),
    ]

    model.fit(
        x_train,
        y_train,
        validation_data=(x_val, y_val),
        epochs=args.epochs,
        batch_size=args.batch,
        callbacks=callbacks,
        verbose=1,
    )

    evaluate_model(model, x_val, y_val)
    model.save(args.output)
    print(f"\nModel saved to {args.output}")


if __name__ == "__main__":
    main()
