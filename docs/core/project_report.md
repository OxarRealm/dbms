# 《数据库新技术实践课程设计》实验报告

## 实验名称
《数据库新技术实践课程设计》

## 实验地点
信息大楼222

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
- 在TokenType中添加JOIN相关关键词（JOIN, ON, INNER, LEFT, RIGHT）
- 在AST节点中添加JoinInfo结构（连接类型、右表名、左表字段、右表字段、运算符）
- 扩展Parser支持JOIN语法解析（FROM Table1 JOIN Table2 ON ...）
- 实现executeJoinQuery方法
- 支持INNER JOIN（内连接，只返回匹配的记录）
- 支持LEFT JOIN（左连接，保留左表所有记录，右表无匹配时填充空值）
- 支持JOIN条件评估（ON子句中的等值连接）
- 支持JOIN查询带WHERE条件过滤

**实现文件**：`include/sql_parser/token.h`, `src/sql_parser/token.cpp`, `include/sql_parser/ast_node.h`, `src/sql_parser/parser.cpp`, `src/query/select_handler.cpp`（扩展）

**3.6.4 查询执行器整合**

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

#### 3.8 AI推荐系统设计
（待补充：推荐算法设计）

#### 3.9 GUI界面设计
（待补充：界面设计说明）

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
- 完成SELECT连接查询（INNER JOIN、LEFT JOIN）
- 完成查询执行器统一调度（QueryExecutor）
- **测试结果**：161个测试全部通过

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

**最后更新**：2026-01-14
