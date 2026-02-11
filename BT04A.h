/**
 * @file BT04A.h
 * @brief BT04A蓝牙模块库 - Arduino版
 * @version 1.0.0
 * @date 2024
 * 
 * 支持功能：
 * - AT指令配置
 * - BLE数据传输
 * - 多种工作模式
 * - 事件回调
 */

#ifndef BT04A_H
#define BT04A_H

#include <Arduino.h>
#include <SoftwareSerial.h>

// 工作模式枚举
enum BT04A_Mode {
    MODE_NORMAL = 0,    // 正常模式
    MODE_AT,            // AT指令模式
    MODE_DATA,          // 数据传输模式
    MODE_SLEEP          // 睡眠模式
};

// 连接状态枚举
enum BT04A_Status {
    STATUS_DISCONNECTED = 0,
    STATUS_CONNECTED,
    STATUS_CONNECTING,
    STATUS_DISCONNECTING
};

// 回调函数类型定义
typedef void (*DataReceivedCallback)(const String& data);
typedef void (*ConnectionCallback)(bool connected);
typedef void (*ErrorCallback)(const String& error);

class BT04A {
public:
    /**
     * @brief 构造函数 - 使用硬件串口
     * @param rxPin 接收引脚
     * @param txPin 发送引脚
     * @param enablePin EN使能引脚
     */
    BT04A(uint8_t rxPin, uint8_t txPin, uint8_t enablePin = -1);
    
    /**
     * @brief 构造函数 - 使用指定串口
     * @param serial 串口对象引用
     * @param enablePin EN使能引脚
     */
    BT04A(HardwareSerial& serial, uint8_t enablePin = -1);
    
    /**
     * @brief 初始化蓝牙模块
     * @param baudRate 波特率（默认115200）
     * @return bool 初始化是否成功
     */
    bool begin(unsigned long baudRate = 115200);
    
    /**
     * @brief 设置工作模式
     * @param mode 工作模式
     * @return bool 设置是否成功
     */
    bool setMode(BT04A_Mode mode);
    
    /**
     * @brief 发送AT指令
     * @param command AT指令
     * @param timeout 超时时间（毫秒）
     * @return String 响应数据
     */
    String sendATCommand(const String& command, unsigned long timeout = 1000);
    
    /**
     * @brief 发送数据
     * @param data 要发送的数据
     * @return bool 发送是否成功
     */
    bool sendData(const String& data);
    bool sendData(const uint8_t* data, size_t length);
    
    /**
     * @brief 读取接收到的数据
     * @return String 接收到的数据
     */
    String readData();
    
    /**
     * @brief 检查是否有数据可读
     * @return bool 是否有数据
     */
    bool available();
    
    /**
     * @brief 重置模块
     * @return bool 重置是否成功
     */
    bool reset();
    
    /**
     * @brief 设置设备名称
     * @param name 设备名称
     * @return bool 设置是否成功
     */
    bool setName(const String& name);
    
    /**
     * @brief 设置PIN码
     * @param pin PIN码（4-6位数字）
     * @return bool 设置是否成功
     */
    bool setPIN(const String& pin);
    
    /**
     * @brief 获取模块版本信息
     * @return String 版本信息
     */
    String getVersion();
    
    /**
     * @brief 获取模块地址
     * @return String MAC地址
     */
    String getAddress();
    
    /**
     * @brief 设置发射功率
     * @param power 功率等级 0-7（7最大）
     * @return bool 设置是否成功
     */
    bool setTxPower(uint8_t power);
    
    /**
     * @brief 设置回调函数
     */
    void onDataReceived(DataReceivedCallback callback);
    void onConnectionChanged(ConnectionCallback callback);
    void onError(ErrorCallback callback);
    
    /**
     * @brief 处理模块事件（需要在loop中调用）
     */
    void handleEvents();
    
    /**
     * @brief 获取连接状态
     * @return BT04A_Status 连接状态
     */
    BT04A_Status getStatus();
    
    /**
     * @brief 进入AT指令模式
     * @return bool 是否成功进入
     */
    bool enterATMode();
    
    /**
     * @brief 退出AT指令模式
     * @return bool 是否成功退出
     */
    bool exitATMode();
    
private:
    // 串口对象
    HardwareSerial* _hardwareSerial;
    SoftwareSerial* _softwareSerial;
    Stream* _serial;
    
    // 引脚定义
    uint8_t _rxPin;
    uint8_t _txPin;
    uint8_t _enablePin;
    
    // 状态变量
    BT04A_Mode _currentMode;
    BT04A_Status _currentStatus;
    bool _useSoftwareSerial;
    bool _initialized;
    
    // 缓冲区
    String _receiveBuffer;
    
    // 回调函数
    DataReceivedCallback _dataCallback;
    ConnectionCallback _connectionCallback;
    ErrorCallback _errorCallback;
    
    // 私有方法
    void _processIncomingData();
    void _parseResponse(const String& response);
    String _readSerial(unsigned long timeout);
    bool _sendRaw(const String& data);
    bool _waitForResponse(const String& expected, unsigned long timeout);
    void _setEnablePin(bool state);
};

#endif // BT04A_H