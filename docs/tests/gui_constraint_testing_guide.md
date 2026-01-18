# GUI约束功能测试指南

> **最后更新**：2026-01-15

---

## 📋 测试概述

本指南提供了一个具体的场景化测试案例，用于验证约束功能的GUI实现。测试场景基于一个学生管理系统，包含学生表和课程表，演示UNIQUE约束、DEFAULT约束的使用。

---

## 🎯 测试场景：学生管理系统

### 场景描述

创建一个学生管理系统数据库，包含两个表：
1. **Students表**：存储学生基本信息
2. **Courses表**：存储课程信息

### 测试数据设计

**Students表结构**：
- `StudentID` (int, KEY, NOT_NULL, UNIQUE) - 学号，主键且唯一
- `Name` (string, NOT_NULL) - 姓名
- `Email` (string, NULL, UNIQUE, DEFAULT '') - 邮箱，唯一约束，默认值为空字符串
- `Age` (int, NULL, DEFAULT 18) - 年龄，默认值18
- `Grade` (string, NULL, DEFAULT 'Freshman') - 年级，默认值'Freshman'

**Courses表结构**：
- `CourseID` (int, KEY, NOT_NULL, UNIQUE) - 课程ID，主键且唯一
- `CourseName` (string, NOT_NULL) - 课程名称
- `Credits` (int, NOT_NULL, DEFAULT 3) - 学分，默认值3

---

## 🧪 详细测试步骤

### 阶段1：创建Students表

#### 步骤1.1：打开创建表对话框
1. 启动应用程序
2. 创建或打开数据库（例如：`student_db.dbf`）
3. 在Table Management界面，点击"Create"按钮
4. **预期结果**：弹出"Create Table"对话框

#### 步骤1.2：设置表名
1. 在"Table Name"输入框中输入：`Students`
2. **预期结果**：表名正确显示

#### 步骤1.3：添加第一个字段 - StudentID
1. 点击"Add Field"按钮
2. 在字段表格中，设置以下值：
   - **Field Name**: `StudentID`
   - **Type**: 选择 `int`（下拉框）
   - **Size**: `0`（int类型不需要size）
   - **KEY**: 选择 `KEY`（下拉框）
   - **NULL**: 选择 `NO_NULL`（下拉框）
   - **VALID**: 选择 `VALID`（下拉框）
   - **UNIQUE**: 选择 `UNIQUE`（下拉框，应该显示"NOT_UNIQUE"和"UNIQUE"两个选项）
   - **DEFAULT**: 留空（QLineEdit输入框）
3. **预期结果**：
   - UNIQUE列显示为下拉框，可以选择"NOT_UNIQUE"或"UNIQUE"
   - DEFAULT列显示为文本输入框，可以输入默认值
   - 所有字段值正确显示

#### 步骤1.4：添加第二个字段 - Name
1. 点击"Add Field"按钮
2. 设置字段：
   - **Field Name**: `Name`
   - **Type**: `string`
   - **Size**: `50`
   - **KEY**: `NOT_KEY`
   - **NULL**: `NO_NULL`
   - **VALID**: `VALID`
   - **UNIQUE**: `NOT_UNIQUE`
   - **DEFAULT**: 留空

#### 步骤1.5：添加第三个字段 - Email
1. 点击"Add Field"按钮
2. 设置字段：
   - **Field Name**: `Email`
   - **Type**: `string`
   - **Size**: `100`
   - **KEY**: `NOT_KEY`
   - **NULL**: `NULL`
   - **VALID**: `VALID`
   - **UNIQUE**: `UNIQUE`（重要：选择UNIQUE）
   - **DEFAULT**: 输入 `''`（空字符串，注意：这里应该可以输入文本）

#### 步骤1.6：添加第四个字段 - Age
1. 点击"Add Field"按钮
2. 设置字段：
   - **Field Name**: `Age`
   - **Type**: `int`
   - **Size**: `0`
   - **KEY**: `NOT_KEY`
   - **NULL**: `NULL`
   - **VALID**: `VALID`
   - **UNIQUE**: `NOT_UNIQUE`
   - **DEFAULT**: 输入 `18`（重要：测试DEFAULT值）

#### 步骤1.7：添加第五个字段 - Grade
1. 点击"Add Field"按钮
2. 设置字段：
   - **Field Name**: `Grade`
   - **Type**: `string`
   - **Size**: `20`
   - **KEY**: `NOT_KEY`
   - **NULL**: `NULL`
   - **VALID**: `VALID`
   - **UNIQUE**: `NOT_UNIQUE`
   - **DEFAULT**: 输入 `Freshman`

#### 步骤1.8：保存表
1. 点击"Create"按钮
2. **预期结果**：
   - 显示成功消息："Table created successfully."
   - Students表出现在表列表中

#### 步骤1.9：验证表结构
1. 在表列表中，选择"Students"表
2. 查看右侧的"Table Information"面板
3. **预期结果**：
   - 表名显示为"Table: Students"
   - 字段数量显示为"Fields: 5"
   - 字段表格显示所有5个字段
   - UNIQUE列显示"YES"或"NO"（文本显示）
   - DEFAULT列显示默认值（如果有）

---

### 阶段2：测试UNIQUE约束

#### 步骤2.1：插入第一条记录
1. 切换到Data Management界面
2. 选择"Students"表
3. 插入记录：
   - StudentID: `1001`
   - Name: `Alice`
   - Email: `alice@example.com`
   - Age: `20`
   - Grade: `Sophomore`
4. 点击"Insert"按钮
5. **预期结果**：记录成功插入

#### 步骤2.2：测试UNIQUE约束 - 违反StudentID唯一性
1. 尝试插入第二条记录：
   - StudentID: `1001`（与第一条相同，违反UNIQUE约束）
   - Name: `Bob`
   - Email: `bob@example.com`
   - Age: `21`
   - Grade: `Junior`
2. 点击"Insert"按钮
3. **预期结果**：
   - 如果约束检查已集成：显示错误消息，例如"Unique constraint violation: Field StudentID value already exists"
   - 如果约束检查未集成：可能允许插入（当前行为）

#### 步骤2.3：测试UNIQUE约束 - 违反Email唯一性
1. 插入记录：
   - StudentID: `1002`
   - Name: `Bob`
   - Email: `alice@example.com`（与第一条相同，违反UNIQUE约束）
   - Age: `21`
   - Grade: `Junior`
2. 点击"Insert"按钮
3. **预期结果**：
   - 如果约束检查已集成：显示错误消息
   - 如果约束检查未集成：可能允许插入

#### 步骤2.4：插入有效的唯一记录
1. 插入记录：
   - StudentID: `1002`
   - Name: `Bob`
   - Email: `bob@example.com`（不同的邮箱）
   - Age: `21`
   - Grade: `Junior`
2. 点击"Insert"按钮
3. **预期结果**：记录成功插入

---

### 阶段3：测试DEFAULT约束

#### 步骤3.1：测试Age字段的默认值
1. 插入记录，但**不填写Age字段**（留空）：
   - StudentID: `1003`
   - Name: `Charlie`
   - Email: `charlie@example.com`
   - Age: （留空）
   - Grade: `Senior`
2. 点击"Insert"按钮
3. **预期结果**：
   - 如果DEFAULT约束已集成：Age字段自动填充为18（默认值）
   - 如果DEFAULT约束未集成：Age字段可能为空或需要手动输入

#### 步骤3.2：测试Grade字段的默认值
1. 插入记录，但**不填写Grade字段**（留空）：
   - StudentID: `1004`
   - Name: `David`
   - Email: `david@example.com`
   - Age: `19`
   - Grade: （留空）
2. 点击"Insert"按钮
3. **预期结果**：
   - 如果DEFAULT约束已集成：Grade字段自动填充为"Freshman"（默认值）
   - 如果DEFAULT约束未集成：Grade字段可能为空

#### 步骤3.3：验证默认值是否正确应用
1. 查询所有记录：`SELECT * FROM Students;`
2. 检查记录：
   - StudentID=1003的记录，Age应该为18
   - StudentID=1004的记录，Grade应该为"Freshman"
3. **预期结果**：默认值正确应用

---

### 阶段4：编辑表结构

#### 步骤4.1：编辑Students表
1. 在Table Management界面，选择"Students"表
2. 点击"Edit"按钮
3. **预期结果**：弹出"Edit Table"对话框，显示当前表结构

#### 步骤4.2：验证约束信息加载
1. 检查字段表格中的UNIQUE和DEFAULT列
2. **预期结果**：
   - UNIQUE列显示为下拉框，当前值正确显示（StudentID和Email应该是"UNIQUE"，其他是"NOT_UNIQUE"）
   - DEFAULT列显示为文本输入框，当前默认值正确显示（Age显示"18"，Grade显示"Freshman"，其他为空）

#### 步骤4.3：修改约束
1. 找到"Name"字段行
2. 将UNIQUE从"NOT_UNIQUE"改为"UNIQUE"
3. 将DEFAULT设置为`'Unknown'`
4. 点击"Edit"按钮保存
5. **预期结果**：表结构成功更新

#### 步骤4.4：验证修改后的约束
1. 尝试插入两条Name相同的记录
2. **预期结果**：
   - 如果约束检查已集成：第二条记录应该被拒绝（违反UNIQUE约束）
   - 如果约束检查未集成：可能允许插入

---

## ✅ 预期测试结果总结

### GUI界面测试

| 测试项 | 预期结果 | 实际结果 | 状态 |
|--------|----------|----------|------|
| UNIQUE列显示 | 下拉框，选项：NOT_UNIQUE, UNIQUE | | ⬜ |
| DEFAULT列显示 | 文本输入框，可输入默认值 | | ⬜ |
| 创建表时设置UNIQUE | 可以正确选择UNIQUE或NOT_UNIQUE | | ⬜ |
| 创建表时设置DEFAULT | 可以输入默认值文本 | | ⬜ |
| 编辑表时加载UNIQUE | UNIQUE值正确显示在下拉框中 | | ⬜ |
| 编辑表时加载DEFAULT | DEFAULT值正确显示在文本框中 | | ⬜ |

### 约束功能测试

| 测试项 | 预期结果 | 实际结果 | 状态 |
|--------|----------|----------|------|
| UNIQUE约束检查 | 违反唯一性时显示错误 | | ⬜ |
| DEFAULT值应用 | 空值时自动填充默认值 | | ⬜ |
| 约束信息保存 | 约束信息正确保存到.dbf文件 | | ⬜ |
| 约束信息加载 | 重新打开表时约束信息正确加载 | | ⬜ |

---

## 🧪 SQL执行界面测试

### 阶段5：使用SQL语句测试约束功能

**前提条件**：确保已经创建了数据库（例如：`student_db.dbf`）

> **注意**：以下测试用例涵盖了字段级约束（UNIQUE、DEFAULT）和表级约束（外键、多字段唯一约束、检查约束）的SQL执行测试。

#### 步骤5.1：创建Students表的SQL语句
1. 切换到SQL Query界面
2. 输入以下SQL语句创建Students表（注意：根据实际数据库文件名调整`INTO`后的数据库名）：
```sql
CREATE TABLE Students (
    StudentID int KEY NO_NULL VALID UNIQUE,
    Name string[50] NOT_KEY NO_NULL VALID,
    Email string[100] NOT_KEY NULL VALID UNIQUE DEFAULT '',
    Age int NOT_KEY NULL VALID DEFAULT 18,
    Grade string[20] NOT_KEY NULL VALID DEFAULT 'Freshman'
) INTO student_db;
```
3. 点击"Execute SQL"按钮
4. **预期结果**：
   - 执行成功，显示"Query executed successfully"
   - Students表出现在Table Management界面
   - 在Table Management界面中，可以查看Students表的结构，验证UNIQUE和DEFAULT列是否正确显示

#### 步骤5.2：插入记录的SQL语句 - 测试DEFAULT值（Age字段）
1. 输入以下SQL语句（Age字段留空，使用空字符串''，测试DEFAULT值）：
```sql
INSERT INTO Students VALUES (1001, 'Alice', 'alice@example.com', '', 'Senior') IN student_db;
```
2. 点击"Execute SQL"按钮
3. **预期结果**：
   - 执行成功，显示"Query executed successfully"
   - Age字段自动填充为18（DEFAULT值）

#### 步骤5.3：插入记录的SQL语句 - 测试DEFAULT值（Grade字段）
1. 输入以下SQL语句（Grade字段留空，使用空字符串''，测试DEFAULT值）：
```sql
INSERT INTO Students VALUES (1002, 'Bob', 'bob@example.com', 21, '') IN student_db;
```
2. 点击"Execute SQL"按钮
3. **预期结果**：
   - 执行成功
   - Grade字段自动填充为'Freshman'（DEFAULT值）

#### 步骤5.4：查询记录验证DEFAULT值
1. 输入以下SQL语句：
```sql
SELECT * FROM Students;
```
2. 点击"Execute SQL"按钮
3. **预期结果**：
   - 返回2条记录
   - StudentID=1001的记录：Age=18（DEFAULT值已应用），Grade='Senior'（用户指定值）
   - StudentID=1002的记录：Age=21（用户指定值），Grade='Freshman'（DEFAULT值已应用）

#### 步骤5.5：插入记录的SQL语句 - 测试UNIQUE约束（Email字段）
1. 输入以下SQL语句（使用已存在的Email，违反UNIQUE约束）：
```sql
INSERT INTO Students VALUES (1003, 'Charlie', 'alice@example.com', 22, 'Junior') IN student_db;
```
2. 点击"Execute SQL"按钮
3. **预期结果**：
   - 执行失败
   - 显示错误消息："Unique constraint violation: value 'alice@example.com' already exists in field 'Email'"

#### 步骤5.6：插入记录的SQL语句 - 测试UNIQUE约束（StudentID字段）
1. 输入以下SQL语句（使用已存在的StudentID，违反主键唯一性约束）：
```sql
INSERT INTO Students VALUES (1001, 'David', 'david@example.com', 23, 'Senior') IN student_db;
```
2. 点击"Execute SQL"按钮
3. **预期结果**：
   - 执行失败
   - 显示错误消息："Primary key constraint violation: Field (StudentID) value already exists"

#### 步骤5.7：插入有效记录的SQL语句
1. 输入以下SQL语句（所有字段值都是唯一的）：
```sql
INSERT INTO Students VALUES (1003, 'Charlie', 'charlie@example.com', 22, 'Junior') IN student_db;
```
2. 点击"Execute SQL"按钮
3. **预期结果**：
   - 执行成功
   - 显示"Query executed successfully"

#### 步骤5.8：查询所有记录验证数据
1. 输入以下SQL语句：
```sql
SELECT * FROM Students;
```
2. 点击"Execute SQL"按钮
3. **预期结果**：
   - 返回3条记录
   - 所有记录的数据正确显示
   - DEFAULT值已正确应用

#### 步骤5.9：测试UPDATE语句的UNIQUE约束检查
1. 输入以下SQL语句（尝试将StudentID=1003的Email更新为已存在的值）：
```sql
UPDATE Students (
    SET Email='alice@example.com'
    WHERE StudentID=1003
) IN student_db;
```
2. 点击"Execute SQL"按钮
3. **预期结果**：
   - 执行失败
   - 显示错误消息："Unique constraint violation: value 'alice@example.com' already exists in field 'Email'"

#### 步骤5.10：测试UPDATE语句的DEFAULT值应用
1. 输入以下SQL语句（将Age更新为空字符串，测试DEFAULT值）：
```sql
UPDATE Students (
    SET Age=''
    WHERE StudentID=1003
) IN student_db;
```
2. 点击"Execute SQL"按钮
3. **预期结果**：
   - 如果DEFAULT约束在UPDATE中应用：Age更新为18（DEFAULT值）
   - 如果DEFAULT约束不在UPDATE中应用：Age可能为空或保持原值
   - 注意：当前实现可能不支持UPDATE时的DEFAULT值应用

#### 步骤5.11：验证UPDATE后的数据
1. 输入以下SQL语句：
```sql
SELECT * FROM Students WHERE StudentID=1003;
```
2. 点击"Execute SQL"按钮
3. **预期结果**：
   - 返回StudentID=1003的记录
   - 检查Age字段的值（根据步骤5.10的实现情况）

---

### 阶段6：综合测试场景

#### 步骤6.1：创建Courses表（带约束）
1. 输入以下SQL语句：
```sql
CREATE TABLE Courses (
    CourseID int KEY NO_NULL VALID UNIQUE,
    CourseName string[100] NOT_KEY NO_NULL VALID,
    Credits int NOT_KEY NO_NULL VALID DEFAULT 3
) INTO student_db;
```
2. 点击"Execute SQL"按钮
3. **预期结果**：执行成功

#### 步骤6.2：插入Courses记录（测试DEFAULT值）
1. 输入以下SQL语句（Credits字段留空，测试DEFAULT值）：
```sql
INSERT INTO Courses VALUES (101, 'Database Systems', '') IN student_db;
```
2. 点击"Execute SQL"按钮
3. **预期结果**：
   - 执行成功
   - Credits字段自动填充为3（DEFAULT值）

#### 步骤6.3：验证Courses表数据
1. 输入以下SQL语句：
```sql
SELECT * FROM Courses;
```
2. 点击"Execute SQL"按钮
3. **预期结果**：
   - 返回1条记录
   - CourseID=101，CourseName='Database Systems'，Credits=3（DEFAULT值已应用）

---

### 阶段7：测试外键约束（FOREIGN KEY）

#### 步骤7.1：创建带外键约束的表
1. 首先创建被引用的表（Departments）：
```sql
CREATE TABLE Departments (
    DeptID int KEY NO_NULL VALID,
    DeptName string[50] NOT_KEY NO_NULL VALID
) INTO student_db;
```
2. 点击"Execute SQL"按钮
3. **预期结果**：执行成功，Departments表已创建

4. 创建引用表（Employees），带外键约束：
```sql
CREATE TABLE Employees (
    EmpID int KEY NO_NULL VALID,
    EmpName string[50] NOT_KEY NO_NULL VALID,
    DeptID int NOT_KEY NULL VALID,
    FOREIGN KEY (DeptID) REFERENCES Departments(DeptID) ON DELETE RESTRICT
) INTO student_db;
```
5. 点击"Execute SQL"按钮
6. **预期结果**：
   - 执行成功，Employees表已创建
   - 外键约束已注册到约束注册表

#### 步骤7.2：测试外键约束 - 插入无效引用
1. 尝试插入引用不存在DeptID的记录：
```sql
INSERT INTO Employees VALUES (1, 'John', 999) IN student_db;
```
2. 点击"Execute SQL"按钮
3. **预期结果**：
   - 执行失败
   - 显示错误消息："Foreign key constraint violation: value '999' not found in referenced table 'Departments' field 'DeptID'"

#### 步骤7.3：测试外键约束 - 插入有效引用
1. 先插入Department记录：
```sql
INSERT INTO Departments VALUES (1, 'IT') IN student_db;
```
2. 插入引用有效DeptID的记录：
```sql
INSERT INTO Employees VALUES (1, 'John', 1) IN student_db;
```
3. 点击"Execute SQL"按钮
4. **预期结果**：
   - 执行成功
   - 记录成功插入

#### 步骤7.4：测试外键约束 - DELETE RESTRICT
1. 尝试删除被引用的Department记录：
```sql
DELETE FROM Departments WHERE DeptID=1 IN student_db;
```
2. 点击"Execute SQL"按钮
3. **预期结果**：
   - 执行失败
   - 显示错误消息："Cannot delete record: Foreign key constraint violation. [约束名] in table 'Employees' references this record"

#### 步骤7.5：测试外键约束 - DELETE CASCADE
1. 删除Employees表并重新创建，使用CASCADE模式：
```sql
DROP TABLE Employees IN student_db;

CREATE TABLE Employees (
    EmpID int KEY NO_NULL VALID,
    EmpName string[50] NOT_KEY NO_NULL VALID,
    DeptID int NOT_KEY NULL VALID,
    FOREIGN KEY (DeptID) REFERENCES Departments(DeptID) ON DELETE CASCADE
) INTO student_db;
```
2. 插入测试数据：
```sql
INSERT INTO Employees VALUES (1, 'John', 1) IN student_db;
INSERT INTO Employees VALUES (2, 'Jane', 1) IN student_db;
```
3. 删除Department记录：
```sql
DELETE FROM Departments WHERE DeptID=1 IN student_db;
```
4. 点击"Execute SQL"按钮
5. **预期结果**：
   - 执行成功
   - Department记录被删除
   - 引用该Department的所有Employees记录也被级联删除

6. 验证级联删除：
```sql
SELECT * FROM Employees;
```
7. **预期结果**：返回0条记录（所有Employees记录已被级联删除）

#### 步骤7.6：测试外键约束 - DELETE SET NULL
1. 删除Employees表并重新创建，使用SET NULL模式：
```sql
DROP TABLE Employees IN student_db;

CREATE TABLE Employees (
    EmpID int KEY NO_NULL VALID,
    EmpName string[50] NOT_KEY NO_NULL VALID,
    DeptID int NOT_KEY NULL VALID,
    FOREIGN KEY (DeptID) REFERENCES Departments(DeptID) ON DELETE SET NULL
) INTO student_db;
```
2. 插入Department和Employees记录：
```sql
INSERT INTO Departments VALUES (2, 'HR') IN student_db;
INSERT INTO Employees VALUES (3, 'Bob', 2) IN student_db;
```
3. 删除Department记录：
```sql
DELETE FROM Departments WHERE DeptID=2 IN student_db;
```
4. 点击"Execute SQL"按钮
5. **预期结果**：
   - 执行成功
   - Department记录被删除
   - Employees记录中的DeptID字段被设置为NULL（空字符串）

6. 验证SET NULL：
```sql
SELECT * FROM Employees WHERE EmpID=3;
```
7. **预期结果**：返回1条记录，DeptID字段为空（NULL）

---

### 阶段8：测试多字段唯一约束（UNIQUE）

#### 步骤8.1：创建带多字段唯一约束的表
1. 输入以下SQL语句：
```sql
CREATE TABLE Orders (
    OrderID int KEY NO_NULL VALID,
    CustomerID int NOT_KEY NO_NULL VALID,
    ProductID int NOT_KEY NO_NULL VALID,
    Quantity int NOT_KEY NULL VALID,
    UNIQUE (CustomerID, ProductID)
) INTO student_db;
```
2. 点击"Execute SQL"按钮
3. **预期结果**：
   - 执行成功
   - Orders表已创建
   - 多字段唯一约束已注册

#### 步骤8.2：插入第一条记录
1. 输入以下SQL语句：
```sql
INSERT INTO Orders VALUES (1, 100, 200, 5) IN student_db;
```
2. 点击"Execute SQL"按钮
3. **预期结果**：执行成功

#### 步骤8.3：测试多字段唯一约束 - 违反约束
1. 尝试插入违反唯一约束的记录（相同的CustomerID和ProductID组合）：
```sql
INSERT INTO Orders VALUES (2, 100, 200, 10) IN student_db;
```
2. 点击"Execute SQL"按钮
3. **预期结果**：
   - 执行失败
   - 显示错误消息："Unique constraint violation: combination of fields (CustomerID, ProductID) already exists"

#### 步骤8.4：插入有效的唯一组合
1. 插入不同的组合：
```sql
INSERT INTO Orders VALUES (2, 100, 201, 3) IN student_db;
INSERT INTO Orders VALUES (3, 101, 200, 7) IN student_db;
```
2. 点击"Execute SQL"按钮
3. **预期结果**：执行成功（不同的CustomerID和ProductID组合）

#### 步骤8.5：测试UPDATE时的多字段唯一约束
1. 尝试更新记录，使其违反唯一约束：
```sql
UPDATE Orders (
    SET ProductID=200
    WHERE OrderID=2
) IN student_db;
```
2. 点击"Execute SQL"按钮
3. **预期结果**：
   - 执行失败
   - 显示错误消息："Unique constraint violation: combination of fields (CustomerID, ProductID) already exists"

---

### 阶段9：测试检查约束（CHECK）

#### 步骤9.1：创建带检查约束的表
1. 输入以下SQL语句：
```sql
CREATE TABLE Products (
    ProductID int KEY NO_NULL VALID,
    ProductName string[50] NOT_KEY NO_NULL VALID,
    Price float NOT_KEY NULL VALID,
    Stock int NOT_KEY NULL VALID,
    CHECK (Price > 0),
    CHECK (Stock >= 0)
) INTO student_db;
```
2. 点击"Execute SQL"按钮
3. **预期结果**：
   - 执行成功
   - Products表已创建
   - 检查约束已注册

#### 步骤9.2：测试检查约束 - 违反Price约束
1. 尝试插入违反检查约束的记录（Price <= 0）：
```sql
INSERT INTO Products VALUES (1, 'Product1', -10, 100) IN student_db;
```
2. 点击"Execute SQL"按钮
3. **预期结果**：
   - 执行失败
   - 显示错误消息："Check constraint violation: value '-10' does not satisfy constraint 'Price > 0'"

#### 步骤9.3：测试检查约束 - 违反Stock约束
1. 尝试插入违反检查约束的记录（Stock < 0）：
```sql
INSERT INTO Products VALUES (2, 'Product2', 10.5, -5) IN student_db;
```
2. 点击"Execute SQL"按钮
3. **预期结果**：
   - 执行失败
   - 显示错误消息："Check constraint violation: value '-5' does not satisfy constraint 'Stock >= 0'"

#### 步骤9.4：插入符合检查约束的记录
1. 插入有效的记录：
```sql
INSERT INTO Products VALUES (1, 'Product1', 10.5, 100) IN student_db;
INSERT INTO Products VALUES (2, 'Product2', 25.0, 0) IN student_db;
```
2. 点击"Execute SQL"按钮
3. **预期结果**：执行成功

#### 步骤9.5：测试UPDATE时的检查约束
1. 尝试更新记录，使其违反检查约束：
```sql
UPDATE Products (
    SET Price=-5
    WHERE ProductID=1
) IN student_db;
```
2. 点击"Execute SQL"按钮
3. **预期结果**：
   - 执行失败
   - 显示错误消息："Check constraint violation: value '-5' does not satisfy constraint 'Price > 0'"

---

### 阶段10：GUI操作测试 - 外键约束

#### 步骤10.1：在GUI中创建带外键约束的表
1. 在Table Management界面，点击"Create"按钮
2. 设置表名：`Employees`
3. 添加字段：
   - `EmpID` (int, KEY, NO_NULL, VALID)
   - `EmpName` (string[50], NOT_KEY, NO_NULL, VALID)
   - `DeptID` (int, NOT_KEY, NULL, VALID)
4. **注意**：当前GUI界面可能不支持直接创建外键约束，需要通过SQL语句创建
5. **预期结果**：表创建成功（字段级约束可通过GUI设置）

#### 步骤10.2：通过SQL创建外键约束后，在GUI中测试
1. 使用SQL语句创建带外键约束的表（参考阶段7.1）
2. 在Data Management界面，选择"Employees"表
3. 尝试插入记录：
   - EmpID: `1`
   - EmpName: `John`
   - DeptID: `999`（不存在的DeptID）
4. 点击"Insert"按钮
5. **预期结果**：
   - 插入失败
   - 显示错误消息："Foreign key constraint violation: value '999' not found in referenced table 'Departments' field 'DeptID'"

#### 步骤10.3：在GUI中测试外键约束的有效引用
1. 先在Data Management界面，选择"Departments"表，插入记录：
   - DeptID: `1`
   - DeptName: `IT`
2. 切换到"Employees"表，插入记录：
   - EmpID: `1`
   - EmpName: `John`
   - DeptID: `1`（有效的DeptID）
3. 点击"Insert"按钮
4. **预期结果**：插入成功

---

### 阶段11：GUI操作测试 - 多字段唯一约束

#### 步骤11.1：通过SQL创建带多字段唯一约束的表
1. 使用SQL语句创建Orders表（参考阶段8.1）
2. 在Data Management界面，选择"Orders"表
3. 插入第一条记录：
   - OrderID: `1`
   - CustomerID: `100`
   - ProductID: `200`
   - Quantity: `5`
4. 点击"Insert"按钮
5. **预期结果**：插入成功

#### 步骤11.2：在GUI中测试多字段唯一约束
1. 尝试插入违反唯一约束的记录：
   - OrderID: `2`
   - CustomerID: `100`（与第一条相同）
   - ProductID: `200`（与第一条相同）
   - Quantity: `10`
2. 点击"Insert"按钮
3. **预期结果**：
   - 插入失败
   - 显示错误消息："Unique constraint violation: combination of fields (CustomerID, ProductID) already exists"

---

### 阶段12：GUI操作测试 - 检查约束

#### 步骤12.1：通过SQL创建带检查约束的表
1. 使用SQL语句创建Products表（参考阶段9.1）
2. 在Data Management界面，选择"Products"表
3. 尝试插入违反检查约束的记录：
   - ProductID: `1`
   - ProductName: `Product1`
   - Price: `-10`（违反Price > 0约束）
   - Stock: `100`
4. 点击"Insert"按钮
5. **预期结果**：
   - 插入失败
   - 显示错误消息："Check constraint violation: value '-10' does not satisfy constraint 'Price > 0'"

#### 步骤12.2：在GUI中插入符合检查约束的记录
1. 插入有效的记录：
   - ProductID: `1`
   - ProductName: `Product1`
   - Price: `10.5`
   - Stock: `100`
2. 点击"Insert"按钮
3. **预期结果**：插入成功

---

## 📊 约束功能测试总结表

### 字段级约束测试

| 测试项 | SQL执行 | GUI操作 | 预期结果 | 状态 |
|--------|---------|---------|----------|------|
| UNIQUE约束检查（单字段） | ✅ | ✅ | 违反时显示错误 | ⬜ |
| DEFAULT值应用（INSERT） | ✅ | ✅ | 空值时自动填充 | ⬜ |
| DEFAULT值应用（UPDATE） | ⚠️ | ⚠️ | 可能不支持 | ⬜ |

### 表级约束测试

| 测试项 | SQL执行 | GUI操作 | 预期结果 | 状态 |
|--------|---------|---------|----------|------|
| 外键约束创建 | ✅ | ⚠️ | SQL支持，GUI需通过SQL | ⬜ |
| 外键约束检查（INSERT） | ✅ | ✅ | 无效引用时显示错误 | ⬜ |
| 外键约束检查（UPDATE） | ✅ | ✅ | 无效引用时显示错误 | ⬜ |
| 外键约束DELETE RESTRICT | ✅ | ✅ | 阻止删除被引用的记录 | ⬜ |
| 外键约束DELETE CASCADE | ✅ | ✅ | 级联删除引用记录 | ⬜ |
| 外键约束DELETE SET NULL | ✅ | ✅ | 将引用字段设置为NULL | ⬜ |
| 多字段唯一约束创建 | ✅ | ⚠️ | SQL支持，GUI需通过SQL | ⬜ |
| 多字段唯一约束检查 | ✅ | ✅ | 违反时显示错误 | ⬜ |
| 检查约束创建 | ✅ | ⚠️ | SQL支持，GUI需通过SQL | ⬜ |
| 检查约束检查 | ✅ | ✅ | 违反时显示错误 | ⬜ |

---

## ⚠️ 注意事项

### 当前实现状态

1. **GUI界面**：✅ UNIQUE和DEFAULT列已添加到字段表格
2. **数据结构**：✅ 已扩展，支持约束字段
3. **约束管理器**：✅ 已实现核心检查逻辑
4. **SQL解析器**：✅ 已扩展约束语法（字段级和表级约束）
5. **DML集成**：✅ 已集成约束检查（INSERT、UPDATE、DELETE）
6. **约束注册表**：✅ 已实现内存存储（ConstraintRegistry）
7. **DDL约束验证**：✅ 已实现外键约束验证（引用表存在、字段存在等）
8. **外键级联操作**：✅ 已实现RESTRICT、CASCADE、SET NULL

### 已知限制

1. **约束存储**：当前使用内存存储（ConstraintRegistry），重启后约束信息会丢失。持久化存储待后续实现。
2. **GUI约束创建**：表级约束（外键、多字段唯一约束、检查约束）需要通过SQL语句创建，GUI界面暂不支持直接创建。
3. **UPDATE DEFAULT值**：UPDATE操作时，DEFAULT值可能不会自动应用（需要明确实现）。
4. **多层级联**：外键级联删除和更新只处理一层，多层嵌套需要递归处理。

### 测试注意事项

1. **约束注册表清理**：测试前建议清理约束注册表，使用SQL语句删除表时会自动清理相关约束。
2. **数据库切换**：切换数据库时，约束注册表会自动清理对应数据库的约束信息。
3. **错误消息**：约束违反时，错误消息应该清晰显示约束类型和违反的字段/值。

---

## 📝 测试报告模板

测试完成后，请记录以下信息：

```
测试日期：YYYY-MM-DD
测试人员：[您的名字]

测试结果：

字段级约束：
1. GUI界面 - UNIQUE列：✅/❌
2. GUI界面 - DEFAULT列：✅/❌
3. UNIQUE约束检查（单字段）：✅/❌
4. DEFAULT值应用（INSERT）：✅/❌
5. DEFAULT值应用（UPDATE）：✅/❌
6. 约束信息保存：✅/❌
7. 约束信息加载：✅/❌

表级约束：
8. 外键约束创建：✅/❌
9. 外键约束检查（INSERT）：✅/❌
10. 外键约束检查（UPDATE）：✅/❌
11. 外键约束DELETE RESTRICT：✅/❌
12. 外键约束DELETE CASCADE：✅/❌
13. 外键约束DELETE SET NULL：✅/❌
14. 多字段唯一约束创建：✅/❌
15. 多字段唯一约束检查：✅/❌
16. 检查约束创建：✅/❌
17. 检查约束检查：✅/❌

发现的问题：
1. [问题描述]
2. [问题描述]

建议：
1. [建议内容]
2. [建议内容]
```

---

**最后更新**：2026-01-15

---

## 🔗 相关文档

- [约束实现状态文档](../development/constraint_implementation_status.md)
- [约束实现进度文档](../development/constraint_implementation_progress.md)
- [项目任务指南](../development/project_task_guide.md)
