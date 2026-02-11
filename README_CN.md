# BT04A蓝牙模块库

用于BT04A蓝牙模块（基于CC2541芯片）的Arduino C++和Python库。

## 功能特性

### Arduino库特性
- 支持硬件串口和软件串口
- AT指令配置
- BLE数据传输
- 多种工作模式切换
- 事件回调机制
- 自动波特率检测
- 连接状态管理

### Python库特性
- 跨平台支持（Windows/Linux/macOS）
- 自动端口检测
- 多线程数据接收
- 异步事件处理
- 完整的AT指令集支持
- 上下文管理器支持

## 安装

### Arduino库安装
1. 将`BT04A`文件夹复制到Arduino的`libraries`目录
2. 重启Arduino IDE
3. 打开示例：文件 → 示例 → BT04A → BasicExample

### Python库安装
```bash
# 从源码安装
pip install .

# 或直接使用
pip install bt04a

```

## 兼容性

| 开发板 | 正常运行 | 运行失败 | 未测试 | 备注 |
| ------------ | :--: | :----: | :----: | :--: |
| Arduino UNO | √ | | | |
| Arduino Mega | √ | | | |
| Arduino Leonardo | √ | | | |
| ESP8266 | √ | | | 大内存支持 |
| micro:bit | √ | | | 大内存支持 |

## 历史版本
2026.2.9 V1.0发布

## 作者
Written by aybau,2026.