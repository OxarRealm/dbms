# 版本迭代记录

> 记录项目的版本迭代历史

---

## 版本规划

### v0.1.0 - 项目初始化阶段（当前）
- [x] 项目结构搭建
- [x] 基础文档创建
- [x] 技术栈确定
- [x] 开发环境配置（CMake, Qt, MSVC）
- [x] CMake配置验证
- [x] 测试脚本框架创建
- [x] 文档结构重新组织

### v0.2.0 - 核心数据结构设计阶段 ✅（已完成）
- [x] 定义TableMode结构
- [x] 设计文件存储格式
- [x] 实现基础的文件I/O接口
- [x] 实现.dbf文件读写
- [x] 实现.dat文件读写

### v0.3.0 - DDL实现阶段 ✅（已完成）
- [x] SQL解析器基础框架
- [x] CREATE TABLE实现
- [x] EDIT TABLE实现
- [x] RENAME TABLE实现
- [x] DROP TABLE实现
- [x] DDL执行器整合

### v0.4.0 - DML实现阶段 ✅（已完成）
- [x] INSERT实现
- [x] DELETE实现
- [x] UPDATE实现
- [x] DML执行器整合

### v0.5.0 - 查询实现阶段 ✅（已完成）
- [x] SELECT单表查询（30测试通过）
- [x] SELECT多表查询（50测试通过）
- [x] SELECT连接查询（57测试通过）
- [x] 查询执行器整合（24测试通过）
- **完成时间**：2026-01-12
- **测试统计**：161个测试全部通过

### v0.6.0 - GUI开发阶段（进行中）
- [x] Qt界面框架搭建 ✅
- [x] 主窗口类实现 ✅
- [x] 应用程序入口实现 ✅
- [x] 基础UI组件（标签、按钮）✅
- [x] 菜单栏和状态栏 ✅
- [x] 字体设置（Segoe UI）✅
- [x] 编译问题修复 ✅
- [x] 主界面布局（标签页系统）✅
- [x] 表管理界面 ✅
- [x] 数据操作界面 ✅
- [x] SQL执行界面 ✅
- [ ] 索引管理界面
- [ ] 推荐系统界面
- **完成时间**：2026-01-14（v0.6.3：SQL执行界面已完成，包括批量执行、主键约束、大小写不敏感、DROP/RENAME TABLE修复）

### v0.7.0 - 核心通用DBMS功能阶段（进行中）
- [x] 完善SQL查询功能（ORDER BY, DISTINCT, LIMIT）✅
- [x] 比较运算符（>, <, >=, <=, !=）✅
- [x] 复杂WHERE条件（AND, OR, NOT）✅
- [ ] LIKE模式匹配（待实现）
- [ ] IN子句（待实现）
- [ ] BETWEEN范围查询（待实现）
- [ ] GROUP BY分组（待实现）
- [ ] 聚合函数（COUNT, SUM, AVG, MAX, MIN）（待实现）
- [ ] HAVING子句（待实现）
- [ ] 完善约束管理（FOREIGN KEY, UNIQUE, CHECK, DEFAULT）
- [ ] 索引SQL接口（CREATE INDEX, DROP INDEX）
- **完成时间**：2026-01-15（部分完成：ORDER BY, DISTINCT, LIMIT, 比较运算符, 复杂WHERE条件）

### v0.8.0 - 数据库新技术实现阶段（已完成）✅
- [x] 相邻索引实现（28测试通过）✅
- [x] 哈希索引实现（29测试通过）✅
- [x] 智能索引建议系统（14测试通过）✅
- [ ] 索引性能测试和优化（可选）
- **技术方案**：方案A（索引技术 + 智能推荐）
- **完成时间**：2026-01-14（全部完成）
- **实际完成时间**：3天（相邻索引1天，哈希索引1天，智能索引建议系统1天）

### v1.0.0 - 完整系统（计划中）
- [ ] 系统集成测试
- [ ] 性能优化
- [ ] 文档完善
- [ ] 课程报告编写

---

## 版本历史

### v0.1.0 - 项目初始化

**发布日期**：2026-01-12

**更新内容**：
- 初始化项目结构（src, include, tests, scripts等完整结构）
- 创建基础文档（.cursorrules, README, 开发日志等）
- 确定技术栈和项目定位（C++, Qt 5.15.2, 通用数据库管理系统）
- 配置开发环境（CMake 4.2.1, Qt 5.15.2, MSVC编译器）
- 创建测试脚本框架（unit_tests, module_tests等）
- 重新组织文档结构（按类型分类）

**主要文件**：
- 创建项目文件夹结构
- 创建.cursorrules、README.md等基础文档
- 配置CMakeLists.txt
- 创建测试脚本框架
- 组织docs文件夹结构

**环境配置**：
- CMake 4.2.1
- Qt 5.15.2 (conda环境)
- MSVC编译器 (Visual Studio 2022 BuildTools)
- Git 2.51.1

**开发者**：项目组

---

### v0.2.0 - 核心数据结构与文件存储

**发布日期**：2026-01-12

**更新内容**：
- 完成核心数据结构定义（TableMode, TableInfo, Record, TableData）
- 实现.dbf文件读写功能（TableManager类）
- 实现.dat文件读写功能（DataManager类）
- 实现基础文件I/O封装（FileManager类）
- 编写并运行所有功能的测试脚本（145个测试全部通过）

**主要文件**：
- `include/core/table_mode.h` - 核心数据结构定义
- `include/core/table_manager.h`, `src/core/table_manager.cpp` - 表结构管理器
- `include/core/data_manager.h`, `src/core/data_manager.cpp` - 数据记录管理器
- `include/core/file_manager.h`, `src/core/file_manager.cpp` - 文件管理器
- `scripts/unit_tests/test_table_mode.cpp` - TableMode测试
- `scripts/unit_tests/test_table_manager.cpp` - TableManager测试
- `scripts/unit_tests/test_data_manager.cpp` - DataManager测试
- `scripts/unit_tests/test_file_manager.cpp` - FileManager测试

**功能特性**：
- 支持多表存储（使用'~'分隔符）
- 支持记录的软删除（标记为无效）
- 支持跨平台文件操作（Windows和Unix/Linux）
- 完整的错误处理机制

**测试结果**：
- 任务1.1：36/36测试通过
- 任务1.2：30/30测试通过
- 任务1.3：38/38测试通过
- 任务1.4：41/41测试通过
- **总计：145/145测试通过**

**开发者**：项目组

---


### v0.3.0 - DDL实现

**发布日期**：2026-01-12

**更新内容**：
- 完成SQL解析器基础框架（词法分析器、语法分析器、AST节点结构）
- 实现CREATE TABLE语句解析和执行
- 实现EDIT TABLE语句解析和执行
- 实现RENAME TABLE语句解析和执行
- 实现DROP TABLE语句解析和执行
- 实现DDL执行器统一调度
- 编写并运行所有功能的测试脚本（211个测试全部通过）

**主要文件**：
- `include/sql_parser/token.h`, `src/sql_parser/token.cpp` - Token定义
- `include/sql_parser/lexer.h`, `src/sql_parser/lexer.cpp` - 词法分析器
- `include/sql_parser/parser.h`, `src/sql_parser/parser.cpp` - 语法分析器
- `include/sql_parser/ast_node.h`, `src/sql_parser/ast_node.cpp` - AST节点
- `include/ddl/create_table_handler.h`, `src/ddl/create_table_handler.cpp` - CREATE TABLE处理器
- `include/ddl/edit_table_handler.h`, `src/ddl/edit_table_handler.cpp` - EDIT TABLE处理器
- `include/ddl/rename_table_handler.h`, `src/ddl/rename_table_handler.cpp` - RENAME TABLE处理器
- `include/ddl/drop_table_handler.h`, `src/ddl/drop_table_handler.cpp` - DROP TABLE处理器
- `include/ddl/ddl_executor.h`, `src/ddl/ddl_executor.cpp` - DDL执行器
- `scripts/unit_tests/test_lexer.cpp` - Lexer测试
- `scripts/unit_tests/test_parser.cpp` - Parser测试
- `scripts/unit_tests/test_create_table_handler.cpp` - CREATE TABLE测试
- `scripts/unit_tests/test_edit_table_handler.cpp` - EDIT TABLE测试
- `scripts/unit_tests/test_rename_table_handler.cpp` - RENAME TABLE测试
- `scripts/unit_tests/test_drop_table_handler.cpp` - DROP TABLE测试
- `scripts/unit_tests/test_ddl_executor.cpp` - DDL执行器测试

**功能特性**：
- 完整的SQL词法分析和语法分析
- 支持所有DDL语句（CREATE, EDIT, RENAME, DROP TABLE）
- 统一的DDL执行器接口
- 完整的错误处理和验证机制
- AST节点设计支持扩展

**测试结果**：
- 任务2.1：78/78测试通过
- 任务2.2：24/24测试通过
- 任务2.3：21/21测试通过
- 任务2.4：23/23测试通过
- 任务2.5：27/27测试通过
- 任务2.6：38/38测试通过
- **总计：211/211测试通过**

**开发者**：项目组

---

### v0.4.0 - DML实现

**发布日期**：2026-01-12

**更新内容**：
- 完成INSERT语句解析和执行
- 完成DELETE语句解析和执行（WHERE子句条件评估）
- 完成UPDATE语句解析和执行（SET和WHERE子句）
- 完成DML执行器统一调度
- 编写并运行所有功能的测试脚本（166个测试全部通过）

**主要文件**：
- `include/dml/insert_handler.h`, `src/dml/insert_handler.cpp` - INSERT处理器
- `include/dml/delete_handler.h`, `src/dml/delete_handler.cpp` - DELETE处理器
- `include/dml/update_handler.h`, `src/dml/update_handler.cpp` - UPDATE处理器
- `include/dml/dml_executor.h`, `src/dml/dml_executor.cpp` - DML执行器
- `src/sql_parser/parser.cpp` - 扩展Parser支持DML语句解析
- `scripts/unit_tests/test_insert_handler.cpp` - INSERT测试
- `scripts/unit_tests/test_delete_handler.cpp` - DELETE测试
- `scripts/unit_tests/test_update_handler.cpp` - UPDATE测试
- `scripts/unit_tests/test_dml_executor.cpp` - DML执行器测试

**功能特性**：
- 完整的DML语句支持（INSERT、DELETE、UPDATE）
- WHERE子句条件评估（等号比较）
- SET子句处理（UPDATE）
- 数据类型验证和转换
- NULL值处理
- 软删除机制（DELETE）
- 批量操作支持
- 统一的DML执行器接口

**测试结果**：
- 任务3.1：43/43测试通过
- 任务3.2：38/38测试通过
- 任务3.3：43/43测试通过
- 任务3.4：42/42测试通过
- **总计：166/166测试通过**

**开发者**：项目组

---

### v0.5.0 - 查询实现

**发布日期**：2026-01-12

**更新内容**：
- 完成SELECT单表查询实现（投影操作、WHERE子句条件过滤）
- 完成SELECT多表查询实现（笛卡尔积、多表投影、多表WHERE条件）
- 完成SELECT连接查询实现（INNER JOIN、LEFT JOIN、JOIN条件评估）
- 完成查询执行器统一调度（QueryExecutor、结果格式化）
- 扩展SQL解析器支持SELECT和JOIN语法
- 编写并运行所有功能的测试脚本（161个测试全部通过）

**主要文件**：
- `include/query/select_handler.h`, `src/query/select_handler.cpp` - SELECT查询处理器
- `include/query/query_executor.h`, `src/query/query_executor.cpp` - 查询执行器
- `include/sql_parser/ast_node.h` - 扩展SelectNode和JoinInfo结构
- `src/sql_parser/parser.cpp` - 扩展Parser支持SELECT和JOIN语法解析
- `include/sql_parser/token.h`, `src/sql_parser/token.cpp` - 添加JOIN相关关键词
- `scripts/unit_tests/test_select_handler.cpp` - SELECT测试
- `scripts/unit_tests/test_query_executor.cpp` - QueryExecutor测试

**功能特性**：
- 完整的SELECT语句支持（单表、多表、JOIN）
- 投影操作（字段选择，支持*和指定字段）
- WHERE子句条件过滤（等号比较）
- 笛卡尔积计算（多表查询）
- JOIN连接查询（INNER JOIN、LEFT JOIN）
- JOIN条件评估（ON子句中的等值连接）
- 查询结果格式化输出（表格格式）
- 统一的查询执行器接口（QueryExecutor）
- 自动语句类型识别和路由

**测试结果**：
- 任务4.1：30/30测试通过
- 任务4.2：50/50测试通过
- 任务4.3：57/57测试通过
- 任务4.4：24/24测试通过
- **总计：161/161测试通过**

**开发者**：项目组

---

### v0.8.0 - 数据库新技术实现（相邻索引）

**发布日期**：2026-01-14

**更新内容**：
- 完成相邻索引（Adjacent Index）实现
- 实现索引构建、更新、删除功能
- 实现基于相邻索引的范围查询优化
- 实现基于相邻索引的顺序扫描优化
- 测试脚本分类整理（按模块组织）
- 编写并运行所有功能的测试脚本（28个测试全部通过）

**主要文件**：
- `include/core/adjacent_index.h`, `src/core/adjacent_index.cpp` - 相邻索引实现
- `scripts/unit_tests/index/test_adjacent_index.cpp` - 相邻索引测试
- `scripts/unit_tests/index/run_test_adjacent_index.ps1` - 测试运行脚本
- `scripts/unit_tests/README.md` - 测试脚本分类说明

**功能特性**：
- 支持多字段索引（可为不同字段分别构建索引）
- 自动排序（构建索引时按字段值排序）
- 相邻关系维护（维护每个记录的上一个和下一个相邻记录指针）
- 范围查询优化（利用索引快速定位范围内的记录）
- 顺序扫描优化（按排序顺序遍历记录）
- 索引更新和删除支持

**测试结果**：
- 任务8.1：28/28测试通过
  - 索引构建测试：通过
  - 相邻关系测试：通过
  - 顺序扫描测试：通过（正确排序：1, 2, 5, 8, 9）
  - 范围查询测试：通过（正确返回：2, 5, 8）
  - 索引更新测试：通过
  - 索引删除测试：通过
  - 空表索引测试：通过

**开发者**：项目组

---

### v0.8.0 - 数据库新技术实现（相邻索引）

**发布日期**：2026-01-14

**更新内容**：
- 完成相邻索引（Adjacent Index）实现
- 实现索引构建、更新、删除功能
- 实现基于相邻索引的范围查询优化
- 实现基于相邻索引的顺序扫描优化
- 测试脚本分类整理（按模块组织）
- 编写并运行所有功能的测试脚本（28个测试全部通过）

**主要文件**：
- `include/core/adjacent_index.h`, `src/core/adjacent_index.cpp` - 相邻索引实现
- `scripts/unit_tests/index/test_adjacent_index.cpp` - 相邻索引测试
- `scripts/unit_tests/index/run_test_adjacent_index.ps1` - 测试运行脚本
- `scripts/unit_tests/README.md` - 测试脚本分类说明

**功能特性**：
- 支持多字段索引（可为不同字段分别构建索引）
- 自动排序（构建索引时按字段值排序）
- 相邻关系维护（维护每个记录的上一个和下一个相邻记录指针）
- 范围查询优化（利用索引快速定位范围内的记录）
- 顺序扫描优化（按排序顺序遍历记录）
- 索引更新和删除支持

**测试结果**：
- 任务8.1：28/28测试通过
  - 索引构建测试：通过
  - 相邻关系测试：通过
  - 顺序扫描测试：通过（正确排序：1, 2, 5, 8, 9）
  - 范围查询测试：通过（正确返回：2, 5, 8）
  - 索引更新测试：通过
  - 索引删除测试：通过
  - 空表索引测试：通过

**开发者**：项目组

---

### v0.8.0 - 数据库新技术实现（智能索引建议系统）

**发布日期**：2026-01-14

**更新内容**：
- 完成智能索引建议系统（Index Advisor）实现
- 实现查询日志记录功能（自动记录所有查询的执行信息）
- 实现字段使用统计功能（统计字段在WHERE子句中的使用频率）
- 实现慢查询识别功能（可配置阈值，识别执行时间较长的查询）
- 实现索引推荐生成功能（基于使用频率和执行时间，自动推荐合适的索引）
- 实现索引效果评估功能（评估现有索引的性能提升效果）
- 集成到QueryExecutor中，自动记录查询日志
- 编写并运行所有功能的测试脚本（14个测试全部通过）

**主要文件**：
- `include/index/index_advisor.h`, `src/index/index_advisor.cpp` - 智能索引建议系统实现
- `scripts/unit_tests/index/test_index_advisor.cpp` - 智能索引建议系统测试
- `scripts/unit_tests/index/run_test_index_advisor.ps1` - 测试运行脚本

**功能特性**：
- 查询日志记录（SQL、表名、WHERE字段、执行时间、结果数量）
- 字段使用统计（使用次数、总执行时间、平均执行时间、是否已有索引）
- 慢查询识别（可配置阈值，默认100ms，按执行时间降序排序）
- 智能索引推荐（基于使用频率和执行时间，自动判断索引类型）
- 索引效果评估（评估现有索引的性能提升效果）
- 日志限制（最多保留1000条日志，避免内存占用过大）

**测试结果**：
- 任务8.3：14/14测试通过
  - 查询日志记录测试：通过
  - 字段使用统计测试：通过
  - 慢查询识别测试：通过（识别出2个慢查询）
  - 索引推荐生成测试：通过
  - 索引效果评估测试：通过
  - 日志清除测试：通过
  - 字段统计获取测试：通过
  - 推荐优先级排序测试：通过

**开发者**：项目组

---

### v0.6.0 - GUI开发阶段

**发布日期**：2026-01-15

**更新内容**：
- 完成Qt界面框架搭建（主窗口类、应用程序入口、基础UI组件）
- 完成主界面布局（标签页系统、状态栏、全局快捷键）
- 完成数据库管理功能（Create Database, Open Database）
- 完成表结构管理界面（创建、编辑、删除表）
- 完成数据操作界面（插入、编辑、删除记录，查看记录）
- 完成SQL执行界面（SQL输入和执行，批量执行，主键约束，JOIN查询支持）
- 修复DROP TABLE后数据未完全删除的问题
- 修复RENAME TABLE后.dbf和.dat文件不同步的问题
- 完善SQL执行测试用例文档（添加JOIN查询测试用例）

**主要文件**：
- `include/gui/main_window.h`, `src/gui/main_window.cpp` - 主窗口实现
- `src/gui/main.cpp` - 应用程序入口
- `include/gui/table_management_widget.h`, `src/gui/table_management_widget.cpp` - 表管理界面
- `include/gui/data_operation_widget.h`, `src/gui/data_operation_widget.cpp` - 数据操作界面
- `include/gui/sql_query_widget.h`, `src/gui/sql_query_widget.cpp` - SQL执行界面
- `src/core/data_manager.cpp` - 数据管理器（修复clearTable和readAllRecords）
- `src/ddl/drop_table_handler.cpp` - DROP TABLE处理器（修复返回值检查）
- `src/ddl/rename_table_handler.cpp` - RENAME TABLE处理器（修复执行顺序）
- `docs/testing/sql_execution_test_cases.md` - SQL执行测试用例文档（完善）

**功能特性**：
- 完整的GUI界面（表管理、数据操作、SQL执行）
- 数据库管理功能（创建、打开数据库）
- 表结构管理（创建、编辑、删除表）
- 数据操作（插入、编辑、删除记录）
- SQL执行（支持DDL、DML、查询语句，批量执行）
- 主键唯一性约束（INSERT和UPDATE操作）
- 大小写不敏感支持（表名、关键字，与主流DBMS对齐）
- JOIN查询支持（INNER JOIN, LEFT JOIN, RIGHT JOIN）
- 所有GUI消息框使用英文和Segoe UI字体
- DROP TABLE硬删除修复（完全删除表结构和数据）
- RENAME TABLE文件同步修复（.dbf和.dat文件保持同步）

**测试结果**：
- GUI程序能够成功编译并运行 ✅
- 主窗口正常显示 ✅
- 表管理界面功能正常 ✅
- 数据操作界面功能正常 ✅
- SQL执行界面功能正常 ✅
- DROP TABLE和RENAME TABLE问题已修复 ✅

**开发者**：项目组

---

## 版本号规则

采用语义化版本号：主版本号.次版本号.修订号 (MAJOR.MINOR.PATCH)

- **MAJOR**：重大架构变更或功能重构
- **MINOR**：新功能添加（向后兼容）
- **PATCH**：Bug修复或小改进（向后兼容）

---

---

### v0.7.0 - SQL查询功能扩展

**发布日期**：2026-01-15

**更新内容**：
- 实现ORDER BY排序功能（单字段和多字段排序，ASC/DESC）
- 实现DISTINCT去重功能
- 实现LIMIT分页功能
- 实现比较运算符（>, <, >=, <=, !=）
- 实现复杂WHERE条件（AND, OR, NOT，支持括号优先级）
- 实现LIKE模式匹配（支持%通配符，前缀匹配、后缀匹配、包含匹配）
- 实现IN子句（支持值列表查询）
- 实现BETWEEN范围查询（支持数值和字符串范围查询，包含边界）
- 实现GROUP BY分组功能
- 实现聚合函数（COUNT, SUM, AVG, MAX, MIN）
- 实现HAVING子句（支持对聚合函数结果和分组字段的过滤）
- 实现FULL OUTER JOIN（全外连接）

**主要文件**：
- `include/sql_parser/token.h` - Token类型扩展（ORDER, BY, ASC, DESC, DISTINCT, LIMIT, AND, OR, NOT, LIKE, BETWEEN, GROUP, HAVING, COUNT, SUM, AVG, MAX, MIN, FULL, OUTER）
- `include/sql_parser/ast_node.h` - AST节点扩展（OrderByInfo, WhereCondition树形结构, AggregateFunction, SelectField, groupBy, havingClause）
- `src/sql_parser/token.cpp` - Token映射扩展
- `src/sql_parser/parser_where.cpp` - WHERE条件解析实现（新建，支持复杂条件和聚合函数）
- `src/sql_parser/parser_select.cpp` - SELECT解析扩展（DISTINCT, ORDER BY, LIMIT, GROUP BY, HAVING, FULL OUTER JOIN）
- `src/query/select_handler.cpp` - 查询执行扩展（applyDistinct, applyOrderBy, applyLimit, executeGroupByQuery, calculateAggregateFromRecords, applyHaving, evaluateHavingCondition, FULL OUTER JOIN逻辑）

**功能特性**：
- ORDER BY支持数值和字符串混合比较，自动识别数值类型
- DISTINCT使用逐行比较实现，避免使用std::set
- LIMIT在ORDER BY之后应用，确保返回排序后的前N条记录
- 复杂WHERE条件支持递归评估，优先级：NOT > AND > OR
- LIKE模式匹配大小写敏感（与PostgreSQL和Oracle对齐）
- IN子句支持值列表查询
- BETWEEN范围查询包含边界值（符合SQL标准）
- GROUP BY使用std::map存储分组，支持多字段分组
- 聚合函数直接从原始记录计算，支持COUNT(*), COUNT(Field), SUM, AVG, MAX, MIN
- HAVING子句支持对聚合函数结果和分组字段的过滤，支持复杂条件
- FULL OUTER JOIN = LEFT JOIN ∪ RIGHT JOIN，保留所有匹配和未匹配的记录

**技术亮点**：
- 使用递归下降解析器解析复杂WHERE条件，支持括号优先级
- 聚合函数计算直接从原始记录进行，避免投影后的数据丢失
- HAVING条件评估支持聚合函数引用，智能匹配字段名
- FULL OUTER JOIN使用std::set跟踪已匹配记录，高效处理未匹配记录

**测试结果**：
- 所有新增SQL功能测试全部通过 ✅
- ORDER BY、DISTINCT、LIMIT测试通过 ✅
- 比较运算符测试通过 ✅
- 复杂WHERE条件测试通过 ✅
- LIKE、IN、BETWEEN测试通过 ✅
- GROUP BY和聚合函数测试通过 ✅
- HAVING子句测试通过 ✅
- FULL OUTER JOIN测试通过 ✅

**开发者**：项目组

---

---

### v0.7.1 - NATURAL JOIN、UNION和子查询实现（2026-01-15）

**版本说明**：
- 实现NATURAL JOIN（自然连接）及其变体
- 实现UNION和UNION ALL（并集操作）
- 实现子查询（标量子查询、IN子查询、EXISTS子查询、关联子查询、嵌套子查询）

**主要功能**：
- NATURAL JOIN自动基于共同字段连接，支持NATURAL LEFT/RIGHT/INNER/FULL JOIN
- UNION支持多个SELECT语句合并，支持去重（UNION）和保留重复（UNION ALL）
- UNION支持全局ORDER BY和LIMIT
- 子查询支持标量比较、IN子查询、EXISTS/NOT EXISTS子查询
- 支持关联子查询（子查询引用外部查询字段）
- 支持嵌套子查询（多层嵌套）
- 修复浮点数比较精度问题（使用数值比较替代字符串比较）

**主要文件**：
- `include/sql_parser/token.h` - Token类型扩展（NATURAL, UNION, ALL, EXISTS）
- `include/sql_parser/ast_node.h` - AST节点扩展（unionQueries, unionAll, subquery字段）
- `include/sql_parser/parser.h` - 添加parseSelectWithoutUnion()和parseSelectAsSubquery()声明
- `include/query/select_handler.h` - 扩展方法签名（outerRecord, outerTableInfo参数）
- `src/sql_parser/token.cpp` - Token映射扩展
- `src/sql_parser/parser_select.cpp` - NATURAL JOIN和UNION解析实现
- `src/sql_parser/parser_where.cpp` - 子查询解析实现
- `src/query/select_handler.cpp` - NATURAL JOIN、UNION和子查询执行实现

**功能特性**：
- NATURAL JOIN自动查找共同字段并构建连接条件
- UNION子查询不能包含ORDER BY或LIMIT（SQL标准）
- UNION去重使用自定义行比较逻辑
- 子查询支持关联引用（TableName.FieldName格式）
- 标量子查询使用数值比较处理浮点数精度问题
- 支持多层嵌套子查询（包括聚合函数子查询）

**技术亮点**：
- 实现parseSelectWithoutUnion()分离UNION解析逻辑
- 实现parseSelectAsSubquery()专门用于子查询解析
- 关联子查询通过outerRecord和outerTableInfo参数传递上下文
- 字段解析优先级：当前子查询表 > 外部查询表 > 当前表
- 浮点数比较使用epsilon（1e-9）避免精度问题

**测试结果**：
- NATURAL JOIN测试全部通过 ✅
- UNION和UNION ALL测试全部通过 ✅
- 子查询测试全部通过 ✅（包括关联子查询和嵌套子查询）

**开发者**：项目组

---

**最后更新**：2026-01-15

**状态**：阶段1、阶段2、阶段3和阶段4（核心数据结构与文件存储、DDL实现、DML实现、查询实现）已完成，683个测试全部通过。阶段8（数据库新技术实现）全部完成，相邻索引实现已完成（28个测试通过），哈希索引实现已完成（29个测试通过），智能索引建议系统实现已完成（14个测试通过）。阶段5.1-5.5（GUI开发）已完成，包括主窗口、表管理界面、数据操作界面、SQL执行界面。阶段7.0全部完成（ORDER BY, DISTINCT, LIMIT, 比较运算符, 复杂WHERE条件, LIKE, IN, BETWEEN, GROUP BY, 聚合函数, HAVING, FULL OUTER JOIN, NATURAL JOIN, UNION, 子查询）。总计754个测试全部通过，所有SQL功能测试全部通过。

---