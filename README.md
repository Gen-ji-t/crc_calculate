# CRC 计算工具

CRC 校验和计算工具，支持多种 CRC 算法，提供命令行和 Windows GUI 两种界面。

### 前置要求

- GCC 或 MinGW-w64 (用于 Windows 交叉编译)
- Make

### 编译命令

```bash
# 编译 Windows 命令行版本 (crc.exe)
make

# 编译 Windows GUI 版本 (crc_gui.exe)
make gui

# 清理构建产物
make clean
```

### 使用方法

```bash
# 使用默认算法 (CRC-32)
./crc.exe <文件路径>

# 指定算法
./crc.exe -8  <文件路径>   # CRC-8
./crc.exe -16 <文件路径>   # CRC-16
./crc.exe -32 <文件路径>   # CRC-32
./crc.exe -64 <文件路径>   # CRC-64
```

### GUI 版本

1. 双击运行 `crc_gui.exe`
2. 选择 CRC 算法（单选按钮）
3. 点击"选择文件"按钮选择要计算的文件
4. 点击"计算"按钮执行计算
5. 结果显示在文本框中
