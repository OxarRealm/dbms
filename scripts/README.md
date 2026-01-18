# 测试脚本目录

本目录包含项目的所有测试脚本，用于进行代码测试、验证和性能分析。

## 📁 目录结构

```
scripts/
├── unit_tests/           # 单元测试脚本
├── module_tests/         # 模块测试脚本
├── integration_tests/    # 集成测试脚本
├── performance_tests/    # 性能测试脚本
├── boundary_tests/       # 边界测试脚本
├── utils/                # 测试工具和辅助脚本
└── README.md             # 本文档
```

## 📝 测试类型说明

### 单元测试 (unit_tests/)
- **目的**：测试单个函数或类的功能
- **范围**：最小的代码单元
- **示例**：TableMode结构体操作、文件I/O函数、字符串处理函数

### 模块测试 (module_tests/)
- **目的**：测试整个模块的功能
- **范围**：Core、DDL、DML、Query、SQL Parser、AI、GUI等模块
- **示例**：TableManager模块测试、DDLExecutor模块测试

### 集成测试 (integration_tests/)
- **目的**：测试多个模块之间的协作
- **范围**：跨模块的功能流程
- **示例**：SQL解析+DDL执行、SQL解析+DML执行、完整SQL流程

### 性能测试 (performance_tests/)
- **目的**：测试系统性能
- **范围**：文件I/O性能、查询性能、推荐算法性能
- **指标**：执行时间、内存使用、文件大小

### 边界测试 (boundary_tests/)
- **目的**：测试边界条件和异常情况
- **范围**：空数据、大数据量、错误输入、边界值
- **示例**：空表、最大字段数、最大记录数、无效SQL语句

## 🚀 使用说明

### 运行所有测试
```bash
# Windows (PowerShell)
.\scripts\run_all_tests.ps1

# Linux/Mac
./scripts/run_all_tests.sh
```

### 运行特定测试
```bash
# 运行单元测试
.\scripts\unit_tests\run_unit_tests.ps1

# 运行模块测试
.\scripts\module_tests\run_module_tests.ps1
```

## 📋 测试脚本命名规范

- 测试脚本：`test_<模块名>_<功能>.ps1` 或 `.sh`
- 运行脚本：`run_<测试类型>_tests.ps1` 或 `.sh`
- 辅助工具：`<工具名>.ps1` 或 `.sh`

## 📊 测试报告

测试结果将保存在 `test_results/` 目录中，包括：
- 测试日志
- 测试报告（HTML/JSON格式）
- 性能分析报告

## 🔧 测试环境要求

- C++编译器（MSVC/GCC/Clang）
- CMake 3.10+
- Qt（用于GUI测试）
- Python 3.x（可选，用于测试脚本）

---

**最后更新**：2025-01-12
