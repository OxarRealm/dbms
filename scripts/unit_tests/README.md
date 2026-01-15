# 单元测试脚本目录

## 目录结构

本目录包含所有单元测试脚本，按模块分类组织：

```
unit_tests/
├── core/              # 核心模块测试
│   ├── test_table_mode.cpp
│   ├── test_table_manager.cpp
│   ├── test_data_manager.cpp
│   └── test_file_manager.cpp
├── sql_parser/        # SQL解析器测试
│   ├── test_lexer.cpp
│   ├── test_parser.cpp
│   └── test_sql_parser.cpp  # SQL解析器手动测试程序
├── ddl/               # DDL功能测试
│   ├── test_create_table_handler.cpp
│   ├── test_edit_table_handler.cpp
│   ├── test_rename_table_handler.cpp
│   ├── test_drop_table_handler.cpp
│   └── test_ddl_executor.cpp
├── dml/               # DML功能测试
│   ├── test_insert_handler.cpp
│   ├── test_delete_handler.cpp
│   ├── test_update_handler.cpp
│   └── test_dml_executor.cpp
├── query/             # 查询功能测试
│   ├── test_select_handler.cpp
│   └── test_query_executor.cpp
├── index/             # 索引技术测试
│   ├── test_adjacent_index.cpp
│   ├── run_test_adjacent_index.ps1
│   ├── test_hash_index.cpp
│   ├── run_test_hash_index.ps1
│   ├── test_index_advisor.cpp
│   └── run_test_index_advisor.ps1
└── run_unit_tests.ps1 # 运行所有测试的主脚本
```

## 运行测试

### 运行单个模块的测试

进入对应模块目录，运行测试脚本：

```powershell
# 例如：运行核心模块测试
cd core
.\run_test_table_mode.ps1

# 例如：运行相邻索引测试
cd index
.\run_test_adjacent_index.ps1

# 例如：运行哈希索引测试
cd index
.\run_test_hash_index.ps1

# 例如：运行智能索引建议系统测试
cd index
.\run_test_index_advisor.ps1
```

### 运行所有测试

在unit_tests目录下运行：

```powershell
.\run_unit_tests.ps1
```

## 测试统计

### 核心模块 (core)
- test_table_mode: 36个测试
- test_table_manager: 30个测试
- test_data_manager: 38个测试
- test_file_manager: 41个测试
- **总计**: 145个测试

### SQL解析器 (sql_parser)
- test_lexer: 78个测试
- test_parser: 已集成到其他测试中
- test_sql_parser: 手动测试程序（用于验证SQL解析逻辑）
- **总计**: 78个测试（不包括test_sql_parser手动测试）

### DDL功能 (ddl)
- test_create_table_handler: 24个测试
- test_edit_table_handler: 21个测试
- test_rename_table_handler: 23个测试
- test_drop_table_handler: 27个测试
- test_ddl_executor: 38个测试
- **总计**: 133个测试

### DML功能 (dml)
- test_insert_handler: 43个测试
- test_delete_handler: 38个测试
- test_update_handler: 43个测试
- test_dml_executor: 42个测试
- **总计**: 166个测试

### 查询功能 (query)
- test_select_handler: 161个测试（包含单表、多表、JOIN查询）
- test_query_executor: 24个测试
- **总计**: 185个测试

### 索引技术 (index)
- test_adjacent_index: 28个测试
- test_hash_index: 29个测试
- test_index_advisor: 14个测试（智能索引建议系统）
- **总计**: 71个测试

### 索引技术 (index)
- test_adjacent_index: 28个测试
- test_hash_index: 29个测试
- **总计**: 57个测试

**所有测试总计**: 736个测试

## 测试脚本说明

每个测试脚本（.ps1文件）负责：
1. 编译对应的测试程序（.cpp文件）
2. 运行测试程序
3. 显示测试结果
4. 返回退出码（0=成功，1=失败）

## 注意事项

- 测试脚本使用g++编译器，需要确保系统已安装g++
- 测试会在`test_results/unit_tests/`目录下生成可执行文件
- 测试过程中会创建临时数据库文件（.dbf和.dat），测试完成后会自动清理

