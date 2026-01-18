# UPDATE CHECK约束检查问题分析

## 问题描述
UPDATE操作无视CHECK约束，而INSERT操作能正确检查CHECK约束。

## 工作流程对比分析

### INSERT工作流程
1. **解析SQL** → `insertNode`
2. **提取数据库路径** → `dbPath` (使用`basePath`组合)
3. **设置数据库路径** → `m_tableManager.setDatabasePath(dbPath)`, `m_dataManager.setDatabasePath(dbPath)`
4. **提取数据库名** → `dbNameForConstraints` (从`insertNode->databaseFileName`提取)
5. **读取表结构** → `tableInfo`
6. **应用默认值** → `applyDefaultValues`
7. **创建记录** → `record = createRecord(insertNode, tableInfo)`
8. **检查约束** → `checkCheckConstraints(tableName, tableInfo, record, dbNameForConstraints)`
   - 调用 `ConstraintRegistry::getInstance().getCheckConstraints(dbName, tableName)`
   - 对每个约束，检查`record.values[fieldIndex]`

### UPDATE工作流程
1. **解析SQL** → `updateNode`
2. **提取数据库路径** → `dbPath` (使用`basePath`组合)
3. **设置数据库路径** → `m_tableManager.setDatabasePath(dbPath)`, `m_dataManager.setDatabasePath(dbPath)`
4. **提取数据库名** → `dbNameForConstraints` (从`updateNode->databaseFileName`提取)
5. **读取表结构** → `tableInfo`
6. **读取所有记录** → `records`
7. **查找匹配的记录** → `recordsToUpdate`
8. **对每条记录**：
   - 创建更新后的记录 → `updatedRecord = records[recordIndex]`
   - 设置新值 → `updatedRecord.values[setFieldIndex] = convertedValue`
   - 检查约束 → `checkCheckConstraints(tableName, tableInfo, updatedRecord, dbNameForConstraints)`
     - 调用 `ConstraintRegistry::getInstance().getCheckConstraints(dbName, tableName)`
     - 对每个约束，检查`updatedRecord.values[fieldIndex]`

## 关键差异点分析

### 1. 数据库名提取
- **INSERT**: 从`insertNode->databaseFileName`提取
- **UPDATE**: 从`updateNode->databaseFileName`提取
- **问题**: 如果SQL解析时`databaseFileName`不同，会导致约束查询失败

### 2. 记录准备
- **INSERT**: `record`是新创建的，所有字段值都来自INSERT语句
- **UPDATE**: `updatedRecord`是从现有记录复制的，只有`setFieldIndex`字段被更新
- **问题**: 如果CHECK约束的字段不是更新的字段，值应该没变，检查应该通过

### 3. 约束检查调用
- **INSERT**: 在插入前检查所有约束
- **UPDATE**: 在更新前检查所有约束
- **问题**: 如果`checkCheckConstraints`没有被调用，或者返回了true但没有实际检查，就会导致问题

## 可能的问题点

### 问题1: `dbNameForConstraints`提取不一致
**场景**: 
- INSERT SQL: `INSERT INTO TestTable VALUES (50) IN student_db;`
- UPDATE SQL: `UPDATE TestTable (SET Value=500 WHERE ID=1) IN student_db;`

**分析**:
- INSERT: `insertNode->databaseFileName = "student_db"` → `dbNameForConstraints = "student_db"`
- UPDATE: `updateNode->databaseFileName = "student_db"` → `dbNameForConstraints = "student_db"`
- 如果约束注册时使用的key是`"student_db"`，那么两者应该都能找到约束

### 问题2: 约束注册表查询失败
**场景**: 
- 约束注册时使用的key是`"student_db"`
- UPDATE查询时使用的key是`"E:\Projects\VSCode\dbms\student_db"`（完整路径）

**分析**:
- 如果`dbNameForConstraints`提取不正确，会导致`getCheckConstraints`返回空列表
- 空列表意味着没有约束需要检查，所以UPDATE会成功

### 问题3: 约束检查逻辑被跳过
**场景**: 
- `checkCheckConstraints`被调用，但`checkConstraints`为空
- 或者`fieldIndex`找不到，导致约束检查被跳过

**分析**:
- 如果`checkConstraints.size() == 0`，函数会直接返回`true`，不进行任何检查
- 如果`fieldIndex == -1`或`fieldIndex >= updatedRecord.values.size()`，约束检查会被跳过

## 测试用例模拟

### 测试用例1: 基本CHECK约束
**表结构**:
```
CREATE TABLE TestTable (
    ID int KEY NO_NULL VALID,
    Value int NOT_KEY NO_NULL VALID,
    CHECK (Value > 0 AND Value < 100)
) INTO student_db;
```

**INSERT测试**:
```sql
INSERT INTO TestTable VALUES (1, 50) IN student_db;
```
**预期**: 成功（50在0-100范围内）

**UPDATE测试**:
```sql
UPDATE TestTable (SET Value=500 WHERE ID=1) IN student_db;
```
**预期**: 失败（500不在0-100范围内）

**代码执行流程**:
1. UPDATE解析SQL → `updateNode->databaseFileName = "student_db"`
2. 提取`dbNameForConstraints = "student_db"`
3. 读取表结构 → `tableInfo`
4. 读取所有记录 → `records`
5. 找到匹配记录 → `recordsToUpdate = [0]`
6. 创建`updatedRecord = records[0]` → `updatedRecord.values = ["1", "50"]`
7. 设置新值 → `updatedRecord.values[1] = "500"`
8. 调用`checkCheckConstraints("TestTable", tableInfo, updatedRecord, "student_db")`
9. 查询约束 → `getCheckConstraints("student_db", "TestTable")`
10. 如果返回空列表 → 直接返回`true`，UPDATE成功（**问题所在**）
11. 如果返回约束列表 → 检查`updatedRecord.values[1] = "500"` → 应该失败

## 调试输出分析

添加的调试输出会显示：
- `[UPDATE] dbNameForConstraints`的值
- `[UPDATE CHECK] checkConstraints.size()`的值
- `[UPDATE CHECK] fieldIndex`的值
- `[UPDATE CHECK] value`的值
- `[UPDATE CHECK] expression`的值

通过这些输出，可以确定：
1. `dbNameForConstraints`是否正确
2. 是否获取到了约束
3. 字段索引是否正确
4. 值是否正确
5. 约束表达式是否正确

## 修复建议

1. **确保`dbNameForConstraints`提取正确**: 与INSERT使用完全相同的逻辑
2. **确保约束注册和查询使用相同的key**: 都使用baseName（不含路径和扩展名）
3. **添加调试输出**: 帮助定位问题
4. **检查约束检查逻辑**: 确保所有约束都被检查，即使字段索引找不到也要记录警告

