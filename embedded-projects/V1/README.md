
# ESP8266 + STM32 温湿度监控系统

**README — 硬件说明与软件说明**

---

## 一、项目概述

本系统基于 STM32F10x 微控制器与 ESP8266 WiFi 模块构建，实现温湿度数据的采集、本地显示与远程 Web 监控。系统由两部分组成：

- **STM32F10x 端**：负责 DHT11 温湿度传感器数据采集、OLED 本地显示、UART 数据发送
- **ESP8266 端**：负责 UART 数据接收、HTTP Web 服务器搭建、温湿度数据远程展示

**系统工作流程：** DHT11 传感器 → STM32F10x 采集 → OLED 显示 + UART 发送 → ESP8266 接收解析 → HTTP Web 页面展示。

---

## 二、硬件部分

### 2.1 系统硬件架构

系统采用双 MCU 架构，STM32F10x 作为主控制器负责传感器数据采集与本地显示，ESP8266 作为 WiFi 协处理器负责网络通信与 Web 服务。两块开发板之间通过 UART 串口通信。

### 2.2 核心元器件清单

| 序号 | 元器件 | 型号/规格 | 数量 | 功能说明 |
|:---:|--------|-----------|:---:|----------|
| 1 | 主控板 | STM32F10x 开发板 | 1 | 温湿度数据采集、OLED 显示、UART 发送 |
| 2 | WiFi 模块 | ESP8266 开发板 | 1 | UART 数据接收、HTTP Web 服务器 |
| 3 | 温湿度传感器 | DHT11 | 1 | 环境温湿度采集（单总线协议） |
| 4 | OLED 显示屏 | 0.96寸 I2C OLED | 1 | 本地温湿度数据显示 |
| 5 | 电阻 | 4.7kΩ 上拉电阻 | 1 | DHT11 数据线上拉 |
| 6 | 杜邦线 | 母对母/母对公 | 若干 | 模块间接线 |
| 7 | USB 数据线 | Micro-USB / USB-A | 2 | STM32 与 ESP8266 分别供电与烧录 |

### 2.3 引脚配置与接线表

| 信号名称 | STM32F10x 引脚 | ESP8266 引脚 | 模块 | 说明 |
|----------|---------------|---------------|------|------|
| DHT11_DATA | PA0 | — | DHT11 | DHT11 数据信号线（单总线），需 4.7kΩ 上拉至 VCC |
| DHT11_VCC | 3.3V | — | DHT11 | DHT11 电源正极（3.3V~5.5V） |
| DHT11_GND | GND | — | DHT11 | DHT11 电源地 |
| OLED_SCL | PB6 | — | OLED | I2C 时钟线（SCL） |
| OLED_SDA | PB7 | — | OLED | I2C 数据线（SDA） |
| OLED_VCC | 3.3V | — | OLED | OLED 电源正极 |
| OLED_GND | GND | — | OLED | OLED 电源地 |
| UART_TX | PA9 (USART1_TX) | GPIO3 (RX) | UART | STM32 发送 → ESP8266 接收（交叉连接） |
| UART_RX | PA10 (USART1_RX) | GPIO2 (TX) | UART | ESP8266 发送 → STM32 接收（交叉连接） |
| UART_GND | GND | GND | UART | 共地连接，确保信号参考一致 |
| ESP8266_VCC | — | 3.3V | ESP8266 | ESP8266 电源正极（需 3.3V 稳压供电） |
| ESP8266_GND | — | GND | ESP8266 | ESP8266 电源地 |
| ESP8266_CH_PD | — | 3.3V | ESP8266 | 使能引脚，需接 3.3V 使能 WiFi 模块 |

### 2.4 模块接线说明

#### 2.4.1 DHT11 温湿度传感器接线

DHT11 采用单总线通信协议，共 4 个引脚：

- **VCC**：接 STM32 开发板 3.3V 电源（DHT11 工作电压 3.3V~5.5V）
- **GND**：接 STM32 开发板 GND
- **DATA**：接 STM32 GPIO 引脚 PA0，需外接 4.7kΩ 上拉电阻连接至 VCC
- **NC（空脚）**：不连接

> **注意事项：** DHT11 数据线上拉电阻不可省略，否则无法正常通信。上拉电阻取值范围 2.2kΩ~10kΩ，推荐 4.7kΩ。

#### 2.4.2 OLED 显示屏接线

OLED 采用 I2C 通信协议，共 4 个引脚：

- **VCC**：接 STM32 开发板 3.3V 电源
- **GND**：接 STM32 开发板 GND
- **SCL（时钟线）**：接 STM32 PB6 引脚（I2C1_SCL）
- **SDA（数据线）**：接 STM32 PB7 引脚（I2C1_SDA）

> **注意事项：** I2C 总线的 SCL 和 SDA 引脚均需外接 4.7kΩ 上拉电阻至 3.3V（部分 OLED 模块已集成上拉电阻，可省略）。

#### 2.4.3 STM32 与 ESP8266 UART 串口接线

STM32 与 ESP8266 通过 UART 串口进行异步串行通信，接线采用交叉方式：

- **STM32 PA9 (USART1_TX)** → **ESP8266 GPIO3 (RX)**：STM32 发送端连接 ESP8266 接收端
- **STM32 PA10 (USART1_RX)** → **ESP8266 GPIO2 (TX)**：ESP8266 发送端连接 STM32 接收端
- **STM32 GND** → **ESP8266 GND**：共地连接，确保双方信号参考电平一致

通信参数：波特率 9600bps，8 数据位，1 停止位，无校验（8N1）。

#### 2.4.4 ESP8266 模块接线

ESP8266 开发板接线说明：

- **VCC**：接 3.3V 稳压电源（注意：ESP8266 不可直接使用 5V 供电）
- **GND**：接电源地
- **CH_PD（使能引脚）**：接 3.3V，使能 WiFi 模块工作
- **GPIO2 (TX)**：连接 STM32 PA10 (RX)
- **GPIO3 (RX)**：连接 STM32 PA9 (TX)

> **注意事项：** ESP8266 工作电流峰值可达 350mA，建议使用独立 3.3V 稳压电源供电，避免通过 STM32 开发板的 3.3V 引脚供电导致电压跌落。

### 2.5 原理图描述

以下为系统原理图的详细文字描述，可根据此描述绘制电路原理图：

#### 2.5.1 STM32F10x 最小系统

STM32F10x 最小系统包含以下核心电路：

- **电源部分**：VDD 接 3.3V，VSS 接 GND；VDD 与 VSS 之间各放置一个 100nF 去耦电容，靠近芯片引脚
- **复位电路**：NRST 引脚通过 10kΩ 上拉电阻接 VDD，同时通过 100nF 电容接 GND，构成 RC 复位电路
- **晶振电路**：OSC32_IN 和 OSC32_OUT 之间接 32.768kHz 低速晶振，用于实时时钟；OSC_IN 和 OSC_OUT 之间接 8MHz 高速晶振，用于系统主时钟
- **BOOT 引脚**：通过 10kΩ 下拉电阻接 GND，确保从 Flash 启动

#### 2.5.2 DHT11 传感器连接电路

DHT11 传感器连接至 STM32 的 PA0 引脚：

- DHT11 的 VCC 引脚接 3.3V 电源
- DHT11 的 GND 引脚接系统 GND
- DHT11 的 DATA 引脚接 STM32 PA0，同时通过 4.7kΩ 上拉电阻接 3.3V
- DHT11 的 NC 引脚悬空不接

数据通信时序：DHT11 通过单总线协议传输 40 位数据（湿度整数 8bit + 湿度小数 8bit + 温度整数 8bit + 温度小数 8bit + 校验和 8bit）。

#### 2.5.3 OLED 显示屏连接电路

0.96寸 I2C OLED 显示屏连接至 STM32 的 I2C1 接口：

- OLED 的 VCC 引脚接 3.3V 电源
- OLED 的 GND 引脚接系统 GND
- OLED 的 SCL 引脚接 STM32 PB6（I2C1_SCL），通过 4.7kΩ 上拉电阻接 3.3V
- OLED 的 SDA 引脚接 STM32 PB7（I2C1_SDA），通过 4.7kΩ 上拉电阻接 3.3V

I2C 地址：默认 0x78（7 位地址）或 0x3C（7 位地址），具体取决于 OLED 驱动芯片（SSD1306）。

#### 2.5.4 UART 串口通信电路

STM32 与 ESP8266 之间的 UART 串口通信电路：

- STM32 的 PA9 引脚配置为 USART1_TX（发送端），连接至 ESP8266 的 GPIO3 (RX) 引脚
- STM32 的 PA10 引脚配置为 USART1_RX（接收端），连接至 ESP8266 的 GPIO2 (TX) 引脚
- STM32 的 GND 引脚与 ESP8266 的 GND 引脚直连，构成共地参考
- 通信波特率：9600bps，数据位 8 位，停止位 1 位，无校验

数据传输格式：STM32 发送格式为 `"T:%d,H:%d\r\n"`（如 `"T:25,H:60\r\n"`），ESP8266 接收后解析温度与湿度数值。

#### 2.5.5 ESP8266 模块连接电路

ESP8266 WiFi 模块连接电路：

- **VCC 引脚**：接 3.3V 稳压电源（建议使用独立 LDO 稳压，输出电流 ≥ 500mA）
- **GND 引脚**：接系统 GND
- **CH_PD 引脚**：通过 10kΩ 上拉电阻接 3.3V，使能 WiFi 模块工作
- **GPIO0 引脚**：通过 10kΩ 下拉电阻接 GND，确保正常启动（非下载模式）
- **GPIO2 引脚（TX）**：连接 STM32 PA10 (RX)
- **GPIO3 引脚（RX）**：连接 STM32 PA9 (TX)

> **注意事项：** ESP8266 的 TX/RX 引脚为 3.3V 电平，与 STM32 的 3.3V 电平直接兼容，无需电平转换。

### 2.6 原理图连接关系汇总表

以下为系统中各模块之间的信号连接关系汇总：

| 信号/电源 | 来源 | 目标 | 连线类型 | 备注 |
|-----------|------|------|----------|------|
| DHT11_DATA | STM32 PA0 | DHT11 DATA | GPIO + 上拉 | 4.7kΩ 上拉至 VCC |
| DHT11_VCC | STM32 3.3V | DHT11 VCC | 电源线 | 3.3V~5.5V 供电 |
| DHT11_GND | STM32 GND | DHT11 GND | 地线 | 系统共地 |
| OLED_SCL | STM32 PB6 | OLED SCL | I2C 信号线 | 4.7kΩ 上拉至 VCC |
| OLED_SDA | STM32 PB7 | OLED SDA | I2C 信号线 | 4.7kΩ 上拉至 VCC |
| OLED_VCC | STM32 3.3V | OLED VCC | 电源线 | 3.3V 供电 |
| OLED_GND | STM32 GND | OLED GND | 地线 | 系统共地 |
| UART_TX | STM32 PA9 | ESP8266 GPIO3 (RX) | UART 信号线 | 交叉连接，TX→RX |
| UART_RX | STM32 PA10 | ESP8266 GPIO2 (TX) | UART 信号线 | 交叉连接，RX→TX |
| UART_GND | STM32 GND | ESP8266 GND | 地线 | 共地，必须连接 |
| ESP8266_VCC | 独立 3.3V 电源 | ESP8266 VCC | 电源线 | 独立稳压供电 |
| ESP8266_CH_PD | 3.3V | ESP8266 CH_PD | 使能信号 | 上拉使能 |

---

## 三、软件部分

### 3.1 软件架构

系统软件分为 STM32 端和 ESP8266 端两部分，分别独立编译烧录：

- **STM32 端（main.c）**：基于 STM32F10x 标准外设库，实现 DHT11 数据采集、OLED 显示、UART 数据发送
- **ESP8266 端（main.cpp）**：基于 Arduino 框架，实现 UART 数据接收解析、HTTP Web 服务器搭建、温湿度数据 Web 展示

### 3.2 STM32 端软件说明

#### 3.2.1 开发环境

- **IDE**：Keil MDK-ARM / STM32CubeIDE
- **芯片型号**：STM32F10x（具体型号如 STM32F103C8T6）
- **标准库**：STM32F10x_StdPeriph_Lib
- **依赖头文件**：stm32f10x.h、Delay.h、OLED.h、Serial.h、DHT11.h

#### 3.2.2 主要功能模块

| 模块 | 功能描述 | 关键函数 |
|------|----------|----------|
| OLED 显示 | 初始化 OLED 屏幕，显示温湿度数值 | OLED_Init()、OLED_ShowString()、OLED_ShowNum() |
| DHT11 采集 | 初始化 DHT11 传感器，读取温湿度数据 | DHT11_Init()、DHT11_Read() |
| 串口通信 | 初始化 UART，发送格式化温湿度数据 | Serial_Init()、Serial_Printf() |
| 主循环 | 定时 2 秒读取传感器，显示并发送数据 | main() while(1) 循环 |

#### 3.2.3 串口通信协议

STM32 通过 UART 发送的数据格式为：

```
T:温度值,H:湿度值\r\n
```

示例：`T:25,H:60\r\n`

其中温度值和湿度值为整数（DHT11 精度为整数），`\r\n` 为行尾结束符。

### 3.3 ESP8266 端软件说明

#### 3.3.1 开发环境

- **IDE**：Arduino IDE
- **芯片型号**：ESP8266（如 ESP-12F）
- **框架**：Arduino ESP8266 Core
- **依赖库**：Arduino.h、ESP8266WiFi.h、ESP8266WebServer.h

#### 3.3.2 主要功能模块

| 模块 | 功能描述 | 关键函数/变量 |
|------|----------|---------------|
| WiFi 连接 | 连接指定 WiFi 网络 | WiFi.begin(ssid, password)、WiFi.status() |
| HTTP 服务器 | 搭建 Web 服务器，监听 80 端口 | ESP8266WebServer server(80)、server.on()、server.begin() |
| 串口接收 | 接收 STM32 发送的温湿度数据 | Serial.begin(9600)、Serial.read() |
| 数据解析 | 解析 "T:XX,H:XX" 格式数据 | processLine()、strstr()、atof() |
| Web 页面 | 渲染 HTML 页面展示温湿度 | handleRoot()、server.send() |

#### 3.3.3 配置参数

| 参数 | 值 | 说明 |
|------|-----|------|
| WiFi SSID | 35 | WiFi 网络名称 |
| WiFi 密码 | sz88888zlf | WiFi 密码 |
| 串口波特率 | 9600 | UART 通信波特率 |
| Web 端口 | 80 | HTTP 服务器监听端口 |
| 串口接收缓冲区 | 64 字节 | rxBuffer 数组大小 |
| 请求命令 | GET_DATA\n | 预留发送命令（当前未使用） |

#### 3.3.4 Web 页面功能

ESP8266 的 Web 服务器提供以下功能：

- **首页（/）**：展示温湿度监控页面，包含温度卡片和湿度卡片
- **温度显示**：以大号蓝色字体显示当前温度值（°C），保留一位小数
- **湿度显示**：以大号蓝色字体显示当前湿度值（%），保留一位小数
- **状态指示**：显示数据接收状态（"数据接收成功" / "数据超出范围" / "数据格式错误" / "等待数据..."）

Web 页面采用响应式设计，适配手机和 PC 浏览器访问。

### 3.4 数据流说明

系统完整数据流如下：

| 步骤 | 方向 | 数据/动作 | 说明 |
|:---:|------|-----------|------|
| 1 | DHT11 → STM32 | 原始温湿度数据 | STM32 通过单总线协议读取 DHT11，获取 40bit 数据 |
| 2 | STM32 → OLED | 温度/湿度数值 | STM32 将读取的温度和湿度数值显示在 OLED 屏幕上 |
| 3 | STM32 → ESP8266 | "T:25,H:60\r\n" | STM32 通过 UART 以 9600bps 发送格式化数据 |
| 4 | ESP8266 接收 | 串口缓冲区数据 | ESP8266 通过 Serial.read() 逐字符接收数据 |
| 5 | ESP8266 解析 | 温度=25.0, 湿度=60.0 | ESP8266 解析 "T:" 和 ",H:" 标记，提取数值 |
| 6 | ESP8266 → 浏览器 | HTML 页面 | ESP8266 将温湿度数据嵌入 HTML 页面返回给客户端 |

### 3.5 编译与烧录

#### 3.5.1 STM32 端烧录

- 使用 Keil MDK-ARM 或 STM32CubeIDE 打开项目
- 确认 DHT11、OLED、Serial 等底层驱动库已正确配置引脚
- 编译通过后，通过 ST-Link / J-Link 烧录器将固件烧录至 STM32
- 烧录后 STM32 自动运行，OLED 显示温湿度，UART 持续发送数据

#### 3.5.2 ESP8266 端烧录

- 使用 Arduino IDE，选择 ESP8266 开发板型号（如 NodeMCU 1.0）
- 修改 WiFi 配置（ssid 和 password）为实际网络信息
- 设置串口监视器波特率为 9600，可查看调试信息
- 编译通过后，通过 USB 烧录至 ESP8266
- 烧录后查看串口监视器获取 ESP8266 的 IP 地址
- 在浏览器中访问 ESP8266 的 IP 地址即可看到温湿度监控页面

### 3.6 调试信息

两个端均通过串口输出调试信息，可通过串口监视器查看：

**STM32 端调试输出：**

- DHT11 Systeam Ready — 系统初始化完成
- DHT11 Read Error — DHT11 读取失败，检查接线

**ESP8266 端调试输出：**

- Serial Initialized — 串口初始化完成
- Connecting to WiFi... — WiFi 连接中
- WiFi connected successfully! IP Address: x.x.x.x — WiFi 连接成功，显示 IP
- HTTP server started — Web 服务器启动
- Parsed -> Temp :XX.X C, Hum :XX.X % — 数据解析成功
- Received data out of range — 数据超出有效范围
- Invalid data format received: ... — 数据格式错误
- Buffer overflow — 接收缓冲区溢出

### 3.7 故障排查

| 故障现象 | 可能原因 | 解决方法 |
|----------|----------|----------|
| OLED 不显示 | I2C 接线错误或上拉电阻缺失 | 检查 PB6/PB7 接线及上拉电阻 |
| DHT11 读取失败 | DATA 线上拉电阻缺失或接线松动 | 检查 4.7kΩ 上拉电阻及 DATA 引脚接线 |
| ESP8266 无法连接 WiFi | SSID 或密码配置错误 | 检查 main.cpp 中 ssid 和 password 配置 |
| Web 页面显示等待数据 | UART 接线错误或波特率不匹配 | 检查 TX/RX 交叉接线及 9600bps 波特率 |
| 数据超出范围 | DHT11 读取异常数据 | 检查 DHT11 传感器是否正常工作 |
| ESP8266 重启 | 供电不足 | 使用独立 3.3V 稳压电源供电 |

---

## 四、系统原理图总览

以下为系统原理图的文字描述，可根据此描述绘制完整的电路原理图：

### 4.1 整体连接关系

系统由两大板块组成：

- **左侧** — STM32F10x 传感器节点：包含 STM32 最小系统、DHT11 温湿度传感器、0.96寸 I2C OLED 显示屏
- **右侧** — ESP8266 WiFi 服务器：包含 ESP8266 模块及其外围电路
- **中间** — UART 串口通信链路：STM32 PA9/PA10 与 ESP8266 GPIO3/GPIO2 交叉连接，共地

### 4.2 信号流向

数据流向（从左到右）：

- DHT11 传感器 →（单总线）→ STM32 PA0：温湿度原始数据
- STM32 →（I2C）→ OLED PB6/PB7：显示数据
- STM32 PA9 (TX) →（UART）→ ESP8266 GPIO3 (RX)：格式化温湿度数据 "T:XX,H:XX"
- ESP8266 →（HTTP）→ 浏览器：Web 页面

### 4.3 电源分配

系统电源分配方案：

- **STM32 开发板**：通过 USB 5V 供电，板载 LDO 降压至 3.3V 供 MCU 和外围模块使用
- **DHT11 传感器**：由 STM32 开发板 3.3V 供电
- **OLED 显示屏**：由 STM32 开发板 3.3V 供电
- **ESP8266 模块**：建议使用独立 3.3V 稳压电源供电（输出电流 ≥ 500mA），避免与 STM32 共用电源导致电压跌落
- **共地**：STM32 与 ESP8266 的 GND 必须连接，确保信号参考电平一致
