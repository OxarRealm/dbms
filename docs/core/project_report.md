# 《数据库新技术实践课程设计》实验报告

## 实验名称
《数据库新技术实践课程设计》

## 实验地点
信息大楼222

## 项目名称
数据库管理系统 (Database Management System, DBMS)

## 所使用的工具软件及环境
- **编程语言**：C++
- **GUI框架**：Qt (C++)
- **开发环境**：VSCode (Cursor)
- **构建工具**：CMake
- **版本控制**：Git
- **操作系统**：Windows 10

---

## 一、实验目的

应用软件课程设计要求基于C/C++、Java等程序设计语言，结合数据库技术、编译技术和文字处理技术设计一个DBMS系统，帮助学生巩固数据结构、算法、程序设计的基础知识。通过本课程设计，加强学生对数据库管理系统的内部机制的理解，学习DBMS的实现技术。

---

## 二、实验要求

### 1. 数据库表字段结构及记录在文件中的存储管理
要求设计表构造模式及其在数据库文件中的存储结构，最后在自选的软件平台上建立能创建、查看表构造模式的用户界面。先自定义表构造模式的结构，然后将其存放到自定义存储结构的数据库文件（.dbf文件）中，要求能在一个数据库文件中存放多张不同的表。

### 2. 数据库表结构的创建及修改
在实验要求1的基础上，能对数据库文件内容进行读写，增加新的字段；删除字段；修改字段名、字段类型等。

### 3. 数据库表记录的插入、删除、修改及查询
设计记录在数据文件中的存储结构形式，并在自选的软件平台上建立用户界面来添加新记录到数据文件中，并能查看数据文件中的记录内容。根据从要求1中数据库文件（.dbf文件）中抽取的有关表的构造模式信息，将记录存放到自设计存储格式的数据记录文件（.dat文件）中，要求能存放不同的多张表的记录内容，能查看不同表的记录内容。

### 4. SQL语句的实现
1) 数据定义语言（DDL）的实现。
2) 实现定义表模式结构，并能修改表结构的语句。
3) 要求实现：定义表（Create Table）; 修改表（Edit Table、Rename Table、Delete Table）；
4) 数据操纵语言（DML）实现。
5) 实现记录数据的插入、删除和修改。
6) 要求实现：数据的插入（Insert）、删除（Delete）、修改（Edit）。
7) 实现数据库单表、多表的选择、投影、连接查询。（要求实现：Select 条件查询）

### 5. 数据库与新技术相关的功能实现
实现人工智能及相关数据库新技术的融合、设计及实现

### 6. 界面设计
在自选的软件平台上建立用户界面。

---

## 三、实验报告内容

### 1. 系统需求分析

#### 1.1 功能需求
（待补充：详细描述系统功能需求）

#### 1.2 非功能需求
（待补充：性能、可用性等需求）

#### 1.3 数据库需求分析
（待补充：数据表设计需求）

### 2. 系统总体设计

#### 2.1 系统架构设计
（待补充：系统整体架构图、模块划分）

#### 2.2 数据库设计
（待补充：数据表结构设计）

#### 2.3 功能模块设计
（待补充：各功能模块设计）

### 3. 系统详细设计

#### 3.1 核心数据结构设计 ✅

**3.1.1 TableMode结构体定义**

TableMode结构体用于定义表的字段结构，包含以下字段：
- `sFieldName[32]`：字段名（最大长度32）
- `sType[8]`：字段类型（如"int", "char"等）
- `iSize`：字段大小（字节数）
- `bKey`：是否为KEY键（1=KEY, 0=NOT_KEY）
- `bNullFlag`：是否允许为空（1=NULL, 0=NO_NULL）
- `bValidFlag`：是否有效（1=VALID, 0=INVALID）

**3.1.2 表结构信息（TableInfo）**

TableInfo结构包含：
- `tableName[64]`：表名
- `fields`：字段列表（vector<TableMode>）

**3.1.3 记录数据结构（Record）**

Record结构用于存储单条记录：
- `values`：字段值列表（vector<string>）
- `validFlag`：有效标识（1=有效, 0=无效/已删除）

**3.1.4 文件存储格式**

- **.dbf文件**：存储表结构模式
  - 使用分隔符'~'分隔不同表
  - 每个表存储：表名、字段数量、字段定义列表
- **.dat文件**：存储表记录数据
  - 使用分隔符'~'分隔不同表的数据
  - 每个表存储：表名、记录数量、字段数量、有效标识数组、记录数据
  - 字段值采用变长存储（先存储长度，再存储内容）
- `validFlag`：记录有效性标识（支持软删除）

**3.1.4 常量定义**

- `FIELD_NAME_LENGTH = 32`：字段名最大长度
- `TYPE_NAME_LENGTH = 8`：类型名最大长度
- `TABLE_NAME_LENGTH = 64`：表名最大长度
- `TABLE_SEPARATOR = '~'`：表分隔符

**实现文件**：`include/core/table_mode.h`

#### 3.2 文件存储设计 ✅

**3.2.1 .dbf文件格式（表结构文件）**

.dbf文件用于存储表结构模式，格式如下：

```
表1:
  '~' (分隔符, 1字节)
  表名 (char[64], 固定长度)
  字段数量 (int, 4字节)
  字段1结构 (TableMode结构体)
  字段2结构 (TableMode结构体)
  ...
  字段n结构 (TableMode结构体)

表2:
  '~' (分隔符)
  ...
```

**3.2.2 .dat文件格式（数据文件）**

.dat文件用于存储表记录数据，格式如下：

```
表1数据:
  '~' (分隔符, 1字节)
  表名 (char[64], 固定长度)
  记录数量 (int, 4字节)
  字段数量 (int, 4字节)
  有效标识数组 (char数组，每个记录1字节)
  记录1字段1数据 (变长：长度int + 内容)
  记录1字段2数据
  ...
  记录1字段n数据
  记录2字段1数据
  ...
```

**特点**：
- 使用'~'分隔符分隔不同表
- 字段值采用变长存储（先存储长度，再存储内容）
- 支持记录的软删除（通过有效标识数组标记）

**实现文件**：
- `include/core/table_manager.h`, `src/core/table_manager.cpp` - .dbf文件读写
- `include/core/data_manager.h`, `src/core/data_manager.cpp` - .dat文件读写

#### 3.3 SQL解析器设计 ✅

**3.3.1 词法分析器（Lexer）**

Lexer类负责将SQL字符串转换为Token序列：
- 支持识别关键词（CREATE, TABLE, EDIT, RENAME, DROP等）
- 支持识别标识符（表名、字段名等）
- 支持识别字面量（字符串、数字）
- 支持识别运算符和分隔符（=, ,, ;, (, )等）
- 自动跳过空白字符

**实现文件**：`include/sql_parser/lexer.h`, `src/sql_parser/lexer.cpp`

**3.3.2 语法分析器（Parser）**

Parser类负责将Token序列转换为抽象语法树（AST）：
- 支持解析CREATE TABLE语句
- 支持解析EDIT TABLE语句
- 支持解析RENAME TABLE语句
- 支持解析DROP TABLE语句
- 使用递归下降解析方法

**实现文件**：`include/sql_parser/parser.h`, `src/sql_parser/parser.cpp`

**3.3.3 抽象语法树（AST）**

AST节点结构：
- `ASTNode` - 基类
- `CreateTableNode` - CREATE TABLE节点
- `EditTableNode` - EDIT TABLE节点
- `RenameTableNode` - RENAME TABLE节点
- `DropTableNode` - DROP TABLE节点
- 使用访问者模式（Visitor Pattern）处理节点

**实现文件**：`include/sql_parser/ast_node.h`, `src/sql_parser/ast_node.cpp`

#### 3.4 DDL实现设计 ✅

**3.4.1 CREATE TABLE实现**

CreateTableHandler类：
- 解析CREATE TABLE SQL语句
- 验证表结构（字段名、类型、标志等）
- 调用TableManager创建表
- 错误处理和验证

**实现文件**：`include/ddl/create_table_handler.h`, `src/ddl/create_table_handler.cpp`

**3.4.2 EDIT TABLE实现**

EditTableHandler类：
- 解析EDIT TABLE SQL语句
- 支持添加新字段（字段不存在时）
- 支持修改现有字段（字段存在时）
- 支持修改字段类型、KEY标志、NULL标志
- 调用TableManager更新表结构

**实现文件**：`include/ddl/edit_table_handler.h`, `src/ddl/edit_table_handler.cpp`

**3.4.3 RENAME TABLE实现**

RenameTableHandler类：
- 解析RENAME TABLE SQL语句
- 更新.dbf文件中的表名
- 更新.dat文件中的表名（数据迁移）
- 验证新旧表名

**实现文件**：`include/ddl/rename_table_handler.h`, `src/ddl/rename_table_handler.cpp`

**3.4.4 DROP TABLE实现**

DropTableHandler类：
- 解析DROP TABLE SQL语句
- 删除.dbf文件中的表结构
- 删除.dat文件中的表数据
- 验证表是否存在

**实现文件**：`include/ddl/drop_table_handler.h`, `src/ddl/drop_table_handler.cpp`

**3.4.5 DDL执行器**

DDLExecutor类：
- 统一调度所有DDL语句
- 自动识别SQL语句类型
- 路由到对应的处理器
- 提供统一的错误处理接口

**实现文件**：`include/ddl/ddl_executor.h`, `src/ddl/ddl_executor.cpp`

#### 3.5 DML实现设计 ✅

**3.5.1 INSERT实现**

InsertHandler类：
- 解析INSERT SQL语句
- 验证值数量和数据类型
- 验证NULL约束
- 转换值类型以匹配字段类型
- 调用DataManager插入记录

**实现文件**：`include/dml/insert_handler.h`, `src/dml/insert_handler.cpp`

**3.5.2 DELETE实现**

DeleteHandler类：
- 解析DELETE SQL语句
- 实现WHERE子句条件评估器（支持等号比较）
- 查找匹配的记录
- 调用DataManager删除记录（软删除，标记为无效）
- 支持批量删除多条匹配的记录

**实现文件**：`include/dml/delete_handler.h`, `src/dml/delete_handler.cpp`

**3.5.3 UPDATE实现**

UpdateHandler类：
- 解析UPDATE SQL语句
- 实现SET子句处理（更新字段值）
- 实现WHERE子句条件评估（支持等号比较）
- 验证更新值的数据类型和NULL约束
- 转换值类型以匹配字段类型
- 调用DataManager更新记录
- 支持批量更新多条匹配的记录

**实现文件**：`include/dml/update_handler.h`, `src/dml/update_handler.cpp`

**3.5.4 DML执行器**

DMLExecutor类：
- 统一调度所有DML语句
- 自动识别SQL语句类型
- 路由到对应的处理器
- 提供统一的错误处理接口
- getAffectedRows方法获取影响的记录数

**实现文件**：`include/dml/dml_executor.h`, `src/dml/dml_executor.cpp`

#### 3.6 查询实现设计 ✅

**3.6.1 SELECT单表查询实现**

SelectHandler类：
- 解析SELECT SQL语句（单表）
- 实现投影操作（字段选择，支持*和指定字段）
- 实现WHERE子句条件过滤（支持等号比较）
- 读取表结构和数据
- 返回QueryResult结果结构

**实现文件**：`include/query/select_handler.h`, `src/query/select_handler.cpp`

**3.6.2 SELECT多表查询实现**

扩展SelectHandler类：
- 扩展SELECT语法解析（多表，FROM Table1, Table2, ...）
- 实现笛卡尔积计算（generateCartesianProduct递归方法）
- 实现多表投影操作（projectMultiTableFields）
- 实现多表WHERE条件评估（evaluateMultiTableCondition）
- 实现多表字段索引查找（findMultiTableFieldIndex）
- SELECT *自动生成TableName.FieldName格式的列名
- 支持简单字段名和TableName.FieldName格式

**实现文件**：`src/query/select_handler.cpp`（扩展）

**3.6.3 SELECT连接查询（JOIN）实现**

扩展SQL解析器和SelectHandler：
- 在TokenType中添加JOIN相关关键词（JOIN, ON, INNER, LEFT, RIGHT, FULL, OUTER, NATURAL）
- 在AST节点中添加JoinInfo结构（连接类型、右表名、左表字段、右表字段、运算符）
- 扩展Parser支持JOIN语法解析（FROM Table1 JOIN Table2 ON ...）
- 实现executeJoinQuery方法
- 支持INNER JOIN（内连接，只返回匹配的记录）
- 支持LEFT JOIN（左连接，保留左表所有记录，右表无匹配时填充空值）
- 支持RIGHT JOIN（右连接，保留右表所有记录，左表无匹配时填充空值）
- 支持FULL OUTER JOIN（全外连接，保留所有匹配和未匹配的记录）
- 支持NATURAL JOIN（自然连接，自动基于共同字段连接，无需ON子句）
  - NATURAL JOIN、NATURAL LEFT JOIN、NATURAL RIGHT JOIN、NATURAL INNER JOIN、NATURAL FULL JOIN
- 支持JOIN条件评估（ON子句中的等值连接）
- 支持JOIN查询带WHERE条件过滤

**实现文件**：`include/sql_parser/token.h`, `src/sql_parser/token.cpp`, `include/sql_parser/ast_node.h`, `src/sql_parser/parser.cpp`, `src/query/select_handler.cpp`（扩展）

**3.6.4 SELECT高级查询功能实现** ✅（已完成）

扩展SQL解析器和SelectHandler：
- **ORDER BY排序**：支持单字段和多字段排序，ASC/DESC方向，数值和字符串混合比较
- **DISTINCT去重**：使用逐行比较实现，避免使用std::set
- **LIMIT分页**：在ORDER BY之后应用，确保返回排序后的前N条记录
- **比较运算符**：支持>, <, >=, <=, !=
- **复杂WHERE条件**：支持AND, OR, NOT逻辑运算符，支持括号优先级，使用递归下降解析器
- **LIKE模式匹配**：支持%通配符，前缀匹配、后缀匹配、包含匹配，大小写敏感
- **IN子句**：支持值列表查询和子查询
- **BETWEEN范围查询**：支持数值和字符串范围查询，包含边界值
- **GROUP BY分组**：支持单字段和多字段分组，使用std::map存储分组
- **聚合函数**：支持COUNT(*), COUNT(Field), SUM, AVG, MAX, MIN，直接从原始记录计算
- **HAVING子句**：支持对聚合函数结果和分组字段的过滤，支持复杂条件
- **UNION和UNION ALL**：
  - 支持UNION（去重）和UNION ALL（保留重复）
  - 支持多个SELECT语句用UNION连接
  - 支持全局ORDER BY和LIMIT（应用于最终合并结果）
  - UNION子查询不能包含ORDER BY或LIMIT（SQL标准）
- **子查询**：
  - 支持标量子查询（=, !=, >, <, >=, <=）
  - 支持IN子查询
  - 支持EXISTS/NOT EXISTS子查询
  - 支持关联子查询（子查询引用外部查询字段，通过outerRecord和outerTableInfo传递上下文）
  - 支持嵌套子查询（多层嵌套，包括聚合函数子查询）
  - 使用数值比较处理浮点数精度问题（std::abs < 1e-9）

**实现文件**：
- `include/sql_parser/token.h` - Token类型扩展（ORDER, BY, ASC, DESC, DISTINCT, LIMIT, AND, OR, NOT, LIKE, BETWEEN, GROUP, HAVING, COUNT, SUM, AVG, MAX, MIN, FULL, OUTER, NATURAL, UNION, ALL, EXISTS）
- `include/sql_parser/ast_node.h` - AST节点扩展（OrderByInfo, WhereCondition树形结构, AggregateFunction, SelectField, groupBy, havingClause, unionQueries, unionAll, subquery字段）
- `include/sql_parser/parser.h` - 添加parseSelectWithoutUnion()和parseSelectAsSubquery()声明
- `include/query/select_handler.h` - 扩展方法签名（outerRecord, outerTableInfo参数）
- `src/sql_parser/token.cpp` - Token映射扩展
- `src/sql_parser/parser_where.cpp` - WHERE条件解析实现（新建，支持复杂条件、聚合函数和子查询）
- `src/sql_parser/parser_select.cpp` - SELECT解析扩展（DISTINCT, ORDER BY, LIMIT, GROUP BY, HAVING, FULL OUTER JOIN, NATURAL JOIN, UNION）
- `src/query/select_handler.cpp` - 查询执行扩展（applyDistinct, applyOrderBy, applyLimit, executeGroupByQuery, calculateAggregateFromRecords, applyHaving, evaluateHavingCondition, FULL OUTER JOIN逻辑, NATURAL JOIN逻辑, executeUnionQuery, executeSubquery, executeSingleTableQueryWithContext）

**3.6.5 查询执行器整合**

QueryExecutor类：
- 统一调度所有SQL语句类型（DDL、DML、QUERY）
- SQL语句类型识别（identifyStatementType静态方法）
- 语句路由（自动分发到相应执行器）
- ExecutionResult结果结构（SUCCESS、ERROR、QUERY_RESULT）
- 查询结果格式化输出（formatQueryResult静态方法，表格格式）
- 支持DDL语句执行（CREATE、DROP等）
- 支持DML语句执行（INSERT、UPDATE、DELETE，包括affectedRows）
- 支持查询语句执行（SELECT）
- 统一的错误处理和错误信息返回

**实现文件**：`include/query/query_executor.h`, `src/query/query_executor.cpp`

#### 3.7 数据库新技术设计 🔄（进行中）

**3.7.1 相邻索引（Adjacent Index）设计** ✅（已完成）

AdjacentIndex类：
- 维护相邻记录的物理位置映射
- 支持有序字段的相邻记录指针
- 实现基于相邻索引的范围查询优化
- 实现基于相邻索引的顺序扫描优化
- 实现索引构建和更新机制
- **完成时间**：2026-01-14
- **测试结果**：28/28测试通过

**实现文件**：`include/core/adjacent_index.h`, `src/core/adjacent_index.cpp`

**3.7.2 哈希索引（Hash Index）设计** ✅（已完成）

HashIndex类：
- 实现哈希表结构（链地址法解决冲突）
- 支持主键哈希索引
- 实现基于哈希索引的点查询优化（O(1)平均时间复杂度）
- 实现索引构建和更新机制
- 实现索引统计功能
- **完成时间**：2026-01-14
- **测试结果**：29/29测试通过

**实现文件**：`include/core/hash_index.h`, `src/core/hash_index.cpp`

**3.7.3 智能索引建议系统（Index Advisor）设计**

IndexAdvisor类：
- 查询日志记录和分析
- 字段使用频率统计
- 慢查询识别
- 索引效果评估算法
- 索引推荐算法
- 推荐结果展示（GUI集成）

**实现文件**：`include/index/index_advisor.h`, `src/index/index_advisor.cpp`

**技术方案**：方案A（索引技术 + 智能推荐）
- ✅ 相邻索引：已完成（2026-01-14，28测试通过）
- ✅ 哈希索引：已完成（2026-01-14，29测试通过）
- ✅ 智能索引建议系统：已完成（2026-01-14，14测试通过）
- **已完成工作量**：3天（相邻索引1天，哈希索引1天，智能索引建议系统1天）
- **测试总计**：71个测试全部通过

**选择理由**：
- 技术覆盖全面（索引、优化、AI）
- 实现难度适中，工作量可控
- 展示效果极佳，适合课程演示
- 技术含量高，体现数据库系统理解
- 符合课程要求（数据库新技术 + AI融合）

#### 3.8 事务管理设计（计划中）
- **职责**：事务管理和ACID特性
- **主要类**：
  - `TransactionManager` - 事务管理器
  - `LockManager` - 锁管理器
  - `LogManager` - 日志管理器（WAL）
- **实现状态**：计划在第二阶段实现

#### 3.9 视图管理设计（计划中）
- **职责**：视图定义和查询
- **主要类**：
  - `ViewManager` - 视图管理器
- **实现状态**：计划在第二阶段实现

#### 3.10 用户和权限管理设计（计划中）
- **职责**：用户认证和权限控制
- **主要类**：
  - `UserManager` - 用户管理器
  - `PermissionManager` - 权限管理器
- **实现状态**：计划在第二阶段实现（必须）

#### 3.9 GUI界面设计（进行中）

**3.9.1 主窗口框架** ✅（已完成，2026-01-14）
- 使用Qt 5.15.2的QMainWindow作为主窗口基类
- 实现MainWindow类（`include/gui/main_window.h`, `src/gui/main_window.cpp`）
- 实现应用程序入口（`src/gui/main.cpp`）
- 实现基础UI组件（欢迎标签、测试按钮）
- 实现菜单栏（文件菜单：退出；帮助菜单：关于）
- 实现状态栏（显示"就绪"状态）
- 设置窗口标题和初始大小（800x600）
- 设置字体（英文使用Segoe UI，符合项目规范）

**3.9.2 主界面布局** ✅（已完成，2026-01-14）
- 实现标签页系统（QTabWidget）
- 5个标签页：Table Management, Index Management, Data Operation, SQL Execution, Guide
- 所有文字改为英文，字体使用Segoe UI
- 状态栏显示数据库名称和当前时间（实时更新）
- 全局键盘快捷键（Ctrl+Q退出, F1关于, Ctrl+Tab切换标签页等）
- 窗口标题包含版本号（v0.6.1）
- Guide标签页显示USER_GUIDE.md内容（与文档同步）

**3.9.3 数据库管理功能** ✅（已完成，2026-01-14）
- 实现Create Database功能（File -> Create Database，Ctrl+N）
  - 使用QFileDialog选择保存位置和文件名
  - 自动创建.dbf和.dat文件（空文件）
  - 创建成功后自动加载数据库
- 实现Open Database功能（File -> Open Database，Ctrl+O）
  - 使用QFileDialog选择.dbf文件
  - 自动提取数据库路径并加载
  - 打开成功后更新状态栏和所有相关界面

**3.9.4 表结构管理界面** ✅（已完成，2026-01-14）
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

**3.9.5 数据操作界面** ✅（已完成，2026-01-14）
- 实现DataOperationWidget组件
  - 表选择下拉框（QComboBox）
  - 数据表格显示（QTableWidget，显示所有有效记录）
  - 操作按钮（Insert, Edit, Delete, Refresh）
  - 状态标签（显示表名和记录数）
- 实现RecordEditDialog对话框
  - 动态生成字段输入表单（根据表结构）
  - 字段标签显示KEY和必填标识（*）
  - 数据类型验证（int, float, double）
  - NULL约束检查
  - 关闭事件处理（确认对话框）
- 实现插入记录功能
  - 打开RecordEditDialog对话框
  - 数据验证和类型转换
  - 调用DataManager插入记录
  - 插入成功后自动刷新表格
- 实现编辑记录功能
  - 选择记录后打开编辑对话框
  - 加载选中记录的数据
  - 修改后保存（处理有效记录索引映射）
  - 更新成功后自动刷新表格
- 实现删除记录功能
  - 选择记录后显示确认对话框
  - 标记记录为无效（软删除）
  - 处理有效记录索引映射
  - 删除成功后自动刷新表格
- 实现表列表自动刷新
  - 切换标签页时自动刷新（onTabChanged）
  - 设置数据库路径时自动刷新（setCurrentDatabase）
  - 创建新表后自动刷新（通过标签页切换触发）

**3.9.6 SQL执行界面** ✅（已完成）
- SQL输入区域（QTextEdit，支持多行输入）
- SQL执行按钮（支持Ctrl+Enter快捷键）
- 批量SQL语句执行（自动分割多条语句，支持分号分隔）
- 结果显示区域（QTableWidget，查询结果表格）
- 成功/失败消息框（使用Segoe UI字体，英文文本）
- 主键唯一性约束检查（INSERT和UPDATE操作）
- 大小写不敏感支持（表名、关键字，与主流DBMS对齐）
- JOIN查询支持（INNER JOIN, LEFT JOIN, RIGHT JOIN）
- **完成时间**：2026-01-14
- **文件**：`include/gui/sql_query_widget.h`, `src/gui/sql_query_widget.cpp`
- **测试文档**：`docs/testing/sql_execution_test_cases.md`

**3.9.7 索引管理界面**（待开发）
- 索引列表显示
- 创建索引功能（支持HASH/ADJACENT/BTREE类型）
- 删除索引功能
- 索引建议显示（基于智能索引建议系统）
- 推荐查询输入
- 推荐结果展示

**3.9.8 索引管理界面**（待开发）
- 索引列表显示
- 索引创建和管理

**实现文件**：
- `include/gui/main_window.h` - 主窗口头文件
- `src/gui/main_window.cpp` - 主窗口实现
- `src/gui/main.cpp` - 应用程序入口

### 4. 系统实现和测试

#### 4.1 开发环境配置
- **编程语言**：C++17
- **GUI框架**：Qt 5.15.2
- **开发环境**：VSCode (Cursor)
- **构建工具**：CMake 4.2.1
- **编译器**：MSVC (Visual Studio 2022 BuildTools)
- **版本控制**：Git 2.51.1
- **Qt安装路径**：C:/Users/Oscar/anaconda3/Library（conda环境）

**环境配置验证**：
- ✅ CMake配置测试通过
- ✅ Qt 5.15.2已正确找到并配置
- ✅ 所有Qt组件（Core, Gui, Widgets）已配置
- ✅ 构建文件生成成功

#### 4.2 核心功能实现

**4.2.1 阶段1：核心数据结构与文件存储** ✅
- 完成核心数据结构定义（TableMode, TableInfo, Record, TableData）
- 实现.dbf文件读写功能（TableManager类）
- 实现.dat文件读写功能（DataManager类）
- 实现基础文件I/O封装（FileManager类）
- **测试结果**：145个测试全部通过

**4.2.2 阶段2：DDL实现** ✅
- 完成SQL解析器基础框架（词法分析器、语法分析器、AST节点结构）
- 实现CREATE TABLE、EDIT TABLE、RENAME TABLE、DROP TABLE语句
- 实现DDL执行器统一调度
- **测试结果**：211个测试全部通过

**4.2.3 阶段3：DML实现** ✅
- 完成INSERT、DELETE、UPDATE语句实现
- 实现WHERE子句条件评估
- 实现DML执行器统一调度
- **测试结果**：166个测试全部通过

**4.2.4 阶段4：查询实现** ✅
- 完成SELECT单表查询（投影操作、WHERE子句）
- 完成SELECT多表查询（笛卡尔积、多表投影）
- 完成SELECT连接查询（INNER JOIN、LEFT JOIN、RIGHT JOIN、FULL OUTER JOIN）
- 完成ORDER BY、DISTINCT、LIMIT功能
- 完成比较运算符和复杂WHERE条件
- 完成LIKE、IN、BETWEEN功能
- 完成GROUP BY、聚合函数、HAVING功能
- 完成查询执行器统一调度（QueryExecutor）
- **测试结果**：161个基础测试全部通过，新增SQL功能测试全部通过

**4.2.5 阶段8：数据库新技术实现** 🔄（进行中）
- ✅ **任务8.1：相邻索引实现**（完成时间：2026-01-14）
  - 实现相邻记录指针维护
  - 实现基于相邻索引的范围查询优化
  - 实现基于相邻索引的顺序扫描优化
  - 实现索引构建、更新、删除功能
  - **测试结果**：28/28测试通过
  - **文件**：`include/core/adjacent_index.h`, `src/core/adjacent_index.cpp`
- ✅ **任务8.2：哈希索引实现**（完成时间：2026-01-14）
  - 实现哈希表索引结构（链地址法解决冲突）
  - 实现多种类型的哈希函数（int, float/double, string）
  - 实现主键哈希索引
  - 实现基于哈希索引的点查询优化（O(1)平均时间复杂度）
  - 实现索引统计功能
  - **测试结果**：29/29测试通过
  - **文件**：`include/core/hash_index.h`, `src/core/hash_index.cpp`
- ✅ **任务8.3：智能索引建议系统**（完成时间：2026-01-14）
  - 实现查询日志记录和分析
  - 实现字段使用统计功能
  - 实现慢查询识别功能
  - 实现索引效果评估和推荐算法
  - 集成到QueryExecutor中，自动记录查询日志
  - **测试结果**：14/14测试通过
  - **文件**：`include/index/index_advisor.h`, `src/index/index_advisor.cpp`
- **已完成工作量**：3天（相邻索引1天，哈希索引1天，智能索引建议系统1天）
- **技术方案**：方案A（索引技术 + 智能推荐）
- **阶段8全部完成**：✅

#### 4.3 系统测试

##### 4.3.1 功能测试 ✅

**阶段1测试**：145个测试全部通过
- 任务1.1：TableMode数据结构测试（36/36通过）
- 任务1.2：TableManager测试（30/30通过）
- 任务1.3：DataManager测试（38/38通过）
- 任务1.4：FileManager测试（41/41通过）

**阶段2测试**：211个测试全部通过
- 任务2.1：SQL解析器基础框架测试（78/78通过）
- 任务2.2：CREATE TABLE测试（24/24通过）
- 任务2.3：EDIT TABLE测试（21/21通过）
- 任务2.4：RENAME TABLE测试（23/23通过）
- 任务2.5：DROP TABLE测试（27/27通过）
- 任务2.6：DDL执行器测试（38/38通过）

**阶段3测试**：166个测试全部通过
- 任务3.1：INSERT测试（43/43通过）
- 任务3.2：DELETE测试（38/38通过）
- 任务3.3：UPDATE测试（43/43通过）
- 任务3.4：DML执行器测试（42/42通过）

**阶段4测试**：161个测试全部通过
- 任务4.1：SELECT单表查询测试（30/30通过）
- 任务4.2：SELECT多表查询测试（50/50通过）
- 任务4.3：SELECT连接查询测试（57/57通过）
- 任务4.4：查询执行器测试（24/24通过）

**阶段1-4测试总计**：683个测试全部通过 ✅

**阶段8.1测试**：28个测试全部通过 ✅
- 任务8.1：相邻索引测试（28/28通过）

**阶段8.2测试**：29个测试全部通过 ✅
- 任务8.2：哈希索引测试（29/29通过）

**阶段8.3测试**：14个测试全部通过 ✅
- 任务8.3：智能索引建议系统测试（14/14通过）

**阶段8测试总计**：71个测试全部通过 ✅
- 任务8.1：相邻索引测试（28/28通过）
- 任务8.2：哈希索引测试（29/29通过）
- 任务8.3：智能索引建议系统测试（14/14通过）

**所有测试总计**：754个测试全部通过 ✅

##### 4.3.2 性能测试
（待补充：GUI开发完成后进行性能测试）

##### 4.3.3 界面测试
（待补充：GUI开发完成后进行界面测试）

### 5. 系统总结和心得体会

#### 5.1 项目总结
（待补充：项目完成情况、功能实现情况）

#### 5.2 技术总结
（待补充：技术要点、难点、解决方案）

#### 5.3 心得体会
（待补充：学习收获、经验总结）

#### 5.4 不足与改进
（待补充：存在的问题、未来改进方向）

---

## 附录：程序源代码

（源代码将单独提交电子版）

---

## 小组成员

| 姓名 | 学号 | 任务分工 |
|------|------|----------|
| 吕金鸣 | 23013085 | 组长 |
| 李雨宣 | 23013083 |  |

---

**注**：本文档为项目报告模板，将在项目开发过程中逐步完善各项内容。

**最后更新**：2026-01-15

**更新内容**：
- 阶段5.1（GUI最小可运行程序）已完成（2026-01-14）
- 阶段5.2（GUI主界面布局）已完成（2026-01-14）
- 阶段5.3（GUI数据库管理和表结构管理界面）已完成（2026-01-14）
- 阶段5.4（GUI数据操作界面）已完成（2026-01-14）
- 阶段5.5（GUI SQL执行界面）已完成（2026-01-15）
- 阶段7.0（SQL查询功能扩展）大部分完成（2026-01-15）
  - ORDER BY、DISTINCT、LIMIT功能实现
  - 比较运算符（>, <, >=, <=, !=）实现
  - 复杂WHERE条件（AND, OR, NOT，支持括号优先级）实现
  - LIKE模式匹配、IN子句、BETWEEN范围查询实现
  - GROUP BY分组、聚合函数（COUNT, SUM, AVG, MAX, MIN）实现
  - HAVING子句实现
  - FULL OUTER JOIN实现
- GUI程序能够成功编译并运行
- 主窗口正常显示，所有基础功能正常工作
- 所有新增SQL功能测试全部通过
- 数据库管理、表管理、数据操作、SQL执行功能已实现并测试通过
- 修复了DROP TABLE后数据未完全删除的问题
- 修复了RENAME TABLE后.dbf和.dat文件不同步的问题
- 完善了SQL执行测试用例文档（添加JOIN查询测试用例）
