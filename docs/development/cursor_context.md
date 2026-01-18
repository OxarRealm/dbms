# 项目进度跟踪文档 (cursor_context.md)

> **说明**：本文档用于跟踪项目的最新进度和状态，当对话token达到限制需要开启新对话时，新对话可以参考此文档了解项目全貌。

---

## 项目基本信息

- **项目名称**：数据库管理系统 (Database Management System, DBMS)
- **课程名称**：数据库新技术实践
- **编程语言**：C++
- **GUI框架**：Qt (C++)
- **开发环境**：VSCode (Cursor)
- **项目定位**：通用数据库管理系统（类似SQL Server）
- **数据库新技术**：多种索引技术（相邻索引、哈希索引、B+树索引）、智能索引建议系统、查询优化
- **项目开始时间**：2026年1月
- **项目方向调整**：2026-01-14 - 从"智能音乐播放管理系统"调整为"通用数据库管理系统"

---

## 项目当前状态

### 整体进度
- **阶段**：阶段4（查询实现）已完成 ✅，阶段8（数据库新技术实现）全部完成 ✅，阶段5（GUI开发）进行中 ⏳，阶段7（核心通用DBMS功能）全部完成 ✅
  - 阶段8.1（相邻索引）已完成 ✅
  - 阶段8.2（哈希索引）已完成 ✅
  - 阶段8.3（智能索引建议系统）已完成 ✅
  - 阶段5.1（最小可运行程序）已完成 ✅
  - 阶段5.2（主界面布局）已完成 ✅
  - 阶段5.3（表结构管理界面）已完成 ✅
  - 阶段5.4（数据操作界面）已完成 ✅
  - 阶段5.5（SQL执行界面）已完成 ✅
  - 阶段7.0（SQL查询功能扩展）全部完成 ✅
    - ORDER BY、DISTINCT、LIMIT ✅
    - 比较运算符（>, <, >=, <=, !=）✅
    - 复杂WHERE条件（AND, OR, NOT）✅
    - LIKE模式匹配、IN子句、BETWEEN范围查询 ✅
    - GROUP BY分组、聚合函数（COUNT, SUM, AVG, MAX, MIN）✅
    - HAVING子句 ✅
    - FULL OUTER JOIN ✅
    - NATURAL JOIN（支持所有变体）✅
    - UNION和UNION ALL（支持多个UNION连接和全局ORDER BY/LIMIT）✅
    - 子查询（标量、IN、EXISTS/NOT EXISTS、关联、嵌套）✅
- **当前任务**：阶段5（GUI开发）- 索引管理界面和推荐系统界面待开发
- **完成度**：90% (阶段1-4和阶段8全部完成，754个测试全部通过；阶段5.1-5.5已完成；阶段7.0全部完成；约束功能全部完成；阶段5.6-5.7待开发)

### 2026-01-18 - 智能推荐系统实现

### 实现内容

1. **常见反模式检测**
   - SELECT * 警告
   - LIKE前通配符警告（`LIKE '%xxx'`, `LIKE '%xxx%'`）
   - 实现位置：`src/index/index_advisor.cpp::detectAntiPatterns()`

2. **基础索引建议增强**
   - 基于查询频率和执行时间智能判断索引类型
   - 当字段使用次数≥3且平均执行时间≥0.1ms时推荐创建索引
   - 根据字段类型自动选择索引类型（Hash/B+Tree/Adjacent）
   - 提供具体的CREATE INDEX SQL语句
   - 实现位置：`src/index/index_advisor.cpp::generateIndexAdviceForQuery()`

3. **查询范围优化检测**
   - 全表扫描检测（无WHERE条件）
   - 缺失索引警告（WHERE字段无索引且执行时间>10ms）
   - 大结果集警告（返回记录>1000且执行时间>50ms）
   - 实现位置：`src/index/index_advisor.cpp::detectRangeOptimization()`

4. **GUI界面集成**
   - 在SQL执行界面右下角添加"Smart Recommendations"面板
   - 实时显示查询建议（反模式、索引推荐、范围优化）
   - 颜色编码区分严重程度（warning/info/error）
   - 实现位置：`src/gui/sql_query_widget.cpp`

### 测试

- **自动测试脚本**：`scripts/module_tests/test_smart_recommendations.cpp`
- **测试运行器**：`scripts/module_tests/test_smart_recommendations.ps1`
- **手动测试SQL**：`test_data/test_smart_recommendations.sql`

### 使用方法

1. 在SQL Execution标签页执行查询
2. 查看右下角"Smart Recommendations"面板
3. 根据建议优化查询或创建索引

## 已完成工作

#### 项目初始化（已完成）
1. ✅ 项目技术栈确定（C++, Qt 5.15.2）
2. ✅ 项目文件夹结构创建（src, include, tests, scripts等完整结构）
3. ✅ 基础文档创建（.cursorrules, README, 开发日志等）
4. ✅ Git仓库初始化（.gitignore配置）
5. ✅ 开发环境配置完成（CMake 4.2.1, Qt 5.15.2, MSVC编译器）
6. ✅ CMake配置验证通过（Qt已正确找到并配置）
7. ✅ 测试脚本框架创建（unit_tests, module_tests, integration_tests等）
8. ✅ 文档结构重新组织（按core, development, project, technical, guides分类）
9. ✅ .cursorrules文档更新规范添加完成

#### 阶段1：核心数据结构与文件存储（已完成）
1. ✅ **任务1.1**：定义核心数据结构
   - TableMode结构体定义（字段结构）
   - TableInfo结构（表结构信息）
   - Record结构（记录数据）
   - TableData结构（表数据信息）
   - 常量定义（FIELD_NAME_LENGTH等）
   - 辅助函数实现
   - **测试结果**：36/36测试通过
   - **文件**：`include/core/table_mode.h`

2. ✅ **任务1.2**：实现.dbf文件读写
   - TableManager类实现
   - 表结构文件的读写操作
   - 多表存储管理（使用'~'分隔符）
   - 表的增删改查操作
   - **测试结果**：30/30测试通过
   - **文件**：`include/core/table_manager.h`, `src/core/table_manager.cpp`

3. ✅ **任务1.3**：实现.dat文件读写
   - DataManager类实现
   - 数据文件的读写操作
   - 多表数据存储管理
   - 记录有效性标识管理（软删除）
   - 记录的增删改查操作
   - **测试结果**：38/38测试通过
   - **文件**：`include/core/data_manager.h`, `src/core/data_manager.cpp`

4. ✅ **任务1.4**：基础文件I/O封装
   - FileManager类实现
   - 文件操作基础接口封装
   - 错误处理机制
   - 文件路径管理（跨平台支持）
   - **测试结果**：41/41测试通过
   - **文件**：`include/core/file_manager.h`, `src/core/file_manager.cpp`

**阶段1测试统计**：145个测试全部通过 ✅

#### 阶段2：DDL实现（已完成）
1. ✅ **任务2.1**：SQL解析器基础框架
   - Token定义和TokenType枚举
   - Lexer类实现（词法分析器）
   - Parser类实现（语法分析器，支持DDL语句）
   - AST节点结构定义（CreateTableNode, EditTableNode, RenameTableNode, DropTableNode）
   - **测试结果**：78/78测试通过
   - **文件**：`include/sql_parser/token.h`, `src/sql_parser/token.cpp`, `include/sql_parser/lexer.h`, `src/sql_parser/lexer.cpp`, `include/sql_parser/parser.h`, `src/sql_parser/parser.cpp`, `include/sql_parser/ast_node.h`, `src/sql_parser/ast_node.cpp`

2. ✅ **任务2.2**：CREATE TABLE实现
   - CreateTableHandler类实现
   - CREATE TABLE语句解析和执行
   - 表结构验证
   - 调用TableManager创建表
   - **测试结果**：24/24测试通过
   - **文件**：`include/ddl/create_table_handler.h`, `src/ddl/create_table_handler.cpp`

3. ✅ **任务2.3**：EDIT TABLE实现
   - EditTableHandler类实现
   - EDIT TABLE语句解析和执行
   - 支持添加新字段和修改现有字段
   - 字段定义验证
   - **测试结果**：21/21测试通过
   - **文件**：`include/ddl/edit_table_handler.h`, `src/ddl/edit_table_handler.cpp`

4. ✅ **任务2.4**：RENAME TABLE实现
   - RenameTableHandler类实现
   - RENAME TABLE语句解析和执行
   - 支持表名重命名（.dbf和.dat文件）
   - 支持数据迁移（重命名时保留记录）
   - **测试结果**：23/23测试通过
   - **文件**：`include/ddl/rename_table_handler.h`, `src/ddl/rename_table_handler.cpp`

5. ✅ **任务2.5**：DROP TABLE实现
   - DropTableHandler类实现
   - DROP TABLE语句解析和执行
   - 支持删除表结构（.dbf文件）和表数据（.dat文件）
   - **测试结果**：27/27测试通过
   - **文件**：`include/ddl/drop_table_handler.h`, `src/ddl/drop_table_handler.cpp`

6. ✅ **任务2.6**：DDL执行器整合
   - DDLExecutor类实现
   - 统一调度所有DDL语句
   - 自动识别SQL语句类型
   - 路由到对应的处理器
   - **测试结果**：38/38测试通过
   - **文件**：`include/ddl/ddl_executor.h`, `src/ddl/ddl_executor.cpp`

**阶段2测试统计**：211个测试全部通过 ✅

#### 阶段3：DML实现（已完成）
1. ✅ **任务3.1**：INSERT实现
   - InsertHandler类实现
   - INSERT语句解析和执行
   - 数据类型验证（int, float, double, char, string）
   - NULL值处理
   - 值数量验证
   - **测试结果**：43/43测试通过
   - **文件**：`include/dml/insert_handler.h`, `src/dml/insert_handler.cpp`

2. ✅ **任务3.2**：DELETE实现
   - DeleteHandler类实现
   - DELETE语句解析和执行
   - WHERE子句条件评估器
   - 记录删除逻辑（软删除，标记为无效）
   - 支持删除单条和多条记录
   - **测试结果**：38/38测试通过
   - **文件**：`include/dml/delete_handler.h`, `src/dml/delete_handler.cpp`

3. ✅ **任务3.3**：UPDATE实现
   - UpdateHandler类实现
   - UPDATE语句解析和执行
   - SET子句处理（更新字段值）
   - WHERE子句条件评估
   - 数据类型验证和转换
   - NULL值处理
   - 支持更新单条和多条记录
   - **测试结果**：43/43测试通过
   - **文件**：`include/dml/update_handler.h`, `src/dml/update_handler.cpp`

4. ✅ **任务3.4**：DML执行器整合
   - DMLExecutor类实现
   - 统一调度所有DML语句（INSERT/DELETE/UPDATE）
   - 自动识别SQL语句类型
   - 路由到对应的处理器
   - 统一的错误处理接口
   - getAffectedRows方法获取影响的记录数
   - **测试结果**：42/42测试通过
   - **文件**：`include/dml/dml_executor.h`, `src/dml/dml_executor.cpp`

**阶段3测试统计**：166个测试全部通过 ✅

#### 阶段4：查询实现（已完成）
1. ✅ **任务4.1**：SELECT单表查询
   - SelectHandler类实现
   - SELECT语句解析和执行
   - 投影操作（字段选择，支持*和指定字段）
   - WHERE子句条件过滤（支持等号比较）
   - QueryResult结果结构
   - **测试结果**：30/30测试通过
   - **文件**：`include/query/select_handler.h`, `src/query/select_handler.cpp`

2. ✅ **任务4.2**：SELECT多表查询
   - 扩展SelectHandler支持多表查询
   - 实现笛卡尔积计算（generateCartesianProduct）
   - 实现多表投影操作（projectMultiTableFields）
   - 实现多表WHERE条件评估（evaluateMultiTableCondition）
   - 支持简单字段名和TableName.FieldName格式
   - SELECT *自动生成TableName.FieldName格式的列名
   - **测试结果**：50/50测试通过
   - **文件**：`src/query/select_handler.cpp`（扩展）

3. ✅ **任务4.3**：SELECT连接查询（JOIN）
   - 在TokenType中添加JOIN相关关键词（JOIN, ON, INNER, LEFT, RIGHT）
   - 在AST节点中添加JoinInfo结构
   - 扩展Parser支持JOIN语法解析
   - 实现executeJoinQuery方法
   - 支持INNER JOIN（内连接）
   - 支持LEFT JOIN（左连接，包括NULL值处理）
   - 支持JOIN条件（ON子句中的等值连接）
   - 支持JOIN查询带WHERE条件
   - **测试结果**：57/57测试通过
   - **文件**：`include/sql_parser/token.h`, `src/sql_parser/token.cpp`, `include/sql_parser/ast_node.h`, `src/sql_parser/parser.cpp`, `src/query/select_handler.cpp`（扩展）

4. ✅ **任务4.4**：查询执行器整合
   - QueryExecutor统一执行器创建
   - SQL语句类型识别（DDL、DML、QUERY）
   - 语句路由（自动分发到相应执行器）
   - ExecutionResult结果结构
   - 查询结果格式化输出（表格格式）
   - 支持DDL、DML、查询语句统一执行
   - 错误处理和错误信息返回
   - **测试结果**：24/24测试通过
   - **文件**：`include/query/query_executor.h`, `src/query/query_executor.cpp`

**阶段4测试统计**：161个测试全部通过 ✅

#### 阶段8：数据库新技术实现（进行中）
1. ✅ **任务8.1**：相邻索引实现（已完成）
   - 实现相邻记录指针维护
   - 实现基于相邻索引的范围查询优化
   - 实现顺序扫描优化
   - 实现索引构建、更新、删除功能
   - **完成时间**：2026-01-14
   - **测试结果**：28/28测试通过
   - **文件**：`include/core/adjacent_index.h`, `src/core/adjacent_index.cpp`
   - **测试文件**：`scripts/unit_tests/index/test_adjacent_index.cpp`, `scripts/unit_tests/index/run_test_adjacent_index.ps1`

2. ✅ **任务8.2**：哈希索引实现（已完成）
   - 实现哈希表索引结构（链地址法解决冲突）
   - 实现主键哈希索引
   - 实现基于哈希索引的点查询优化（O(1)平均时间复杂度）
   - 实现索引统计功能
   - **完成时间**：2026-01-14
   - **测试结果**：29/29测试通过
   - **文件**：`include/core/hash_index.h`, `src/core/hash_index.cpp`
   - **测试文件**：`scripts/unit_tests/index/test_hash_index.cpp`, `scripts/unit_tests/index/run_test_hash_index.ps1`

3. ✅ **任务8.3**：智能索引建议系统（已完成）
   - 实现查询日志记录和分析
   - 实现字段使用频率统计
   - 实现慢查询识别功能
   - 实现索引效果评估和推荐算法
   - 集成到QueryExecutor中，自动记录查询日志
   - **完成时间**：2026-01-14
   - **测试结果**：14/14测试通过
   - **文件**：`include/index/index_advisor.h`, `src/index/index_advisor.cpp`
   - **测试文件**：`scripts/unit_tests/index/test_index_advisor.cpp`, `scripts/unit_tests/index/run_test_index_advisor.ps1`

**阶段8.1实际完成时间**：2026-01-14（1天）
**阶段8.2实际完成时间**：2026-01-14（1天）
**阶段8.3实际完成时间**：2026-01-14（1天）
**阶段8总完成时间**：3天（全部完成）

#### 阶段5：Qt GUI界面开发（进行中）
1. ✅ **任务5.1**：最小可运行程序（已完成）
   - 创建主窗口类（MainWindow）
   - 实现应用程序入口（main.cpp）
   - 实现基础UI组件（欢迎标签、测试按钮）
   - 实现菜单栏（文件、帮助菜单）
   - 实现状态栏
   - 设置字体（英文使用Segoe UI）
   - 修复编译错误（parser_select.cpp重复定义、Qt MOC问题）
   - **完成时间**：2026-01-14
   - **文件**：`include/gui/main_window.h`, `src/gui/main_window.cpp`, `src/gui/main.cpp`
   - **状态**：GUI能够正常显示 ✅

2. ✅ **任务5.2**：主界面布局（已完成）
   - 实现标签页系统（QTabWidget）
   - 5个标签页：Table Management, Index Management, Data Operation, SQL Execution, Guide
   - 所有文字改为英文
   - 所有字体使用Segoe UI
   - 状态栏显示数据库名称和当前时间
   - 实现全局键盘快捷键（Ctrl+Q, F1, Ctrl+Tab等）
   - 窗口标题包含版本号（v0.6.1）
   - Guide标签页显示USER_GUIDE.md内容
   - **完成时间**：2026-01-14
   - **文件**：`include/gui/main_window.h`, `src/gui/main_window.cpp`
   - **状态**：主界面布局已完成 ✅

3. ✅ **任务5.3**：表结构管理界面（已完成）
   - 实现数据库管理功能（Create Database, Open Database）
   - 实现表列表显示（左侧列表）
   - 实现表信息显示（右侧详细信息面板）
   - 实现创建表功能（TableEditDialog）
   - 实现编辑表功能（支持字段修改）
   - 实现删除表功能（带确认对话框）
   - 实现刷新表列表功能
   - 修复界面显示问题（居中显示、按钮文本、关闭事件处理）
   - **完成时间**：2026-01-14
   - **文件**：`include/gui/table_management_widget.h`, `src/gui/table_management_widget.cpp`
   - **状态**：表结构管理界面已完成 ✅

4. ✅ **任务5.4**：数据操作界面（已完成）
   - 实现数据表格显示（QTableWidget）
   - 实现插入记录功能（RecordEditDialog）
   - 实现编辑记录功能
   - 实现删除记录功能（带确认对话框）
   - 实现表列表自动刷新（切换标签页时、创建新表后）
   - 修复表列表刷新问题（创建新表后自动刷新）
   - **完成时间**：2026-01-14
   - **文件**：`include/gui/data_operation_widget.h`, `src/gui/data_operation_widget.cpp`
   - **状态**：数据操作界面已完成 ✅
5. ✅ **任务5.5**：SQL执行界面（已完成）
   - 实现SQL语句输入和执行
   - 实现批量SQL语句执行（自动分割多条语句）
   - 实现结果展示（成功/失败消息框，查询结果表格）
   - 实现主键唯一性约束（INSERT和UPDATE）
   - 修复大小写不敏感问题（表名、关键字）
   - 修复CREATE TABLE char[length]语法支持
   - 修复JOIN查询TableName.FieldName语法支持
   - 修复UPDATE 0行受影响的消息提示
   - 修复DROP TABLE后数据未完全删除的问题
   - 修复RENAME TABLE后.dbf和.dat文件不同步的问题
   - 所有GUI消息框使用英文和Segoe UI字体
   - **完成时间**：2026-01-14
   - **文件**：`include/gui/sql_query_widget.h`, `src/gui/sql_query_widget.cpp`
   - **状态**：SQL执行界面已完成 ✅
6. ⏳ **任务5.6**：索引管理界面（待开发）
7. ⏳ **任务5.7**：推荐系统界面（待开发）

#### 约束功能实现（已完成）
1. ✅ **数据结构扩展**（已完成）
   - 扩展TableMode结构，添加sDefaultValue和bUnique字段
   - 创建constraint.h定义约束结构（ForeignKeyConstraint, UniqueConstraint, CheckConstraint）
   - 扩展CreateTableNode AST节点，添加约束字段
   - **完成时间**：2026-01-15

2. ✅ **SQL解析器扩展**（已完成）
   - 扩展TokenType枚举，添加约束相关关键词（UNIQUE, DEFAULT, CHECK, FOREIGN, REFERENCES, CASCADE, RESTRICT, SET, NULL_KEYWORD, NO, ACTION, AND, OR）
   - 实现parseForeignKeyConstraint()解析外键约束
   - 实现parseUniqueConstraint()解析多字段唯一约束
   - 实现parseCheckConstraint()解析检查约束（支持AND/OR逻辑表达式）
   - 扩展parseCreateTable()支持约束语法
   - 扩展parseFlags()支持UNIQUE和DEFAULT关键字
   - 修复parseInsert()和parseUpdate()支持负数解析
   - **完成时间**：2026-01-15
   - **文件**：`include/sql_parser/token.h`, `src/sql_parser/token.cpp`, `include/sql_parser/parser.h`, `src/sql_parser/parser.cpp`, `src/sql_parser/parser_constraints.cpp`

3. ✅ **约束管理器实现**（已完成）
   - 实现ConstraintManager类，提供约束检查接口
   - 实现checkUniqueConstraint()检查唯一约束（单字段和多字段）
   - 实现checkForeignKey()检查外键引用完整性
   - 实现evaluateCheckExpression()评估检查约束表达式（支持AND/OR逻辑）
   - 实现applyDefaultValues()应用默认值
   - **完成时间**：2026-01-15
   - **文件**：`include/core/constraint_manager.h`, `src/core/constraint_manager.cpp`

4. ✅ **约束注册表实现**（已完成）
   - 实现ConstraintRegistry单例，管理内存中的约束定义
   - 提供约束注册、查询、清除接口
   - **完成时间**：2026-01-15
   - **文件**：`include/core/constraint_registry.h`, `src/core/constraint_registry.cpp`

5. ✅ **约束存储管理器实现**（已完成）
   - 实现ConstraintStorageManager类，负责约束持久化
   - 实现saveConstraints()保存约束到.cst文件
   - 实现loadConstraints()从.cst文件加载约束
   - 实现deleteTableConstraints()删除表的约束
   - **完成时间**：2026-01-15
   - **文件**：`include/core/constraint_storage.h`, `src/core/constraint_storage.cpp`

6. ✅ **DML约束检查集成**（已完成）
   - InsertHandler集成约束检查（applyDefaultValues, checkUniqueConstraints, checkForeignKeyConstraints, checkCheckConstraints）
   - UpdateHandler集成约束检查（checkUniqueConstraints, checkForeignKeyConstraints, checkCheckConstraints）
   - DeleteHandler集成外键级联删除（checkForeignKeyConstraints，支持CASCADE和SET NULL）
   - 修复GUI插入和更新操作，使用SQL执行路径确保约束检查
   - **完成时间**：2026-01-15
   - **文件**：`src/dml/insert_handler.cpp`, `src/dml/insert_handler_constraints.cpp`, `src/dml/update_handler.cpp`, `src/dml/update_handler_constraints.cpp`, `src/dml/delete_handler.cpp`

7. ✅ **GUI约束管理界面**（已完成）
   - TableEditDialog扩展，添加Constraints标签页
   - 实现外键约束添加/编辑/删除界面
   - 实现多字段唯一约束添加/编辑/删除界面
   - 实现检查约束添加/编辑/删除界面
   - 实现View Constraints按钮，快速查看所有约束
   - 修复约束加载和显示问题
   - **完成时间**：2026-01-15
   - **文件**：`include/gui/table_management_widget.h`, `src/gui/table_management_widget.cpp`

8. ✅ **约束持久化**（已完成）
   - 修复约束加载时的数据库名称一致性（使用base name而非full path）
   - 确保约束在应用重启后仍然有效
   - **完成时间**：2026-01-15

**约束功能测试统计**：所有约束功能测试通过 ✅

### 待开始工作
1. ⏳ 阶段5：Qt GUI界面开发（5.1-5.3已完成，5.4-5.7待开发）
2. ⏳ 阶段6：AI智能推荐算法实现
3. ⏳ 阶段7：系统测试与优化

---

## 项目结构

```
database-design/
├── src/                    # 源代码目录
│   ├── core/              # 核心功能（文件操作、数据结构）
│   ├── ddl/               # DDL实现
│   ├── dml/               # DML实现
│   ├── query/             # 查询实现
│   ├── sql_parser/        # SQL解析
│   ├── index/             # 索引管理（相邻索引、哈希索引、智能索引建议）
│   └── gui/               # Qt界面
├── include/                # 头文件目录（与src结构对应）
├── resources/              # 资源文件
├── tests/                  # 测试代码
├── build/                  # 构建输出
├── examples/               # 示例文件
├── data/                   # 数据文件（.dbf, .dat）
├── docs/                   # 文档目录（已分类组织）
│   ├── core/              # 核心文档（开发日志、迭代记录、项目报告）
│   ├── development/       # 开发相关文档（进度跟踪、任务指南）
│   ├── project/           # 项目相关文档（架构设计、初始化总结）
│   ├── technical/         # 技术文档（技术分析、需求对齐、流程分析）
│   ├── guides/            # 指南文档（环境配置、初始化指南）
│   └── README.md          # 文档目录说明
├── .cursorrules            # Cursor项目规则
├── README.md               # 项目说明
└── .gitignore              # Git忽略文件
```

---

## 数据库设计（智能音乐播放管理系统）

### 核心数据表设计（计划）

#### 1. 用户表 (Users)
- 用户ID (UserID, int, KEY)
- 用户名 (UserName, char[50])
- 注册时间 (RegisterTime, char[20])

#### 2. 歌曲表 (Songs)
- 歌曲ID (SongID, int, KEY)
- 歌曲名 (SongName, char[100])
- 艺术家 (Artist, char[50])
- 风格 (Genre, char[30])
- 年代 (Year, int)
- 时长 (Duration, int) - 单位：秒

#### 3. 播放记录表 (PlayRecords)
- 记录ID (RecordID, int, KEY)
- 用户ID (UserID, int)
- 歌曲ID (SongID, int)
- 播放时间 (PlayTime, char[20])
- 播放次数 (PlayCount, int)

#### 4. 歌单表 (Playlists)
- 歌单ID (PlaylistID, int, KEY)
- 用户ID (UserID, int)
- 歌单名 (PlaylistName, char[100])
- 创建时间 (CreateTime, char[20])

#### 5. 歌单-歌曲关联表 (PlaylistSongs)
- 关联ID (RelationID, int, KEY)
- 歌单ID (PlaylistID, int)
- 歌曲ID (SongID, int)

---

## 数据库新技术设计

### 已实现的数据库新技术
1. **相邻索引（Adjacent Index）** ✅
   - 范围查询优化
   - 顺序扫描优化
   - 维护相邻记录指针

2. **哈希索引（Hash Index）** ✅
   - 点查询优化（O(1)平均时间复杂度）
   - 链地址法解决冲突
   - 支持多种数据类型

3. **智能索引建议系统（Index Advisor）** ✅
   - 查询日志记录和分析
   - 字段使用频率统计
   - 慢查询识别
   - 自动索引推荐

### 计划实现的数据库新技术
1. **B+树索引（B+Tree Index）** ⏳
   - 通用索引，支持排序和范围查询
   - 简化版B+树（2-3层）
   - 预计工作量：7-10天

2. **查询执行计划优化** ⏳
   - 连接顺序优化
   - 谓词下推
   - EXPLAIN语句
   - 预计工作量：5-7天

3. **查询缓存机制** ⏳
   - LRU缓存策略
   - 缓存失效机制
   - 预计工作量：2-3天

---

## 关键技术决策

1. **文件存储格式**
   - .dbf文件：存储表结构（使用分隔符'~'）
   - .dat文件：存储记录数据（使用分隔符'~'）
   - 存储方式：先使用变长方式，后续可优化

2. **SQL语法**
   - 自定义SQL语法（参考课程要求格式）
   - 支持DDL、DML、查询语句

3. **Qt版本**
   - 建议使用Qt 5.15+ 或 Qt 6.x
   - 使用Qt Designer设计界面

4. **C++标准**
   - 使用C++14或C++17

5. **数据库新技术选择（2026-01-12决策）**
   - **选择方案A**：索引技术 + 智能推荐
   - **技术组合**：
     - 相邻索引（Adjacent Index）- 3-5天
     - 哈希索引（Hash Index）- 4-6天
     - 智能索引建议系统（Index Advisor）- 5-7天
     - B+树索引（计划中，第二阶段）
   - **总工作量**：12-18天（数据库新技术部分）
   - **选择理由**：
     - 技术覆盖全面（索引、优化、AI）
     - 实现难度适中，工作量可控
     - 展示效果极佳，适合课程演示
     - 技术含量高，体现数据库系统理解
     - 符合课程要求（数据库新技术 + AI融合）

---

## 开发计划（初步）

### 第一阶段：核心数据结构与文件存储 ✅（已完成）
- ✅ 定义TableMode结构
- ✅ 实现.dbf文件读写
- ✅ 实现.dat文件读写
- ✅ 基础文件I/O封装
- ✅ 基础测试（145个测试全部通过）

### 第二阶段：DDL实现 ✅（已完成）
- ✅ SQL解析器（词法分析、语法分析、AST设计）
- ✅ CREATE TABLE实现
- ✅ EDIT TABLE实现
- ✅ RENAME TABLE实现
- ✅ DROP TABLE实现
- ✅ DDL执行器整合
- ✅ 基础测试（211个测试全部通过）

### 第三阶段：DML实现 ✅（已完成）
- ✅ INSERT实现
- ✅ DELETE实现
- ✅ UPDATE实现
- ✅ DML执行器整合
- ✅ 基础测试（166个测试全部通过）

### 第四阶段：查询实现 ✅（已完成）
- ✅ SELECT单表查询（30测试通过）
- ✅ SELECT多表查询（50测试通过）
- ✅ SELECT连接查询（57测试通过）
- ✅ 查询执行器整合（24测试通过）
- ✅ 基础测试（161个测试全部通过）

### 第五阶段：GUI开发（进行中）
- ✅ Qt界面框架搭建
- ✅ 表管理界面
- ✅ 数据操作界面
- ⏳ SQL执行界面
- ⏳ 索引管理界面

### 第六阶段：核心通用DBMS功能（第一阶段）
- ✅ 完善SQL查询功能（ORDER BY, GROUP BY, DISTINCT, LIMIT等）✅
- ✅ 完善约束管理（FOREIGN KEY, UNIQUE, CHECK, DEFAULT）✅
- ⏳ 索引SQL接口（CREATE INDEX, DROP INDEX）
- ✅ SQL执行界面 ✅

### 第七阶段：高级DBMS功能（第二阶段）
- ⏳ 事务管理（BEGIN/COMMIT/ROLLBACK）
- ⏳ 视图功能（CREATE/ALTER/DROP VIEW）
- ⏳ 数据库管理（DROP DATABASE, 备份恢复）
- ⏳ 数据导入导出（CSV, SQL脚本）
- ⏳ 用户和权限管理（必须）
- ⏳ B+树索引实现（强烈推荐）

### 第八阶段：扩展功能（第三阶段）
- ⏳ 存储过程和函数
- ⏳ 触发器
- ⏳ 查询优化器（完整版）
- ⏳ 性能监控界面
- ⏳ 查询缓存机制（可选）

### 第九阶段：测试与优化
- 单元测试
- 集成测试
- 性能优化
- 文档完善

---

## 当前遇到的问题

- 相邻索引实现过程中遇到的问题：
  - PowerShell脚本编码问题（已解决，改用英文输出）
  - 测试文件include路径问题（已解决，更新为相对路径）
  - 缺少头文件`#include <iostream>`（已解决）
  - 测试脚本文件重复（已解决，删除根目录下的重复文件）

- 哈希索引实现过程中遇到的问题：
  - 无（哈希索引实现顺利，所有测试通过）

- 智能索引建议系统实现过程中遇到的问题：
  - 测试中查询执行失败导致日志未记录（已解决，修改为无论成功与否都记录日志）
  - 测试脚本需要包含所有依赖的源文件（已解决，更新测试脚本包含所有必要的源文件）

---

## 下一步行动

1. 开始阶段4：查询实现
   - SELECT单表查询实现
   - SELECT多表查询实现
   - SELECT连接查询实现
   - 查询执行器整合

---

## 重要链接和参考

- Qt文档：https://doc.qt.io/
- C++参考：https://en.cppreference.com/
- 课程参考书籍：《数据库管理系统内部结构及其C语言实现》

---

## 更新记录

- **2026-01-12**：项目初始化，创建项目结构和基础文档
- **2026-01-12**：环境配置完成（CMake, Qt, MSVC），文档结构重新组织，准备开始开发
- **2026-01-12**：阶段1全部完成
  - 任务1.1：定义核心数据结构（36测试通过）
  - 任务1.2：实现.dbf文件读写（30测试通过）
  - 任务1.3：实现.dat文件读写（38测试通过）
  - 任务1.4：基础文件I/O封装（41测试通过）
  - 总计：145个测试全部通过
- **2026-01-12**：阶段2全部完成
  - 任务2.1：SQL解析器基础框架（78测试通过）
  - 任务2.2：CREATE TABLE实现（24测试通过）
  - 任务2.3：EDIT TABLE实现（21测试通过）
  - 任务2.4：RENAME TABLE实现（23测试通过）
  - 任务2.5：DROP TABLE实现（27测试通过）
  - 任务2.6：DDL执行器整合（38测试通过）
  - 总计：211个测试全部通过
- **2026-01-12**：阶段3全部完成
  - 任务3.1：INSERT实现（43测试通过）
  - 任务3.2：DELETE实现（38测试通过）
  - 任务3.3：UPDATE实现（43测试通过）
  - 任务3.4：DML执行器整合（42测试通过）
  - 总计：166个测试全部通过
- **2026-01-12**：阶段4全部完成
  - 任务4.1：SELECT单表查询（30测试通过）
  - 任务4.2：SELECT多表查询（50测试通过）
  - 任务4.3：SELECT连接查询（57测试通过）
  - 任务4.4：查询执行器整合（24测试通过）
  - 总计：161个测试全部通过
- **2026-01-12**：数据库新技术方案确定
  - 选择方案A：索引技术 + 智能推荐
  - 技术组合：相邻索引 + 哈希索引 + 智能索引建议系统 + B+树索引（计划中）
  - 预计工作量：12-18天（数据库新技术部分）
  - 详细分析文档：`docs/technical/database_new_technology_selection.md`
- **2026-01-14**：阶段8.1（相邻索引实现）完成
  - 任务8.1：相邻索引实现（28测试通过）
  - 测试脚本分类整理完成（按模块组织：core, ddl, dml, query, sql_parser, index）
  - 修复测试脚本路径和编码问题
  - 总计：28个测试全部通过
  - **文件**：`include/core/adjacent_index.h`, `src/core/adjacent_index.cpp`
  - **测试文件**：`scripts/unit_tests/index/test_adjacent_index.cpp`, `scripts/unit_tests/index/run_test_adjacent_index.ps1`
- **2026-01-14**：阶段8.2（哈希索引实现）完成
  - 任务8.2：哈希索引实现（29测试通过）
  - 实现哈希表结构（链地址法解决冲突）
  - 实现多种类型的哈希函数（int, float/double, string）
  - 实现点查询优化（O(1)平均时间复杂度）
  - 实现索引统计功能
  - 总计：29个测试全部通过
  - **文件**：`include/core/hash_index.h`, `src/core/hash_index.cpp`
  - **测试文件**：`scripts/unit_tests/index/test_hash_index.cpp`, `scripts/unit_tests/index/run_test_hash_index.ps1`
- **2026-01-14**：阶段8.3（智能索引建议系统）完成
  - 任务8.3：智能索引建议系统（14测试通过）

- **2026-01-18**：智能索引建议系统完善（B+树索引支持）
  - 完善IndexAdvisor支持B+树索引（btree）检测和推荐
  - 修复字段名比较问题（使用strcmp）
  - 优化索引类型选择策略（hash/btree/adjacent优先级）
  - 创建完整功能测试脚本（test_index_advisor_system）
  - 实现查询日志记录功能（自动记录所有查询的执行信息）
  - 实现字段使用统计功能（统计字段在WHERE子句中的使用频率）
  - 实现慢查询识别功能（可配置阈值，识别执行时间较长的查询）
  - 实现索引推荐生成功能（基于使用频率和执行时间，自动推荐合适的索引）
  - 实现索引效果评估功能（评估现有索引的性能提升效果）
  - 集成到QueryExecutor中，自动记录查询日志
  - 总计：14个测试全部通过
  - **文件**：`include/index/index_advisor.h`, `src/index/index_advisor.cpp`
  - **测试文件**：`scripts/unit_tests/index/test_index_advisor.cpp`, `scripts/unit_tests/index/run_test_index_advisor.ps1`
- **2026-01-14**：阶段8（数据库新技术实现）全部完成
  - 阶段8.1：相邻索引（28测试通过）
  - 阶段8.2：哈希索引（29测试通过）
  - 阶段8.3：智能索引建议系统（14测试通过）
  - **总计**：71个测试全部通过
  - **完成时间**：3天（全部完成）
- **2026-01-14**：阶段5.1（GUI最小可运行程序）完成
  - 创建主窗口类（MainWindow）和应用程序入口（main.cpp）
  - 实现基础UI组件（欢迎标签、测试按钮）
  - 实现菜单栏（文件、帮助菜单）和状态栏
  - 设置字体（英文使用Segoe UI）
  - 修复编译错误（parser_select.cpp重复定义、Qt MOC问题、编码问题）
  - **状态**：GUI能够正常显示 ✅
  - **文件**：`include/gui/main_window.h`, `src/gui/main_window.cpp`, `src/gui/main.cpp`
  - **修复内容**：
    - 修复`parser_select.cpp`缺少头文件包含问题
    - 修复`parser.cpp`和`parser_select.cpp`中`parseSelect()`重复定义问题
    - 修复Qt MOC未处理`main_window.h`的问题（在CMakeLists.txt中显式添加）
    - 添加`/utf-8`编译选项解决编码问题
- **2026-01-14**：阶段5.2（主界面布局）完成
  - 实现标签页系统（QTabWidget）
  - 5个标签页：Table Management, Index Management, Data Operation, SQL Execution, Guide
  - 所有文字改为英文，字体使用Segoe UI
  - 状态栏显示数据库名称和当前时间
  - 实现全局键盘快捷键
  - 窗口标题包含版本号（v0.6.1）
  - Guide标签页显示USER_GUIDE.md内容
  - **完成时间**：2026-01-14
  - **状态**：主界面布局已完成 ✅
- **2026-01-14**：阶段5.3（表结构管理界面）完成
  - 实现数据库管理功能（Create Database, Open Database）
  - 实现表列表显示和表信息显示
  - 实现创建、编辑、删除表功能
  - 实现刷新表列表功能
  - 修复界面显示问题
  - **完成时间**：2026-01-14
  - **状态**：表结构管理界面已完成 ✅
- **2026-01-14**：阶段5.4（数据操作界面）完成
  - 实现数据表格显示（QTableWidget，显示所有有效记录）
  - 实现插入记录功能（RecordEditDialog，字段输入表单，数据验证）
  - 实现编辑记录功能（选择记录后编辑，保存修改）
  - 实现删除记录功能（选择记录后删除，带确认对话框）
  - 实现表列表自动刷新（切换标签页时、创建新表后）
  - 修复表列表刷新问题（创建新表后Data Operation标签页自动刷新）
  - 修复编译错误（TableInfo未定义问题，需要包含完整定义而非前向声明）
  - **完成时间**：2026-01-14
  - **文件**：`include/gui/data_operation_widget.h`, `src/gui/data_operation_widget.cpp`
  - **状态**：数据操作界面已完成 ✅
  - **修复内容**：
    - 修复`data_operation_widget.h`中`TableInfo`未定义问题（改为包含`core/table_mode.h`）
    - 修复表列表刷新问题（在`onTabChanged`和`setCurrentDatabase`中添加刷新逻辑）
    - 将`loadTableList()`设为public方法，供外部调用
- **2026-01-14**：阶段5.5（SQL执行界面）完成
  - 实现SQL语句输入和执行功能
  - 实现批量SQL语句执行（自动分割多条语句，支持分号分隔）
  - 实现结果展示（成功/失败消息框，查询结果表格）
  - 实现主键唯一性约束检查（INSERT和UPDATE操作，包括GUI和SQL执行）
  - 修复大小写不敏感问题（表名、关键字，与主流DBMS对齐）
  - 修复CREATE TABLE char[length]和char(length)语法支持
  - 修复JOIN查询TableName.FieldName语法支持（添加DOT token）
  - 修复UPDATE 0行受影响的消息提示（更符合标准SQL行为）
  - 修复DROP TABLE后数据未完全删除的问题（clearTable硬删除逻辑）
  - 修复RENAME TABLE后.dbf和.dat文件不同步的问题（调整执行顺序）
  - 所有GUI消息框使用英文和Segoe UI字体
  - 完善SQL执行测试用例（添加JOIN查询测试用例和未实现功能测试用例）
  - **完成时间**：2026-01-14
  - **文件**：`include/gui/sql_query_widget.h`, `src/gui/sql_query_widget.cpp`
  - **测试文档**：`docs/testing/sql_execution_test_cases.md`
  - **状态**：SQL执行界面已完成 ✅
  - **修复内容**：
    - 修复`clearTable`函数，确保硬删除所有大小写变体的数据
    - 修复`readAllRecords`函数，移除对.dbf文件的检查，直接从.dat文件读取（支持RENAME TABLE场景）
    - 修复`dropTable`函数，检查`clearTable`的返回值
    - 修复`renameTable`函数，调整执行顺序：先迁移数据，再更新.dbf文件
    - 移动`test_sql_parser.cpp`到`scripts/unit_tests/sql_parser/`目录

---

**最后更新时间**：2026-01-15
