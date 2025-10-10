# ulog 日志系统使用说明

## 简介

ulog 是一个轻量级的嵌入式日志系统，具有以下特点：
- **非阻塞**: 日志写入到缓冲区，异步发送
- **分级管理**: 支持 TRACE, DEBUG, INFO, WARN, ERROR, FATAL 六个级别
- **循环缓冲**: 4KB 缓冲区，最多 32 条日志
- **安全**: 可以在中断中使用

## 日志级别

| 级别 | 宏 | 用途 |
|------|---|------|
| TRACE | `utrace()` | 最详细的跟踪信息 |
| DEBUG | `udebug()` | 调试信息 |
| INFO  | `uinfo()`  | 一般信息（**推荐**） |
| WARN  | `uwarn()`  | 警告信息 |
| ERROR | `uerror()` | 错误信息 |
| FATAL | `ufatal()` | 致命错误 |

## 使用示例

### 1. 基础用法

```c
#include "ulog_buffer.h"

// 普通日志
uinfo("System started\n");
uinfo("Temperature: %d degrees\n", temp);

// 警告日志
uwarn("Memory usage: %d%%\n", mem_usage);

// 错误日志
uerror("Connection failed, code=%d\n", err_code);
```

### 2. 在中断中使用

```c
__INTERRUPT
void GPIOB_IRQHandler(void)
{
    // ✅ 安全：使用 ulog（非阻塞）
    udebug("Button interrupt triggered\n");
    
    // ❌ 危险：不要使用 PRINT（会阻塞）
    // PRINT("Button pressed\n");  // 不要这样做！
}
```

### 3. 调试开关

修改 `Driver/ulog/inc/ulog_config.h`:

```c
// 显示所有日志（开发阶段）
#define ULOG_DEFAULT_LEVEL ULOG_LEVEL_TRACE

// 只显示重要日志（生产环境）
#define ULOG_DEFAULT_LEVEL ULOG_LEVEL_INFO

// 关闭所有日志（发布版本）
#define ULOG_DEFAULT_LEVEL ULOG_LEVEL_NONE
```

## 输出格式

```
[timestamp][级别] 消息内容
```

示例：
```
[12345][I] System started
[12560][D] Button state changed: 1
[12890][W] Battery low: 15%
[13120][E] Connection timeout
```

级别标识：
- `T` = TRACE
- `D` = DEBUG
- `I` = INFO
- `W` = WARN
- `E` = ERROR
- `F` = FATAL

## 性能说明

- **写入速度**: 微秒级（仅写缓冲区）
- **发送周期**: 每 10ms 发送最多 20 条日志
- **缓冲容量**: 32 条日志（4KB）
- **溢出处理**: 缓冲区满时丢弃新日志

## 注意事项

### ✅ 推荐做法

1. **启动阶段**: 使用 `PRINT()` 确保能看到关键信息
2. **运行阶段**: 使用 `uinfo()` / `udebug()` / `uerror()` 等
3. **中断中**: **只能**使用 `ulog`，不能用 `PRINT`
4. **调试时**: 可临时提升日志级别到 `ULOG_LEVEL_TRACE`
5. **发布时**: 设置为 `ULOG_LEVEL_INFO` 或更高

### ❌ 避免的做法

1. ❌ 在中断中使用 `PRINT()`（会阻塞）
2. ❌ 在时间敏感代码中使用 `PRINT()`
3. ❌ 频繁输出大量日志（超过缓冲区容量）
4. ❌ 日志消息超过 128 字节

## 工具函数

### 打印数组（16进制）

```c
uint8_t data[] = {0x12, 0x34, 0x56, 0x78};
ulog_array_to_hex("MAC", data, 4);
// 输出: [xxxxx][I] MAC:12 34 56 78
```

### 打印数组（10进制）

```c
uint8_t temps[] = {25, 26, 24, 23};
ulog_array_to_dec("Temps", temps, 4);
// 输出: [xxxxx][I] Temps:25 26 24 23
```

## 与 PRINT 的对比

| 特性 | PRINT | ulog |
|------|-------|------|
| 阻塞性 | ✅ 阻塞（慢） | ❌ 非阻塞（快） |
| 中断安全 | ❌ 不安全 | ✅ 安全 |
| 分级管理 | ❌ 无 | ✅ 有 |
| 缓冲机制 | ❌ 无 | ✅ 有 |
| 配置开关 | ❌ 无 | ✅ 有 |
| 适用场景 | 启动阶段 | 运行时 |

## 故障排除

### 问题1: 看不到日志输出

**原因**: 日志级别过滤
**解决**: 检查 `ulog_config.h` 中的 `ULOG_DEFAULT_LEVEL`

### 问题2: 日志输出不完整

**原因**: 缓冲区满
**解决**: 
1. 减少日志输出频率
2. 增加 `ULOG_BUFFER_SIZE`（注意 RAM 限制）

### 问题3: 日志有延迟

**原因**: 这是正常的，因为是异步输出
**特点**: 延迟最多 10ms，不影响实时性

## 配置参数

在 `ulog_buffer.h` 中可修改：

```c
#define ULOG_BUFFER_SIZE 4096       // 缓冲区大小（字节）
#define ULOG_MAX_ENTRY_SIZE 128     // 单条日志最大长度
#define ULOG_MAX_ENTRIES 32         // 最大日志条数
#define PRINT_LOG_PERIOD 16         // 处理周期（10ms）
```

---

*更新时间: 2025-10-10*

