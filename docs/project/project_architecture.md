# 项目架构设计文档

> 数据库管理系统 (DBMS) - 系统架构设计

## 1. 系统概述

### 1.1 系统目标
实现一个完整的通用数据库管理系统（DBMS），类似SQL Server，支持完整的数据库管理功能，包括表结构管理、数据操作、SQL查询、索引管理、事务管理、视图、用户权限等核心功能。

### 1.2 系统特点
- 自定义文件存储格式（.dbf和.dat文件）
- 完整的SQL语法支持（DDL、DML、查询）
- 多种索引技术（相邻索引、哈希索引、B+树索引）
- 智能索引建议系统（AI辅助查询优化）
- 图形化用户界面（Qt）
- 通用DBMS功能（事务、视图、权限管理等）

## 2. 系统架构

### 2.1 整体架构

```
┌─────────────────────────────────────────────────┐
│              Qt GUI Layer (GUI)                 │
│  (表管理界面、数据操作界面、SQL界面、推荐界面)      │
└───────────────────┬─────────────────────────────┘
                    │
┌───────────────────▼─────────────────────────────┐
│           SQL Parser Layer (SQL解析层)           │
│     (词法分析、语法分析、SQL语句解析)              │
└───────────────────┬─────────────────────────────┘
                    │
┌───────────────────▼─────────────────────────────┐
│        Command Layer (命令执行层)                │
│  ┌──────────┬──────────┬──────────┬──────────┐  │
│  │   DDL    │   DML    │  Query   │   AI     │  │
│  │ (表管理)  │(数据操作)│ (查询)   │ (推荐)    │  │
│  └──────────┴──────────┴──────────┴──────────┘  │
└───────────────────┬─────────────────────────────┘
                    │
┌───────────────────▼─────────────────────────────┐
│         Core Layer (核心层)                      │
│  ┌──────────────────────────────────────────┐   │
│  │  Table Manager (表管理器)                 │   │
│  │  - .dbf文件读写                           │   │
│  │  - 表结构管理                             │   │
│  └──────────────────────────────────────────┘   │
│  ┌──────────────────────────────────────────┐   │
│  │  Data Manager (数据管理器)                │   │
│  │  - .dat文件读写                           │   │
│  │  - 记录管理                               │   │
│  └──────────────────────────────────────────┘   │
│  ┌──────────────────────────────────────────┐   │
│  │  File I/O (文件I/O)                      │   │
│  │  - 文件操作封装                           │   │
│  └──────────────────────────────────────────┘   │
└─────────────────────────────────────────────────┘
```

### 2.2 模块划分

#### 2.2.1 Core模块（核心模块）
- **职责**：文件I/O、数据结构定义、基础操作
- **主要类**：
  - `TableMode` - 表结构定义
  - `FileManager` - 文件管理器
  - `TableManager` - 表管理器（.dbf文件）
  - `DataManager` - 数据管理器（.dat文件）

#### 2.2.2 DDL模块 ✅（已完成）
- **职责**：数据定义语言实现
- **主要类**：
  - `DDLExecutor` - DDL执行器（统一调度所有DDL语句）
  - `CreateTableHandler` - CREATE TABLE处理
  - `EditTableHandler` - EDIT TABLE处理
  - `RenameTableHandler` - RENAME TABLE处理
  - `DropTableHandler` - DROP TABLE处理
- **实现状态**：所有DDL功能已实现并通过测试（211个测试全部通过）

#### 2.2.3 DML模块 ✅（已完成）
- **职责**：数据操纵语言实现
- **主要类**：
  - `DMLExecutor` - DML执行器（统一调度所有DML语句）
  - `InsertHandler` - INSERT处理
  - `DeleteHandler` - DELETE处理
  - `UpdateHandler` - UPDATE处理
- **实现状态**：所有DML功能已实现并通过测试（166个测试全部通过）

#### 2.2.4 Query模块 ✅（已完成，持续扩展中）
- **职责**：查询实现
- **主要类**：
  - `QueryExecutor` - 查询执行器（统一调度所有SQL语句类型）
  - `SelectHandler` - SELECT处理（单表、多表、JOIN查询、聚合查询）
  - `QueryResult` - 查询结果结构
  - `ExecutionResult` - 执行结果结构
- **实现状态**：基础查询功能已实现并通过测试（161个测试全部通过），扩展功能大部分完成
  - ✅ SELECT单表查询（投影操作、WHERE子句）
  - ✅ SELECT多表查询（笛卡尔积、多表投影）
  - ✅ SELECT连接查询（INNER JOIN、LEFT JOIN、RIGHT JOIN、FULL OUTER JOIN）
  - ✅ ORDER BY排序（单字段、多字段，ASC/DESC）
  - ✅ DISTINCT去重
  - ✅ LIMIT分页
  - ✅ 比较运算符（>, <, >=, <=, !=）
  - ✅ 复杂WHERE条件（AND, OR, NOT，支持括号优先级）
  - ✅ LIKE模式匹配（支持%通配符，大小写敏感）
  - ✅ IN子句（支持值列表查询）
  - ✅ BETWEEN范围查询（支持数值和字符串，包含边界）
  - ✅ GROUP BY分组（支持单字段和多字段分组）
  - ✅ 聚合函数（COUNT, SUM, AVG, MAX, MIN）
  - ✅ HAVING子句（支持对聚合函数结果和分组字段的过滤）
  - ✅ 查询执行器整合（结果格式化）
  - ⏳ LIKE模式匹配（待实现）
  - ⏳ IN子句（待实现）
  - ⏳ BETWEEN范围查询（待实现）
  - ⏳ GROUP BY分组（待实现）
  - ⏳ 聚合函数（COUNT, SUM, AVG, MAX, MIN）（待实现）
  - ⏳ HAVING子句（待实现）

#### 2.2.5 SQL Parser模块 ✅（已完成DDL、DML和Query部分，持续扩展中）
- **职责**：SQL语句解析
- **主要类**：
  - `Lexer` - 词法分析器（已完成，支持所有SQL关键词，包括ORDER BY、DISTINCT、LIMIT、比较运算符、逻辑运算符、聚合函数、GROUP BY、HAVING、FULL OUTER JOIN）
  - `Parser` - 语法分析器（已完成DDL、DML和Query部分，包括复杂WHERE条件解析、聚合函数解析、GROUP BY、HAVING、FULL OUTER JOIN）
  - `ASTNode` - 抽象语法树节点（已完成DDL、DML和Query节点，包括SelectNode、JoinInfo、OrderByInfo、WhereCondition、AggregateFunction、SelectField）
  - `Token` - Token定义（已完成，包括JOIN、ORDER BY、DISTINCT、LIMIT、比较运算符、逻辑运算符、聚合函数、GROUP BY、HAVING、FULL OUTER JOIN相关关键词）
- **实现状态**：DDL、DML和Query语句解析已实现并通过测试，扩展功能大部分完成
  - ✅ DDL解析：78个测试通过
  - ✅ DML解析：已集成到各Handler测试中
  - ✅ Query解析：已集成到SelectHandler测试中（161个测试通过）
  - ✅ ORDER BY、DISTINCT、LIMIT解析：已实现
  - ✅ 比较运算符解析：已实现
  - ✅ 复杂WHERE条件解析（递归下降解析器）：已实现
  - ✅ LIKE、IN、BETWEEN解析：已实现
  - ✅ GROUP BY、HAVING解析：已实现
  - ✅ 聚合函数解析（COUNT, SUM, AVG, MAX, MIN）：已实现
  - ✅ FULL OUTER JOIN解析：已实现
  - ⏳ NATURAL JOIN解析：待实现
  - ⏳ UNION解析：待实现
  - ⏳ 子查询解析：待实现
  - ⏳ 聚合函数解析：待实现

#### 2.2.6 Index模块（数据库新技术）✅（已完成）
- **职责**：索引技术实现和智能索引建议
- **主要类**：
  - `AdjacentIndex` - 相邻索引（范围查询优化）✅（已完成）
  - `HashIndex` - 哈希索引（点查询优化）✅（已完成）
  - `IndexAdvisor` - 智能索引建议系统（AI与数据库结合）✅（已完成）
- **实现状态**：
  - ✅ 相邻索引：已完成（2026-01-14，28测试通过）
  - ✅ 哈希索引：已完成（2026-01-14，29测试通过）
  - ✅ 智能索引建议系统：已完成（2026-01-14，14测试通过）
- **已完成工作量**：3天
- **测试总计**：71个测试全部通过

#### 2.2.7 事务管理模块（计划中）
- **职责**：事务管理和ACID特性
- **主要类**：
  - `TransactionManager` - 事务管理器
  - `LockManager` - 锁管理器
  - `LogManager` - 日志管理器（WAL）
- **实现状态**：计划在第二阶段实现

#### 2.2.8 视图管理模块（计划中）
- **职责**：视图定义和查询
- **主要类**：
  - `ViewManager` - 视图管理器
- **实现状态**：计划在第二阶段实现

#### 2.2.9 用户和权限管理模块（计划中）
- **职责**：用户认证和权限控制
- **主要类**：
  - `UserManager` - 用户管理器
  - `PermissionManager` - 权限管理器
- **实现状态**：计划在第二阶段实现（必须）

#### 2.2.8 GUI模块（部分实现）
- **职责**：用户界面
- **主要类**：
  - `MainWindow` - 主窗口 ✅（已完成，2026-01-14）
  - `TableManagementWidget` - 表管理界面 ✅（已完成，2026-01-14）
  - `TableEditDialog` - 表编辑对话框 ✅（已完成，2026-01-14）
  - `DataOperationWidget` - 数据操作界面 ✅（已完成，2026-01-14）
  - `SQLQueryWidget` - SQL执行界面 ✅（已完成，2026-01-15）
  - `IndexManagementWidget` - 索引管理界面（待开发）
- **实现状态**：
  - ✅ 主窗口框架：已完成（2026-01-14）
  - ✅ 应用程序入口：已完成（main.cpp）
  - ✅ 基础UI组件：已完成（标签、按钮、菜单栏、状态栏）
  - ✅ 字体设置：已完成（英文使用Segoe UI）
  - ✅ 编译配置：已完成（CMakeLists.txt支持Qt和MOC）
  - ✅ 主界面布局：已完成（2026-01-14）
    - 标签页系统（5个标签页：Table Management, Index Management, Data Operation, SQL Execution, Guide）
    - 状态栏显示数据库名称和当前时间
    - 全局键盘快捷键（Ctrl+Q, F1, Ctrl+Tab等）
    - 窗口标题包含版本号（v0.6.3）
    - Guide标签页显示USER_GUIDE.md内容
  - ✅ 数据库管理功能：已完成（2026-01-14）
    - Create Database（创建数据库，Ctrl+N）
    - Open Database（打开数据库，Ctrl+O）
    - 数据库路径统一管理（由MainWindow管理）
    - 状态栏显示当前数据库名称
  - ✅ 表结构管理界面：已完成（2026-01-14）
    - 表列表显示（左侧列表）
    - 表信息显示（右侧详细信息面板）
    - 创建表功能（TableEditDialog，支持字段定义）
    - 编辑表功能（修改表结构）
    - 删除表功能（带确认对话框）
    - 刷新表列表功能
  - ✅ 数据操作界面：已完成（2026-01-14）
    - 数据表格显示（QTableWidget）
    - 插入记录功能（RecordEditDialog）
    - 编辑记录功能
    - 删除记录功能（带确认对话框）
    - 表列表自动刷新（切换标签页时、创建新表后）
  - ✅ SQL执行界面：已完成（2026-01-15）
    - SQL语句输入和执行（QTextEdit，支持多行输入）
    - 批量SQL语句执行（自动分割多条语句，支持分号分隔）
    - 结果展示（成功/失败消息框，查询结果表格QTableWidget）
    - 主键唯一性约束检查（INSERT和UPDATE操作）
    - 大小写不敏感支持（表名、关键字，与主流DBMS对齐）
    - JOIN查询支持（INNER JOIN, LEFT JOIN, RIGHT JOIN）
    - CREATE TABLE char[length]和char(length)语法支持
    - JOIN查询TableName.FieldName语法支持
    - UPDATE 0行受影响的消息提示（符合标准SQL行为）
    - DROP TABLE硬删除修复（完全删除表结构和数据）
    - RENAME TABLE文件同步修复（.dbf和.dat文件保持同步）
    - 所有GUI消息框使用英文和Segoe UI字体
  - ⏳ 索引管理界面：待开发
  - ⏳ 推荐系统界面：待开发

## 3. 数据结构设计

### 3.1 表结构定义（TableMode）

```cpp
#define FIELD_NAME_LENGTH 32

typedef struct {
    char sFieldName[FIELD_NAME_LENGTH];  // 字段名
    char sType[8];                       // 字段类型
    int iSize;                           // 字长
    char bKey;                           // 是否为KEY键
    char bNullFlag;                      // 是否允许为空
    char bValidFlag;                     // 是否有效
} TableMode, *PTableMode;
```

### 3.2 文件存储格式

#### 3.2.1 .dbf文件格式（表结构文件）

```
表1:
  '~' (分隔符, 1字节)
  表名 (char数组)
  字段数量 (int, 4字节)
  字段1结构 (TableMode结构)
  字段2结构 (TableMode结构)
  ...
  字段n结构 (TableMode结构)

表2:
  '~' (分隔符)
  ...
```

#### 3.2.2 .dat文件格式（数据文件）

```
表1数据:
  '~' (分隔符, 1字节)
  表名 (char数组)
  记录数量 (int, 4字节)
  字段数量 (int, 4字节)
  有效标识数组 (char数组，每个记录1字节)
  记录1字段1数据
  记录1字段2数据
  ...
  记录1字段n数据
  记录2字段1数据
  ...
```

## 4. SQL语法设计

### 4.1 DDL语法

```sql
-- 创建表
CREATE TABLE TableName (
    FieldName1 Type1 KEY_Flag1 NULL_Flag1 VALID_Flag1,
    FieldName2 Type2 KEY_Flag2 NULL_Flag2 VALID_Flag2,
    ...
) INTO DatabaseFileName;

-- 编辑表
EDIT TABLE TableName (
    FieldName Type KEY_Flag NULL_Flag VALID_Flag
) IN DatabaseFileName;

-- 重命名表
RENAME TABLE OldTableName NewTableName IN DatabaseFileName;

-- 删除表
DROP TABLE TableName IN DatabaseFileName;
```

### 4.2 DML语法

```sql
-- 插入
INSERT INTO TableName VALUES (value1, value2, ...) IN DatabaseFileName;

-- 删除
DELETE FROM TableName WHERE Field=Value IN DatabaseFileName;

-- 更新
UPDATE TableName (
    SET Field1=Value1
    WHERE Field2=Value2
) IN DatabaseFileName;
```

### 4.3 查询语法

```sql
SELECT * FROM TableName1, TableName2 WHERE Condition;
```

## 5. 索引技术设计

### 5.1 索引技术流程

```
1. 索引构建
   - 相邻索引：维护相邻记录指针
   - 哈希索引：构建哈希表
   - B+树索引：构建B+树结构（计划中）

2. 查询优化
   - 点查询：使用哈希索引（O(1)）
   - 范围查询：使用相邻索引或B+树索引
   - 排序查询：使用B+树索引

3. 索引建议
   - 查询日志记录
   - 字段使用频率统计
   - 慢查询识别
   - 自动索引推荐

4. 索引管理
   - CREATE INDEX
   - DROP INDEX
   - SHOW INDEXES
```

### 5.2 索引数据结构

- **相邻索引**：`map<TableField, AdjacentIndexItem>` - 相邻记录指针映射
- **哈希索引**：`map<TableField, HashTable>` - 哈希表结构（链地址法）
- **B+树索引**：`map<TableField, BTreeNode>` - B+树节点结构（计划中）
- **索引建议**：`vector<IndexRecommendation>` - 索引推荐列表

## 6. 数据流设计

### 6.1 SQL执行流程

```
用户输入SQL
    ↓
SQL解析（词法分析 → 语法分析 → AST）
    ↓
命令路由（DDL/DML/Query）
    ↓
执行器处理
    ↓
Core层文件操作
    ↓
返回结果
```

### 6.2 索引管理流程

```
用户创建索引
    ↓
选择表、字段、索引类型
    ↓
构建索引结构
    ↓
更新索引映射
    ↓
查询时使用索引优化
    ↓
返回优化后的查询结果
```

## 7. 接口设计

### 7.1 Core模块接口

```cpp
class TableManager {
    bool createTable(const string& dbName, const string& tableName, 
                     const vector<TableMode>& fields);
    bool loadTable(const string& dbName, const string& tableName, 
                   vector<TableMode>& fields);
    bool updateTable(const string& dbName, const string& tableName, 
                     const vector<TableMode>& fields);
    bool deleteTable(const string& dbName, const string& tableName);
};

class DataManager {
    bool insertRecord(const string& dbName, const string& tableName, 
                      const vector<string>& values);
    bool deleteRecord(const string& dbName, const string& tableName, 
                      const Condition& condition);
    bool updateRecord(const string& dbName, const string& tableName, 
                      const Condition& condition, const map<string, string>& updates);
    vector<Record> selectRecords(const string& dbName, const string& tableName, 
                                  const Condition& condition);
};
```

### 7.2 SQL解析器接口

```cpp
class SQLParser {
    SQLStatement* parse(const string& sql);
};

class SQLStatement {
    StatementType type;
    // ... 其他属性
};
```

### 7.3 推荐引擎接口

```cpp
class RecommendationEngine {
    vector<RecommendationItem> recommend(const string& userId, 
                                         const RecommendationOptions& options);
};
```

## 8. 错误处理

- 使用返回值（bool/错误码）表示操作成功/失败
- 关键操作需要错误检查和日志记录
- 用户友好的错误提示

## 9. 性能考虑

- 文件I/O优化（缓冲、批量操作）
- 索引优化（相邻索引、哈希索引）
- 查询优化（智能索引建议系统）
- 推荐算法性能（矩阵计算优化）

## 10. 数据库新技术设计

### 10.1 相邻索引（Adjacent Index）

**设计目标**：优化范围查询和顺序扫描性能

**实现方案**：
- 在.dat文件中维护相邻记录的物理位置映射
- 对于有序字段，建立相邻记录指针，优化范围查询
- 支持快速的范围查询和顺序遍历

**性能提升**：
- 范围查询速度提升 3-5倍
- 顺序扫描效率提升 2-3倍

### 10.2 哈希索引（Hash Index）

**设计目标**：优化主键点查询性能

**实现方案**：
- 实现哈希表结构（链地址法解决冲突）
- 支持按主键或唯一字段建立哈希索引
- 快速定位记录位置（O(1)时间复杂度）

**性能提升**：
- 主键查询从O(n)优化到O(1)

### 10.3 智能索引建议系统（Index Advisor）

**设计目标**：AI与数据库结合，自动推荐索引策略

**实现方案**：
- 记录所有SQL查询及执行时间
- 分析WHERE子句中的字段使用频率
- 识别慢查询
- 自动推荐索引策略
- 模拟索引效果（估算性能提升）

**技术特点**：
- 体现AI与数据库技术融合
- 展示效果极佳（可视化分析）
- 实用价值高（自动优化数据库性能）

## 11. 扩展性设计

- 模块化设计，便于功能扩展
- 接口抽象，便于算法替换
- 插件化架构（可选）

---

**最后更新**：2026-01-15

**重要更新**：
- 2026-01-12 - 确定数据库新技术方案（方案A：索引技术+智能推荐）
- 2026-01-14 - 相邻索引实现完成
- 2026-01-14 - 哈希索引实现完成
- 2026-01-14 - 智能索引建议系统实现完成
- 2026-01-14 - GUI主界面布局完成
- 2026-01-14 - GUI表结构管理界面完成
- 2026-01-14 - GUI数据操作界面完成
- 2026-01-15 - GUI SQL执行界面完成（包括批量执行、主键约束、大小写不敏感、JOIN查询支持、DROP/RENAME TABLE修复）
- 2026-01-15 - SQL查询功能扩展（ORDER BY, DISTINCT, LIMIT, 比较运算符, 复杂WHERE条件, LIKE, IN, BETWEEN, GROUP BY, 聚合函数, HAVING, FULL OUTER JOIN）
