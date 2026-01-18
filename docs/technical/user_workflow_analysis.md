# 用户操作流程分析

> 数据库管理系统 (DBMS) - 用户操作流程、代码调用与效果展示

**注意**：本文档创建于项目方向调整之前，部分内容（如音乐推荐场景）已过时，但通用操作流程仍然有效。

## 📋 概述

本文档详细描述系统的用户操作流程，包括：
1. **用户操作**：用户在界面上的操作步骤
2. **代码调用链**：从用户操作到最终执行的代码路径
3. **最终效果**：用户看到的界面反馈和数据结果

---

## 🎯 核心用户场景

### 场景1：创建数据库表（表结构管理）

#### 用户操作流程

**方式1：通过GUI界面创建表**
1. 用户打开应用程序
2. 用户点击"表管理"标签页
3. 用户点击"创建表"按钮
4. 在弹出的对话框中输入：
   - 数据库文件名（如：MusicDB）
   - 表名（如：Songs）
   - 字段信息：
     - 字段1：SongID, int, KEY, NO_NULL, VALID
     - 字段2：SongName, char[100], NOT_KEY, NO_NULL, VALID
     - 字段3：Artist, char[50], NOT_KEY, NULL, VALID
     - ...
5. 用户点击"确定"按钮

**方式2：通过SQL语句创建表**
1. 用户打开应用程序
2. 用户点击"SQL执行"标签页
3. 用户在SQL输入框中输入：
   ```sql
   CREATE TABLE Songs (
       SongID int KEY NO_NULL VALID,
       SongName char[100] NOT_KEY NO_NULL VALID,
       Artist char[50] NOT_KEY NULL VALID
   ) INTO MusicDB;
   ```
4. 用户点击"执行"按钮

#### 代码调用链

```
用户操作
    ↓
GUI层：MainWindow / TableManagementWidget / SQLQueryWidget
    ↓
调用 SQLParser::parse(sqlString)
    ↓
SQL解析层：
    - Lexer（词法分析）→ Token流
    - Parser（语法分析）→ AST（抽象语法树）
    ↓
DDL层：DDLExecutor::execute(ast)
    - CreateTableHandler::handle(createTableAST)
    ↓
Core层：TableManager::createTable(dbName, tableName, fields)
    ↓
文件I/O层：
    - 读取 MusicDB.dbf（如果存在）
    - 解析现有表结构
    - 添加新表结构
    - 写入 MusicDB.dbf（追加或更新）
    ↓
返回结果
```

#### 代码模块调用

```cpp
// GUI层
TableManagementWidget::onCreateTableButtonClicked() {
    // 收集用户输入
    QString dbName = dbNameInput->text();
    QString tableName = tableNameInput->text();
    QList<FieldInfo> fields = getFieldInfoFromUI();
    
    // 调用DDL执行器
    DDLExecutor executor;
    bool success = executor.createTable(dbName, tableName, fields);
    
    // 显示结果
    if (success) {
        showSuccessMessage("表创建成功");
        refreshTableList();
    } else {
        showErrorMessage("表创建失败：" + executor.getLastError());
    }
}

// DDL层
DDLExecutor::createTable(QString dbName, QString tableName, QList<FieldInfo> fields) {
    // 验证输入
    if (!validateTableName(tableName)) return false;
    if (!validateFields(fields)) return false;
    
    // 调用TableManager
    TableManager tableManager;
    return tableManager.createTable(dbName, tableName, convertToTableMode(fields));
}

// Core层
TableManager::createTable(QString dbName, QString tableName, QVector<TableMode> fields) {
    // 构建文件路径
    QString dbfPath = dbName + ".dbf";
    
    // 读取现有表结构（如果文件存在）
    QMap<QString, QVector<TableMode>> tables;
    if (QFile::exists(dbfPath)) {
        tables = loadAllTables(dbfPath);
    }
    
    // 检查表是否已存在
    if (tables.contains(tableName)) {
        m_lastError = "表已存在";
        return false;
    }
    
    // 添加新表
    tables[tableName] = fields;
    
    // 写入文件
    return writeAllTables(dbfPath, tables);
}
```

#### 最终效果

- **界面反馈**：
  - 成功：显示绿色提示"表 Songs 创建成功"
  - 失败：显示红色错误提示（如"表已存在"或"字段定义错误"）
  
- **数据结果**：
  - MusicDB.dbf 文件中新增表 Songs 的结构信息
  - 文件格式：
    ```
    '~'  // 分隔符
    "Songs"  // 表名
    3  // 字段数量
    TableMode结构1  // SongID字段
    TableMode结构2  // SongName字段
    TableMode结构3  // Artist字段
    ```

- **界面更新**：
  - 表列表中显示新创建的表 "Songs"
  - 可以查看表结构（字段列表）

---

### 场景2：插入数据记录

#### 用户操作流程

**方式1：通过GUI界面插入**
1. 用户点击"数据操作"标签页
2. 用户选择数据库（如：MusicDB）
3. 用户选择表（如：Songs）
4. 系统自动显示表的字段结构
5. 用户在表单中填写数据：
   - SongID: 1
   - SongName: "Bohemian Rhapsody"
   - Artist: "Queen"
6. 用户点击"插入"按钮

**方式2：通过SQL语句插入**
1. 用户在SQL输入框中输入：
   ```sql
   INSERT INTO Songs VALUES (1, 'Bohemian Rhapsody', 'Queen') IN MusicDB;
   ```
2. 用户点击"执行"按钮

#### 代码调用链

```
用户操作
    ↓
GUI层：DataOperationWidget / SQLQueryWidget
    ↓
调用 SQLParser::parse(sqlString) 或直接调用DML
    ↓
DML层：DMLExecutor::execute(ast)
    - InsertHandler::handle(insertAST)
    ↓
Core层：DataManager::insertRecord(dbName, tableName, values)
    ↓
1. 调用 TableManager::loadTable(dbName, tableName) 获取表结构
2. 验证数据类型和约束
3. 读取 MusicDB.dat 文件
4. 添加新记录
5. 写入 MusicDB.dat 文件
    ↓
返回结果
```

#### 代码模块调用

```cpp
// DML层
InsertHandler::handle(InsertAST* ast) {
    QString dbName = ast->getDatabaseName();
    QString tableName = ast->getTableName();
    QVector<QString> values = ast->getValues();
    
    // 获取表结构
    TableManager tableManager;
    QVector<TableMode> fields;
    if (!tableManager.loadTable(dbName, tableName, fields)) {
        m_error = "表不存在";
        return false;
    }
    
    // 验证数据
    if (!validateData(fields, values)) {
        m_error = "数据类型或约束不匹配";
        return false;
    }
    
    // 调用DataManager
    DataManager dataManager;
    return dataManager.insertRecord(dbName, tableName, values);
}

// Core层
DataManager::insertRecord(QString dbName, QString tableName, QVector<QString> values) {
    // 构建文件路径
    QString datPath = dbName + ".dat";
    
    // 读取现有数据
    QMap<QString, TableData> tablesData;
    if (QFile::exists(datPath)) {
        tablesData = loadAllTableData(datPath);
    }
    
    // 获取或创建表数据
    TableData& tableData = tablesData[tableName];
    
    // 添加新记录
    Record newRecord;
    newRecord.fields = values;
    newRecord.validFlag = 1;  // 有效
    tableData.records.append(newRecord);
    tableData.recordCount++;
    
    // 写入文件
    return writeAllTableData(datPath, tablesData);
}
```

#### 最终效果

- **界面反馈**：
  - 成功：显示"记录插入成功"
  - 失败：显示错误信息（如"数据类型错误"、"违反约束"）
  
- **数据结果**：
  - MusicDB.dat 文件中新增一条记录
  - 记录格式：
    ```
    '~'  // 分隔符
    "Songs"  // 表名
    1  // 记录数量
    3  // 字段数量
    [1]  // 有效标识数组（1表示有效）
    "1"  // SongID值
    "Bohemian Rhapsody"  // SongName值
    "Queen"  // Artist值
    ```

- **界面更新**：
  - 数据表格中显示新插入的记录
  - 记录数量更新

---

### 场景3：查询数据（SELECT）

#### 用户操作流程

1. 用户在SQL输入框中输入：
   ```sql
   SELECT * FROM Songs WHERE Genre='Rock' IN MusicDB;
   ```
2. 用户点击"执行"按钮

#### 代码调用链

```
用户操作
    ↓
GUI层：SQLQueryWidget
    ↓
调用 SQLParser::parse(sqlString)
    ↓
SQL解析层：生成SELECT AST
    ↓
Query层：QueryExecutor::execute(selectAST)
    - SelectHandler::handle(selectAST)
    ↓
1. 解析WHERE条件
2. 调用 DataManager::selectRecords(dbName, tableName, condition)
3. 过滤记录（应用WHERE条件）
4. 投影字段（应用SELECT字段列表）
    ↓
返回结果集
    ↓
GUI层：格式化并显示结果
```

#### 代码模块调用

```cpp
// Query层
SelectHandler::handle(SelectAST* ast) {
    QString dbName = ast->getDatabaseName();
    QVector<QString> tableNames = ast->getTableNames();
    Condition* condition = ast->getCondition();
    QVector<QString> selectFields = ast->getSelectFields();
    
    // 单表查询
    if (tableNames.size() == 1) {
        DataManager dataManager;
        QVector<Record> records = dataManager.selectRecords(
            dbName, tableNames[0], condition
        );
        
        // 应用投影
        QVector<QVector<QString>> result;
        for (const Record& record : records) {
            QVector<QString> row;
            for (const QString& field : selectFields) {
                row.append(getFieldValue(record, field));
            }
            result.append(row);
        }
        
        return result;
    }
    
    // 多表查询（JOIN）
    // ... JOIN逻辑
}
```

#### 最终效果

- **界面反馈**：
  - 在结果表格中显示查询结果
  - 显示查询到的记录数量（如"找到 15 条记录"）

- **数据结果**：
  - 结果表格显示：
    ```
    SongID | SongName           | Artist
    -------+--------------------+--------
    1      | Bohemian Rhapsody  | Queen
    2      | Stairway to Heaven | Led Zeppelin
    ...
    ```

- **界面更新**：
  - SQL执行时间显示（如"执行时间：0.023秒"）
  - 查询结果可以导出（可选）

---

### 场景4：智能推荐（AI功能）

#### 用户操作流程

1. 用户点击"智能推荐"标签页
2. 用户选择推荐类型：
   - 基于用户推荐
   - 基于歌曲推荐
3. 用户输入参数：
   - 用户ID：1（如果选择基于用户）
   - 或 歌曲ID：5（如果选择基于歌曲）
4. 用户点击"开始推荐"按钮
5. 系统显示推荐进度（可选）
6. 系统显示推荐结果

#### 代码调用链

```
用户操作
    ↓
GUI层：RecommendationWidget
    ↓
AI层：RecommendationEngine::recommend(userId/songId, options)
    ↓
1. 读取播放记录数据（PlayRecords表）
2. 读取歌曲数据（Songs表）
3. 构建用户-歌曲矩阵
4. 计算相似度（协同过滤 / 内容推荐）
5. 生成推荐列表（Top-N）
6. 添加推荐理由
    ↓
返回推荐结果
    ↓
GUI层：格式化并显示推荐结果
```

#### 代码模块调用

```cpp
// AI层
RecommendationEngine::recommend(QString userId, RecommendationOptions options) {
    // 1. 读取数据
    DataManager dataManager;
    QVector<Record> playRecords = dataManager.selectRecords(
        "MusicDB", "PlayRecords", 
        Condition("UserID", "=", userId)
    );
    QVector<Record> songs = dataManager.selectRecords("MusicDB", "Songs", nullptr);
    
    // 2. 构建用户-歌曲矩阵
    UserSongMatrix matrix = buildMatrix(playRecords, songs);
    
    // 3. 计算推荐
    QVector<RecommendationItem> recommendations;
    
    if (options.algorithm == "CollaborativeFiltering") {
        CollaborativeFiltering cf;
        recommendations = cf.recommend(matrix, userId, options.topN);
    } else if (options.algorithm == "ContentBased") {
        ContentBasedFiltering cbf;
        recommendations = cbf.recommend(songs, userId, playRecords, options.topN);
    }
    
    // 4. 添加推荐理由
    for (RecommendationItem& item : recommendations) {
        item.reason = generateRecommendationReason(item);
    }
    
    return recommendations;
}
```

#### 最终效果

- **界面反馈**：
  - 显示推荐结果列表：
    ```
    推荐歌曲：
    
    1. Song: "Stairway to Heaven"
       Artist: "Led Zeppelin"
       推荐分数: 0.85
       推荐理由: "基于您常听的摇滚风格歌曲推荐"
    
    2. Song: "Hotel California"
       Artist: "Eagles"
       推荐分数: 0.82
       推荐理由: "与您喜欢的Queen风格相似"
    
    ...
    ```
  - 显示推荐算法类型和执行时间

- **数据结果**：
  - 推荐结果可以保存到推荐记录表（可选）
  - 用户可以选择将推荐歌曲添加到歌单

---

### 场景5：修改表结构（EDIT TABLE）

#### 用户操作流程

1. 用户点击"表管理"标签页
2. 用户选择表（如：Songs）
3. 用户点击"编辑表结构"按钮
4. 用户修改字段信息（添加字段、删除字段、修改字段属性）
5. 用户点击"保存"按钮

或通过SQL：
```sql
EDIT TABLE Songs (
    NewField char[50] NOT_KEY NULL VALID
) IN MusicDB;
```

#### 代码调用链

```
用户操作
    ↓
GUI层 / SQL解析
    ↓
DDL层：EditTableHandler::handle(editTableAST)
    ↓
Core层：TableManager::updateTable(dbName, tableName, newFields)
    ↓
1. 读取现有表结构
2. 应用修改（添加/删除/修改字段）
3. 验证修改（检查数据兼容性）
4. 更新.dbf文件
5. 如果删除字段，需要更新.dat文件中的记录
    ↓
返回结果
```

#### 最终效果

- **界面反馈**：
  - 成功：显示"表结构修改成功"
  - 失败：显示错误（如"无法删除正在使用的字段"）

- **数据结果**：
  - MusicDB.dbf 文件中的表结构已更新
  - 如果删除字段，MusicDB.dat 文件中的记录需要相应更新

---

## 📊 完整操作流程示例：创建数据库并录入数据

### 完整流程

1. **创建用户表**
   ```sql
   CREATE TABLE Users (
       UserID int KEY NO_NULL VALID,
       UserName char[50] NOT_KEY NO_NULL VALID,
       RegisterTime char[20] NOT_KEY NULL VALID
   ) INTO MusicDB;
   ```

2. **创建歌曲表**
   ```sql
   CREATE TABLE Songs (
       SongID int KEY NO_NULL VALID,
       SongName char[100] NOT_KEY NO_NULL VALID,
       Artist char[50] NOT_KEY NULL VALID,
       Genre char[30] NOT_KEY NULL VALID,
       Year int NOT_KEY NULL VALID,
       Duration int NOT_KEY NULL VALID
   ) INTO MusicDB;
   ```

3. **插入用户数据**
   ```sql
   INSERT INTO Users VALUES (1, 'Alice', '2025-01-01') IN MusicDB;
   INSERT INTO Users VALUES (2, 'Bob', '2025-01-02') IN MusicDB;
   ```

4. **插入歌曲数据**
   ```sql
   INSERT INTO Songs VALUES (1, 'Bohemian Rhapsody', 'Queen', 'Rock', 1975, 355) IN MusicDB;
   INSERT INTO Songs VALUES (2, 'Stairway to Heaven', 'Led Zeppelin', 'Rock', 1971, 482) IN MusicDB;
   ```

5. **创建播放记录表**
   ```sql
   CREATE TABLE PlayRecords (
       RecordID int KEY NO_NULL VALID,
       UserID int NOT_KEY NO_NULL VALID,
       SongID int NOT_KEY NO_NULL VALID,
       PlayTime char[20] NOT_KEY NULL VALID,
       PlayCount int NOT_KEY NULL VALID
   ) INTO MusicDB;
   ```

6. **插入播放记录**
   ```sql
   INSERT INTO PlayRecords VALUES (1, 1, 1, '2025-01-10 10:00:00', 5) IN MusicDB;
   INSERT INTO PlayRecords VALUES (2, 1, 2, '2025-01-10 11:00:00', 3) IN MusicDB;
   ```

7. **查询数据**
   ```sql
   SELECT * FROM Songs WHERE Genre='Rock' IN MusicDB;
   ```

8. **智能推荐**
   - 用户选择：基于用户ID=1推荐
   - 系统推荐相似歌曲

---

## 🎨 GUI界面布局（预期）

### 主窗口结构

```
┌─────────────────────────────────────────────────────────┐
│  智能音乐播放管理系统                      [最小化][关闭]│
├─────────────────────────────────────────────────────────┤
│  [文件] [编辑] [视图] [帮助]                            │
├─────────────────────────────────────────────────────────┤
│  [表管理] [数据操作] [SQL执行] [智能推荐] [系统设置]    │
├─────────────────────────────────────────────────────────┤
│                                                         │
│  当前标签页内容（表管理/数据操作/SQL执行/智能推荐）      │
│                                                         │
│                                                         │
├─────────────────────────────────────────────────────────┤
│  状态栏: 数据库: MusicDB | 记录数: 100 | 就绪          │
└─────────────────────────────────────────────────────────┘
```

### 表管理界面

```
┌─────────────────────────────────────────────────────────┐
│  数据库: [MusicDB ▼]    [刷新]                          │
├─────────────────────────────────────────────────────────┤
│  表列表:                           │  表结构:           │
│  ┌───────────────────┐            │  ┌───────────────┐ │
│  │ ☑ Users          │            │  │ UserID (int)  │ │
│  │ ☑ Songs          │            │  │ UserName      │ │
│  │ ☐ PlayRecords    │            │  │ RegisterTime  │ │
│  └───────────────────┘            │  └───────────────┘ │
│                                    │                    │
│  [创建表] [编辑表] [删除表]        │                    │
└─────────────────────────────────────────────────────────┘
```

---

## 🔄 数据流程图

```
用户操作
    ↓
GUI界面（Qt Widget）
    ↓
业务逻辑层（DDL/DML/Query/AI）
    ↓
数据访问层（TableManager/DataManager）
    ↓
文件I/O层（.dbf/.dat文件操作）
    ↓
磁盘存储
```

---

**最后更新**：2026-01-14
