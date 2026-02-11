/**
 * @file test.ino
 * @brief BT04A蓝牙模块基础示例
 */

#include <BT04A.h>

// 定义引脚
#define BT_RX_PIN 2
#define BT_TX_PIN 3
#define BT_EN_PIN 4

// 创建BT04A实例
BT04A bt(BT_RX_PIN, BT_TX_PIN, BT_EN_PIN);

void setup() {
    Serial.begin(115200);
    while (!Serial);
    
    Serial.println("BT04A Bluetooth Module Example");
    
    // 初始化蓝牙模块
    if (bt.begin(115200)) {
        Serial.println("BT04A initialized successfully!");
        
        // 获取模块信息
        Serial.print("Version: ");
        Serial.println(bt.getVersion());
        
        Serial.print("MAC Address: ");
        Serial.println(bt.getAddress());
        
        // 设置设备名称
        bt.setName("MyBT04A");
        
        // 设置PIN码
        bt.setPIN("1234");
        
        // 设置发射功率
        bt.setTxPower(7);
        
        // 设置回调函数
        bt.onDataReceived(onDataReceived);
        bt.onConnectionChanged(onConnectionChanged);
        bt.onError(onError);
        
        Serial.println("Waiting for connection...");
    } else {
        Serial.println("Failed to initialize BT04A!");
        while (1);
    }
}

void loop() {
    // 处理蓝牙事件
    bt.handleEvents();
    
    // 从串口读取数据并发送到蓝牙
    if (Serial.available()) {
        String data = Serial.readString();
        if (bt.sendData(data)) {
            Serial.print("Sent: ");
            Serial.println(data);
        }
    }
    
    // 读取蓝牙数据并显示
    if (bt.available()) {
        String data = bt.readData();
        Serial.print("Received: ");
        Serial.println(data);
    }
    
    delay(10);
}

// 回调函数：数据接收
void onDataReceived(const String& data) {
    Serial.print("[Callback] Data received: ");
    Serial.println(data);
}

// 回调函数：连接状态变化
void onConnectionChanged(bool connected) {
    Serial.print("[Callback] Connection ");
    Serial.println(connected ? "established" : "lost");
}

// 回调函数：错误处理
void onError(const String& error) {
    Serial.print("[Callback] Error: ");
    Serial.println(error);
}
