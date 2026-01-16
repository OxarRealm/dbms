# GUI图形化操作全面测试指南

> **最后更新**：2026-01-16
> 
> 本文档提供基于GUI图形化操作的全面测试指南，覆盖DBMS系统的所有功能。测试采用场景化设计，前后操作连贯，可以一次性完成所有测试。

---

## 📋 测试概述

### 测试目标
验证DBMS系统通过GUI界面实现的所有功能，包括：
- 数据库管理（创建、打开）
- 表结构管理（创建、编辑、重命名、删除）
- 数据操作（插入、更新、删除、查看）
- 约束管理（主键、唯一、默认值、外键、检查约束）
- 索引管理（创建、查看、删除）

### 测试场景
基于一个**学生管理系统**，包含以下表：
- **Students表**：学生基本信息
- **Courses表**：课程信息
- **Enrollments表**：选课记录（连接Students和Courses）

### 前置条件
1. 已编译并运行DBMS应用程序
2. 准备好测试数据（本指南会逐步创建）

---

## 🎯 第一部分：数据库和表结构管理

### 步骤1.1：创建数据库

1. 启动应用程序
2. 点击菜单 **File** -> **Create Database**（或按 `Ctrl+N`）
3. 在文件对话框中选择保存位置
4. 输入数据库名称：`student_management`
5. 点击 **Save**
6. **预期结果**：
   - 显示成功消息框："Database 'student_management' created successfully."
   - 状态栏显示当前数据库：`student_management`

---

### 步骤1.2：创建Students表

1. 导航到 **Table Management** 标签页
2. 点击 **Create Table** 按钮
3. 在对话框中：
   - **Table Name**: `Students`
   - 点击 **Add Field** 添加字段：
     - Field 1:
       - Name: `StudentID`
       - Type: `int`
       - Size: `4`（自动）
       - KEY: `KEY`
       - NULL: `NO_NULL`
       - VALID: `VALID`
       - UNIQUE: `UNIQUE`
       - DEFAULT: （留空）
     - Field 2:
       - Name: `Name`
       - Type: `string`
       - Size: `0`（变长）
       - KEY: `NOT_KEY`
       - NULL: `NO_NULL`
       - VALID: `VALID`
       - UNIQUE: `NOT_UNIQUE`
       - DEFAULT: （留空）
     - Field 3:
       - Name: `Email`
       - Type: `string`
       - Size: `0`
       - KEY: `NOT_KEY`
       - NULL: `NULL`
       - VALID: `VALID`
       - UNIQUE: `UNIQUE`
       - DEFAULT: `''`（空字符串）
     - Field 4:
       - Name: `Age`
       - Type: `int`
       - Size: `4`
       - KEY: `NOT_KEY`
       - NULL: `NULL`
       - VALID: `VALID`
       - UNIQUE: `NOT_UNIQUE`
       - DEFAULT: `18`
     - Field 5:
       - Name: `Grade`
       - Type: `string`
       - Size: `0`
       - KEY: `NOT_KEY`
       - NULL: `NULL`
       - VALID: `VALID`
       - UNIQUE: `NOT_UNIQUE`
       - DEFAULT: `'Freshman'`
4. 点击 **Create** 按钮
5. **预期结果**：
   - 显示成功消息框："Table created successfully."
   - Students表出现在左侧表列表中
   - 右侧显示表信息：5个字段

---

### 步骤1.3：创建Courses表

1. 在 **Table Management** 标签页，点击 **Create Table**
2. 在对话框中：
   - **Table Name**: `Courses`
   - 添加字段：
     - Field 1:
       - Name: `CourseID`
       - Type: `int`
       - KEY: `KEY`
       - NULL: `NO_NULL`
       - VALID: `VALID`
       - UNIQUE: `UNIQUE`
     - Field 2:
       - Name: `CourseName`
       - Type: `string`
       - KEY: `NOT_KEY`
       - NULL: `NO_NULL`
       - VALID: `VALID`
     - Field 3:
       - Name: `Credits`
       - Type: `int`
       - KEY: `NOT_KEY`
       - NULL: `NULL`
       - VALID: `VALID`
       - DEFAULT: `3`
     - Field 4:
       - Name: `Price`
       - Type: `float`
       - KEY: `NOT_KEY`
       - NULL: `NULL`
       - VALID: `VALID`
       - DEFAULT: `0.0`
3. 切换到 **Constraints** 标签页
4. 添加CHECK约束：
   - 点击 **Add Check Constraint**
   - Field: `Price`
   - Expression: `> 0 AND < 10000`
   - Constraint Name: `PriceRange`
   - 点击 **OK**
5. 点击 **Create** 按钮
6. **预期结果**：
   - 显示成功消息框
   - Courses表出现在表列表中
   - 表包含4个字段和1个CHECK约束

---

### 步骤1.4：创建Enrollments表（带外键约束）

1. 在 **Table Management** 标签页，点击 **Create Table**
2. 在对话框中：
   - **Table Name**: `Enrollments`
   - 添加字段：
     - Field 1:
       - Name: `EnrollmentID`
       - Type: `int`
       - KEY: `KEY`
       - NULL: `NO_NULL`
       - VALID: `VALID`
     - Field 2:
       - Name: `StudentID`
       - Type: `int`
       - KEY: `NOT_KEY`
       - NULL: `NO_NULL`
       - VALID: `VALID`
     - Field 3:
       - Name: `CourseID`
       - Type: `int`
       - KEY: `NOT_KEY`
       - NULL: `NO_NULL`
       - VALID: `VALID`
     - Field 4:
       - Name: `Grade`
       - Type: `string`
       - KEY: `NOT_KEY`
       - NULL: `NULL`
       - VALID: `VALID`
3. 切换到 **Constraints** 标签页
4. 添加外键约束：
   - 点击 **Add Foreign Key**
   - Foreign Key Field: `StudentID`
   - References Table: `Students`
   - References Field: `StudentID`
   - ON DELETE: `CASCADE`
   - ON UPDATE: `RESTRICT`
   - Constraint Name: `FK_Enrollments_Students`
   - 点击 **OK**
   - 再次点击 **Add Foreign Key**
   - Foreign Key Field: `CourseID`
   - References Table: `Courses`
   - References Field: `CourseID`
   - ON DELETE: `CASCADE`
   - ON UPDATE: `RESTRICT`
   - Constraint Name: `FK_Enrollments_Courses`
   - 点击 **OK**
5. 添加多字段唯一约束：
   - 点击 **Add Unique Constraint**
   - Fields: 选择 `StudentID` 和 `CourseID`（按住Ctrl多选）
   - Constraint Name: `UQ_Enrollments_Student_Course`
   - 点击 **OK**
6. 点击 **Create** 按钮
7. **预期结果**：
   - 显示成功消息框
   - Enrollments表出现在表列表中
   - 表包含4个字段、2个外键约束和1个多字段唯一约束

---

### 步骤1.5：验证表结构

1. 在 **Table Management** 标签页，依次点击三个表（Students, Courses, Enrollments）
2. 检查右侧表信息显示：
   - Students表：5个字段，StudentID为KEY
   - Courses表：4个字段，CourseID为KEY，有CHECK约束
   - Enrollments表：4个字段，EnrollmentID为KEY，有外键约束
3. 对于Enrollments表，点击 **View Constraints** 按钮
4. **预期结果**：
   - 显示所有约束信息（2个外键，1个多字段唯一约束）

---

## 🎯 第二部分：数据操作（基础功能）

### 步骤2.1：插入Students表数据

1. 导航到 **Data Operation** 标签页
2. 在表下拉框中选择 `Students`
3. 点击 **Insert Record** 按钮
4. 在对话框中输入：
   - StudentID: `1`
   - Name: `Alice`
   - Email: `alice@example.com`
   - Age: （留空，使用默认值18）
   - Grade: （留空，使用默认值'Freshman'）
5. 点击 **OK**
6. **预期结果**：
   - 显示成功消息框："Record inserted successfully."
   - 数据表格显示1条记录，Age=18, Grade='Freshman'（默认值生效）

7. 再次点击 **Insert Record**，输入：
   - StudentID: `2`
   - Name: `Bob`
   - Email: `bob@example.com`
   - Age: `20`
   - Grade: `'Sophomore'`
8. 点击 **OK**
9. **预期结果**：数据表格显示2条记录

10. 再次点击 **Insert Record**，输入：
    - StudentID: `3`
    - Name: `Charlie`
    - Email: （留空，使用默认值''）
    - Age: `19`
    - Grade: `'Junior'`
11. 点击 **OK**
12. **预期结果**：数据表格显示3条记录，第3条Email为空字符串（默认值生效）

---

### 步骤2.2：测试唯一约束（违反Email唯一性）

1. 在 **Data Operation** 标签页，选择 `Students` 表
2. 点击 **Insert Record**
3. 输入：
   - StudentID: `4`
   - Name: `David`
   - Email: `alice@example.com`（与第1条记录重复）
   - Age: `21`
   - Grade: `'Senior'`
4. 点击 **OK**
5. **预期结果**：
   - 显示错误消息框："Failed to insert record: Unique constraint violation: Field (Email) value already exists"
   - 记录未插入，数据表格仍显示3条记录

---

### 步骤2.3：测试主键唯一约束

1. 点击 **Insert Record**
2. 输入：
   - StudentID: `1`（与第1条记录重复）
   - Name: `Eve`
   - Email: `eve@example.com`
   - Age: `22`
   - Grade: `'Senior'`
3. 点击 **OK**
4. **预期结果**：
   - 显示错误消息框："Primary key constraint violation: Field (StudentID) value already exists"
   - 记录未插入

---

### 步骤2.4：插入Courses表数据

1. 在表下拉框中选择 `Courses`
2. 点击 **Insert Record**
3. 输入：
   - CourseID: `1`
   - CourseName: `Database Systems`
   - Credits: （留空，使用默认值3）
   - Price: `1500.50`
4. 点击 **OK**
5. **预期结果**：显示成功消息框，数据表格显示1条记录，Credits=3（默认值）

6. 再次插入：
   - CourseID: `2`
   - CourseName: `Operating Systems`
   - Credits: `4`
   - Price: `1800.00`
7. 点击 **OK**
8. **预期结果**：数据表格显示2条记录

---

### 步骤2.5：测试CHECK约束（违反Price约束）

1. 在 **Data Operation** 标签页，选择 `Courses` 表
2. 点击 **Insert Record**
3. 输入：
   - CourseID: `3`
   - CourseName: `Invalid Course`
   - Credits: `2`
   - Price: `-100`（违反CHECK约束：Price > 0 AND Price < 10000）
4. 点击 **OK**
5. **预期结果**：
   - 显示错误消息框："Failed to insert record: CHECK constraint violation: Field (Price) value (-100) does not satisfy constraint: > 0 AND < 10000"
   - 记录未插入

6. 再次尝试插入：
   - CourseID: `3`
   - CourseName: `Invalid Course 2`
   - Credits: `2`
   - Price: `15000`（违反CHECK约束：Price < 10000）
7. 点击 **OK**
8. **预期结果**：
   - 显示错误消息框："CHECK constraint violation: Field (Price) value (15000) does not satisfy constraint: > 0 AND < 10000"
   - 记录未插入

---

### 步骤2.6：插入Enrollments表数据

1. 在表下拉框中选择 `Enrollments`
2. 点击 **Insert Record**
3. 输入：
   - EnrollmentID: `1`
   - StudentID: `1`（引用Students表的StudentID=1）
   - CourseID: `1`（引用Courses表的CourseID=1）
   - Grade: `'A'`
4. 点击 **OK**
5. **预期结果**：显示成功消息框，数据表格显示1条记录

6. 再次插入：
   - EnrollmentID: `2`
   - StudentID: `1`
   - CourseID: `2`
   - Grade: `'B'`
7. 点击 **OK**
8. **预期结果**：数据表格显示2条记录

9. 再次插入：
   - EnrollmentID: `3`
   - StudentID: `2`
   - CourseID: `1`
   - Grade: `'A'`
10. 点击 **OK**
11. **预期结果**：数据表格显示3条记录

---

### 步骤2.7：测试外键约束（违反引用完整性）

1. 在 **Data Operation** 标签页，选择 `Enrollments` 表
2. 点击 **Insert Record**
3. 输入：
   - EnrollmentID: `4`
   - StudentID: `999`（Students表中不存在）
   - CourseID: `1`
   - Grade: `'C'`
4. 点击 **OK**
5. **预期结果**：
   - 显示错误消息框："Failed to insert record: Foreign key constraint violation: Referenced record not found in table Students for field StudentID"
   - 记录未插入

6. 再次尝试：
   - EnrollmentID: `4`
   - StudentID: `1`
   - CourseID: `999`（Courses表中不存在）
   - Grade: `'C'`
7. 点击 **OK**
8. **预期结果**：
   - 显示错误消息框："Foreign key constraint violation: Referenced record not found in table Courses for field CourseID"
   - 记录未插入

---

### 步骤2.8：测试多字段唯一约束

1. 在 **Data Operation** 标签页，选择 `Enrollments` 表
2. 点击 **Insert Record**
3. 输入：
   - EnrollmentID: `4`
   - StudentID: `1`
   - CourseID: `1`（与第1条记录重复：StudentID=1, CourseID=1）
   - Grade: `'F'`
4. 点击 **OK**
5. **预期结果**：
   - 显示错误消息框："Failed to insert record: Unique constraint violation: Multi-field unique constraint (StudentID, CourseID) violated"
   - 记录未插入

---

## 🎯 第三部分：数据更新操作

### 步骤3.1：更新Students表记录

1. 在 **Data Operation** 标签页，选择 `Students` 表
2. 在数据表格中选择第1条记录（StudentID=1, Name=Alice）
3. 点击 **Edit Record** 按钮
4. 在对话框中修改：
   - Name: `Alice Smith`（修改）
   - 其他字段保持不变
5. 点击 **OK**
6. **预期结果**：
   - 显示成功消息框："Record updated successfully."
   - 数据表格中第1条记录的Name更新为"Alice Smith"

---

### 步骤3.2：测试更新时的唯一约束

1. 在 `Students` 表中，选择第2条记录（StudentID=2, Email=bob@example.com）
2. 点击 **Edit Record**
3. 修改：
   - Email: `alice@example.com`（与第1条记录重复）
4. 点击 **OK**
5. **预期结果**：
   - 显示错误消息框："Failed to update record: Unique constraint violation: Field (Email) value already exists"
   - 记录未更新，Email仍为bob@example.com

---

### 步骤3.3：测试更新时的CHECK约束

1. 在 **Data Operation** 标签页，选择 `Courses` 表
2. 选择第1条记录（CourseID=1, Price=1500.50）
3. 点击 **Edit Record**
4. 修改：
   - Price: `-100`（违反CHECK约束）
5. 点击 **OK**
6. **预期结果**：
   - 显示错误消息框："Failed to update record: CHECK constraint violation: Field (Price) value (-100) does not satisfy constraint: > 0 AND < 10000"
   - 记录未更新，Price仍为1500.50

---

### 步骤3.4：测试更新时的多字段唯一约束

1. 在 **Data Operation** 标签页，选择 `Enrollments` 表
2. 选择第2条记录（EnrollmentID=2, StudentID=1, CourseID=2）
3. 点击 **Edit Record**
4. 修改：
   - CourseID: `1`（与第1条记录形成重复：StudentID=1, CourseID=1）
5. 点击 **OK**
6. **预期结果**：
   - 显示错误消息框："Failed to update record: Unique constraint violation: Multi-field unique constraint (StudentID, CourseID) violated"
   - 记录未更新

---

## 🎯 第四部分：数据删除操作

### 步骤4.1：删除单条记录

1. 在 **Data Operation** 标签页，选择 `Students` 表
2. 在数据表格中选择第3条记录（StudentID=3, Name=Charlie）
3. 点击 **Delete Record** 按钮
4. 在确认对话框中点击 **Yes**
5. **预期结果**：
   - 显示成功消息框："Record deleted successfully."
   - 数据表格中第3条记录消失，只剩2条记录

---

### 步骤4.2：测试外键级联删除（CASCADE）

1. 在 **Data Operation** 标签页，选择 `Students` 表
2. 选择第1条记录（StudentID=1, Name=Alice Smith）
3. 点击 **Delete Record**
4. 在确认对话框中点击 **Yes**
5. **预期结果**：
   - 显示成功消息框："Record deleted successfully."
   - Students表中StudentID=1的记录被删除
   - 切换到 `Enrollments` 表，检查：
     - EnrollmentID=1和EnrollmentID=2的记录应该也被删除（因为StudentID=1，外键ON DELETE CASCADE）
     - 数据表格中应该只剩1条记录（EnrollmentID=3）

---

### 步骤4.3：验证级联删除结果

1. 在 **Data Operation** 标签页，选择 `Enrollments` 表
2. **预期结果**：
   - 数据表格只显示1条记录（EnrollmentID=3, StudentID=2, CourseID=1）
   - 之前StudentID=1的两条选课记录已被级联删除

---

## 🎯 第五部分：表结构编辑

### 步骤5.1：编辑表结构（添加字段）

1. 导航到 **Table Management** 标签页
2. 在表列表中选择 `Students` 表
3. 点击 **Edit Table** 按钮
4. 在对话框中：
   - 点击 **Add Field**
   - 添加新字段：
     - Name: `Phone`
     - Type: `string`
     - KEY: `NOT_KEY`
     - NULL: `NULL`
     - VALID: `VALID`
     - UNIQUE: `NOT_UNIQUE`
     - DEFAULT: （留空）
5. 点击 **Edit** 按钮
6. **预期结果**：
   - 显示成功消息框："Table updated successfully."
   - Students表现在包含6个字段（新增Phone字段）

---

### 步骤5.2：验证编辑后的表结构

1. 在 **Table Management** 标签页，选择 `Students` 表
2. 检查右侧表信息显示
3. **预期结果**：
   - 显示6个字段，包括新添加的Phone字段
   - 原有字段保持不变

---

### 步骤5.3：编辑表结构（添加约束）

1. 在 **Table Management** 标签页，选择 `Students` 表
2. 点击 **Edit Table**
3. 切换到 **Constraints** 标签页
4. 添加CHECK约束：
   - 点击 **Add Check Constraint**
   - Field: `Age`
   - Expression: `>= 16 AND <= 100`
   - Constraint Name: `AgeRange`
   - 点击 **OK**
5. 点击 **Edit** 按钮
6. **预期结果**：
   - 显示成功消息框
   - Students表现在有CHECK约束

---

### 步骤5.4：测试新添加的CHECK约束

1. 导航到 **Data Operation** 标签页，选择 `Students` 表
2. 点击 **Insert Record**
3. 输入：
   - StudentID: `4`
   - Name: `David`
   - Email: `david@example.com`
   - Age: `15`（违反CHECK约束：Age >= 16）
   - Grade: `'Freshman'`
4. 点击 **OK**
5. **预期结果**：
   - 显示错误消息框："CHECK constraint violation: Field (Age) value (15) does not satisfy constraint: >= 16 AND <= 100"
   - 记录未插入

---

### 步骤5.5：重命名表

1. 导航到 **Table Management** 标签页
2. 在表列表中选择 `Courses` 表
3. 注意：GUI中重命名功能需要通过SQL执行，这里先跳过，在SQL测试中测试

---

### 步骤5.6：删除表（验证数据也被删除）

1. 在 **Table Management** 标签页，创建一个测试表：
   - 点击 **Create Table**
   - Table Name: `TestTable`
   - 添加字段：
     - Name: `ID`, Type: `int`, KEY: `KEY`, NULL: `NO_NULL`
     - Name: `Value`, Type: `string`, KEY: `NOT_KEY`, NULL: `NULL`
   - 点击 **Create**
2. 切换到 **Data Operation** 标签页
3. 选择 `TestTable`，插入2条记录：
   - ID: `1`, Value: `Test1`
   - ID: `2`, Value: `Test2`
4. 切换回 **Table Management** 标签页
5. 选择 `TestTable`，点击 **Delete Table**
6. 在确认对话框中点击 **Yes**
7. **预期结果**：
   - 显示成功消息框："Table deleted successfully."
   - TestTable从表列表中消失
8. 重新创建同名表：
   - 点击 **Create Table**
   - Table Name: `TestTable`
   - 添加相同字段
   - 点击 **Create**
9. 切换到 **Data Operation** 标签页，选择 `TestTable`
10. **预期结果**：
    - 表应该是空的，**不应该**显示之前插入的2条记录
    - 这证明删除表时，.dat文件中的数据也被正确删除

---

## 🎯 第六部分：约束管理界面

### 步骤6.1：查看约束信息

1. 在 **Table Management** 标签页，选择 `Enrollments` 表
2. 点击右侧的 **View Constraints** 按钮
3. **预期结果**：
   - 显示约束信息对话框
   - 显示2个外键约束（FK_Enrollments_Students, FK_Enrollments_Courses）
   - 显示1个多字段唯一约束（UQ_Enrollments_Student_Course）

---

### 步骤6.2：编辑表约束（添加外键）

1. 在 **Table Management** 标签页，选择 `Students` 表
2. 点击 **Edit Table**
3. 切换到 **Constraints** 标签页
4. 查看当前约束列表（应该显示之前添加的CHECK约束AgeRange）
5. 点击 **Edit** 按钮保存（不添加新约束）
6. **预期结果**：表结构保持不变

---

### 步骤6.3：编辑表约束（删除约束）

1. 在 **Table Management** 标签页，选择 `Students` 表
2. 点击 **Edit Table**
3. 切换到 **Constraints** 标签页
4. 在CHECK约束列表中选择 `AgeRange` 约束
5. 点击 **Remove Check Constraint** 按钮
6. 点击 **Edit** 按钮
7. **预期结果**：
   - 显示成功消息框
   - AgeRange约束被删除
8. 验证：尝试插入Age=15的记录，应该成功（因为CHECK约束已删除）

---

## 🎯 第七部分：索引管理

### 步骤7.1：创建哈希索引

1. 导航到 **Index Management** 标签页
2. 在表下拉框中选择 `Students`
3. 在字段下拉框中选择 `StudentID`
4. 选择索引类型：`Hash Index`
5. 点击 **Create Index** 按钮
6. **预期结果**：
   - 显示成功消息框："Index created successfully."
   - 索引出现在索引列表中

---

### 步骤7.2：创建相邻索引

1. 在 **Index Management** 标签页
2. 表：`Students`
3. 字段：`Age`
4. 索引类型：`Adjacent Index`
5. 点击 **Create Index**
6. **预期结果**：
   - 显示成功消息框
   - 索引出现在索引列表中

---

### 步骤7.3：查看索引统计信息

1. 在 **Index Management** 标签页
2. 在索引列表中选择刚创建的哈希索引
3. 查看索引统计信息
4. **预期结果**：
   - 显示索引类型、字段名、桶数量、总项数、最大链长度等信息

---

### 步骤7.4：删除索引

1. 在 **Index Management** 标签页
2. 在索引列表中选择一个索引
3. 点击 **Delete Index** 按钮
4. 在确认对话框中点击 **Yes**
5. **预期结果**：
   - 显示成功消息框："Index deleted successfully."
   - 索引从列表中消失

---

## 🎯 第八部分：智能索引建议系统

### 步骤8.1：使用索引建议功能

1. 在 **Index Management** 标签页
2. 在"Query Input"文本框中输入查询：
   ```
   SELECT * FROM Students WHERE StudentID = 1;
   ```
3. 点击 **Get Recommendations** 按钮
4. **预期结果**：
   - 显示索引建议结果
   - 可能推荐在StudentID字段上创建哈希索引（如果还没有）

---

### 步骤8.2：查看推荐结果

1. 查看推荐结果列表
2. **预期结果**：
   - 显示推荐的索引类型、字段、预期性能提升等信息

---

## 🎯 测试完成检查清单

### 数据库管理
- [ ] 创建数据库成功
- [ ] 打开数据库成功
- [ ] 状态栏正确显示数据库名称

### 表结构管理
- [ ] 创建表成功（包含字段定义）
- [ ] 编辑表成功（添加字段）
- [ ] 编辑表成功（添加约束）
- [ ] 查看表信息正确
- [ ] 删除表成功（数据也被删除）
- [ ] 重新创建同名表时，旧数据不出现

### 数据操作
- [ ] 插入记录成功
- [ ] 默认值正确应用
- [ ] 主键唯一约束正确检查
- [ ] 字段唯一约束正确检查
- [ ] 多字段唯一约束正确检查
- [ ] 外键约束正确检查
- [ ] CHECK约束正确检查（INSERT）
- [ ] 更新记录成功
- [ ] CHECK约束正确检查（UPDATE）
- [ ] 唯一约束正确检查（UPDATE）
- [ ] 删除记录成功
- [ ] 外键级联删除正确工作

### 约束管理
- [ ] 查看约束信息正确
- [ ] 添加约束成功
- [ ] 删除约束成功
- [ ] 约束持久化（重启后仍有效）

### 索引管理
- [ ] 创建哈希索引成功
- [ ] 创建相邻索引成功
- [ ] 查看索引统计信息正确
- [ ] 删除索引成功
- [ ] 索引建议功能正常

---

## 📝 注意事项

1. **测试顺序**：请按照本指南的顺序执行测试，因为前面的操作会影响后面的测试结果
2. **数据依赖**：某些测试依赖于之前插入的数据，请确保按顺序执行
3. **错误处理**：所有错误情况都应该显示清晰的错误消息
4. **数据持久化**：测试过程中可以关闭并重新打开数据库，验证数据是否持久化
5. **约束持久化**：删除表后重新创建，约束应该重新定义；关闭并重新打开数据库，约束应该仍然有效

---

## 🔍 问题排查

如果测试失败，请检查：
1. 数据库是否正确打开
2. 表结构是否正确创建
3. 约束是否正确定义
4. 错误消息是否清晰显示
5. 控制台是否有调试输出

---

**最后更新**：2026-01-16

