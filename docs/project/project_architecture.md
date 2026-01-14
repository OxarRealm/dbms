# 项目架构设计文档

> 智能音乐播放管理系统 - 系统架构设计

## 1. 系统概述

### 1.1 系统目标
实现一个完整的数据库管理系统（DBMS），支持表结构管理、数据操作、SQL查询以及AI智能推荐功能。

### 1.2 系统特点
- 自定义文件存储格式（.dbf和.dat文件）
- 完整的SQL语法支持（DDL、DML、查询）
- 图形化用户界面（Qt）
- AI智能推荐功能

## 2. 系统架构

### 2.1 整体架构

```
┌─────────────────────────────────────────────────┐
│              Qt GUI Layer (GUI)                  │
│  (表管理界面、数据操作界面、SQL界面、推荐界面)    │
└───────────────────┬─────────────────────────────┘
                    │
┌───────────────────▼─────────────────────────────┐
│           SQL Parser Layer (SQL解析层)           │
│     (词法分析、语法分析、SQL语句解析)            │
└───────────────────┬─────────────────────────────┘
                    │
┌───────────────────▼─────────────────────────────┐
│        Command Layer (命令执行层)                │
│  ┌──────────┬──────────┬──────────┬──────────┐  │
│  │   DDL    │   DML    │  Query   │   AI     │  │
│  │ (表管理) │(数据操作)│ (查询)   │ (推荐)   │  │
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

#### 2.2.4 Query模块 ✅（已完成）
- **职责**：查询实现
- **主要类**：
  - `QueryExecutor` - 查询执行器（统一调度所有SQL语句类型）
  - `SelectHandler` - SELECT处理（单表、多表、JOIN查询）
  - `QueryResult` - 查询结果结构
  - `ExecutionResult` - 执行结果结构
- **实现状态**：所有查询功能已实现并通过测试（161个测试全部通过）
  - SELECT单表查询（投影操作、WHERE子句）
  - SELECT多表查询（笛卡尔积、多表投影）
  - SELECT连接查询（INNER JOIN、LEFT JOIN）
  - 查询执行器整合（结果格式化）

#### 2.2.5 SQL Parser模块 ✅（已完成DDL、DML和Query部分）
- **职责**：SQL语句解析
- **主要类**：
  - `Lexer` - 词法分析器（已完成，支持所有SQL关键词）
  - `Parser` - 语法分析器（已完成DDL、DML和Query部分）
  - `ASTNode` - 抽象语法树节点（已完成DDL、DML和Query节点，包括SelectNode和JoinInfo）
  - `Token` - Token定义（已完成，包括JOIN相关关键词）
- **实现状态**：DDL、DML和Query语句解析已实现并通过测试
  - DDL解析：78个测试通过
  - DML解析：已集成到各Handler测试中
  - Query解析：已集成到SelectHandler测试中（161个测试通过）

#### 2.2.6 Index模块（数据库新技术）🔄（进行中）
- **职责**：索引技术实现和智能索引建议
- **主要类**：
  - `AdjacentIndex` - 相邻索引（范围查询优化）✅（已完成）
  - `HashIndex` - 哈希索引（点查询优化）⏳（进行中）
  - `IndexAdvisor` - 智能索引建议系统（AI与数据库结合）⏳（计划中）
  - `QueryLogger` - 查询日志记录器⏳（计划中）
- **实现状态**：
  - ✅ 相邻索引：已完成（2026-01-14，28测试通过）
  - ⏳ 哈希索引：进行中（预计4-6天）
  - ⏳ 智能索引建议系统：计划中（预计5-7天）
- **已完成工作量**：1天
- **预计剩余工作量**：9-13天

#### 2.2.7 AI模块
- **职责**：智能推荐算法
- **主要类**：
  - `RecommendationEngine` - 推荐引擎
  - `CollaborativeFiltering` - 协同过滤算法
  - `ContentBasedFiltering` - 内容推荐算法
  - `SimilarityCalculator` - 相似度计算器

#### 2.2.8 GUI模块
- **职责**：用户界面
- **主要类**：
  - `MainWindow` - 主窗口
  - `TableManagementWidget` - 表管理界面
  - `DataOperationWidget` - 数据操作界面
  - `SQLQueryWidget` - SQL查询界面
  - `RecommendationWidget` - 推荐界面

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

## 5. AI推荐系统设计

### 5.1 推荐算法流程

```
1. 数据准备
   - 构建用户-歌曲播放矩阵
   - 提取歌曲特征向量

2. 相似度计算
   - 用户相似度（基于播放历史）
   - 歌曲相似度（基于特征）

3. 推荐生成
   - 协同过滤推荐
   - 内容推荐
   - 混合推荐

4. 结果排序与返回
   - Top-N推荐
   - 推荐理由生成
```

### 5.2 数据结构

- **用户-歌曲矩阵**：`map<UserID, map<SongID, PlayCount>>`
- **歌曲特征向量**：`map<SongID, FeatureVector>`
- **推荐结果**：`vector<RecommendationItem>`

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

### 6.2 推荐系统流程

```
用户选择查询条件
    ↓
读取播放记录和歌曲数据
    ↓
构建数据矩阵
    ↓
计算相似度
    ↓
生成推荐列表
    ↓
返回推荐结果（GUI展示）
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
- 对于有序字段（如SongID、Year），建立相邻记录指针
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

**最后更新**：2026-01-14

**重要更新**：
- 2026-01-12 - 确定数据库新技术方案（方案A：索引技术+智能推荐）
- 2026-01-14 - 相邻索引实现完成
