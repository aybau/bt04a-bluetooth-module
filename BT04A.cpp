/**
 * @file BT04A.cpp
 * @brief BT04A蓝牙模块库实现
 */

#include "BT04A.h"

// 构造函数 - 使用软件串口
BT04A::BT04A(uint8_t rxPin, uint8_t txPin, uint8_t enablePin) :
    _hardwareSerial(nullptr),
    _softwareSerial(nullptr),
    _serial(nullptr),
    _rxPin(rxPin),
    _txPin(txPin),
    _enablePin(enablePin),
    _currentMode(MODE_NORMAL),
    _currentStatus(STATUS_DISCONNECTED),
    _useSoftwareSerial(true),
    _initialized(false),
    _dataCallback(nullptr),
    _connectionCallback(nullptr),
    _errorCallback(nullptr)
{
    _softwareSerial = new SoftwareSerial(rxPin, txPin);
    _serial = _softwareSerial;
}

// 构造函数 - 使用硬件串口
BT04A::BT04A(HardwareSerial& serial, uint8_t enablePin) :
    _hardwareSerial(&serial),
    _softwareSerial(nullptr),
    _serial(&serial),
    _rxPin(0),
    _txPin(0),
    _enablePin(enablePin),
    _currentMode(MODE_NORMAL),
    _currentStatus(STATUS_DISCONNECTED),
    _useSoftwareSerial(false),
    _initialized(false),
    _dataCallback(nullptr),
    _connectionCallback(nullptr),
    _errorCallback(nullptr)
{
}

// 初始化模块
bool BT04A::begin(unsigned long baudRate) {
    if (_initialized) return true;
    
    // 配置使能引脚
    if (_enablePin != 255) {
        pinMode(_enablePin, OUTPUT);
        _setEnablePin(HIGH);
        delay(100);
    }
    
    // 初始化串口
    if (_useSoftwareSerial) {
        _softwareSerial->begin(baudRate);
    } else {
        _hardwareSerial->begin(baudRate);
    }
    
    delay(500); // 等待模块启动
    
    // 测试AT指令
    String response = sendATCommand("AT", 500);
    if (response.indexOf("OK") == -1) {
        // 尝试不同的波特率
        unsigned long baudRates[] = {9600, 19200, 38400, 57600, 115200, 230400};
        for (int i = 0; i < 6; i++) {
            if (_useSoftwareSerial) {
                _softwareSerial->end();
                _softwareSerial->begin(baudRates[i]);
            } else {
                _hardwareSerial->end();
                _hardwareSerial->begin(baudRates[i]);
            }
            delay(100);
            response = sendATCommand("AT", 300);
            if (response.indexOf("OK") != -1) {
                Serial.println("Found baud rate: " + String(baudRates[i]));
                break;
            }
        }
    }
    
    _initialized = (response.indexOf("OK") != -1);
    
    if (_initialized) {
        // 设置默认参数
        sendATCommand("AT+ROLE0");  // 外设模式
        sendATCommand("AT+IMME1");  // 上电不自动广播
        sendATCommand("AT+NOTI1");  // 开启通知
        sendATCommand("AT+ADTY3");  // 任意设备可连接
        setMode(MODE_NORMAL);
    }
    
    return _initialized;
}

// 发送AT指令
String BT04A::sendATCommand(const String& command, unsigned long timeout) {
    if (!_initialized) return "";
    
    // 确保在AT模式
    if (_currentMode != MODE_AT) {
        enterATMode();
    }
    
    _serial->print(command);
    _serial->print("\r\n");
    
    return _readSerial(timeout);
}

// 发送字符串数据
bool BT04A::sendData(const String& data) {
    if (_currentStatus != STATUS_CONNECTED) return false;
    
    if (_currentMode != MODE_DATA) {
        setMode(MODE_DATA);
    }
    
    return _sendRaw(data);
}

// 发送原始数据
bool BT04A::sendData(const uint8_t* data, size_t length) {
    if (_currentStatus != STATUS_CONNECTED) return false;
    
    if (_currentMode != MODE_DATA) {
        setMode(MODE_DATA);
    }
    
    for (size_t i = 0; i < length; i++) {
        _serial->write(data[i]);
    }
    
    return true;
}

// 读取数据
String BT04A::readData() {
    String data = _receiveBuffer;
    _receiveBuffer = "";
    return data;
}

// 检查是否有数据
bool BT04A::available() {
    _processIncomingData();
    return _receiveBuffer.length() > 0;
}

// 重置模块
bool BT04A::reset() {
    if (_enablePin != 255) {
        _setEnablePin(LOW);
        delay(100);
        _setEnablePin(HIGH);
        delay(1000);
        return true;
    }
    
    // 通过AT指令重置
    String response = sendATCommand("AT+RESET", 2000);
    return response.indexOf("OK") != -1;
}

// 设置设备名称
bool BT04A::setName(const String& name) {
    String command = "AT+NAME" + name;
    String response = sendATCommand(command);
    return response.indexOf("OK") != -1;
}

// 设置PIN码
bool BT04A::setPIN(const String& pin) {
    String command = "AT+PASS" + pin;
    String response = sendATCommand(command);
    return response.indexOf("OK") != -1;
}

// 获取版本信息
String BT04A::getVersion() {
    String response = sendATCommand("AT+VERR?");
    if (response.indexOf("+VERR:") != -1) {
        int start = response.indexOf("+VERR:") + 6;
        int end = response.indexOf("\r\n", start);
        return response.substring(start, end);
    }
    return "";
}

// 获取MAC地址
String BT04A::getAddress() {
    String response = sendATCommand("AT+LADDR?");
    if (response.indexOf("+LADDR:") != -1) {
        int start = response.indexOf("+LADDR:") + 7;
        int end = response.indexOf("\r\n", start);
        return response.substring(start, end);
    }
    return "";
}

// 设置发射功率
bool BT04A::setTxPower(uint8_t power) {
    if (power > 7) power = 7;
    String command = "AT+POWE" + String(power);
    String response = sendATCommand(command);
    return response.indexOf("OK") != -1;
}

// 设置回调函数
void BT04A::onDataReceived(DataReceivedCallback callback) {
    _dataCallback = callback;
}

void BT04A::onConnectionChanged(ConnectionCallback callback) {
    _connectionCallback = callback;
}

void BT04A::onError(ErrorCallback callback) {
    _errorCallback = callback;
}

// 处理事件
void BT04A::handleEvents() {
    _processIncomingData();
}

// 获取连接状态
BT04A_Status BT04A::getStatus() {
    return _currentStatus;
}

// 进入AT模式
bool BT04A::enterATMode() {
    if (_currentMode == MODE_AT) return true;
    
    _sendRaw("+++");
    delay(100);
    
    if (_waitForResponse("OK", 500)) {
        _currentMode = MODE_AT;
        return true;
    }
    return false;
}

// 退出AT模式
bool BT04A::exitATMode() {
    String response = sendATCommand("AT+ENTM");
    if (response.indexOf("OK") != -1) {
        _currentMode = MODE_NORMAL;
        return true;
    }
    return false;
}

// 设置工作模式
bool BT04A::setMode(BT04A_Mode mode) {
    switch(mode) {
        case MODE_AT:
            return enterATMode();
        case MODE_NORMAL:
        case MODE_DATA:
            return exitATMode();
        case MODE_SLEEP:
            return sendATCommand("AT+SLEEP").indexOf("OK") != -1;
        default:
            return false;
    }
}

// ============ 私有方法 ============

// 处理接收数据
void BT04A::_processIncomingData() {
    while (_serial->available()) {
        char c = _serial->read();
        _receiveBuffer += c;
        
        // 检查连接状态变化
        if (_receiveBuffer.endsWith("OK+CONN")) {
            _currentStatus = STATUS_CONNECTED;
            if (_connectionCallback) {
                _connectionCallback(true);
            }
            _receiveBuffer = "";
        }
        else if (_receiveBuffer.endsWith("OK+LOST")) {
            _currentStatus = STATUS_DISCONNECTED;
            if (_connectionCallback) {
                _connectionCallback(false);
            }
            _receiveBuffer = "";
        }
        // 检查数据结束
        else if (c == '\n') {
            if (_dataCallback && _receiveBuffer.length() > 0) {
                _dataCallback(_receiveBuffer);
            }
            _receiveBuffer = "";
        }
    }
}

// 解析响应
void BT04A::_parseResponse(const String& response) {
    // 在这里添加特定的响应解析逻辑
}

// 读取串口数据
String BT04A::_readSerial(unsigned long timeout) {
    String response = "";
    unsigned long startTime = millis();
    
    while (millis() - startTime < timeout) {
        while (_serial->available()) {
            char c = _serial->read();
            response += c;
        }
        
        if (response.length() > 0 && 
            (response.endsWith("\r\nOK\r\n") || 
             response.endsWith("\r\nERROR\r\n") ||
             response.endsWith("\r\nFAIL\r\n"))) {
            break;
        }
        
        delay(10);
    }
    
    return response;
}

// 发送原始数据
bool BT04A::_sendRaw(const String& data) {
    _serial->print(data);
    return true;
}

// 等待特定响应
bool BT04A::_waitForResponse(const String& expected, unsigned long timeout) {
    unsigned long startTime = millis();
    String response = "";
    
    while (millis() - startTime < timeout) {
        while (_serial->available()) {
            char c = _serial->read();
            response += c;
            
            if (response.indexOf(expected) != -1) {
                return true;
            }
        }
        delay(10);
    }
    
    return false;
}

// 设置使能引脚
void BT04A::_setEnablePin(bool state) {
    if (_enablePin != 255) {
        digitalWrite(_enablePin, state ? HIGH : LOW);
    }
}