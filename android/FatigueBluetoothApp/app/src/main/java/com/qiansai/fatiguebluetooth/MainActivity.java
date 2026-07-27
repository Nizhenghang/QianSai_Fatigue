package com.qiansai.fatiguebluetooth;

import android.Manifest;
import android.app.Activity;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothGatt;
import android.bluetooth.BluetoothGattCallback;
import android.bluetooth.BluetoothGattCharacteristic;
import android.bluetooth.BluetoothGattDescriptor;
import android.bluetooth.BluetoothGattService;
import android.bluetooth.BluetoothManager;
import android.bluetooth.BluetoothProfile;
import android.bluetooth.BluetoothSocket;
import android.bluetooth.le.BluetoothLeScanner;
import android.bluetooth.le.ScanCallback;
import android.bluetooth.le.ScanResult;
import android.content.Context;
import android.content.pm.PackageManager;
import android.graphics.Color;
import android.os.Build;
import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;
import android.view.Gravity;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.LinearLayout;
import android.widget.ScrollView;
import android.widget.Spinner;
import android.widget.TextView;

import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.lang.reflect.Method;
import java.nio.charset.StandardCharsets;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.UUID;

public class MainActivity extends Activity {
    private static final int REQ_BT_CONNECT = 1001;
    private static final UUID SPP_UUID = UUID.fromString("00001101-0000-1000-8000-00805F9B34FB");
    private static final UUID CCCD_UUID = UUID.fromString("00002902-0000-1000-8000-00805F9B34FB");

    private BluetoothAdapter bluetoothAdapter;
    private BluetoothLeScanner bleScanner;
    private BluetoothSocket socket;
    private BluetoothGatt bluetoothGatt;
    private BluetoothGattCharacteristic notifyCharacteristic;
    private final List<BluetoothDevice> bondedDevices = new ArrayList<>();
    private final List<BluetoothDevice> bleDevices = new ArrayList<>();
    private final Set<String> bleAddresses = new HashSet<>();
    private final Handler handler = new Handler(Looper.getMainLooper());

    private Spinner deviceSpinner;
    private Button connectButton;
    private Button scanBleButton;
    private TextView connectionText;
    private TextView levelText;
    private TextView scoreText;
    private TextView confidenceText;
    private TextView adviceText;
    private TextView logText;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        BluetoothManager manager = (BluetoothManager) getSystemService(Context.BLUETOOTH_SERVICE);
        bluetoothAdapter = manager == null ? BluetoothAdapter.getDefaultAdapter() : manager.getAdapter();
        bleScanner = bluetoothAdapter == null ? null : bluetoothAdapter.getBluetoothLeScanner();
        buildUi();
        ensureBluetoothPermission();
        refreshDevices();
    }

    @Override
    protected void onDestroy() {
        closeSocket();
        closeBle();
        super.onDestroy();
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, String[] permissions, int[] grantResults) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults);
        if (requestCode == REQ_BT_CONNECT) {
            refreshDevices();
        }
    }

    private void buildUi() {
        LinearLayout root = new LinearLayout(this);
        root.setOrientation(LinearLayout.VERTICAL);
        root.setPadding(dp(18), dp(18), dp(18), dp(18));
        root.setBackgroundColor(Color.rgb(245, 247, 250));

        TextView title = text("驾驶疲劳监测", 26, Color.rgb(17, 24, 39), true);
        root.addView(title);

        connectionText = text("未连接", 15, Color.rgb(75, 85, 99), false);
        root.addView(connectionText);

        deviceSpinner = new Spinner(this);
        root.addView(deviceSpinner, new LinearLayout.LayoutParams(-1, dp(48)));

        connectButton = new Button(this);
        connectButton.setText("连接经典蓝牙");
        connectButton.setAllCaps(false);
        connectButton.setOnClickListener(v -> connectSelectedDevice());
        root.addView(connectButton, new LinearLayout.LayoutParams(-1, dp(48)));

        scanBleButton = new Button(this);
        scanBleButton.setText("扫描/连接 BLE");
        scanBleButton.setAllCaps(false);
        scanBleButton.setOnClickListener(v -> scanBleDevices());
        root.addView(scanBleButton, new LinearLayout.LayoutParams(-1, dp(48)));

        LinearLayout statusPanel = panel();
        levelText = text("状态：等待数据", 24, Color.rgb(17, 24, 39), true);
        scoreText = text("疲劳分数：--", 20, Color.rgb(37, 99, 235), true);
        confidenceText = text("模型置信度：--", 16, Color.rgb(75, 85, 99), false);
        adviceText = text("建议：请先连接蓝牙模块", 16, Color.rgb(55, 65, 81), false);
        statusPanel.addView(levelText);
        statusPanel.addView(scoreText);
        statusPanel.addView(confidenceText);
        statusPanel.addView(adviceText);
        root.addView(statusPanel);

        TextView logTitle = text("接收日志", 17, Color.rgb(31, 41, 55), true);
        root.addView(logTitle);

        ScrollView scrollView = new ScrollView(this);
        logText = text("", 13, Color.rgb(55, 65, 81), false);
        logText.setGravity(Gravity.START);
        scrollView.addView(logText);
        root.addView(scrollView, new LinearLayout.LayoutParams(-1, 0, 1));

        setContentView(root);
    }

    private void ensureBluetoothPermission() {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.S) {
            List<String> permissions = new ArrayList<>();
            if (checkSelfPermission(Manifest.permission.BLUETOOTH_CONNECT) != PackageManager.PERMISSION_GRANTED) {
                permissions.add(Manifest.permission.BLUETOOTH_CONNECT);
            }
            if (checkSelfPermission(Manifest.permission.BLUETOOTH_SCAN) != PackageManager.PERMISSION_GRANTED) {
                permissions.add(Manifest.permission.BLUETOOTH_SCAN);
            }
            if (!permissions.isEmpty()) {
                requestPermissions(permissions.toArray(new String[0]), REQ_BT_CONNECT);
            }
        } else if (checkSelfPermission(Manifest.permission.ACCESS_FINE_LOCATION) != PackageManager.PERMISSION_GRANTED) {
            requestPermissions(new String[]{Manifest.permission.ACCESS_FINE_LOCATION}, REQ_BT_CONNECT);
        }
    }

    private boolean hasBluetoothPermission() {
        return Build.VERSION.SDK_INT < Build.VERSION_CODES.S ||
                checkSelfPermission(Manifest.permission.BLUETOOTH_CONNECT) == PackageManager.PERMISSION_GRANTED;
    }

    private boolean hasBleScanPermission() {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.S) {
            return checkSelfPermission(Manifest.permission.BLUETOOTH_SCAN) == PackageManager.PERMISSION_GRANTED &&
                    checkSelfPermission(Manifest.permission.BLUETOOTH_CONNECT) == PackageManager.PERMISSION_GRANTED;
        }
        return checkSelfPermission(Manifest.permission.ACCESS_FINE_LOCATION) == PackageManager.PERMISSION_GRANTED;
    }

    private void refreshDevices() {
        bondedDevices.clear();
        List<String> names = new ArrayList<>();

        if (bluetoothAdapter == null) {
            names.add("本机不支持蓝牙");
        } else if (!hasBluetoothPermission()) {
            names.add("请授予蓝牙权限");
        } else {
            Set<BluetoothDevice> devices = bluetoothAdapter.getBondedDevices();
            for (BluetoothDevice device : devices) {
                String name = device.getName() == null ? "Unknown" : device.getName();
                if (isTargetName(name)) {
                    bondedDevices.add(device);
                    names.add(name + "  " + device.getAddress());
                }
            }
            if (names.isEmpty()) {
                names.add("请先在系统蓝牙中配对 HC-05");
            }
        }

        ArrayAdapter<String> adapter = new ArrayAdapter<>(this, android.R.layout.simple_spinner_dropdown_item, names);
        deviceSpinner.setAdapter(adapter);
    }

    private void scanBleDevices() {
        if (!hasBleScanPermission()) {
            ensureBluetoothPermission();
            return;
        }
        if (bleScanner == null) {
            appendLog("BLE 扫描器不可用。");
            return;
        }

        closeBle();
        bleDevices.clear();
        bleAddresses.clear();
        setConnection("正在扫描 BLE...");
        appendLog("开始扫描 BLE 设备。");

        try {
            bleScanner.startScan(scanCallback);
            handler.postDelayed(() -> {
                try {
                    bleScanner.stopScan(scanCallback);
                } catch (Exception ignored) {
                }
                connectFirstBleDevice();
            }, 6000);
        } catch (Exception e) {
            setConnection("BLE 扫描失败");
            appendLog("BLE 扫描失败：" + e.getMessage());
        }
    }

    private final ScanCallback scanCallback = new ScanCallback() {
        @Override
        public void onScanResult(int callbackType, ScanResult result) {
            BluetoothDevice device = result.getDevice();
            String address = device.getAddress();
            String name = safeName(device);
            if (address != null && isTargetName(name) && !bleAddresses.contains(address)) {
                bleAddresses.add(address);
                bleDevices.add(device);
                appendLog("发现 BLE：" + name + "  " + address);
            }
        }
    };

    private void connectFirstBleDevice() {
        if (bleDevices.isEmpty()) {
            setConnection("未发现 BLE 模块");
            appendLog("未发现 MHX/BT/HC 名称的 BLE 设备。");
            return;
        }
        BluetoothDevice device = bleDevices.get(0);
        setConnection("连接 BLE：" + safeName(device));
        try {
            bluetoothGatt = device.connectGatt(this, false, gattCallback);
        } catch (Exception e) {
            setConnection("BLE 连接失败");
            appendLog("BLE 连接失败：" + e.getMessage());
        }
    }

    private final BluetoothGattCallback gattCallback = new BluetoothGattCallback() {
        @Override
        public void onConnectionStateChange(BluetoothGatt gatt, int status, int newState) {
            if (newState == BluetoothProfile.STATE_CONNECTED) {
                runOnUiThread(() -> setConnection("BLE 已连接，发现服务中..."));
                gatt.discoverServices();
            } else if (newState == BluetoothProfile.STATE_DISCONNECTED) {
                runOnUiThread(() -> setConnection("BLE 已断开"));
            }
        }

        @Override
        public void onServicesDiscovered(BluetoothGatt gatt, int status) {
            BluetoothGattCharacteristic found = findNotifyCharacteristic(gatt);
            if (found == null) {
                runOnUiThread(() -> {
                    setConnection("BLE 未找到串口特征");
                    appendLog("BLE 服务中没有 notify/read 特征。");
                });
                return;
            }

            notifyCharacteristic = found;
            gatt.setCharacteristicNotification(found, true);
            BluetoothGattDescriptor descriptor = found.getDescriptor(CCCD_UUID);
            if (descriptor != null) {
                descriptor.setValue(BluetoothGattDescriptor.ENABLE_NOTIFICATION_VALUE);
                gatt.writeDescriptor(descriptor);
            }
            runOnUiThread(() -> {
                setConnection("BLE 已连接：" + safeName(gatt.getDevice()));
                appendLog("BLE 通知已开启。");
            });
        }

        @Override
        public void onCharacteristicChanged(BluetoothGatt gatt, BluetoothGattCharacteristic characteristic) {
            byte[] value = characteristic.getValue();
            if (value != null && value.length > 0) {
                String text = new String(value, StandardCharsets.UTF_8);
                runOnUiThread(() -> feedReceivedText(text));
            }
        }
    };

    private BluetoothGattCharacteristic findNotifyCharacteristic(BluetoothGatt gatt) {
        for (BluetoothGattService service : gatt.getServices()) {
            for (BluetoothGattCharacteristic characteristic : service.getCharacteristics()) {
                int props = characteristic.getProperties();
                if ((props & BluetoothGattCharacteristic.PROPERTY_NOTIFY) != 0 ||
                        (props & BluetoothGattCharacteristic.PROPERTY_INDICATE) != 0) {
                    return characteristic;
                }
            }
        }
        for (BluetoothGattService service : gatt.getServices()) {
            for (BluetoothGattCharacteristic characteristic : service.getCharacteristics()) {
                int props = characteristic.getProperties();
                if ((props & BluetoothGattCharacteristic.PROPERTY_READ) != 0) {
                    return characteristic;
                }
            }
        }
        return null;
    }

    private void connectSelectedDevice() {
        if (!hasBluetoothPermission()) {
            ensureBluetoothPermission();
            return;
        }
        if (bondedDevices.isEmpty()) {
            refreshDevices();
            appendLog("未找到已配对的 HC-05。");
            return;
        }

        int index = Math.max(0, deviceSpinner.getSelectedItemPosition());
        BluetoothDevice device = bondedDevices.get(index);
        setConnection("连接中：" + safeName(device));

        new Thread(() -> {
            closeSocket();
            closeBle();
            try {
                socket = connectWithFallback(device);
                runOnUiThread(() -> setConnection("已连接：" + safeName(device)));
                readLoop();
            } catch (Exception e) {
                runOnUiThread(() -> {
                    setConnection("连接失败");
                    appendLog("连接失败：" + e.getMessage());
                });
                closeSocket();
            }
        }).start();
    }

    private BluetoothSocket connectWithFallback(BluetoothDevice device) throws Exception {
        Exception lastError;

        try {
            BluetoothSocket secureSocket = device.createRfcommSocketToServiceRecord(SPP_UUID);
            secureSocket.connect();
            return secureSocket;
        } catch (Exception e) {
            lastError = e;
            closeSocket();
        }

        try {
            BluetoothSocket insecureSocket = device.createInsecureRfcommSocketToServiceRecord(SPP_UUID);
            insecureSocket.connect();
            return insecureSocket;
        } catch (Exception e) {
            lastError = e;
            closeSocket();
        }

        try {
            Method method = device.getClass().getMethod("createRfcommSocket", int.class);
            BluetoothSocket channelSocket = (BluetoothSocket) method.invoke(device, 1);
            channelSocket.connect();
            return channelSocket;
        } catch (Exception e) {
            lastError = e;
            closeSocket();
        }

        throw lastError;
    }

    private void readLoop() throws Exception {
        BufferedReader reader = new BufferedReader(new InputStreamReader(socket.getInputStream(), StandardCharsets.UTF_8));
        String line;
        while ((line = reader.readLine()) != null) {
            String received = line.trim();
            runOnUiThread(() -> handleLine(received));
        }
    }

    private final StringBuilder receiveBuffer = new StringBuilder();

    private void feedReceivedText(String text) {
        receiveBuffer.append(text);
        int index;
        while ((index = receiveBuffer.indexOf("\n")) >= 0) {
            String line = receiveBuffer.substring(0, index).trim();
            receiveBuffer.delete(0, index + 1);
            if (!line.isEmpty()) {
                handleLine(line);
            }
        }
    }

    private void handleLine(String line) {
        appendLog(line);
        if (!line.startsWith("BT|")) {
            return;
        }

        String[] parts = line.split("\\|");
        Map<String, String> fields = new HashMap<>();
        for (String part : parts) {
            int pos = part.indexOf('=');
            if (pos > 0 && pos < part.length() - 1) {
                fields.put(part.substring(0, pos), part.substring(pos + 1));
            }
        }

        String type = parts.length > 1 ? parts[1] : "";
        String status = value(fields, "status", "UNKNOWN");
        String score = value(fields, "score", "--");
        String confidence = value(fields, "conf", "--");

        if ("STATE".equals(type) || "ALERT".equals(type)) {
            levelText.setText("状态：" + statusToChinese(status));
            scoreText.setText("疲劳分数：" + score);
            confidenceText.setText("模型置信度：" + confidence + "%");
            adviceText.setText("建议：" + adviceForStatus(status));
            levelText.setTextColor(colorForStatus(status));
        }
    }

    private String statusToChinese(String status) {
        if ("NORMAL".equals(status)) return "正常";
        if ("LIGHT".equals(status)) return "轻度疲劳";
        if ("WARN".equals(status)) return "中度疲劳";
        if ("DANGER".equals(status)) return "严重疲劳";
        if ("NO_FACE".equals(status)) return "未检测到人脸";
        return status;
    }

    private String adviceForStatus(String status) {
        if ("NORMAL".equals(status)) return "状态稳定，继续保持。";
        if ("LIGHT".equals(status)) return "注意力下降，建议调整坐姿并保持专注。";
        if ("WARN".equals(status)) return "疲劳趋势明显，建议尽快停车休息。";
        if ("DANGER".equals(status)) return "严重疲劳报警，请立即停车休息。";
        if ("NO_FACE".equals(status)) return "请回到摄像头检测范围内。";
        return "请观察驾驶员状态。";
    }

    private int colorForStatus(String status) {
        if ("NORMAL".equals(status)) return Color.rgb(22, 163, 74);
        if ("LIGHT".equals(status)) return Color.rgb(234, 179, 8);
        if ("WARN".equals(status)) return Color.rgb(249, 115, 22);
        if ("DANGER".equals(status)) return Color.rgb(220, 38, 38);
        if ("NO_FACE".equals(status)) return Color.rgb(79, 70, 229);
        return Color.rgb(17, 24, 39);
    }

    private void appendLog(String line) {
        String old = logText.getText().toString();
        String next = line + "\n" + old;
        if (next.length() > 5000) {
            next = next.substring(0, 5000);
        }
        logText.setText(next);
    }

    private void setConnection(String text) {
        connectionText.setText(text);
    }

    private String value(Map<String, String> fields, String key, String fallback) {
        String value = fields.get(key);
        return value == null ? fallback : value;
    }

    private String safeName(BluetoothDevice device) {
        if (!hasBluetoothPermission()) {
            return "HC-05";
        }
        String name = device.getName();
        return name == null ? device.getAddress() : name;
    }

    private boolean isTargetName(String name) {
        if (name == null) {
            return false;
        }
        String upper = name.toUpperCase();
        return upper.contains("HC") || upper.contains("BT") || upper.contains("MHX");
    }

    private TextView text(String value, int sp, int color, boolean bold) {
        TextView textView = new TextView(this);
        textView.setText(value);
        textView.setTextSize(sp);
        textView.setTextColor(color);
        textView.setPadding(0, dp(6), 0, dp(6));
        if (bold) {
            textView.setTypeface(android.graphics.Typeface.DEFAULT_BOLD);
        }
        return textView;
    }

    private LinearLayout panel() {
        LinearLayout layout = new LinearLayout(this);
        layout.setOrientation(LinearLayout.VERTICAL);
        layout.setPadding(dp(16), dp(14), dp(16), dp(14));
        layout.setBackgroundColor(Color.WHITE);
        LinearLayout.LayoutParams params = new LinearLayout.LayoutParams(-1, -2);
        params.setMargins(0, dp(16), 0, dp(16));
        layout.setLayoutParams(params);
        return layout;
    }

    private int dp(int value) {
        return (int) (value * getResources().getDisplayMetrics().density + 0.5f);
    }

    private void closeSocket() {
        try {
            if (socket != null) {
                socket.close();
            }
        } catch (Exception ignored) {
        }
        socket = null;
    }

    private void closeBle() {
        try {
            if (bluetoothGatt != null) {
                bluetoothGatt.disconnect();
                bluetoothGatt.close();
            }
        } catch (Exception ignored) {
        }
        bluetoothGatt = null;
        notifyCharacteristic = null;
    }
}
