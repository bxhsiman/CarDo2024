# CarDo
## 项目简介
基于stm32f103RCxT6的小车控制系统，实现小车的蓝牙通信、自动寻迹等功能。

### 任务说明
类似2024年电赛H题，在半圆形轨迹上实现以下任务:
1. 巡线一圈
2. 去除椭圆形长边 巡线一圈
3. 去除椭圆形长边 绕行8字

## 项目结构
``` shell
CarDo
|--readme.md # 项目说明
|--Core # 核心代码
|--Drivers # 驱动代码
|--MiddleWares # 中间件
|--USB_DEVICE # USB设备
|--CmakeLists.txt  # cmake配置文件
|--car_2023.ioc # CubeMX配置文件
```