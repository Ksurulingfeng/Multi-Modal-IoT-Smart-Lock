## 项目介绍

Multi-Modal-IoT-Smart-Lock 是一个基于 STM32F103C8T6 微控制器和 FreeRTOS 实时操作系统的多模态物联网智能门锁系统。
该系统支持指纹、RFID和密码三种认证方式，并集成了MQTT远程控制功能，实现了本地与云端协同管理的安全门锁解决方案。

## 功能特性

- 指纹、RFID和密码三种认证方式
- MQTT远程控制功能
- 本地与云端协同管理
- 基于FreeRTOS的实时操作系统

## 系统架构

- 主控制器：STM32F103C8T6
- 认证模块：AS608光学指纹模块、MFRC522射频模块、3x4矩阵键盘
- 通信模块：ESP8266 WiFi模块
- 存储模块：AT24C04 256字节I2C EEPROM

## 配置文件说明

1. 将 `config.template.h` 重命名为 `config.h`
2. 在 `config.h` 中填入你的实际配置信息
