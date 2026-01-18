# DROP TABLE 测试用例

## 测试目标
验证删除表后，重新创建同名表时不会加载旧数据。

## 测试用例 1：GUI 操作测试

### 步骤 1：创建表并插入数据
1. 打开数据库（例如：`student_db`）
2. 在 **Table Management** 标签页：
   - 点击 **Create Table**
   - 表名：`TestTable`
   - 添加字段：
     - `ID` (int, KEY, NO_NULL, VALID)
     - `Name` (string, NOT_KEY, NULL, VALID)
   - 点击 **OK** 创建表

3. 在 **Data Operation** 标签页：
   - 选择表：`TestTable`
   - 点击 **Insert Record**
   - 输入数据：
     - ID: `1`
     - Name: `Test1`
   - 点击 **OK** 插入记录
   - 再次插入一条记录：
     - ID: `2`
     - Name: `Test2`

4. 验证数据：
   - 在 **Data Operation** 标签页中，应该能看到两条记录

### 步骤 2：删除表
1. 在 **Table Management** 标签页：
   - 选择表：`TestTable`
   - 点击 **Delete Table**
   - 确认删除

2. 验证删除：
   - 在 **Table Management** 标签页中，`TestTable` 应该不再出现在表列表中
   - 在 **Data Operation** 标签页中，表下拉列表中应该没有 `TestTable`

### 步骤 3：重新创建同名表
1. 在 **Table Management** 标签页：
   - 点击 **Create Table**
   - 表名：`TestTable`（与之前删除的表同名）
   - 添加字段：
     - `ID` (int, KEY, NO_NULL, VALID)
     - `Name` (string, NOT_KEY, NULL, VALID)
   - 点击 **OK** 创建表

### 步骤 4：验证新表为空
1. 在 **Data Operation** 标签页：
   - 选择表：`TestTable`
   - **预期结果**：表应该是空的，**不应该**显示之前插入的两条记录（ID=1, Name=Test1 和 ID=2, Name=Test2）

2. 如果看到旧数据，说明删除失败，需要检查调试输出

---

## 测试用例 2：SQL 执行测试

### 步骤 1：创建表并插入数据
在 **SQL Execution** 标签页执行以下 SQL：

```sql
CREATE TABLE TestTable (
    ID int KEY NO_NULL VALID,
    Name string NOT_KEY NULL VALID
) INTO student_db;
```

```sql
INSERT INTO TestTable VALUES (1, 'Test1') IN student_db;
```

```sql
INSERT INTO TestTable VALUES (2, 'Test2') IN student_db;
```

验证数据：
```sql
SELECT * FROM TestTable;
```
**预期结果**：应该返回两条记录

### 步骤 2：删除表
执行以下 SQL：

```sql
DROP TABLE TestTable IN student_db;
```

验证删除：
```sql
SELECT * FROM TestTable;
```
**预期结果**：应该报错，提示表不存在

### 步骤 3：重新创建同名表
执行以下 SQL：

```sql
CREATE TABLE TestTable (
    ID int KEY NO_NULL VALID,
    Name string NOT_KEY NULL VALID
) INTO student_db;
```

### 步骤 4：验证新表为空
执行以下 SQL：

```sql
SELECT * FROM TestTable;
```

**预期结果**：应该返回空结果（0条记录），**不应该**返回之前插入的两条记录

---

## 调试信息

如果测试失败，请检查控制台输出中的以下调试信息：

1. `DEBUG: clearTable - Found and skipping table: ...` - 表示找到了目标表并跳过
2. `DEBUG: clearTable - Target table '...' found: YES/NO` - 表示是否找到目标表
3. `DEBUG: clearTable - Remaining tables in file: ...` - 表示删除后剩余的表数量
4. `DEBUG: clearTable - Successfully rewrote .dat file, table '...' removed.` - 表示成功重写文件

如果看到 `found: NO`，说明表名匹配失败，可能是：
- 表名大小写不匹配（虽然使用了大小写不敏感比较）
- 表名包含特殊字符
- 文件路径问题

---

## 预期结果总结

- ✅ 删除表后，表不再出现在表列表中
- ✅ 删除表后，重新创建同名表时，新表应该是空的
- ✅ 新表中**不应该**包含旧表的任何数据
- ✅ 如果删除前有其他表的数据，其他表的数据应该保持不变

