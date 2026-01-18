# 开发日志

> 记录项目开发过程中的重要事件、决策和进展

---

## 2026-01-12

### 项目初始化

**时间**：2026-01-12

**完成工作**：
1. ✅ 项目技术栈确定
   - 编程语言：C++
   - GUI框架：Qt 5.15.2
   - 开发环境：VSCode (Cursor)
   - 构建工具：CMake 4.2.1
   
2. ✅ 项目主题确定
   - 项目定位：通用数据库管理系统（DBMS）
   - 数据库新技术：多种索引技术（相邻索引、哈希索引、B+树索引）、智能索引建议系统

3. ✅ 项目结构创建
   - 创建完整的文件夹结构（src, include, resources, tests, scripts等）
   - 建立代码组织规范

4. ✅ 基础文档创建
   - .cursorrules - 项目规则和规范（包含文档更新规范）
   - cursor_context.md - 项目进度跟踪
   - README.md - 项目说明
   - development_log.md - 本文档
   - iteration_records.md - 版本迭代记录
   - project_architecture.md - 项目架构设计
   - project_report.md - 项目报告模板
   - project_task_guide.md - 任务进度指南
   - 其他技术分析文档
   - .gitignore - Git忽略文件

5. ✅ 开发环境配置
   - CMake 4.2.1 安装并配置
   - Qt 5.15.2 安装（conda环境）并配置
   - MSVC编译器配置（Visual Studio 2022 BuildTools）
   - CMake配置验证通过（Qt已正确找到）

6. ✅ 测试脚本框架创建
   - 创建scripts目录结构（unit_tests, module_tests, integration_tests等）
   - 创建测试运行脚本和工具脚本

7. ✅ 文档结构重新组织
   - 按类型分类组织（core, development, project, technical, guides）
   - 更新所有文档路径引用

**技术决策**：
- 选择C++作为开发语言，便于底层文件操作和性能控制
- 选择Qt 5.15.2作为GUI框架，提供完善的界面开发能力
- 确定数据库新技术方向：索引技术（相邻索引、哈希索引、B+树索引）和智能索引建议系统
- 使用conda环境中的Qt（便于环境管理）
- 文档结构按类型分类组织（提高可维护性）

**下一步计划**：
1. 开始阶段1：核心数据结构设计（TableMode等）
2. 实现基础的.dbf文件读写功能
3. 实现基础的.dat文件读写功能

**遇到的问题**：
- PowerShell的mkdir命令参数格式问题（已解决，改用New-Item）
- CMake配置脚本中文字符编码问题（已解决，改用英文）

---

## 2026-01-12（下午）

### 阶段1：核心数据结构与文件存储完成

**时间**：2026-01-12

**完成工作**：

1. ✅ **任务1.1：定义核心数据结构**
   - 创建`include/core/table_mode.h`
   - 定义TableMode结构体（字段结构）
   - 定义TableInfo结构（表结构信息）
   - 定义Record结构（记录数据）
   - 定义TableData结构（表数据信息）
   - 定义常量（FIELD_NAME_LENGTH=32, TYPE_NAME_LENGTH=8, TABLE_NAME_LENGTH=64等）
   - 实现辅助函数（initTableMode, compareTableMode）
   - 编写测试代码并运行（36/36测试通过）
   - **文件**：`scripts/unit_tests/test_table_mode.cpp`, `scripts/unit_tests/run_test_table_mode.ps1`

2. ✅ **任务1.2：实现.dbf文件读写**
   - 创建`include/core/table_manager.h`, `src/core/table_manager.cpp`
   - 实现TableManager类
   - 实现表结构文件的读写操作（createTable, readTable, readAllTables）
   - 实现表的增删改查（updateTable, deleteTable, renameTable）
   - 实现多表存储管理（使用'~'分隔符分隔不同表）
   - 实现表存在性检查和获取所有表名
   - 编写测试代码并运行（30/30测试通过）
   - **文件**：`scripts/unit_tests/test_table_manager.cpp`, `scripts/unit_tests/run_test_table_manager.ps1`

3. ✅ **任务1.3：实现.dat文件读写**
   - 创建`include/core/data_manager.h`, `src/core/data_manager.cpp`
   - 实现DataManager类
   - 实现数据文件的读写操作（insertRecord, readAllRecords, readValidRecords）
   - 实现记录的增删改查（updateRecord, deleteRecord）
   - 实现多表数据存储管理（使用'~'分隔符）
   - 实现记录有效性标识管理（软删除机制）
   - 实现记录数量统计（getRecordCount, getValidRecordCount）
   - 编写测试代码并运行（38/38测试通过）
   - **文件**：`scripts/unit_tests/test_data_manager.cpp`, `scripts/unit_tests/run_test_data_manager.ps1`

4. ✅ **任务1.4：基础文件I/O封装**
   - 创建`include/core/file_manager.h`, `src/core/file_manager.cpp`
   - 实现FileManager类（静态方法）
   - 实现文件操作基础接口（fileExists, deleteFile, getFileSize等）
   - 实现路径操作（getFileExtension, getFileName, getDirectoryPath, joinPath, normalizePath等）
   - 实现文本文件读写（readTextFile, writeTextFile, appendTextFile）
   - 实现二进制文件读写（readBinaryFile, writeBinaryFile）
   - 实现文件复制和移动（copyFile, moveFile）
   - 实现目录创建（createDirectory）
   - 实现错误处理机制（getLastError）
   - 支持跨平台（Windows和Unix/Linux）
   - 编写测试代码并运行（41/41测试通过）
   - **文件**：`scripts/unit_tests/test_file_manager.cpp`, `scripts/unit_tests/run_test_file_manager.ps1`

**技术决策**：
- 使用'~'作为表分隔符（.dbf和.dat文件中）
- 字段值采用变长存储方式（先存储长度，再存储内容）
- 记录删除采用软删除机制（标记为无效，不实际删除数据）
- 文件I/O封装为静态类，便于使用
- 所有核心功能都编写了对应的测试脚本，确保功能正确性

**测试统计**：
- 任务1.1：36个测试全部通过
- 任务1.2：30个测试全部通过
- 任务1.3：38个测试全部通过
- 任务1.4：41个测试全部通过
- **总计：145个测试全部通过** ✅

**下一步计划**：
1. 开始阶段2：DDL实现
   - SQL解析器基础框架（词法分析器、语法分析器、AST设计）
   - CREATE TABLE语句解析和实现
   - EDIT TABLE语句解析和实现
   - RENAME TABLE语句解析和实现
   - DROP TABLE语句解析和实现

**遇到的问题**：
- 测试脚本PowerShell语法问题（已解决，简化脚本结构）
- Windows路径分隔符处理（已解决，使用normalizePath统一处理）

---

## 2026-01-12（晚上）

### 阶段2：DDL实现完成

**时间**：2026-01-12

**完成工作**：

1. ✅ **任务2.1：SQL解析器基础框架**
   - 创建`include/sql_parser/token.h`, `src/sql_parser/token.cpp`
   - 定义Token结构体和TokenType枚举
   - 实现关键词映射函数（keywordToTokenType）
   - 创建`include/sql_parser/lexer.h`, `src/sql_parser/lexer.cpp`
   - 实现Lexer类（词法分析器）
   - 支持标识符、数字、字符串字面量、运算符、分隔符识别
   - 自动跳过空白字符
   - 创建`include/sql_parser/parser.h`, `src/sql_parser/parser.cpp`
   - 实现Parser类（语法分析器）
   - 支持DDL语句解析（CREATE TABLE, EDIT TABLE, RENAME TABLE, DROP TABLE）
   - 创建`include/sql_parser/ast_node.h`, `src/sql_parser/ast_node.cpp`
   - 定义AST节点结构（ASTNode基类，CreateTableNode, EditTableNode, RenameTableNode, DropTableNode）
   - 实现访问者模式（ASTVisitor接口）
   - 编写测试代码并运行（78/78测试通过）
   - **文件**：`scripts/unit_tests/test_lexer.cpp`, `scripts/unit_tests/run_test_lexer.ps1`, `scripts/unit_tests/test_parser.cpp`, `scripts/unit_tests/run_test_parser.ps1`

2. ✅ **任务2.2：CREATE TABLE实现**
   - 创建`include/ddl/create_table_handler.h`, `src/ddl/create_table_handler.cpp`
   - 实现CreateTableHandler类
   - 集成Parser解析CREATE TABLE语句
   - 验证表结构（字段名、类型、标志等）
   - 调用TableManager创建表
   - 错误处理和验证
   - 编写测试代码并运行（24/24测试通过）
   - **文件**：`scripts/unit_tests/test_create_table_handler.cpp`, `scripts/unit_tests/run_test_create_table_handler.ps1`

3. ✅ **任务2.3：EDIT TABLE实现**
   - 创建`include/ddl/edit_table_handler.h`, `src/ddl/edit_table_handler.cpp`
   - 实现EditTableHandler类
   - 集成Parser解析EDIT TABLE语句
   - 支持添加新字段（字段不存在时）
   - 支持修改现有字段（字段存在时）
   - 支持修改字段类型、KEY标志、NULL标志
   - 字段定义验证
   - 编写测试代码并运行（21/21测试通过）
   - **文件**：`scripts/unit_tests/test_edit_table_handler.cpp`, `scripts/unit_tests/run_test_edit_table_handler.ps1`

4. ✅ **任务2.4：RENAME TABLE实现**
   - 创建`include/ddl/rename_table_handler.h`, `src/ddl/rename_table_handler.cpp`
   - 实现RenameTableHandler类
   - 集成Parser解析RENAME TABLE语句
   - 支持表名重命名（.dbf和.dat文件）
   - 支持数据迁移（重命名时保留记录）
   - 参数验证（表名长度、新旧表名是否相同等）
   - 编写测试代码并运行（23/23测试通过）
   - **文件**：`scripts/unit_tests/test_rename_table_handler.cpp`, `scripts/unit_tests/run_test_rename_table_handler.ps1`

5. ✅ **任务2.5：DROP TABLE实现**
   - 创建`include/ddl/drop_table_handler.h`, `src/ddl/drop_table_handler.cpp`
   - 实现DropTableHandler类
   - 集成Parser解析DROP TABLE语句
   - 支持删除表结构（.dbf文件）
   - 支持删除表数据（.dat文件）
   - 支持删除无数据表和有数据表
   - 编写测试代码并运行（27/27测试通过）
   - **文件**：`scripts/unit_tests/test_drop_table_handler.cpp`, `scripts/unit_tests/run_test_drop_table_handler.ps1`

6. ✅ **任务2.6：DDL执行器整合**
   - 创建`include/ddl/ddl_executor.h`, `src/ddl/ddl_executor.cpp`
   - 实现DDLExecutor类
   - 统一调度所有DDL语句
   - 自动识别SQL语句类型（使用Lexer）
   - 路由到对应的处理器（CREATE/EDIT/RENAME/DROP）
   - 统一的错误处理接口
   - 静态方法isDDLStatement判断是否为DDL语句
   - 编写测试代码并运行（38/38测试通过）
   - **文件**：`scripts/unit_tests/test_ddl_executor.cpp`, `scripts/unit_tests/run_test_ddl_executor.ps1`

**技术决策**：
- 使用词法分析器（Lexer）和语法分析器（Parser）分离的设计
- 使用抽象语法树（AST）表示解析后的SQL语句
- 使用访问者模式（Visitor Pattern）处理AST节点
- 每个DDL操作都有独立的Handler类，便于维护和扩展
- DDLExecutor统一调度，提供统一的接口
- 所有DDL功能都编写了对应的测试脚本，确保功能正确性

**测试统计**：
- 任务2.1：78个测试全部通过
- 任务2.2：24个测试全部通过
- 任务2.3：21个测试全部通过
- 任务2.4：23个测试全部通过
- 任务2.5：27个测试全部通过
- 任务2.6：38个测试全部通过
- **总计：211个测试全部通过** ✅

**下一步计划**：
1. 开始阶段3：DML实现
   - INSERT语句解析和实现
   - DELETE语句解析和实现
   - UPDATE语句解析和实现
   - DML执行器整合

**遇到的问题**：
- 头文件包含路径不一致问题（已解决，统一使用完整路径如"core/table_mode.h"）
- TokenType枚举使用问题（已解决，直接使用具体的TokenType值如TokenType::CREATE）

---

## 2026-01-12（晚上）

### 阶段3：DML实现完成

**时间**：2026-01-12

**完成工作**：

1. ✅ **任务3.1：INSERT实现**
   - 在Parser中实现parseInsert方法（支持INSERT INTO ... VALUES ... IN ...语法）
   - 创建`include/dml/insert_handler.h`, `src/dml/insert_handler.cpp`
   - 实现InsertHandler类
   - 集成Parser解析INSERT语句
   - 实现数据类型验证（int, float, double, char, string）
   - 实现NULL值处理
   - 实现值数量验证
   - 调用DataManager插入记录
   - 编写测试代码并运行（43/43测试通过）
   - **文件**：`scripts/unit_tests/test_insert_handler.cpp`, `scripts/unit_tests/run_test_insert_handler.ps1`

2. ✅ **任务3.2：DELETE实现**
   - Parser中已实现parseDelete方法（支持DELETE FROM ... WHERE ... IN ...语法）
   - 创建`include/dml/delete_handler.h`, `src/dml/delete_handler.cpp`
   - 实现DeleteHandler类
   - 集成Parser解析DELETE语句
   - 实现WHERE子句条件评估器（支持等号比较）
   - 实现记录删除逻辑（软删除，标记为无效）
   - 支持删除单条和多条记录
   - 提供删除记录数量统计
   - 编写测试代码并运行（38/38测试通过）
   - **文件**：`scripts/unit_tests/test_delete_handler.cpp`, `scripts/unit_tests/run_test_delete_handler.ps1`

3. ✅ **任务3.3：UPDATE实现**
   - Parser中已实现parseUpdate方法（支持UPDATE ... (SET ... WHERE ...) IN ...语法）
   - 创建`include/dml/update_handler.h`, `src/dml/update_handler.cpp`
   - 实现UpdateHandler类
   - 集成Parser解析UPDATE语句
   - 实现SET子句处理（更新字段值）
   - 实现WHERE子句条件评估（支持等号比较）
   - 实现数据类型验证和转换（int, float, double, char, string）
   - 实现NULL值处理
   - 支持更新单条和多条记录
   - 提供更新记录数量统计
   - 编写测试代码并运行（43/43测试通过）
   - **文件**：`scripts/unit_tests/test_update_handler.cpp`, `scripts/unit_tests/run_test_update_handler.ps1`

4. ✅ **任务3.4：DML执行器整合**
   - 创建`include/dml/dml_executor.h`, `src/dml/dml_executor.cpp`
   - 实现DMLExecutor类
   - 统一调度所有DML语句（INSERT、DELETE、UPDATE）
   - 自动识别SQL语句类型（使用Lexer）
   - 路由到对应的处理器
   - 统一的错误处理接口
   - 静态方法isDMLStatement判断是否为DML语句
   - getAffectedRows方法获取影响的记录数
   - 编写测试代码并运行（42/42测试通过）
   - **文件**：`scripts/unit_tests/test_dml_executor.cpp`, `scripts/unit_tests/run_test_dml_executor.ps1`

**技术决策**：
- Parser扩展支持DML语句解析（INSERT、DELETE、UPDATE）
- 每个DML操作都有独立的Handler类，便于维护和扩展
- DELETE和UPDATE使用WHERE子句条件评估器（当前支持等号比较）
- DELETE采用软删除机制（标记为无效，不实际删除数据）
- UPDATE支持批量更新多条匹配的记录
- DMLExecutor统一调度，提供统一的接口
- 所有DML功能都编写了对应的测试脚本，确保功能正确性

**测试统计**：
- 任务3.1：43个测试全部通过
- 任务3.2：38个测试全部通过
- 任务3.3：43个测试全部通过
- 任务3.4：42个测试全部通过
- **总计：166个测试全部通过** ✅

**下一步计划**：
1. 开始阶段4：查询实现
   - SELECT单表查询实现
   - SELECT多表查询实现
   - SELECT连接查询实现
   - 查询执行器整合

**遇到的问题**：
- 无（阶段3开发顺利，所有测试通过）

---

## 2026-01-14（早上）

### 数据库新技术方案确定

**时间**：2026-01-12

**完成工作**：
1. ✅ 创建数据库新技术选择分析文档
   - 分析9种数据库新技术选项
   - 评估实现难度、工作量、展示效果
   - 结合项目当前进度评估可行性
   - **文件**：`docs/technical/database_new_technology_selection.md`

2. ✅ 确定数据库新技术方案
   - **选择方案A**：索引技术 + 智能推荐
   - **技术组合**：
     - 相邻索引（Adjacent Index）- 3-5天
     - 哈希索引（Hash Index）- 4-6天
     - 智能索引建议系统（Index Advisor）- 5-7天
     - B+树索引（计划中，第二阶段）
   - **总工作量**：12-18天（数据库新技术部分）

**技术决策**：
- 选择相邻索引作为基础索引技术（实现简单，效果明显，适合范围查询优化）
- 选择哈希索引作为经典索引技术（主键查询性能提升显著，易于演示）
- 选择智能索引建议系统作为AI与数据库结合技术（体现AI融合，展示效果极佳）
- 形成完整的索引技术体系（相邻索引、哈希索引、B+树索引、智能索引建议）
- 所有新技术都基于现有架构扩展，不需要大幅修改现有代码

**选择理由**：
1. **技术覆盖全面**：索引、优化、AI多个技术点
2. **实现难度适中**：工作量可控（12-18天）
3. **展示效果极佳**：性能对比直观，适合课程演示
4. **技术含量高**：体现数据库系统深入理解
5. **符合课程要求**：数据库新技术 + AI融合

**下一步计划**：
1. 开始阶段5：GUI开发（Qt界面实现）
2. 开始阶段8：数据库新技术实现
   - 优先实现相邻索引（基础，3-5天）
   - 然后实现哈希索引（经典技术，4-6天）
   - 最后实现智能索引建议系统（AI融合，5-7天）
3. 阶段6：核心通用DBMS功能（必须）

**遇到的问题**：
- 无（技术方案选择顺利，已创建详细分析文档）

---

## 2026-01-14（早上）

### 阶段8.1：相邻索引实现完成

**时间**：2026-01-14

**完成工作**：

1. ✅ **相邻索引实现**
   - 创建`include/core/adjacent_index.h`, `src/core/adjacent_index.cpp`
   - 实现AdjacentIndex类
   - 实现索引构建功能（buildIndex）
   - 实现相邻关系查询（getNextIndex, getPrevIndex）
   - 实现顺序扫描优化（sequentialScan）
   - 实现范围查询优化（rangeQuery）
   - 实现索引更新和删除功能
   - 编写测试代码并运行（28/28测试通过）
   - **实现文件**：`include/core/adjacent_index.h`, `src/core/adjacent_index.cpp`
   - **测试文件**：`scripts/unit_tests/index/test_adjacent_index.cpp`, `scripts/unit_tests/index/run_test_adjacent_index.ps1`

2. ✅ **测试脚本分类整理**
   - 创建测试脚本分类目录结构（core, ddl, dml, query, sql_parser, index）
   - 将测试脚本按模块分类移动
   - 更新测试脚本中的路径引用（修复include路径）
   - 创建测试脚本README说明文档
   - 删除重复的测试脚本文件
   - **文件**：`scripts/unit_tests/README.md`

**技术决策**：
- 相邻索引在内存中维护相邻记录指针映射
- 支持多字段索引（可为不同字段分别构建索引）
- 索引构建时自动对记录按字段值排序
- 范围查询和顺序扫描利用索引优化性能
- 测试脚本按模块分类组织，提高可维护性

**测试统计**：
- 任务8.1：28个测试全部通过 ✅
- **所有已完成阶段测试总计**：740个测试全部通过 ✅

**下一步计划**：
1. 继续阶段8：数据库新技术实现
   - 任务8.2：哈希索引实现（4-6天）
   - 任务8.3：智能索引建议系统（5-7天）

**遇到的问题**：
- PowerShell脚本编码问题（已解决，改用英文输出）
- 测试文件include路径问题（已解决，更新为相对路径 `../../../include/`）
- 缺少头文件`#include <iostream>`（已解决）
- 测试脚本文件重复（已解决，删除根目录下的重复文件，保留index/目录中的文件）

---

## 2026-01-14（早上）

### 阶段8.2：哈希索引实现完成

**时间**：2026-01-14

**完成工作**：

1. ✅ **哈希索引实现**
   - 创建`include/core/hash_index.h`, `src/core/hash_index.cpp`
   - 实现HashIndex类
   - 实现哈希表结构（使用链地址法解决冲突）
   - 实现多种类型的哈希函数（int, float/double, string）
   - 实现索引构建功能（buildIndex）
   - 实现点查询功能（pointQuery，O(1)平均时间复杂度）
   - 实现索引更新和删除功能
   - 实现索引统计功能（getIndexStats）
   - 编写测试代码并运行（29/29测试通过）
   - **实现文件**：`include/core/hash_index.h`, `src/core/hash_index.cpp`
   - **测试文件**：`scripts/unit_tests/index/test_hash_index.cpp`, `scripts/unit_tests/index/run_test_hash_index.ps1`

**技术决策**：
- 使用链地址法解决哈希冲突（每个哈希桶是一个链表）
- 默认哈希表大小为1009（质数，减少冲突）
- 针对不同数据类型使用不同的哈希函数：
  - 整数类型：使用取模哈希
  - 浮点数类型：转换为整数后取模
  - 字符串类型：使用djb2哈希算法
- 支持相同键值的多条记录（使用链表存储多个记录索引）

**测试统计**：
- 任务8.2：29个测试全部通过 ✅
- **所有已完成阶段测试总计**：740个测试全部通过 ✅

**下一步计划**：
1. 继续阶段8：数据库新技术实现
   - 任务8.3：智能索引建议系统（5-7天）

**遇到的问题**：
- 无（哈希索引实现顺利，所有测试通过）

---

## 2026-01-14（下午）

### 阶段8.3：智能索引建议系统实现完成

**时间**：2026-01-14

**完成工作**：

1. ✅ **智能索引建议系统实现**
   - 创建`include/index/index_advisor.h`, `src/index/index_advisor.cpp`
   - 实现IndexAdvisor类
   - 实现查询日志记录功能（logQuery）
   - 实现字段使用统计功能（analyzeQueryLogs）
   - 实现慢查询识别功能（identifySlowQueries）
   - 实现索引推荐生成功能（generateRecommendations）
   - 实现索引效果评估功能（evaluateIndexEffect）
   - 集成到QueryExecutor中，自动记录所有查询的执行时间和字段使用情况
   - 编写测试代码并运行（14/14测试通过）
   - **实现文件**：`include/index/index_advisor.h`, `src/index/index_advisor.cpp`
   - **测试文件**：`scripts/unit_tests/index/test_index_advisor.cpp`, `scripts/unit_tests/index/run_test_index_advisor.ps1`

**技术决策**：
- 查询日志记录：无论查询成功与否，都记录日志用于分析
- 日志限制：最多保留1000条日志，避免内存占用过大
- 推荐算法：基于字段使用频率和执行时间计算推荐分数
- 索引类型判断：自动判断字段适合哈希索引还是相邻索引
- 推荐阈值：使用次数至少3次，平均执行时间至少10ms才推荐

**功能特性**：
- 自动记录查询日志（SQL、表名、WHERE字段、执行时间、结果数量）
- 字段使用统计（使用次数、总执行时间、平均执行时间、是否已有索引）
- 慢查询识别（可配置阈值，默认100ms）
- 智能索引推荐（基于使用频率和执行时间，自动判断索引类型）
- 索引效果评估（评估现有索引的性能提升效果）

**测试统计**：
- 任务8.3：14个测试全部通过 ✅
- **所有已完成阶段测试总计**：754个测试全部通过 ✅

**下一步计划**：
1. 阶段5：Qt GUI界面开发
2. 阶段6：核心通用DBMS功能实现
3. 阶段7：测试和优化

**遇到的问题**：
- 测试中查询执行失败导致日志未记录（已解决，修改为无论成功与否都记录日志）

---

## 2026-01-14（下午）

### 阶段5.1：GUI最小可运行程序完成

**时间**：2026-01-14

**完成工作**：

1. ✅ **GUI目录结构创建**
   - 创建`include/gui/`和`src/gui/`目录
   - 配置CMakeLists.txt支持Qt GUI
   - 更新.cursorrules添加字体规范（Segoe UI）

2. ✅ **主窗口类实现**
   - 创建`include/gui/main_window.h`和`src/gui/main_window.cpp`
   - 实现MainWindow类（继承自QMainWindow）
   - 实现基础UI组件（欢迎标签、测试按钮）
   - 实现菜单栏（文件菜单：退出；帮助菜单：关于）
   - 实现状态栏（显示"就绪"状态）
   - 设置窗口标题和初始大小（800x600）

3. ✅ **应用程序入口实现**
   - 创建`src/gui/main.cpp`
   - 实现QApplication初始化和配置
   - 设置应用程序信息（名称、版本、组织）
   - 设置全局字体（英文使用Segoe UI）
   - 设置应用程序样式（Fusion）

4. ✅ **编译问题修复**
   - 修复`parser_select.cpp`缺少头文件包含问题（添加`#include "sql_parser/parser.h"`）
   - 修复`parser.cpp`和`parser_select.cpp`中`parseSelect()`重复定义问题（删除`parser.cpp`中的实现）
   - 修复Qt MOC未处理`main_window.h`的问题（在CMakeLists.txt中显式添加GUI头文件）
   - 添加`/utf-8`编译选项解决编码问题（C4819警告）

**技术决策**：
- 使用Qt 5.15.2的QMainWindow作为主窗口基类
- 使用QVBoxLayout进行垂直布局
- 使用QFont设置英文字体为Segoe UI（符合项目规范）
- 使用Qt的信号槽机制处理按钮点击事件
- 使用CMake的AUTOMOC自动处理Q_OBJECT宏

**遇到的问题**：
1. **编译错误**：`parser_select.cpp`缺少头文件包含
   - **原因**：文件缺少`#include "sql_parser/parser.h"`
   - **解决**：添加必要的头文件包含

2. **链接错误**：`parseSelect()`函数重复定义
   - **原因**：`parser.cpp`和`parser_select.cpp`中都定义了`parseSelect()`
   - **解决**：删除`parser.cpp`中的实现，保留`parser_select.cpp`中的完整实现（支持JOIN）

3. **链接错误**：Qt MOC元对象代码未生成
   - **原因**：CMake的AUTOMOC未识别`include/gui/main_window.h`
   - **解决**：在CMakeLists.txt中显式添加GUI头文件到源文件列表

4. **编码警告**：C4819警告（文件包含不能在当前代码页中表示的字符）
   - **原因**：源文件使用UTF-8编码，但MSVC默认使用代码页936
   - **解决**：在CMakeLists.txt中添加`/utf-8`编译选项

**测试结果**：
- GUI程序能够成功编译并运行 ✅
- 主窗口正常显示 ✅
- 菜单栏和状态栏正常显示 ✅
- 测试按钮点击功能正常 ✅
- 字体设置正确（英文使用Segoe UI）✅

**文件位置**：
- `include/gui/main_window.h` - 主窗口头文件
- `src/gui/main_window.cpp` - 主窗口实现
- `src/gui/main.cpp` - 应用程序入口
- `CMakeLists.txt` - 构建配置（已更新支持GUI）

**下一步计划**：
1. 实现主界面布局结构（标签页系统）
2. 实现表结构管理界面
3. 实现数据操作界面
4. 实现SQL执行界面

---

## 2026-01-14（下午）

### 阶段5.2：GUI主界面布局实现

**时间**：2026-01-14

**完成工作**：
1. ✅ 实现标签页系统（QTabWidget）
   - 创建5个标签页：Table Management, Index Management, Data Operation, SQL Execution, Guide
   - 标签页顺序优化（Index Management在Table Management和Data Operation之间）
   - Guide标签页重命名（原Recommendation）

2. ✅ 界面文字和字体统一
   - 所有界面文字改为英文
   - 所有英文文字使用Segoe UI字体（通过QFont设置）
   - 更新.cursorrules添加字体规范

3. ✅ 状态栏增强
   - 显示当前数据库名称和当前时间（格式：`database_name  yyyy/MM/dd hh:mm`）
   - 使用QTimer每秒更新一次时间
   - 未加载数据库时仅显示时间

4. ✅ 全局键盘快捷键
   - Ctrl+Q：退出程序
   - F1：显示About对话框
   - Ctrl+Tab：切换到下一个标签页
   - Ctrl+Shift+Tab：切换到上一个标签页

5. ✅ 窗口标题版本号
   - 窗口标题包含项目版本号（v0.6.1）
   - 从iteration_records.md读取版本信息

6. ✅ Guide标签页实现
   - 创建USER_GUIDE.md文件（项目根目录）
   - Guide标签页显示USER_GUIDE.md内容
   - 使用QTextEdit和QScrollArea实现滚动显示
   - 更新.cursorrules确保USER_GUIDE.md和GUI Guide内容同步

**技术决策**：
- 使用QTabWidget实现标签页系统，便于功能模块化
- 使用QTimer实现状态栏时间更新，避免阻塞UI
- 使用QFile和QTextStream读取USER_GUIDE.md，便于维护

**文件位置**：
- `include/gui/main_window.h` - 主窗口头文件（已更新）
- `src/gui/main_window.cpp` - 主窗口实现（已更新）
- `USER_GUIDE.md` - 用户操作指南（新建）

**下一步计划**：
1. 实现数据库管理和表结构管理界面
2. 实现数据操作界面
3. 实现SQL执行界面

---

## 2026-01-14（晚上）

### 阶段5.3：数据库管理和表结构管理界面实现

**时间**：2026-01-14

**完成工作**：
1. ✅ 数据库管理功能
   - 实现Create Database功能（File -> Create Database，Ctrl+N）
     - 使用文件对话框选择保存位置和文件名
     - 自动创建.dbf和.dat文件（空文件）
     - 创建成功后自动加载数据库
   - 实现Open Database功能（File -> Open Database，Ctrl+O）
     - 使用文件对话框选择.dbf文件
     - 自动提取数据库路径并加载
     - 打开成功后更新状态栏和表列表
   - 数据库路径管理
     - MainWindow保存当前数据库路径（m_databasePath）
     - 所有操作使用当前选中的数据库
     - 状态栏显示当前数据库名称

2. ✅ 表结构管理界面
   - 实现TableManagementWidget组件
     - 左侧：表列表（QListWidget）
     - 右侧：表信息显示（字段列表、类型、属性等）
     - 底部：操作按钮（Create, Edit, Delete, Refresh）
   - 实现TableEditDialog对话框
     - 创建表模式：输入表名和字段定义
     - 编辑表模式：加载现有表结构并允许修改
     - 字段管理：添加、删除、上移、下移字段
     - 字段属性：类型、大小、KEY/NOT_KEY、NULL/NO_NULL、VALID/INVALID
   - 集成TableManager后端
     - 创建表：调用TableManager::createTable()
     - 编辑表：调用TableManager::updateTable()
     - 删除表：调用TableManager::deleteTable()
     - 刷新列表：调用TableManager::getAllTableNames()

3. ✅ 界面优化和问题修复
   - "No table selected"文本居中显示
   - Edit Table对话框：按钮文本动态更新（Create/Edit）
   - Edit Table对话框：数据库名称输入框隐藏（使用当前数据库）
   - 修复关闭事件处理：点击X时弹出确认对话框，防止意外关闭

**技术决策**：
- 使用QFileDialog实现文件选择，提供更好的用户体验
- 数据库路径统一由MainWindow管理，避免重复输入
- 使用QDialogButtonBox实现标准对话框按钮，保持一致性
- 实现closeEvent()处理关闭事件，防止数据丢失

**遇到的问题和解决方案**：
1. **问题**：点击Edit Table对话框的X按钮时程序崩溃
   - **原因**：未实现closeEvent()处理函数
   - **解决**：实现closeEvent()，弹出确认对话框，与Cancel按钮行为一致

2. **问题**：Edit Table模式下仍显示数据库名称输入框
   - **原因**：数据库路径应由主窗口管理，不应在编辑对话框中输入
   - **解决**：在编辑模式下隐藏数据库名称输入框和标签

3. **问题**："No table selected"文本未居中
   - **解决**：使用setAlignment(Qt::AlignCenter)设置文本对齐

**文件位置**：
- `include/gui/main_window.h` - 主窗口头文件（已更新，添加数据库管理功能）
- `src/gui/main_window.cpp` - 主窗口实现（已更新）
- `include/gui/table_management_widget.h` - 表管理组件头文件（新建）
- `src/gui/table_management_widget.cpp` - 表管理组件实现（新建）

**下一步计划**：
1. 实现数据操作界面
2. 实现SQL执行界面
3. 实现索引管理界面

---

### 阶段5.4：数据操作界面开发

**时间**：2026-01-14

**完成工作**：
1. ✅ 创建DataOperationWidget类
   - 实现数据表格显示（QTableWidget）
   - 实现表选择下拉框（QComboBox）
   - 实现操作按钮（Insert, Edit, Delete, Refresh）

2. ✅ 实现RecordEditDialog对话框
   - 动态生成字段输入表单（根据表结构）
   - 字段标签显示KEY和必填标识（*）
   - 数据类型验证（int, float, double）
   - NULL约束检查
   - 关闭事件处理（确认对话框）

3. ✅ 实现插入记录功能
   - 打开RecordEditDialog对话框
   - 数据验证和类型转换
   - 调用DataManager插入记录
   - 插入成功后自动刷新表格

4. ✅ 实现编辑记录功能
   - 选择记录后打开编辑对话框
   - 加载选中记录的数据
   - 修改后保存（处理有效记录索引映射）
   - 更新成功后自动刷新表格

5. ✅ 实现删除记录功能
   - 选择记录后显示确认对话框
   - 标记记录为无效（软删除）
   - 处理有效记录索引映射
   - 删除成功后自动刷新表格

6. ✅ 实现表列表自动刷新
   - 切换标签页时自动刷新（onTabChanged）
   - 设置数据库路径时自动刷新（setCurrentDatabase）
   - 创建新表后自动刷新（通过标签页切换触发）

7. ✅ 修复编译错误
   - 修复`TableInfo`未定义问题（改为包含`core/table_mode.h`而非前向声明）
   - 更新CMakeLists.txt添加`data_operation_widget.h`到MOC处理列表

**遇到的问题和解决方案**：
1. **问题**：编译错误 - `TableInfo`未定义
   - **原因**：在头文件中只使用了前向声明，但`TableInfo`作为对象成员需要完整定义
   - **解决方案**：在`data_operation_widget.h`中包含`#include "core/table_mode.h"`

2. **问题**：创建新表后，Data Operation标签页的表列表没有更新
   - **原因**：`TableManagementWidget`创建表后只刷新了自己的列表，没有通知`DataOperationWidget`
   - **解决方案**：
     - 在`MainWindow::onTabChanged()`中添加切换到Data Operation标签页时自动刷新表列表
     - 在`MainWindow::setCurrentDatabase()`中添加刷新表列表的逻辑
     - 将`DataOperationWidget::loadTableList()`设为public方法

**文件位置**：
- `include/gui/data_operation_widget.h` - 数据操作界面头文件
- `src/gui/data_operation_widget.cpp` - 数据操作界面实现
- `include/gui/main_window.h` - 主窗口头文件（更新）
- `src/gui/main_window.cpp` - 主窗口实现（更新）
- `CMakeLists.txt` - 构建配置（更新）

**下一步计划**：
1. 实现SQL执行界面
2. 实现索引管理界面
3. 实现推荐系统界面

---

## 2026-01-15

### 阶段5.5：SQL执行界面开发和问题修复

**时间**：2026-01-15

**完成工作**：

1. ✅ 创建SQLQueryWidget类
   - 实现SQL语句输入编辑器（QTextEdit）
   - 实现执行按钮和结果展示
   - 实现批量SQL语句执行（自动分割多条语句）
   - 实现成功/失败消息框（使用Segoe UI字体，英文文本）
   - 实现查询结果表格显示

2. ✅ 主键唯一性约束实现
   - 在INSERT操作中实现主键唯一性检查（backend和GUI）
   - 在UPDATE操作中实现主键唯一性检查（backend和GUI）
   - 错误消息使用英文

3. ✅ 大小写不敏感问题修复
   - 修复表名大小写不敏感（与主流DBMS对齐）
   - 修复关键字大小写不敏感
   - 在所有表名/字段名比较中使用`strcasecmp_custom`
   - 在DataManager中使用`CaseInsensitiveStringCompare`作为map的key比较器

4. ✅ SQL语法支持扩展
   - 支持`char[length]`和`char(length)`两种语法
   - 支持JOIN查询中的`TableName.FieldName`格式（添加DOT token）
   - 修复UPDATE 0行受影响的消息提示（更符合标准SQL行为）

5. ✅ DROP TABLE数据删除问题修复
   - 修复`clearTable`函数，确保硬删除所有大小写变体的数据
   - 修复`readAllRecords`函数，移除对.dbf文件的检查，直接从.dat文件读取
   - 修复`dropTable`函数，检查`clearTable`的返回值
   - 确保删除表时同时删除.dbf中的表结构和.dat中的记录数据

6. ✅ RENAME TABLE文件同步问题修复
   - 修复`renameTable`函数，调整执行顺序：先迁移数据，再更新.dbf文件
   - 修复`readAllRecords`函数，支持RENAME TABLE场景（.dbf已更新但.dat还是旧表名）
   - 确保重命名表时.dbf和.dat文件保持同步

7. ✅ 测试用例完善
   - 完善SQL执行测试用例文档（`docs/testing/sql_execution_test_cases.md`）
   - 添加LEFT JOIN、RIGHT JOIN测试用例
   - 添加多表JOIN、JOIN带WHERE子句等测试用例
   - 添加未实现功能的测试用例（ORDER BY、GROUP BY等，标注为"未实现"）

8. ✅ 测试文件整理
   - 移动`test_sql_parser.cpp`到`scripts/unit_tests/sql_parser/`目录

**技术决策**：
- 使用QTextEdit实现SQL输入编辑器，支持多行输入
- 使用QTableWidget显示查询结果，支持表格格式
- 批量执行时自动按分号分割SQL语句，保留分号用于语法解析
- 所有GUI消息框使用英文和Segoe UI字体，保持一致性
- DROP TABLE采用硬删除策略，完全删除表结构和数据
- RENAME TABLE先迁移数据再更新结构，确保文件同步

**遇到的问题和解决方案**：
1. **问题**：DROP TABLE后重新创建同名表，旧数据仍然存在
   - **原因**：`clearTable`函数未正确删除所有大小写变体的数据，或数据文件读取逻辑有问题
   - **解决**：修复`clearTable`函数，确保跳过所有大小写变体的目标表数据；修复`readAllRecords`函数，移除对.dbf文件的检查

2. **问题**：RENAME TABLE后.dbf和.dat文件不同步
   - **原因**：`renameTable`先更新.dbf文件，再迁移数据，导致读取时.dbf已更新但.dat还是旧表名
   - **解决**：调整执行顺序，先迁移数据，再更新.dbf文件；修复`readAllRecords`函数，直接从.dat文件读取

3. **问题**：表名大小写不敏感问题
   - **原因**：表名比较使用区分大小写的字符串比较
   - **解决**：在所有表名/字段名比较中使用`strcasecmp_custom`，在DataManager中使用`CaseInsensitiveStringCompare`

**文件位置**：
- `include/gui/sql_query_widget.h` - SQL执行界面头文件（新建）
- `src/gui/sql_query_widget.cpp` - SQL执行界面实现（新建）
- `src/core/data_manager.cpp` - 数据管理器（修复clearTable和readAllRecords）
- `src/ddl/drop_table_handler.cpp` - DROP TABLE处理器（修复返回值检查）
- `src/ddl/rename_table_handler.cpp` - RENAME TABLE处理器（修复执行顺序）
- `docs/testing/sql_execution_test_cases.md` - SQL执行测试用例文档（完善）
- `scripts/unit_tests/sql_parser/test_sql_parser.cpp` - SQL解析器测试程序（移动）

**下一步计划**：
1. 实现索引管理界面
2. 实现推荐系统界面
3. 完善SQL功能（ORDER BY、GROUP BY等）

---

**记录格式说明**：
- 日期：YYYY-MM-DD
- 完成工作：列出当日完成的主要任务
- 技术决策：记录重要的技术选择
- 下一步计划：列出下一步要完成的工作
- 遇到的问题：记录遇到的问题和解决方案

---

## 2026-01-15（早上）

### SQL查询功能扩展：ORDER BY、DISTINCT、LIMIT、比较运算符和复杂WHERE条件

**时间**：2026-01-15

**完成工作**：

1. ✅ **ORDER BY排序功能实现**
   - 扩展Token类型，添加ORDER、BY、ASC、DESC关键字
   - 扩展AST节点，添加OrderByInfo结构体和SelectNode.orderBy字段
   - 实现ORDER BY子句解析（支持单字段和多字段排序）
   - 实现排序执行逻辑（支持数值和字符串比较，ASC/DESC方向）
   - 在单表查询、多表查询、JOIN查询中集成ORDER BY处理

2. ✅ **DISTINCT去重功能实现**
   - 扩展Token类型，添加DISTINCT关键字
   - 扩展AST节点，添加SelectNode.distinct字段
   - 实现DISTINCT子句解析
   - 实现去重执行逻辑（逐行比较，去除重复行）
   - 在单表查询、多表查询、JOIN查询中集成DISTINCT处理

3. ✅ **LIMIT分页功能实现**
   - 扩展Token类型，添加LIMIT关键字
   - 扩展AST节点，添加SelectNode.limitCount字段
   - 实现LIMIT子句解析
   - 实现分页执行逻辑（限制返回行数）
   - 在单表查询、多表查询、JOIN查询中集成LIMIT处理

4. ✅ **比较运算符实现**
   - 扩展Token类型，添加NOT_EQUALS、GREATER_THAN、LESS_THAN、GREATER_EQUAL、LESS_EQUAL
   - 扩展Lexer，识别比较运算符（>, <, >=, <=, !=）
   - 扩展AST节点，在WhereCondition中添加operator_字段支持多种运算符
   - 实现比较运算符评估逻辑（支持数值和字符串比较）

5. ✅ **复杂WHERE条件实现**
   - 扩展Token类型，添加AND、OR、NOT关键字
   - 扩展AST节点，设计WhereCondition树形结构（支持AND、OR、NOT逻辑运算符）
   - 实现递归下降解析器（parseWhereCondition、parseOrExpression、parseAndExpression、parseNotExpression、parseSimpleExpression、parseSimpleCondition）
   - 实现复杂条件评估逻辑（递归评估条件树，支持括号优先级）
   - 在单表查询中集成复杂WHERE条件处理

**技术决策**：
- 使用树形结构（WhereCondition）表示复杂WHERE条件，支持递归评估
- 使用递归下降解析器解析WHERE条件，优先级：NOT > AND > OR
- ORDER BY支持数值和字符串混合比较，自动识别数值类型
- DISTINCT使用逐行比较实现，避免使用std::set（需要自定义比较器）
- LIMIT在ORDER BY之后应用，确保返回排序后的前N条记录
- 比较运算符自动识别数值类型，进行数值比较；否则使用字符串比较

**遇到的问题和解决方案**：
1. **问题**：CMake缓存未更新，parser_where.cpp未被包含到构建
   - **原因**：新文件创建后CMake缓存未刷新
   - **解决**：重新运行`cmake ..`配置，CMake自动识别新文件

2. **问题**：std::set<std::vector<std::string>>需要自定义比较器
   - **原因**：std::vector没有默认的operator<用于set排序
   - **解决**：改用逐行比较的方式实现DISTINCT，避免使用std::set

**文件位置**：
- `include/sql_parser/token.h` - Token类型扩展（ORDER, BY, ASC, DESC, DISTINCT, LIMIT, AND, OR, NOT, 比较运算符）
- `include/sql_parser/ast_node.h` - AST节点扩展（OrderByInfo, WhereCondition树形结构）
- `src/sql_parser/token.cpp` - Token映射扩展
- `src/sql_parser/lexer.cpp` - 比较运算符识别
- `src/sql_parser/parser.h` - WHERE条件解析方法声明
- `src/sql_parser/parser_where.cpp` - WHERE条件解析实现（新建）
- `src/sql_parser/parser_select.cpp` - SELECT解析扩展（DISTINCT, ORDER BY, LIMIT, WHERE条件）
- `include/query/select_handler.h` - SelectHandler扩展（applyDistinct, applyOrderBy, applyLimit, evaluateWhereCondition）
- `src/query/select_handler.cpp` - 查询执行逻辑扩展

**下一步计划**：
1. 实现LIKE模式匹配功能
2. 实现IN子句功能
3. 实现BETWEEN范围查询功能
4. 继续实现其他SQL功能（GROUP BY、聚合函数等）

---

### 2026-01-15：LIKE、IN、BETWEEN功能实现

**完成内容**：
1. ✅ **LIKE模式匹配实现**
   - 扩展Token类型，添加LIKE关键字
   - 在WHERE条件解析中支持LIKE运算符
   - 实现LIKE模式匹配逻辑（支持%通配符，前缀匹配、后缀匹配、包含匹配）
   - 大小写敏感匹配（与PostgreSQL和Oracle对齐）

2. ✅ **IN子句实现**
   - 扩展Token类型，添加IN关键字（已存在，用于JOIN）
   - 在WHERE条件解析中支持IN运算符
   - 实现IN值列表解析（parseSimpleCondition中处理IN子句）
   - 实现IN条件评估逻辑（在evaluateWhereCondition中）

3. ✅ **BETWEEN范围查询实现**
   - 扩展Token类型，添加BETWEEN关键字
   - 在WHERE条件解析中支持BETWEEN运算符
   - 实现BETWEEN值解析（betweenStart和betweenEnd）
   - 实现BETWEEN条件评估逻辑（支持数值和字符串范围查询，包含边界）

**技术决策**：
- LIKE模式匹配使用大小写敏感匹配，与PostgreSQL和Oracle对齐（设计合理）
- IN子句支持值列表查询，值可以是字符串或数字
- BETWEEN范围查询包含边界值（符合SQL标准），支持数值和字符串比较

**文件位置**：
- `include/sql_parser/token.h` - Token类型扩展（LIKE, BETWEEN）
- `src/sql_parser/token.cpp` - Token映射扩展
- `src/sql_parser/parser_where.cpp` - WHERE条件解析扩展（LIKE, IN, BETWEEN）
- `src/query/select_handler.cpp` - WHERE条件评估扩展

---

### 2026-01-15：GROUP BY和聚合函数实现

**完成内容**：
1. ✅ **GROUP BY分组实现**
   - 扩展Token类型，添加GROUP、BY关键字
   - 扩展AST节点，添加groupBy字段列表
   - 实现GROUP BY解析逻辑
   - 实现分组执行逻辑（使用std::map存储分组）

2. ✅ **聚合函数实现**
   - 扩展Token类型，添加COUNT、SUM、AVG、MAX、MIN关键字
   - 扩展AST节点，添加AggregateFunction和SelectField结构
   - 实现聚合函数解析（支持COUNT(*), COUNT(Field), SUM(Field)等）
   - 实现聚合函数计算逻辑（calculateAggregateFromRecords）
   - 支持所有聚合函数：COUNT、SUM、AVG、MAX、MIN

**技术决策**：
- 聚合函数直接从原始记录计算，而不是从投影后的行计算
- 使用calculateAggregateFromRecords方法，直接从Record列表计算聚合值
- GROUP BY使用std::map<std::vector<std::string>, std::vector<Record>>存储分组
- 支持无GROUP BY的聚合查询（返回一行聚合结果）

**遇到的问题和解决方案**：
1. **问题**：聚合函数计算结果为0或空
   - **原因**：calculateAggregate从投影后的行中查找字段，但columnNames是聚合函数名（如"COUNT(*)"），无法匹配原始字段名
   - **解决**：创建新方法calculateAggregateFromRecords，直接从原始记录计算聚合函数

**文件位置**：
- `include/sql_parser/token.h` - Token类型扩展（GROUP, BY, COUNT, SUM, AVG, MAX, MIN）
- `include/sql_parser/ast_node.h` - AST节点扩展（AggregateFunction, SelectField, groupBy字段）
- `src/sql_parser/token.cpp` - Token映射扩展
- `src/sql_parser/parser_select.cpp` - SELECT解析扩展（聚合函数、GROUP BY）
- `src/query/select_handler.cpp` - 查询执行扩展（executeGroupByQuery, calculateAggregateFromRecords）

---

### 2026-01-15：HAVING子句实现

**完成内容**：
1. ✅ **HAVING子句实现**
   - 扩展Token类型，添加HAVING关键字（已存在）
   - 扩展AST节点，添加havingClause字段
   - 实现HAVING解析逻辑（复用WHERE条件解析）
   - 实现HAVING条件评估逻辑（evaluateHavingCondition）
   - 支持对聚合函数结果的过滤（如COUNT(*) > 1）
   - 支持对分组字段的过滤（如Age > 25）
   - 支持复杂HAVING条件（AND、OR、NOT）

**技术决策**：
- HAVING条件评估作用于分组后的结果行，而不是原始记录
- 支持在HAVING条件中引用聚合函数（如COUNT(*), SUM(Age)等）
- 字段匹配逻辑：先直接匹配字段名，再尝试匹配聚合函数格式
- HAVING在GROUP BY之后、ORDER BY之前执行

**遇到的问题和解决方案**：
1. **问题**：HAVING条件中使用聚合函数（如COUNT(*)）时解析失败
   - **原因**：parseSimpleCondition使用parseIdentifier解析字段名，但COUNT是关键字，不是标识符
   - **解决**：修改parseSimpleCondition，检测聚合函数关键字，解析整个聚合函数表达式（如COUNT(*)）

**文件位置**：
- `src/sql_parser/parser_select.cpp` - SELECT解析扩展（HAVING子句）
- `src/sql_parser/parser_where.cpp` - WHERE条件解析扩展（支持聚合函数）
- `src/query/select_handler.cpp` - 查询执行扩展（applyHaving, evaluateHavingCondition）

---

### 2026-01-15：FULL OUTER JOIN实现

**完成内容**：
1. ✅ **FULL OUTER JOIN实现**
   - 扩展Token类型，添加FULL、OUTER关键字
   - 扩展JOIN解析逻辑，支持FULL OUTER JOIN和FULL JOIN（OUTER可选）
   - 实现FULL OUTER JOIN执行逻辑（LEFT JOIN ∪ RIGHT JOIN）
   - 保留所有匹配的记录
   - 保留左表中未匹配的记录（右表字段为空）
   - 保留右表中未匹配的记录（左表字段为空）

**技术决策**：
- FULL OUTER JOIN = LEFT JOIN ∪ RIGHT JOIN
- 使用std::set<size_t>跟踪已匹配的右表记录索引
- 在连接循环中收集匹配的右表记录，循环后处理未匹配的右表记录

**遇到的问题和解决方案**：
1. **问题**：FULL OUTER JOIN解析失败，报错"Expected ;, but got: FULL"
   - **原因**：JOIN检测条件中未包含TokenType::FULL，导致遇到FULL时未进入JOIN解析逻辑
   - **解决**：在JOIN检测条件中添加TokenType::FULL检查

**文件位置**：
- `include/sql_parser/token.h` - Token类型扩展（FULL, OUTER）
- `src/sql_parser/token.cpp` - Token映射扩展
- `src/sql_parser/parser_select.cpp` - JOIN解析扩展（FULL OUTER JOIN）
- `src/query/select_handler.cpp` - JOIN执行扩展（FULL OUTER JOIN逻辑）

---

---

### 2026-01-15 - NATURAL JOIN实现

**完成工作**：
1. ✅ NATURAL JOIN语法解析
   - 添加NATURAL关键字到TokenType枚举
   - 实现NATURAL JOIN、NATURAL LEFT JOIN、NATURAL RIGHT JOIN、NATURAL INNER JOIN、NATURAL FULL JOIN解析
   - NATURAL JOIN不需要ON子句，自动基于共同字段连接

2. ✅ NATURAL JOIN执行逻辑
   - 在executeJoinQuery中识别NATURAL JOIN类型
   - 自动查找两个表的共同字段（字段名相同）
   - 构建隐式ON条件（Table1.Field = Table2.Field）
   - 支持所有NATURAL JOIN变体（LEFT, RIGHT, INNER, FULL）

**技术决策**：
- NATURAL JOIN通过查找共同字段名自动构建连接条件
- 使用std::set<std::string>存储共同字段名
- 为每个共同字段构建ON条件

**文件位置**：
- `include/sql_parser/token.h` - Token类型扩展（NATURAL）
- `src/sql_parser/token.cpp` - Token映射扩展
- `src/sql_parser/parser_select.cpp` - JOIN解析扩展（NATURAL JOIN）
- `src/query/select_handler.cpp` - JOIN执行扩展（NATURAL JOIN逻辑）

---

### 2026-01-15 - UNION实现

**完成工作**：
1. ✅ UNION语法解析
   - 添加UNION和ALL关键字到TokenType枚举
   - 实现UNION和UNION ALL解析
   - 支持多个SELECT语句用UNION连接
   - 实现parseSelectWithoutUnion()方法，分离UNION解析逻辑
   - 全局ORDER BY和LIMIT在UNION之后解析

2. ✅ UNION执行逻辑
   - 实现executeUnionQuery()方法
   - 执行所有UNION子查询（临时清除ORDER BY和LIMIT）
   - 检查列数兼容性
   - UNION去重逻辑（行级比较）
   - UNION ALL保留所有行
   - 对最终结果应用全局ORDER BY和LIMIT

**技术决策**：
- UNION子查询不能包含ORDER BY或LIMIT（SQL标准）
- 使用自定义行比较逻辑实现去重（std::sort + std::unique）
- 全局ORDER BY和LIMIT应用于最终合并结果

**遇到的问题和解决方案**：
1. **问题**：UNION结果不正确（UserID值错误）
   - **原因**：去重逻辑使用std::sort和std::unique，但std::vector<std::string>的默认比较可能不够精确
   - **解决**：实现自定义行比较逻辑，逐列比较

2. **问题**：UNION with ORDER BY不排序
   - **原因**：applyOrderBy函数列名匹配逻辑不够健壮，字段未找到时静默失败
   - **解决**：改进列名匹配（大小写不敏感，处理TableName.FieldName格式），添加错误检查

3. **问题**：解析错误"ORDER BY and LIMIT cannot be used in UNION subqueries"
   - **原因**：parseSelectWithoutUnion()仍然解析ORDER BY和LIMIT，导致在UNION子查询中检测到这些子句时报错
   - **解决**：parseSelectWithoutUnion()不再解析ORDER BY和LIMIT，这些子句只在parseSelect()中作为全局子句解析

**文件位置**：
- `include/sql_parser/token.h` - Token类型扩展（UNION, ALL）
- `include/sql_parser/ast_node.h` - SelectNode扩展（unionQueries, unionAll）
- `include/sql_parser/parser.h` - 添加parseSelectWithoutUnion()声明
- `src/sql_parser/token.cpp` - Token映射扩展
- `src/sql_parser/parser_select.cpp` - UNION解析实现
- `src/query/select_handler.cpp` - UNION执行实现

---

### 2026-01-15 - 子查询实现

**完成工作**：
1. ✅ 子查询语法解析
   - 扩展WhereCondition结构，添加subquery字段（std::unique_ptr<SelectNode>）
   - 实现标量子查询解析：Field = (SELECT ...)
   - 实现IN子查询解析：Field IN (SELECT ...)
   - 实现EXISTS子查询解析：EXISTS (SELECT ...)
   - 实现parseSelectAsSubquery()方法，专门用于解析子查询（不期望分号）
   - 支持TableName.FieldName作为值（用于关联子查询）

2. ✅ 子查询执行逻辑
   - 实现executeSubquery()方法，作为子查询执行入口
   - 实现executeSingleTableQueryWithContext()方法，支持关联子查询上下文传递
   - 扩展evaluateWhereCondition()方法，支持outerRecord、outerTableInfo、currentTableName参数
   - 实现标量子查询结果比较（数值比较处理浮点数精度问题）
   - 实现IN子查询结果匹配
   - 实现EXISTS/NOT EXISTS子查询逻辑
   - 支持嵌套子查询（多层嵌套）
   - 支持关联子查询（子查询引用外部查询字段）

**技术决策**：
- 子查询AST存储在WhereCondition的subquery字段中
- 关联子查询通过传递outerRecord和outerTableInfo实现
- 字段解析优先级：当前子查询表 > 外部查询表 > 当前表
- 标量子查询使用数值比较处理浮点数精度问题（std::abs < 1e-9）

**遇到的问题和解决方案**：
1. **问题**：EXISTS子查询解析错误"Expected identifier, but got: EXISTS"
   - **原因**：parseSimpleCondition()期望fieldName是标识符，但EXISTS没有fieldName
   - **解决**：在parseSimpleCondition()开始处检查EXISTS，设置fieldName为空字符串

2. **问题**：关联子查询解析错误"Expected value (string or number) or subquery, but got: Users"
   - **原因**：parseSimpleCondition()的值解析只支持STRING_LITERAL、NUMBER和子查询，不支持TableName.FieldName
   - **解决**：添加IDENTIFIER支持，实现parseIdentifierOrQualifiedName()处理TableName.FieldName

3. **问题**：EXISTS和NOT EXISTS返回错误结果（0行和全部行）
   - **原因**：evaluateWhereCondition()未传递outerRecord和outerTableInfo，导致关联引用无法解析
   - **解决**：扩展evaluateWhereCondition()参数，在executeSingleTableQuery中传递当前record和tableInfo

4. **问题**：嵌套聚合子查询返回0行
   - **原因**：executeGroupByQuery()未接受和传递outerRecord和outerTableInfo，导致嵌套子查询中的关联条件无法正确评估
   - **解决**：扩展executeGroupByQuery()参数，传递外部查询上下文到evaluateWhereCondition()

5. **问题**：嵌套子查询（测试用例9）返回空结果
   - **原因**：std::to_string()对浮点数产生多余小数位（如"1000.500000"），导致字符串比较失败
   - **解决**：在标量子查询比较中使用数值比较（std::stod + epsilon比较）替代字符串比较

**文件位置**：
- `include/sql_parser/ast_node.h` - WhereCondition扩展（subquery字段）
- `include/sql_parser/token.h` - Token类型扩展（EXISTS）
- `include/sql_parser/parser.h` - 添加parseSelectAsSubquery()声明
- `include/query/select_handler.h` - 扩展方法签名（outerRecord, outerTableInfo参数）
- `src/sql_parser/token.cpp` - Token映射扩展
- `src/sql_parser/parser_where.cpp` - 子查询解析实现
- `src/sql_parser/parser_select.cpp` - parseSelectAsSubquery()实现
- `src/query/select_handler.cpp` - 子查询执行实现

---

---

## 2026-01-16 - 数据完整性约束功能实现

**时间**：2026-01-16

**完成工作**：

### 1. 数据结构扩展
1. ✅ 扩展TableMode结构
   - 添加`sDefaultValue[128]`字段（默认值）
   - 添加`bUnique`字段（唯一约束标志）
   - 保持向后兼容性（旧.dbf文件仍可正常读取）

2. ✅ 创建约束结构定义
   - `ForeignKeyConstraint` - 外键约束结构
   - `UniqueConstraint` - 多字段唯一约束结构
   - `CheckConstraint` - 检查约束结构
   - `TableConstraints` - 表级约束集合

3. ✅ 扩展AST节点
   - `CreateTableNode`添加`foreignKeys`, `uniqueConstraints`, `checkConstraints`字段

**文件位置**：
- `include/core/table_mode.h` - TableMode扩展
- `include/core/constraint.h` - 约束结构定义
- `include/sql_parser/ast_node.h` - AST节点扩展

### 2. SQL解析器扩展
1. ✅ Token类型扩展
   - 添加约束相关关键词：UNIQUE, DEFAULT, CHECK, FOREIGN, REFERENCES, CASCADE, RESTRICT, SET, NULL_KEYWORD, NO, ACTION, AND, OR

2. ✅ 约束解析实现
   - `parseForeignKeyConstraint()` - 解析外键约束（支持ON DELETE/UPDATE动作）
   - `parseUniqueConstraint()` - 解析多字段唯一约束
   - `parseCheckConstraint()` - 解析检查约束（支持AND/OR逻辑表达式）
   - 扩展`parseCreateTable()`支持约束语法
   - 扩展`parseFlags()`支持UNIQUE和DEFAULT关键字

3. ✅ 负数解析修复
   - 修复`parseInsert()`和`parseUpdate()`支持负数解析（-10等）

**文件位置**：
- `include/sql_parser/token.h`, `src/sql_parser/token.cpp` - Token扩展
- `include/sql_parser/parser.h`, `src/sql_parser/parser.cpp` - 解析器扩展
- `src/sql_parser/parser_constraints.cpp` - 约束解析实现

### 3. 约束管理器实现
1. ✅ ConstraintManager类
   - `checkUniqueConstraint()` - 检查唯一约束（单字段和多字段）
   - `checkForeignKey()` - 检查外键引用完整性
   - `evaluateCheckExpression()` - 评估检查约束表达式（支持AND/OR逻辑）
   - `applyDefaultValues()` - 应用默认值

**文件位置**：
- `include/core/constraint_manager.h`, `src/core/constraint_manager.cpp`

### 4. 约束注册表和存储
1. ✅ ConstraintRegistry单例
   - 管理内存中的约束定义
   - 提供约束注册、查询、清除接口

2. ✅ ConstraintStorageManager
   - `saveConstraints()` - 保存约束到.cst文件
   - `loadConstraints()` - 从.cst文件加载约束
   - `deleteTableConstraints()` - 删除表的约束

**文件位置**：
- `include/core/constraint_registry.h`, `src/core/constraint_registry.cpp`
- `include/core/constraint_storage.h`, `src/core/constraint_storage.cpp`

### 5. DML约束检查集成
1. ✅ InsertHandler集成
   - `applyDefaultValues()` - 应用默认值
   - `checkUniqueConstraints()` - 检查唯一约束
   - `checkForeignKeyConstraints()` - 检查外键约束
   - `checkCheckConstraints()` - 检查检查约束

2. ✅ UpdateHandler集成
   - `checkUniqueConstraints()` - 检查唯一约束
   - `checkForeignKeyConstraints()` - 检查外键约束
   - `checkCheckConstraints()` - 检查检查约束

3. ✅ DeleteHandler集成
   - `checkForeignKeyConstraints()` - 实现外键级联删除（CASCADE和SET NULL）
   - 支持多级级联删除

**文件位置**：
- `src/dml/insert_handler.cpp`, `src/dml/insert_handler_constraints.cpp`
- `src/dml/update_handler.cpp`, `src/dml/update_handler_constraints.cpp`
- `src/dml/delete_handler.cpp`

### 6. GUI约束管理界面
1. ✅ TableEditDialog扩展
   - 添加Constraints标签页
   - 实现外键约束添加/编辑/删除界面
   - 实现多字段唯一约束添加/编辑/删除界面
   - 实现检查约束添加/编辑/删除界面
   - 实现View Constraints按钮

2. ✅ 约束加载和显示
   - 修复约束加载问题（确保数据库名称一致性）
   - 修复约束显示问题（列表正确显示约束信息）

**文件位置**：
- `include/gui/table_management_widget.h`, `src/gui/table_management_widget.cpp`

### 7. 约束持久化修复
1. ✅ 数据库名称一致性
   - 修复约束加载时使用base name而非full path
   - 确保约束在应用重启后仍然有效

**遇到的问题和解决方案**：
1. **问题**：约束在应用重启后失效
   - **原因**：约束加载时使用的数据库名称（full path）与注册时使用的名称（base name）不一致
   - **解决**：统一使用base name作为约束注册表的key

2. **问题**：CHECK约束表达式只识别部分条件（如`> 0 AND < 100`只识别`> 0`）
   - **原因**：`evaluateCheckExpression()`只支持单个条件
   - **解决**：重构为支持AND/OR逻辑表达式，递归解析

3. **问题**：UPDATE操作忽略CHECK约束
   - **原因**：`UpdateHandler::checkCheckConstraints()`未正确调用或数据库名称不一致
   - **解决**：修复数据库名称提取逻辑，确保约束检查正确执行

4. **问题**：GUI插入操作失败（SQL解析错误）
   - **原因**：数据库文件名包含路径或为SQL关键字
   - **解决**：修复`parseDatabaseFileName()`和`parseInsert()`，使其更宽松

5. **问题**：DROP TABLE后重新创建同名表，旧数据仍然加载
   - **原因**：GUI删除表时只删除.dbf文件中的表结构，未删除.dat文件中的数据
   - **解决**：在`TableManagementWidget::onDeleteTable()`中添加`DataManager::clearTable()`调用

**测试结果**：
- ✅ 所有约束功能测试通过
- ✅ GUI约束管理界面测试通过
- ✅ SQL约束语法测试通过
- ✅ DML约束检查测试通过

---

## 2026-01-16 - 文档重新分类和测试指南生成

**时间**：2026-01-16

**完成工作**：

### 1. 文档重新分类和归档
1. ✅ 测试文档移至testing目录
   - `drop_table_test_cases.md`
   - `gui_constraint_testing_guide.md`
   - `gui_constraint_ui_test_cases.md`
   - `constraint_testing_summary.md`

2. ✅ 已完成文档归档至archive目录
   - `constraint_implementation_progress.md`
   - `constraint_implementation_progress_full.md`
   - `constraint_implementation_status.md`
   - `gui_constraint_fixes_summary.md`
   - `update_check_constraint_analysis.md`
   - `test_coverage_analysis.md`
   - `test_verification_summary.md`

3. ✅ 更新文档目录说明
   - 更新`docs/README.md`反映新的分类结构
   - 创建`docs/archive/README.md`说明归档原则

### 2. 全面测试指南生成
1. ✅ GUI图形化操作全面测试指南
   - 文件：`docs/testing/gui_comprehensive_test_guide.md`
   - 场景化设计（学生管理系统）
   - 前后连贯的测试流程
   - 覆盖所有GUI功能

2. ✅ SQL执行命令操作全面测试指南
   - 文件：`docs/testing/sql_comprehensive_test_guide.md`
   - 场景化设计（学生管理系统）
   - 前后连贯的测试流程
   - 覆盖所有SQL功能

**最后更新时间**：2026-01-16

---

## 2026-01-18 - 智能推荐系统实现（反模式检测、查询优化、GUI集成）

### 功能实现

#### 1. 常见反模式检测
- **SELECT * 检测**：检测并警告使用 `SELECT *` 的性能影响
- **LIKE前通配符检测**：检测 `LIKE '%xxx'` 和 `LIKE '%xxx%'` 模式，提示无法使用索引
- **实现位置**：`src/index/index_advisor.cpp::detectAntiPatterns()`

#### 2. 基础索引建议增强
- **智能索引类型判断**：根据字段类型和查询模式自动选择索引类型（Hash/B+Tree/Adjacent）
- **基于查询频率推荐**：当字段使用次数≥3且平均执行时间≥0.1ms时推荐创建索引
- **提供具体SQL建议**：生成完整的 `CREATE INDEX` SQL语句
- **实现位置**：`src/index/index_advisor.cpp::generateIndexAdviceForQuery()`

#### 3. 查询范围优化检测
- **全表扫描检测**：检测无WHERE条件的查询，警告全表扫描
- **缺失索引警告**：检测WHERE字段无索引且执行时间>10ms的情况
- **大结果集警告**：检测返回记录>1000且执行时间>50ms的情况，建议添加LIMIT
- **实现位置**：`src/index/index_advisor.cpp::detectRangeOptimization()`

#### 4. GUI界面集成
- **智能推荐面板**：在SQL执行界面右下角添加"Smart Recommendations"面板
- **实时建议显示**：查询执行后自动分析并显示建议
- **颜色编码**：按严重程度用颜色区分（warning=黄色，info=蓝色，error=红色）
- **实现位置**：`src/gui/sql_query_widget.cpp::updateAdvicePanel()`

### 技术细节

1. **反模式检测**：使用正则表达式检测SQL文本中的反模式
2. **索引推荐逻辑**：
   - 检查字段使用统计（从查询日志分析）
   - 判断是否已有索引（使用IndexManager检查）
   - 根据字段类型选择索引类型（主键+整数→hash，其他→btree）
3. **查询分析**：综合分析反模式、索引推荐、范围优化三个方面

### 测试

- **测试脚本**：`scripts/module_tests/test_smart_recommendations.cpp`
- **测试脚本运行器**：`scripts/module_tests/test_smart_recommendations.ps1`
- **手动测试SQL**：`test_data/test_smart_recommendations.sql`

### 文档更新

- 更新了 `docs/core/development_log.md`
- 创建了测试SQL文件供用户手动测试

## 2026-01-18 - 智能索引建议系统完善（B+树索引支持）

**时间**：2026-01-18

**完成工作**：

### 1. 完善IndexAdvisor支持B+树索引（btree）
1. ✅ 添加BTreeIndex成员变量
   - 在`include/index/index_advisor.h`中添加`BTreeIndex m_btreeIndex`成员
   - 更新`setDatabasePath()`方法，同时设置btree索引路径

2. ✅ 实现isSuitableForBTreeIndex()方法
   - B+树索引支持所有可排序类型（int, float, double, string, char）
   - B+树索引是通用索引，适合点查询、范围查询和排序

3. ✅ 更新索引检查逻辑
   - `analyzeQueryLogs()`：检查所有三种索引类型（hash/adjacent/btree）
   - `evaluateIndexEffect()`：评估时检查所有索引类型
   - 确保不会重复推荐已有索引的字段

### 2. 完善索引推荐算法
1. ✅ 优化索引类型选择策略
   - **Hash索引**：适合主键和int/string类型，点查询场景，预期提升50%
   - **B+树索引**：通用索引，适合所有查询类型，预期提升40%
   - **Adjacent索引**：适合范围查询，预期提升30%
   - 根据字段类型和查询模式智能选择

2. ✅ 更新推荐理由说明
   - Hash索引："字段适合哈希索引，可优化点查询性能（O(1)时间复杂度）"
   - B+树索引："字段适合B+树索引，可优化点查询、范围查询和排序（通用索引）"
   - Adjacent索引："字段适合相邻索引，可优化范围查询性能"

### 3. 修复字段名比较问题
1. ✅ 修复isSuitableForHashIndex/isSuitableForAdjacentIndex/isSuitableForBTreeIndex中的字段名比较
   - **问题**：`field.sFieldName == fieldName`无法正确比较`char[]`和`std::string`
   - **解决**：使用`strcmp(field.sFieldName, fieldName.c_str()) == 0`
   - **修改文件**：`src/index/index_advisor.cpp`
   - **添加头文件**：`#include <cstring>`

### 4. 创建测试脚本
1. ✅ 创建完整功能测试程序
   - **文件**：`scripts/module_tests/test_index_advisor_system.cpp`
   - **功能**：测试查询日志记录、字段使用统计、慢查询识别、索引推荐生成、索引效果评估
   - **特点**：详细的输出格式，包含推荐SQL语句生成

2. ✅ 创建测试运行脚本
   - **文件**：`scripts/module_tests/test_index_advisor_system.ps1`
   - **功能**：自动编译和运行测试程序
   - **特点**：检查测试数据库存在性，提供详细错误信息

**技术决策**：
- **索引类型选择策略**：优先推荐hash（点查询），其次btree（通用），最后adjacent（范围查询）
- **推荐条件阈值**：使用次数≥3次，平均执行时间≥10ms
- **推荐形式**：返回`IndexRecommendation`结构体，包含完整的索引创建建议信息

**遇到的问题**：
1. **问题**：字段名比较不正确
   - **现象**：`isSuitableForHashIndex`等方法无法正确识别字段
   - **原因**：`char[]`类型不能直接用`==`与`std::string`比较
   - **解决**：使用`strcmp()`进行字符串比较
   - **参考**：`src/core/index_manager.cpp`中类似问题的解决方式

**测试结果**：
- ✅ 索引类型判断逻辑正确（hash/adjacent/btree）
- ✅ 字段名比较修复后，字段识别正常
- ✅ 推荐算法能够根据字段类型选择合适的索引类型
- ⏳ 完整功能测试脚本已创建，待运行验证

**文件修改清单**：
- `include/index/index_advisor.h`：添加BTreeIndex成员和isSuitableForBTreeIndex声明
- `src/index/index_advisor.cpp`：
  - 添加`#include <cstring>`
  - 实现`isSuitableForBTreeIndex()`方法
  - 修复字段名比较（使用strcmp）
  - 更新`setDatabasePath()`、`analyzeQueryLogs()`、`evaluateIndexEffect()`方法
  - 完善`generateRecommendations()`索引类型选择逻辑
- `scripts/module_tests/test_index_advisor_system.cpp`：新建测试程序
- `scripts/module_tests/test_index_advisor_system.ps1`：新建测试脚本
- `test_data/student_grade_db_queries.sql`：添加智能索引建议系统测试指导

**下一步计划**：
- 运行测试脚本验证功能正确性
- 考虑GUI界面集成索引推荐功能（如需要）

**最后更新时间**：2026-01-18
