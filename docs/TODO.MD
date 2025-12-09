
# [INST]

## 设计概览

* ISA 表 / 元数据（opcode/funct/funct3/… + 指令格式定义）
* Decoder（binary → structured representation → assembly）
* Encoder（assembly → structured representation → binary）
* 格式／语法解析器（Parser for assembly, tokenizer）
* 输出／打印／格式化 — 把结构化 representation 变成可读的 assembly 或者 hex/bin
* 测试 / 验证 / 回归测试框架 “自定义扩展 / 插件” 支持，以便日后扩展新的 opcode/extension。
* 参考 [rvcodec]
---

### 0. 项目初始化／基础设施

* [ ] 定义代码结构，大致目录
    + `isa/` — ISA 描述（opcode tables, definitions）
    + `core/` — decoder/encoder 引擎逻辑
    + `parser/` — assembly 文本解析、tokenizing
    + `util/` — 通用工具 （bitfield ops, error handling, string formatting）
    + `tests/` — 单元测试与回归测试
---

### 1. ISA 描述 / 元数据模块

* [ ] 定义一种数据结构，用于描述一种指令格式（R-type, I-type, S-type, B-type, U-type, J-type, 还有 compressed C-type 如果将来支持）
* [ ] 定义一种数据结构，用于描述每条指令的 opcode/funct3/funct7/funct2/etc，以及它所属的格式、操作数类型（rd, rs1, rs2, imm, …）
* [ ] 准备一个“指令表”（instruction table / opcode database）：把 base ISA（例如 RV32I）里所有 instruction 的元数据写入。
* [ ] （可选）支持扩展 ISA：定义机制容易添加新的 instruction（例如 M, A, F, D, C, custom）

---

### 2. Binary → Decode → Internal Representation（Decoder）模块

* [ ] 定义一个 Internal Representation（IR）结构 — 包含： mnemonic（名称）、operand fields（rd, rs1, rs2, imm, …）、ISA variant／extension、原始 binary / bit-fields 信息
* [ ] 实现 bit-field 提取工具（mask + shift + sign-extend immediate）
* [ ] 根据 instruction table，实现 decode flow：从原始 32-bit（或 16-bit for compressed） binary → 确定 opcode → 匹配对应 entry → 提取 fields → 填充 IR
* [ ] 对于无法 decode 的 binary，返回 “invalid / unknown / unsupported” 状态

---

### 3. Assembly Parser + Encoder 模块

* [ ] 实现一个简单 assembly parser（tokenizer + parser）：识别 mnemonic、寄存器名（x0, x1... 或 ABI 名称）、立即数（decimal / hex / signed / unsigned）、逗号／括号等分隔符
* [ ] 将 parsed assembly 转换为同样的 Internal Representation（IR）结构 — 验证 operand 数量、operand 类型是否与 instruction definition 匹配
* [ ] 从 IR 生成 binary — 对 fields 进行 encode：rd／rs1／rs2 → bit-positions, immediate → proper sign-extend + encoding, funct3/funct7/opcode bits → 写入。
* [ ] 对于不合法／不支持的 assembly，报错

---

### 4. 输出 / 格式化 / 用户接口

* [ ] 为 IR → assembly（string）的 “pretty-print” 函数，以标准 RISC-V 汇编格式输出
* [ ] 为 binary → hex / binary 字符串的输出函数（例如 `0x...`, `0b...`）
* [ ] 提供命令行接口（CLI） — 支持：给定 hex/bin → 输出 assembly；给定 assembly → 输出 hex/bin；批量处理文件／stdin／stdout
* [ ] （可选）提供库接口（API）：使得 decoder/encoder 能被别的程序调用

---

### 5. 测试 / 验证 / 回归

* [ ] 编写单元测试：对每种 format（R/I/S/B/U/J）都至少一个 instruction，测试 decode → encode → decode round-trip 是否一致
* [ ] 测试边缘情况：immediate 最大／最小值、负立即数、寄存器边界（x0, x31）、非法 binary／非法 assembly／unsupported instruction → 是否正确报错／处理
* [ ] （可选）使用已知的 RISC-V 测试用例／指令集测试集（如果有的话）做系统测试

---

### 6. 扩展功能（可选 / 后期）

* [ ] 支持压缩指令集（C-extension，16-bit instructions）
* [ ] 支持更多 ISA 扩展（M, A, F, D, custom） — 需要扩充 instruction table / metadata
* [ ] 支持 batch-mode / file-mode：读入一个二进制流／ELF／hex dump，输出 disassembly listing
* [ ] 支持带注释的输出／格式化（例如 immediate 的十进制 + hex，寄存器用 ABI 名称 + x-name 双写）
* [ ] 支持自定义／插件机制 —— 用户自己添加新 opcode / custom extension 时，只需给表 + metadata，无需改 core decode/encode 逻辑
* [ ] 性能优化 —— 若需要高速 decode（比如作为仿真／JIT 前端）

---


### 关键设计

* RISC-V 指令 encoding 非常多样（不同格式, immediate 各种拼接／sign-extend, compressed instructions 16-bit + 32-bit 混合） — 初期建议只支持 RV32I 32-bit 基本指令，以降低复杂性。
* assembly parser 的鲁棒性 — 注册名字 (x0/x1… / ABI 名称) + 空格 / 分隔符 /立即数格式多样，需要做好 error handling。
* immediate 的 sign-extend／bit-field抽取／拼装 — 容易出错，建议封装为 utility 函数。
* 从元数据 (instruction table) 到 code 的 mapping 要设计得灵活，使扩展容易。

---

# [RVOS]

## 1.Boot（启动与基本运行环境）
---
### 1.1 启动与链接（）

* [ ] 链接脚本 text/data/bss/stack/heap
* [ ] 启动符号
* [ ] 数据段拷贝与 bss 清零

### 1.2 启动代码（汇编）

* [ ] `_start`：关中断、初始化 sp
* [ ] 设置 mtvec 指向 trap entry
* [ ] 切换到 C：调用 `kernel_boot()`
* [ ] 初始化 UART 供早期打印使用

### 1.3 Boot 阶段自检

* [ ] 栈越界检测（可选）
* [ ] 简单内核日志系统初始化

---

# 2. 内存管理（Memory Management）

---

最小 RTOS 内存模块，一般分静态区和动态区：

## 2.1 静态内存

* [ ] 管理全局 TCB / 内核对象的静态内存池
* [ ] 内核栈分配器（每任务一个独立栈）

### 2.2 动态内存（heap）

* [ ] 最小 malloc/free（如 first-fit）
* [ ] 内核级分配器：`kalloc()` / `kfree()`
* [ ] TCB 动态创建支持
* [ ] heap 区越界检测（magic、guard word）

### 2.3 高级（可选）

* [ ] TLSF（O(1) 分配器）
* [ ] 多区域 heap
* [ ] 内存统计：剩余/已用、碎片率

---

## 3. 上下文（Context）切换

---

这是内核核心能力：

### 3.1 Task Context 定义

* [ ] 保存寄存器：ra、sp、gp、tp、t0-t6、s0-s11、a0-a7
* [ ] 定义 context_t 结构体
* [ ] 任务启动初始上下文构造器（stack frame）

### 3.2 上下文切换实现

* [ ] 编写 `context_switch(old_sp, new_sp)` 汇编
* [ ] 保存/恢复所有 callee-saved（ABI 保证）
* [ ] 切换栈指针
* [ ] 从新任务的 epc 继续执行

### 3.3 Soft-trigger 机制

* [ ] 添加 `pend_schedule()` 函数
* [ ] 在 trap 中触发上下文切换（软件中断）

---

## 4. 多任务（Task Management）

---

### 4.1 TCB（Task Control Block）

* [ ] sp、priority、state、tick、链表节点
* [ ] Task entry + 参数处理

### 4.2 任务创建与管理 API

* [ ] `task_create()`
* [ ] `task_delete()`
* [ ] `task_yield()`
* [ ] `task_sleep(ticks)`
* [ ] `task_suspend()/resume()`

### 4.3 Ready/Delay 队列

* [ ] 多优先级就绪队列（bitmap）
* [ ] 延时队列（按 tick 递减）

---

## 5. Trap（异常处理）

---

### 5.1 Trap 入口（汇编）

* [ ] 保存全寄存器
* [ ] 切换到内核栈（如使用）
* [ ] 跳转 C 级 trap handler

### 5.2 Trap 分发

* [ ] 异常类型判断（instruction fault, load fault, ecall）
* [ ] 中断类型判断（timer, software, external）
* [ ] trap→scheduler 通路（timer software）

### 5.3 异常处理

* [ ] 非法指令 panic
* [ ] load/store fault report
* [ ] ecall 实现系统调用入口

---

## 6. 中断（Interrupt）

---

### 6.1 M 模式中断控制

* [ ] mie/mstatus API
* [ ] 全局中断开关
* [ ] 中断优先级（RISC-V M 模式简单即可）

### 6.2 IRQ 框架

* [ ] irq_register(irqno, handler)
* [ ] irq_enable / irq_disable
* [ ] 默认处理程序

### 6.3 外设中断支持

* [ ] UART RX/TX（可选）
* [ ] PLIC 支持（如 QEMU 或 SoC）

---

## 7. 定时器（Timer）

---

### 7.1 CLINT / MTIME

* [ ] mtime 读
* [ ] mtimecmp 写
* [ ] 设定系统 tick（例如 1ms）

### 7.2 Tick 模块

* [ ] tick++
* [ ] 唤醒延时任务
* [ ] 通知调度器（pend_schedule）

### 7.3 高级

* [ ] 支持 tickless（可选）

---

## 8. 抢占式多任务（Preemptive Scheduling）

---

### 8.1 Tick 驱动调度

* [ ] tick 中断触发调度
* [ ] 当前任务时间片耗尽 → yield
* [ ] 最高优先级任务立即运行

### 8.2 优先级调度器

* [ ] bitmap 查找最高优先级任务
* [ ] O(1) 调度
* [ ] 多队列结构

### 8.3 中断嵌套与调度

* [ ] 中断退出时检查是否需要调度
* [ ] 实现 nested interrupt depth

---

## 9. 任务同步与锁（Synchronization）

---

### 9.1 Mutex

* [ ] 基本互斥
* [ ] 优先级继承（处理优先级反转）

### 9.2 Semaphore

* [ ] 二值信号量
* [ ] 计数信号量

### 9.3 Event Group / Flags

* [ ] 等待事件集合
* [ ] OR/AND 等等待模式

### 9.4 IPC

* [ ] 消息队列
* [ ] Mailbox
* [ ] RingBuffer（无锁/临界区版）

---

## 10. 软件定时器（Soft Timer）

---

### 10.1 Timer Wheel 结构

* [ ] 软件定时器链表
* [ ] 基于 tick 驱动

### 10.2 API

* [ ] swtimer_create()
* [ ] start/stop/reset
* [ ] 回调函数机制

---

## 11. 系统调用（Syscall）

---

### 11.1 ecall 框架

* [ ] 用户态可选（若实现）
* [ ] M-mode 提供服务

### 11.2 常用系统调用

* [ ] sys_yield
* [ ] sys_sleep
* [ ] sys_get_tick
* [ ] sys_mutex / sys_sem 等（内核抽象）

### 11.3 系统调用号表

* [ ] syscall vector
* [ ] 参数传递 ABI（a0-a7）

---

## 12. 其他扩展模块（可选）

---

* 软件 watchdog
* 设备驱动层 HAL
* Shell 命令行
* 动态任务加载
* VFS 文件系统
* SMP 支持（多核 RISC-V）

---

1. Boot
2. 内存管理
3. 上下文切换
4. 多任务
5. Trap
6. 中断
7. 定时器
8. 抢占式调度
9. 同步（锁/信号量/队列）
10. 软件定时器
11. 系统调用
12. 扩展模块（根据项目需求）

---

[rvcodec]: https://luplab.gitlab.io/rvcodecjs/

---
