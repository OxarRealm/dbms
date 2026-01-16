# SQL执行命令操作全面测试指南

> **最后更新**：2026-01-16
> 
> 本文档提供基于SQL执行命令的全面测试指南，覆盖DBMS系统的所有SQL功能。测试采用场景化设计，前后操作连贯，可以一次性完成所有测试。

---

## 📋 测试概述

### 测试目标
验证DBMS系统通过SQL语句实现的所有功能，包括：
- DDL语句（CREATE TABLE, EDIT TABLE, RENAME TABLE, DROP TABLE）
- DML语句（INSERT, UPDATE, DELETE）
- 查询语句（SELECT，包括各种JOIN、子查询、聚合函数等）
- 约束功能（PRIMARY KEY, UNIQUE, DEFAULT, FOREIGN KEY, CHECK）
- 高级查询功能（ORDER BY, DISTINCT, LIMIT, GROUP BY, HAVING, UNION等）

### 测试场景
基于一个**学生管理系统**，包含以下表：
- **Students表**：学生基本信息
- **Courses表**：课程信息
- **Enrollments表**：选课记录（连接Students和Courses）

### 前置条件
1. 已编译并运行DBMS应用程序
2. 导航到 **SQL Execution** 标签页
3. 创建或打开数据库：`student_management`

---

## 🎯 第一部分：数据库和表结构管理（DDL）

### 步骤1.1：创建数据库

**说明**：数据库创建通过GUI完成（File -> Create Database），这里假设数据库 `student_management` 已创建。

---

### 步骤1.2：创建Students表

**SQL语句**：
```sql
CREATE TABLE Students (
    StudentID int KEY NO_NULL VALID UNIQUE,
    Name string NOT_KEY NO_NULL VALID,
    Email string NOT_KEY NULL VALID UNIQUE DEFAULT '',
    Age int NOT_KEY NULL VALID DEFAULT 18,
    Grade string NOT_KEY NULL VALID DEFAULT 'Freshman'
) INTO student_management;
```

**执行步骤**：
1. 在SQL输入框中输入上述SQL语句
2. 点击 **Execute** 按钮（或按 `Ctrl+Enter`）
3. **预期结果**：
   - 显示成功消息框："DDL statement executed successfully"
   - 状态标签显示："DDL statement executed successfully"
   - 在Table Management标签页可以看到Students表

---

### 步骤1.3：创建Courses表（带CHECK约束）

**SQL语句**：
```sql
CREATE TABLE Courses (
    CourseID int KEY NO_NULL VALID UNIQUE,
    CourseName string NOT_KEY NO_NULL VALID,
    Credits int NOT_KEY NULL VALID DEFAULT 3,
    Price float NOT_KEY NULL VALID DEFAULT 0.0,
    CHECK (Price > 0 AND Price < 10000)
) INTO student_management;
```

**执行步骤**：
1. 在SQL输入框中输入上述SQL语句
2. 点击 **Execute**
3. **预期结果**：
   - 显示成功消息框
   - Courses表创建成功，包含CHECK约束

---

### 步骤1.4：创建Enrollments表（带外键和多字段唯一约束）

**SQL语句**：
```sql
CREATE TABLE Enrollments (
    EnrollmentID int KEY NO_NULL VALID,
    StudentID int NOT_KEY NO_NULL VALID,
    CourseID int NOT_KEY NO_NULL VALID,
    Grade string NOT_KEY NULL VALID,
    FOREIGN KEY (StudentID) REFERENCES Students(StudentID) ON DELETE CASCADE ON UPDATE RESTRICT,
    FOREIGN KEY (CourseID) REFERENCES Courses(CourseID) ON DELETE CASCADE ON UPDATE RESTRICT,
    UNIQUE (StudentID, CourseID)
) INTO student_management;
```

**执行步骤**：
1. 在SQL输入框中输入上述SQL语句
2. 点击 **Execute**
3. **预期结果**：
   - 显示成功消息框
   - Enrollments表创建成功，包含2个外键约束和1个多字段唯一约束

---

### 步骤1.5：验证表结构

**SQL语句**：
```sql
SELECT * FROM Students;
SELECT * FROM Courses;
SELECT * FROM Enrollments;
```

**执行步骤**：
1. 依次执行上述三个SELECT语句
2. **预期结果**：
   - 每个SELECT都返回空结果（表刚创建，没有数据）
   - 但表结构正确，列名显示正确

---

## 🎯 第二部分：数据插入操作（DML - INSERT）

### 步骤2.1：插入Students表数据（测试默认值）

**SQL语句**：
```sql
INSERT INTO Students VALUES (1, 'Alice', 'alice@example.com', 18, 'Freshman') IN student_management;
```

**执行步骤**：
1. 执行上述SQL语句
2. **预期结果**：
   - 显示成功消息框："DML statement executed successfully, 1 row(s) affected"

**SQL语句**（测试默认值）：
```sql
INSERT INTO Students VALUES (2, 'Bob', 'bob@example.com', NULL, NULL) IN student_management;
```

**执行步骤**：
1. 执行上述SQL语句（Age和Grade为NULL，应该使用默认值）
2. **预期结果**：
   - 显示成功消息框
   - 验证：执行 `SELECT * FROM Students WHERE StudentID = 2;`
   - 应该显示Age=18, Grade='Freshman'（默认值生效）

**SQL语句**（测试Email默认值）：
```sql
INSERT INTO Students VALUES (3, 'Charlie', NULL, 19, 'Junior') IN student_management;
```

**执行步骤**：
1. 执行上述SQL语句
2. **预期结果**：
   - 显示成功消息框
   - 验证：执行 `SELECT * FROM Students WHERE StudentID = 3;`
   - 应该显示Email=''（空字符串，默认值生效）

---

### 步骤2.2：测试唯一约束（违反Email唯一性）

**SQL语句**：
```sql
INSERT INTO Students VALUES (4, 'David', 'alice@example.com', 21, 'Senior') IN student_management;
```

**执行步骤**：
1. 执行上述SQL语句（Email='alice@example.com'与StudentID=1重复）
2. **预期结果**：
   - 显示错误消息框："Failed to insert record: Unique constraint violation: Field (Email) value already exists"
   - 记录未插入

---

### 步骤2.3：测试主键唯一约束

**SQL语句**：
```sql
INSERT INTO Students VALUES (1, 'Eve', 'eve@example.com', 22, 'Senior') IN student_management;
```

**执行步骤**：
1. 执行上述SQL语句（StudentID=1与已有记录重复）
2. **预期结果**：
   - 显示错误消息框："Primary key constraint violation: Field (StudentID) value already exists"
   - 记录未插入

---

### 步骤2.4：插入Courses表数据

**SQL语句**：
```sql
INSERT INTO Courses VALUES (1, 'Database Systems', 3, 1500.50) IN student_management;
INSERT INTO Courses VALUES (2, 'Operating Systems', 4, 1800.00) IN student_management;
INSERT INTO Courses VALUES (3, 'Computer Networks', 3, 1200.75) IN student_management;
```

**执行步骤**：
1. 可以批量执行上述三条SQL语句（一次性粘贴，用分号分隔）
2. **预期结果**：
   - 如果批量执行：显示批量执行成功消息框
   - 如果单独执行：每条都显示成功消息框
   - 验证：执行 `SELECT * FROM Courses;` 应该显示3条记录

---

### 步骤2.5：测试CHECK约束（违反Price约束）

**SQL语句**（Price为负数）：
```sql
INSERT INTO Courses VALUES (4, 'Invalid Course', 2, -100) IN student_management;
```

**执行步骤**：
1. 执行上述SQL语句
2. **预期结果**：
   - 显示错误消息框："Failed to insert record: CHECK constraint violation: Field (Price) value (-100) does not satisfy constraint: > 0 AND < 10000"

**SQL语句**（Price超过上限）：
```sql
INSERT INTO Courses VALUES (4, 'Invalid Course 2', 2, 15000) IN student_management;
```

**执行步骤**：
1. 执行上述SQL语句
2. **预期结果**：
   - 显示错误消息框："CHECK constraint violation: Field (Price) value (15000) does not satisfy constraint: > 0 AND < 10000"

---

### 步骤2.6：插入Enrollments表数据

**SQL语句**：
```sql
INSERT INTO Enrollments VALUES (1, 1, 1, 'A') IN student_management;
INSERT INTO Enrollments VALUES (2, 1, 2, 'B') IN student_management;
INSERT INTO Enrollments VALUES (3, 2, 1, 'A') IN student_management;
INSERT INTO Enrollments VALUES (4, 2, 3, 'B') IN student_management;
```

**执行步骤**：
1. 批量执行上述SQL语句
2. **预期结果**：
   - 显示批量执行成功消息框
   - 验证：执行 `SELECT * FROM Enrollments;` 应该显示4条记录

---

### 步骤2.7：测试外键约束（违反引用完整性）

**SQL语句**（StudentID不存在）：
```sql
INSERT INTO Enrollments VALUES (5, 999, 1, 'C') IN student_management;
```

**执行步骤**：
1. 执行上述SQL语句
2. **预期结果**：
   - 显示错误消息框："Failed to insert record: Foreign key constraint violation: Referenced record not found in table Students for field StudentID"

**SQL语句**（CourseID不存在）：
```sql
INSERT INTO Enrollments VALUES (5, 1, 999, 'C') IN student_management;
```

**执行步骤**：
1. 执行上述SQL语句
2. **预期结果**：
   - 显示错误消息框："Foreign key constraint violation: Referenced record not found in table Courses for field CourseID"

---

### 步骤2.8：测试多字段唯一约束

**SQL语句**：
```sql
INSERT INTO Enrollments VALUES (5, 1, 1, 'F') IN student_management;
```

**执行步骤**：
1. 执行上述SQL语句（StudentID=1, CourseID=1与EnrollmentID=1重复）
2. **预期结果**：
   - 显示错误消息框："Failed to insert record: Unique constraint violation: Multi-field unique constraint (StudentID, CourseID) violated"

---

## 🎯 第三部分：数据更新操作（DML - UPDATE）

### 步骤3.1：更新Students表记录

**SQL语句**：
```sql
UPDATE Students (
    SET Name='Alice Smith'
    WHERE StudentID=1
) IN student_management;
```

**执行步骤**：
1. 执行上述SQL语句
2. **预期结果**：
   - 显示成功消息框："DML statement executed successfully, 1 row(s) affected"
   - 验证：执行 `SELECT * FROM Students WHERE StudentID=1;` 应该显示Name='Alice Smith'

---

### 步骤3.2：测试更新时的唯一约束

**SQL语句**：
```sql
UPDATE Students (
    SET Email='alice@example.com'
    WHERE StudentID=2
) IN student_management;
```

**执行步骤**：
1. 执行上述SQL语句（将StudentID=2的Email更新为'alice@example.com'，与StudentID=1重复）
2. **预期结果**：
   - 显示错误消息框："Failed to update record: Unique constraint violation: Field (Email) value already exists"
   - 记录未更新

---

### 步骤3.3：测试更新时的CHECK约束

**SQL语句**：
```sql
UPDATE Courses (
    SET Price=-100
    WHERE CourseID=1
) IN student_management;
```

**执行步骤**：
1. 执行上述SQL语句
2. **预期结果**：
   - 显示错误消息框："Failed to update record: CHECK constraint violation: Field (Price) value (-100) does not satisfy constraint: > 0 AND < 10000"
   - 记录未更新

---

### 步骤3.4：测试更新时的多字段唯一约束

**SQL语句**：
```sql
UPDATE Enrollments (
    SET CourseID=1
    WHERE EnrollmentID=2
) IN student_management;
```

**执行步骤**：
1. 执行上述SQL语句（将EnrollmentID=2的CourseID更新为1，与EnrollmentID=1形成重复：StudentID=1, CourseID=1）
2. **预期结果**：
   - 显示错误消息框："Failed to update record: Unique constraint violation: Multi-field unique constraint (StudentID, CourseID) violated"
   - 记录未更新

---

### 步骤3.5：批量更新多条记录

**SQL语句**：
```sql
UPDATE Students (
    SET Age=20
    WHERE Age=18
) IN student_management;
```

**执行步骤**：
1. 执行上述SQL语句
2. **预期结果**：
   - 显示成功消息框："DML statement executed successfully, X row(s) affected"（X为匹配的记录数）

---

## 🎯 第四部分：数据删除操作（DML - DELETE）

### 步骤4.1：删除单条记录

**SQL语句**：
```sql
DELETE FROM Students WHERE StudentID=3 IN student_management;
```

**执行步骤**：
1. 执行上述SQL语句
2. **预期结果**：
   - 显示成功消息框："DML statement executed successfully, 1 row(s) affected"
   - 验证：执行 `SELECT * FROM Students;` 应该不包含StudentID=3的记录

---

### 步骤4.2：测试外键级联删除（CASCADE）

**SQL语句**：
```sql
DELETE FROM Students WHERE StudentID=1 IN student_management;
```

**执行步骤**：
1. 执行上述SQL语句
2. **预期结果**：
   - 显示成功消息框："DML statement executed successfully, 1 row(s) affected"
   - 验证：执行 `SELECT * FROM Enrollments;`
   - EnrollmentID=1和EnrollmentID=2的记录应该也被删除（因为StudentID=1，外键ON DELETE CASCADE）
   - 应该只剩2条记录（EnrollmentID=3和EnrollmentID=4）

---

### 步骤4.3：测试外键SET NULL（如果定义了）

**说明**：如果Enrollments表的外键定义为ON DELETE SET NULL，则删除引用记录时，外键字段应设置为NULL。当前测试场景使用CASCADE，此测试可跳过。

---

### 步骤4.4：批量删除多条记录

**SQL语句**：
```sql
DELETE FROM Enrollments WHERE Grade='B' IN student_management;
```

**执行步骤**：
1. 执行上述SQL语句
2. **预期结果**：
   - 显示成功消息框："DML statement executed successfully, X row(s) affected"（X为匹配的记录数）

---

## 🎯 第五部分：基础查询（SELECT）

### 步骤5.1：单表查询（SELECT *）

**SQL语句**：
```sql
SELECT * FROM Students;
```

**执行步骤**：
1. 执行上述SQL语句
2. **预期结果**：
   - 显示成功消息框："Query executed successfully. X row(s) returned."
   - 结果表显示所有列和所有行

---

### 步骤5.2：指定字段查询

**SQL语句**：
```sql
SELECT StudentID, Name, Email FROM Students;
```

**执行步骤**：
1. 执行上述SQL语句
2. **预期结果**：
   - 显示成功消息框
   - 结果表只显示StudentID, Name, Email三列

---

### 步骤5.3：WHERE条件查询（等值比较）

**SQL语句**：
```sql
SELECT * FROM Students WHERE StudentID=2;
```

**执行步骤**：
1. 执行上述SQL语句
2. **预期结果**：
   - 显示成功消息框
   - 结果表只显示StudentID=2的记录

---

### 步骤5.4：WHERE条件查询（比较运算符）

**SQL语句**：
```sql
SELECT * FROM Courses WHERE Price > 1500;
SELECT * FROM Courses WHERE Price < 2000;
SELECT * FROM Courses WHERE Price >= 1500.50;
SELECT * FROM Courses WHERE Price <= 1800.00;
SELECT * FROM Courses WHERE Price != 1500.50;
```

**执行步骤**：
1. 依次执行上述SQL语句
2. **预期结果**：
   - 每个查询都返回符合条件的记录
   - 比较运算符正确工作

---

### 步骤5.5：WHERE条件查询（AND/OR/NOT）

**SQL语句**：
```sql
SELECT * FROM Courses WHERE Price > 1200 AND Price < 2000;
SELECT * FROM Students WHERE Age = 18 OR Age = 20;
SELECT * FROM Students WHERE NOT Age = 18;
```

**执行步骤**：
1. 依次执行上述SQL语句
2. **预期结果**：
   - 每个查询都返回符合条件的记录
   - 逻辑运算符正确工作

---

### 步骤5.6：WHERE条件查询（复杂条件）

**SQL语句**：
```sql
SELECT * FROM Courses WHERE (Price > 1200 AND Price < 2000) OR CourseID = 1;
```

**执行步骤**：
1. 执行上述SQL语句
2. **预期结果**：
   - 返回满足条件的记录
   - 括号优先级正确

---

### 步骤5.7：LIKE模式匹配

**SQL语句**：
```sql
SELECT * FROM Students WHERE Name LIKE 'A%';
SELECT * FROM Students WHERE Email LIKE '%@example.com';
SELECT * FROM Students WHERE Name LIKE '%lice%';
```

**执行步骤**：
1. 依次执行上述SQL语句
2. **预期结果**：
   - 第一个查询：返回Name以'A'开头的记录
   - 第二个查询：返回Email以'@example.com'结尾的记录
   - 第三个查询：返回Name包含'lice'的记录

---

### 步骤5.8：IN子句

**SQL语句**：
```sql
SELECT * FROM Students WHERE StudentID IN (1, 2, 3);
SELECT * FROM Courses WHERE CourseID IN (1, 2);
```

**执行步骤**：
1. 依次执行上述SQL语句
2. **预期结果**：
   - 返回StudentID或CourseID在指定列表中的记录

---

### 步骤5.9：BETWEEN范围查询

**SQL语句**：
```sql
SELECT * FROM Courses WHERE Price BETWEEN 1200 AND 1800;
SELECT * FROM Students WHERE Age BETWEEN 18 AND 20;
```

**执行步骤**：
1. 依次执行上述SQL语句
2. **预期结果**：
   - 返回Price或Age在指定范围内的记录（包含边界）

---

## 🎯 第六部分：排序和分页（ORDER BY, DISTINCT, LIMIT）

### 步骤6.1：ORDER BY排序（升序）

**SQL语句**：
```sql
SELECT * FROM Students ORDER BY Age ASC;
SELECT * FROM Courses ORDER BY Price ASC;
```

**执行步骤**：
1. 依次执行上述SQL语句
2. **预期结果**：
   - 结果按Age或Price升序排列

---

### 步骤6.2：ORDER BY排序（降序）

**SQL语句**：
```sql
SELECT * FROM Students ORDER BY Age DESC;
SELECT * FROM Courses ORDER BY Price DESC;
```

**执行步骤**：
1. 依次执行上述SQL语句
2. **预期结果**：
   - 结果按Age或Price降序排列

---

### 步骤6.3：ORDER BY多字段排序

**SQL语句**：
```sql
SELECT * FROM Students ORDER BY Age ASC, StudentID DESC;
```

**执行步骤**：
1. 执行上述SQL语句
2. **预期结果**：
   - 结果先按Age升序，Age相同时按StudentID降序

---

### 步骤6.4：DISTINCT去重

**SQL语句**：
```sql
SELECT DISTINCT Age FROM Students;
SELECT DISTINCT Grade FROM Students;
```

**执行步骤**：
1. 依次执行上述SQL语句
2. **预期结果**：
   - 结果只显示唯一的Age或Grade值（去除重复）

---

### 步骤6.5：LIMIT分页

**SQL语句**：
```sql
SELECT * FROM Students LIMIT 2;
SELECT * FROM Courses LIMIT 1;
```

**执行步骤**：
1. 依次执行上述SQL语句
2. **预期结果**：
   - 结果最多返回指定数量的记录

---

### 步骤6.6：组合使用（ORDER BY + LIMIT）

**SQL语句**：
```sql
SELECT * FROM Students ORDER BY Age DESC LIMIT 2;
SELECT * FROM Courses ORDER BY Price DESC LIMIT 1;
```

**执行步骤**：
1. 依次执行上述SQL语句
2. **预期结果**：
   - 结果按指定字段降序排列，只返回前N条记录

---

### 步骤6.7：组合使用（DISTINCT + ORDER BY + LIMIT）

**SQL语句**：
```sql
SELECT DISTINCT Age FROM Students ORDER BY Age DESC LIMIT 3;
```

**执行步骤**：
1. 执行上述SQL语句
2. **预期结果**：
   - 结果显示唯一的Age值，按降序排列，最多返回3条

---

## 🎯 第七部分：多表查询和连接（JOIN）

### 步骤7.1：多表查询（笛卡尔积）

**SQL语句**：
```sql
SELECT * FROM Students, Courses;
```

**执行步骤**：
1. 执行上述SQL语句
2. **预期结果**：
   - 显示成功消息框
   - 结果表显示两个表的笛卡尔积（所有可能的组合）

---

### 步骤7.2：多表查询（带WHERE条件）

**SQL语句**：
```sql
SELECT * FROM Students, Enrollments WHERE Students.StudentID = Enrollments.StudentID;
```

**执行步骤**：
1. 执行上述SQL语句
2. **预期结果**：
   - 显示成功消息框
   - 结果表显示通过StudentID连接的两个表的数据

---

### 步骤7.3：INNER JOIN

**SQL语句**：
```sql
SELECT * FROM Students INNER JOIN Enrollments ON Students.StudentID = Enrollments.StudentID;
```

**执行步骤**：
1. 执行上述SQL语句
2. **预期结果**：
   - 显示成功消息框
   - 结果表显示内连接后的数据（只返回匹配的记录）

---

### 步骤7.4：LEFT JOIN

**SQL语句**：
```sql
SELECT * FROM Students LEFT JOIN Enrollments ON Students.StudentID = Enrollments.StudentID;
```

**执行步骤**：
1. 执行上述SQL语句
2. **预期结果**：
   - 显示成功消息框
   - 结果表显示左连接后的数据（Students表的所有记录，以及匹配的Enrollments记录）
   - 如果Students表中某条记录在Enrollments表中没有匹配，Enrollments字段显示为空值

---

### 步骤7.5：RIGHT JOIN

**SQL语句**：
```sql
SELECT * FROM Students RIGHT JOIN Enrollments ON Students.StudentID = Enrollments.StudentID;
```

**执行步骤**：
1. 执行上述SQL语句
2. **预期结果**：
   - 显示成功消息框
   - 结果表显示右连接后的数据（Enrollments表的所有记录，以及匹配的Students记录）

---

### 步骤7.6：FULL OUTER JOIN

**SQL语句**：
```sql
SELECT * FROM Students FULL OUTER JOIN Enrollments ON Students.StudentID = Enrollments.StudentID;
```

**执行步骤**：
1. 执行上述SQL语句
2. **预期结果**：
   - 显示成功消息框
   - 结果表显示全外连接后的数据（所有匹配和未匹配的记录）

---

### 步骤7.7：NATURAL JOIN

**SQL语句**：
```sql
SELECT * FROM Students NATURAL JOIN Enrollments;
```

**执行步骤**：
1. 执行上述SQL语句（如果Students和Enrollments有共同字段StudentID）
2. **预期结果**：
   - 显示成功消息框
   - 结果表显示自然连接后的数据（基于共同字段自动连接）

---

### 步骤7.8：多表JOIN

**SQL语句**：
```sql
SELECT Students.Name, Courses.CourseName, Enrollments.Grade 
FROM Students 
INNER JOIN Enrollments ON Students.StudentID = Enrollments.StudentID 
INNER JOIN Courses ON Enrollments.CourseID = Courses.CourseID;
```

**执行步骤**：
1. 执行上述SQL语句
2. **预期结果**：
   - 显示成功消息框
   - 结果表显示三个表连接后的数据（学生姓名、课程名称、成绩）

---

### 步骤7.9：JOIN + WHERE条件

**SQL语句**：
```sql
SELECT * FROM Students 
INNER JOIN Enrollments ON Students.StudentID = Enrollments.StudentID 
WHERE Students.Age > 18;
```

**执行步骤**：
1. 执行上述SQL语句
2. **预期结果**：
   - 显示成功消息框
   - 结果表显示连接后的数据，但只包含Age>18的学生

---

### 步骤7.10：JOIN + ORDER BY + LIMIT

**SQL语句**：
```sql
SELECT Students.Name, Courses.CourseName, Enrollments.Grade 
FROM Students 
INNER JOIN Enrollments ON Students.StudentID = Enrollments.StudentID 
INNER JOIN Courses ON Enrollments.CourseID = Courses.CourseID 
ORDER BY Enrollments.Grade ASC 
LIMIT 3;
```

**执行步骤**：
1. 执行上述SQL语句
2. **预期结果**：
   - 显示成功消息框
   - 结果表显示连接后的数据，按成绩升序排列，只返回前3条

---

## 🎯 第八部分：聚合函数和分组（GROUP BY, HAVING）

### 步骤8.1：COUNT(*)统计

**SQL语句**：
```sql
SELECT COUNT(*) FROM Students;
SELECT COUNT(*) FROM Courses;
SELECT COUNT(*) FROM Enrollments;
```

**执行步骤**：
1. 依次执行上述SQL语句
2. **预期结果**：
   - 每个查询返回一行一列：COUNT(*) = 对应表的记录数

---

### 步骤8.2：COUNT(Field)统计非空字段

**SQL语句**：
```sql
SELECT COUNT(Age) FROM Students;
SELECT COUNT(Email) FROM Students;
```

**执行步骤**：
1. 依次执行上述SQL语句
2. **预期结果**：
   - 返回非空Age或Email字段的数量

---

### 步骤8.3：SUM求和

**SQL语句**：
```sql
SELECT SUM(Price) FROM Courses;
SELECT SUM(Credits) FROM Courses;
```

**执行步骤**：
1. 依次执行上述SQL语句
2. **预期结果**：
   - 返回Price或Credits的总和

---

### 步骤8.4：AVG平均值

**SQL语句**：
```sql
SELECT AVG(Age) FROM Students;
SELECT AVG(Price) FROM Courses;
```

**执行步骤**：
1. 依次执行上述SQL语句
2. **预期结果**：
   - 返回Age或Price的平均值

---

### 步骤8.5：MAX最大值

**SQL语句**：
```sql
SELECT MAX(Age) FROM Students;
SELECT MAX(Price) FROM Courses;
```

**执行步骤**：
1. 依次执行上述SQL语句
2. **预期结果**：
   - 返回Age或Price的最大值

---

### 步骤8.6：MIN最小值

**SQL语句**：
```sql
SELECT MIN(Age) FROM Students;
SELECT MIN(Price) FROM Courses;
```

**执行步骤**：
1. 依次执行上述SQL语句
2. **预期结果**：
   - 返回Age或Price的最小值

---

### 步骤8.7：GROUP BY单字段分组

**SQL语句**：
```sql
SELECT StudentID, COUNT(*) FROM Enrollments GROUP BY StudentID;
SELECT CourseID, COUNT(*) FROM Enrollments GROUP BY CourseID;
```

**执行步骤**：
1. 依次执行上述SQL语句
2. **预期结果**：
   - 返回按StudentID或CourseID分组的记录数和每个分组的记录数

---

### 步骤8.8：GROUP BY多字段分组

**SQL语句**：
```sql
SELECT StudentID, CourseID, COUNT(*) FROM Enrollments GROUP BY StudentID, CourseID;
```

**执行步骤**：
1. 执行上述SQL语句
2. **预期结果**：
   - 返回按StudentID和CourseID组合分组的记录数

---

### 步骤8.9：GROUP BY + 聚合函数组合

**SQL语句**：
```sql
SELECT StudentID, COUNT(*), AVG(CAST(Grade AS float)) FROM Enrollments GROUP BY StudentID;
```

**说明**：如果Grade是字符串类型，此查询可能不适用。改用：

```sql
SELECT CourseID, COUNT(*), AVG(Price) FROM Courses GROUP BY CourseID;
```

**执行步骤**：
1. 执行上述SQL语句
2. **预期结果**：
   - 返回每个CourseID的产品数量（都是1，因为CourseID是主键）和平均价格

---

### 步骤8.10：HAVING过滤分组结果

**SQL语句**：
```sql
SELECT StudentID, COUNT(*) FROM Enrollments GROUP BY StudentID HAVING COUNT(*) > 1;
```

**执行步骤**：
1. 执行上述SQL语句
2. **预期结果**：
   - 只返回选课数量大于1的StudentID

---

### 步骤8.11：HAVING使用聚合函数条件

**SQL语句**：
```sql
SELECT CourseID, AVG(Price) FROM Courses GROUP BY CourseID HAVING AVG(Price) > 1000;
```

**执行步骤**：
1. 执行上述SQL语句
2. **预期结果**：
   - 只返回平均价格大于1000的CourseID（实际上每个CourseID只有一条记录）

---

### 步骤8.12：GROUP BY + WHERE + HAVING组合

**SQL语句**：
```sql
SELECT StudentID, COUNT(*) FROM Enrollments WHERE Grade != 'F' GROUP BY StudentID HAVING COUNT(*) >= 1;
```

**执行步骤**：
1. 执行上述SQL语句
2. **预期结果**：
   - 返回成绩不为'F'的选课记录，按StudentID分组，且分组后数量>=1

---

### 步骤8.13：GROUP BY + ORDER BY组合

**SQL语句**：
```sql
SELECT StudentID, COUNT(*) FROM Enrollments GROUP BY StudentID ORDER BY COUNT(*) DESC;
```

**执行步骤**：
1. 执行上述SQL语句
2. **预期结果**：
   - 结果按选课数量降序排列

---

## 🎯 第九部分：UNION并集操作

### 步骤9.1：UNION基本用法（去重）

**SQL语句**：
```sql
SELECT StudentID FROM Students WHERE Age = 18 UNION SELECT StudentID FROM Students WHERE Age = 20;
```

**执行步骤**：
1. 执行上述SQL语句
2. **预期结果**：
   - 显示成功消息框
   - 结果表显示Age=18或Age=20的StudentID，自动去除重复

---

### 步骤9.2：UNION ALL保留重复

**SQL语句**：
```sql
SELECT StudentID FROM Students WHERE Age = 18 UNION ALL SELECT StudentID FROM Students WHERE Age = 20;
```

**执行步骤**：
1. 执行上述SQL语句
2. **预期结果**：
   - 显示成功消息框
   - 结果表显示Age=18或Age=20的StudentID，保留所有重复

---

### 步骤9.3：多列UNION

**SQL语句**：
```sql
SELECT StudentID, Name FROM Students WHERE Age = 18 UNION SELECT StudentID, Name FROM Students WHERE Age = 20;
```

**执行步骤**：
1. 执行上述SQL语句
2. **预期结果**：
   - 显示成功消息框
   - 结果表显示两列：StudentID和Name，来自Age=18或Age=20的记录

---

### 步骤9.4：多个UNION连接

**SQL语句**：
```sql
SELECT StudentID FROM Students WHERE Age = 18 
UNION SELECT StudentID FROM Students WHERE Age = 20 
UNION SELECT StudentID FROM Students WHERE Age = 19;
```

**执行步骤**：
1. 执行上述SQL语句
2. **预期结果**：
   - 显示成功消息框
   - 结果表显示Age为18、20或19的StudentID

---

### 步骤9.5：UNION + ORDER BY

**SQL语句**：
```sql
SELECT StudentID, Name FROM Students WHERE Age = 18 
UNION SELECT StudentID, Name FROM Students WHERE Age = 20 
ORDER BY StudentID;
```

**执行步骤**：
1. 执行上述SQL语句
2. **预期结果**：
   - 显示成功消息框
   - 结果表按StudentID排序显示

---

## 🎯 第十部分：子查询（Subquery）

### 步骤10.1：标量子查询（等值比较）

**SQL语句**：
```sql
SELECT * FROM Students WHERE StudentID = (SELECT StudentID FROM Students WHERE Name = 'Alice Smith');
```

**执行步骤**：
1. 执行上述SQL语句
2. **预期结果**：
   - 显示成功消息框
   - 结果表显示StudentID等于子查询返回值的记录（Alice Smith的记录）

---

### 步骤10.2：标量子查询（大于比较）

**SQL语句**：
```sql
SELECT * FROM Students WHERE Age > (SELECT Age FROM Students WHERE Name = 'Bob');
```

**执行步骤**：
1. 执行上述SQL语句
2. **预期结果**：
   - 显示成功消息框
   - 结果表显示Age大于Bob的Age的所有用户

---

### 步骤10.3：IN子查询

**SQL语句**：
```sql
SELECT * FROM Students WHERE StudentID IN (SELECT StudentID FROM Enrollments);
```

**执行步骤**：
1. 执行上述SQL语句
2. **预期结果**：
   - 显示成功消息框
   - 结果表显示StudentID在Enrollments表中出现的所有学生

---

### 步骤10.4：EXISTS子查询

**SQL语句**：
```sql
SELECT * FROM Students WHERE EXISTS (SELECT * FROM Enrollments WHERE Enrollments.StudentID = Students.StudentID);
```

**执行步骤**：
1. 执行上述SQL语句
2. **预期结果**：
   - 显示成功消息框
   - 结果表显示在Enrollments表中有对应记录的学生（关联子查询）

---

### 步骤10.5：NOT EXISTS子查询

**SQL语句**：
```sql
SELECT * FROM Students WHERE NOT EXISTS (SELECT * FROM Enrollments WHERE Enrollments.StudentID = Students.StudentID);
```

**执行步骤**：
1. 执行上述SQL语句
2. **预期结果**：
   - 显示成功消息框
   - 结果表显示在Enrollments表中没有对应记录的学生

---

### 步骤10.6：子查询与聚合函数

**SQL语句**：
```sql
SELECT * FROM Students WHERE Age > (SELECT AVG(Age) FROM Students);
```

**执行步骤**：
1. 执行上述SQL语句
2. **预期结果**：
   - 显示成功消息框
   - 结果表显示Age大于平均年龄的学生

---

### 步骤10.7：嵌套子查询

**SQL语句**：
```sql
SELECT * FROM Students WHERE StudentID = (
    SELECT StudentID FROM Enrollments 
    WHERE CourseID = (SELECT CourseID FROM Courses WHERE Price = (SELECT MAX(Price) FROM Courses))
);
```

**执行步骤**：
1. 执行上述SQL语句
2. **预期结果**：
   - 显示成功消息框
   - 结果表显示拥有最高价格课程的学生

---

### 步骤10.8：子查询与ORDER BY组合

**SQL语句**：
```sql
SELECT * FROM Students WHERE Age >= (SELECT MIN(Age) FROM Students) ORDER BY Age;
```

**执行步骤**：
1. 执行上述SQL语句
2. **预期结果**：
   - 显示成功消息框
   - 结果表显示所有学生（因为MIN(Age)是最小值），按Age排序

---

## 🎯 第十一部分：表结构编辑（DDL - EDIT TABLE）

### 步骤11.1：编辑表结构（添加字段）

**SQL语句**：
```sql
EDIT TABLE Students (
    Phone string NOT_KEY NULL VALID
) IN student_management;
```

**执行步骤**：
1. 执行上述SQL语句
2. **预期结果**：
   - 显示成功消息框："DDL statement executed successfully"
   - Students表现在包含6个字段（新增Phone字段）

---

### 步骤11.2：验证编辑后的表结构

**SQL语句**：
```sql
SELECT * FROM Students;
```

**执行步骤**：
1. 执行上述SQL语句
2. **预期结果**：
   - 结果表显示6列，包括新添加的Phone列（值为空或NULL）

---

### 步骤11.3：编辑表结构（添加CHECK约束）

**说明**：EDIT TABLE不支持添加约束，约束需要通过CREATE TABLE或GUI界面添加。此步骤跳过。

---

## 🎯 第十二部分：表重命名和删除（DDL - RENAME TABLE, DROP TABLE）

### 步骤12.1：重命名表

**SQL语句**：
```sql
RENAME TABLE Courses CourseInfo IN student_management;
```

**执行步骤**：
1. 执行上述SQL语句
2. **预期结果**：
   - 显示成功消息框："DDL statement executed successfully"
   - Courses表被重命名为CourseInfo
   - 验证：执行 `SELECT * FROM CourseInfo;` 应该显示原Courses表的数据

---

### 步骤12.2：验证重命名后的表

**SQL语句**：
```sql
SELECT * FROM CourseInfo;
SELECT * FROM Courses;
```

**执行步骤**：
1. 执行第一个SQL语句
2. **预期结果**：
   - 显示成功消息框，结果表显示原Courses表的数据
3. 执行第二个SQL语句
4. **预期结果**：
   - 显示错误消息框："Table 'Courses' does not exist"（因为表已重命名）

---

### 步骤12.3：删除表（验证数据也被删除）

**SQL语句**：
```sql
CREATE TABLE TestTable (
    ID int KEY NO_NULL VALID,
    Value string NOT_KEY NULL VALID
) INTO student_management;

INSERT INTO TestTable VALUES (1, 'Test1') IN student_management;
INSERT INTO TestTable VALUES (2, 'Test2') IN student_management;

SELECT * FROM TestTable;
```

**执行步骤**：
1. 依次执行上述SQL语句
2. **预期结果**：
   - TestTable创建成功
   - 插入2条记录成功
   - SELECT显示2条记录

**SQL语句**（删除表）：
```sql
DROP TABLE TestTable IN student_management;
```

**执行步骤**：
1. 执行上述SQL语句
2. **预期结果**：
   - 显示成功消息框："DDL statement executed successfully"
   - 验证：执行 `SELECT * FROM TestTable;` 应该报错："Table 'TestTable' does not exist"

**SQL语句**（重新创建同名表）：
```sql
CREATE TABLE TestTable (
    ID int KEY NO_NULL VALID,
    Value string NOT_KEY NULL VALID
) INTO student_management;

SELECT * FROM TestTable;
```

**执行步骤**：
1. 依次执行上述SQL语句
2. **预期结果**：
   - TestTable重新创建成功
   - SELECT返回空结果（0条记录），**不应该**显示之前插入的2条记录
   - 这证明DROP TABLE时，.dat文件中的数据也被正确删除

---

## 🎯 第十三部分：组合功能测试

### 步骤13.1：JOIN + WHERE + ORDER BY + LIMIT

**SQL语句**：
```sql
SELECT Students.Name, CourseInfo.CourseName, Enrollments.Grade 
FROM Students 
INNER JOIN Enrollments ON Students.StudentID = Enrollments.StudentID 
INNER JOIN CourseInfo ON Enrollments.CourseID = CourseInfo.CourseID 
WHERE CourseInfo.Price > 1200 
ORDER BY Enrollments.Grade ASC 
LIMIT 3;
```

**执行步骤**：
1. 执行上述SQL语句
2. **预期结果**：
   - 显示成功消息框
   - 结果表显示价格>1200的课程，按成绩升序排列，只返回前3条

---

### 步骤13.2：GROUP BY + HAVING + ORDER BY

**SQL语句**：
```sql
SELECT StudentID, COUNT(*), AVG(CAST(Grade AS float)) 
FROM Enrollments 
GROUP BY StudentID 
HAVING COUNT(*) > 1 
ORDER BY AVG(CAST(Grade AS float)) DESC;
```

**说明**：如果Grade是字符串类型，此查询可能不适用。改用：

```sql
SELECT CourseID, COUNT(*), AVG(Price) 
FROM CourseInfo 
GROUP BY CourseID 
HAVING COUNT(*) >= 1 
ORDER BY AVG(Price) DESC;
```

**执行步骤**：
1. 执行上述SQL语句
2. **预期结果**：
   - 显示成功消息框
   - 结果表显示满足条件的分组，按平均值降序排列

---

### 步骤13.3：UNION + ORDER BY + LIMIT

**SQL语句**：
```sql
SELECT StudentID, Name FROM Students WHERE Age = 18 
UNION SELECT StudentID, Name FROM Students WHERE Age = 20 
ORDER BY StudentID 
LIMIT 2;
```

**执行步骤**：
1. 执行上述SQL语句
2. **预期结果**：
   - 显示成功消息框
   - 结果表显示UNION结果，按StudentID排序，只返回前2条

---

### 步骤13.4：子查询 + JOIN + WHERE

**SQL语句**：
```sql
SELECT * FROM Students 
INNER JOIN Enrollments ON Students.StudentID = Enrollments.StudentID 
WHERE Enrollments.CourseID IN (SELECT CourseID FROM CourseInfo WHERE Price > (SELECT AVG(Price) FROM CourseInfo));
```

**执行步骤**：
1. 执行上述SQL语句
2. **预期结果**：
   - 显示成功消息框
   - 结果表显示价格高于平均价格的课程及其学生信息

---

### 步骤13.5：复杂WHERE条件 + DISTINCT + ORDER BY

**SQL语句**：
```sql
SELECT DISTINCT Students.Name 
FROM Students 
INNER JOIN Enrollments ON Students.StudentID = Enrollments.StudentID 
WHERE Students.Age > 18 AND Enrollments.Grade LIKE 'A%' 
ORDER BY Students.Name ASC;
```

**执行步骤**：
1. 执行上述SQL语句
2. **预期结果**：
   - 显示成功消息框
   - 结果表显示Age>18且成绩以'A'开头的学生姓名，去重并按姓名升序排列

---

## 🎯 第十四部分：批量执行测试

### 步骤14.1：批量执行多条INSERT语句

**SQL语句**（一次性粘贴，用分号分隔）：
```sql
INSERT INTO Students VALUES (5, 'Eve', 'eve@example.com', 22, 'Senior') IN student_management;
INSERT INTO Students VALUES (6, 'Frank', 'frank@example.com', 21, 'Senior') IN student_management;
INSERT INTO Students VALUES (7, 'Grace', 'grace@example.com', 19, 'Sophomore') IN student_management;
```

**执行步骤**：
1. 在SQL输入框中一次性粘贴上述三条SQL语句
2. 点击 **Execute**
3. **预期结果**：
   - 显示批量执行成功消息框："Batch execution completed successfully. Total statements: 3, Successful: 3, Total rows affected: 3"
   - 所有3条记录都成功插入

---

### 步骤14.2：批量执行混合语句（部分成功，部分失败）

**SQL语句**：
```sql
INSERT INTO Students VALUES (8, 'Henry', 'henry@example.com', 20, 'Junior') IN student_management;
INSERT INTO Students VALUES (1, 'Duplicate', 'duplicate@example.com', 99, 'Senior') IN student_management;
INSERT INTO Students VALUES (9, 'Ivy', 'ivy@example.com', 18, 'Freshman') IN student_management;
```

**执行步骤**：
1. 批量执行上述三条SQL语句
2. **预期结果**：
   - 显示批量执行完成但包含错误的消息框
   - 显示：Total statements: 3, Successful: 2, Failed: 1
   - 错误信息：Statement 2: Primary key constraint violation: Field (StudentID) value already exists
   - 第1条和第3条记录成功插入，第2条因主键冲突失败

---

### 步骤14.3：批量执行包含查询的语句

**SQL语句**：
```sql
INSERT INTO Students VALUES (10, 'Jack', 'jack@example.com', 23, 'Senior') IN student_management;
SELECT * FROM Students WHERE StudentID=10;
```

**执行步骤**：
1. 批量执行上述两条SQL语句
2. **预期结果**：
   - 显示批量执行成功消息框
   - 结果表显示StudentID=10的记录
   - 消息框显示："Last query returned 1 row(s)."

---

## 🎯 测试完成检查清单

### DDL功能
- [ ] CREATE TABLE成功（包含字段定义和约束）
- [ ] EDIT TABLE成功（添加字段）
- [ ] RENAME TABLE成功
- [ ] DROP TABLE成功（数据也被删除）
- [ ] 重新创建同名表时，旧数据不出现

### DML功能
- [ ] INSERT成功
- [ ] 默认值正确应用
- [ ] 主键唯一约束正确检查
- [ ] 字段唯一约束正确检查
- [ ] 多字段唯一约束正确检查
- [ ] 外键约束正确检查
- [ ] CHECK约束正确检查（INSERT）
- [ ] UPDATE成功
- [ ] CHECK约束正确检查（UPDATE）
- [ ] 唯一约束正确检查（UPDATE）
- [ ] DELETE成功
- [ ] 外键级联删除正确工作

### 查询功能
- [ ] SELECT单表查询成功
- [ ] SELECT指定字段查询成功
- [ ] WHERE条件查询成功（等值、比较运算符、逻辑运算符）
- [ ] LIKE模式匹配成功
- [ ] IN子句成功
- [ ] BETWEEN范围查询成功
- [ ] ORDER BY排序成功（ASC/DESC，单字段/多字段）
- [ ] DISTINCT去重成功
- [ ] LIMIT分页成功
- [ ] 多表查询成功（笛卡尔积）
- [ ] INNER JOIN成功
- [ ] LEFT JOIN成功
- [ ] RIGHT JOIN成功
- [ ] FULL OUTER JOIN成功
- [ ] NATURAL JOIN成功
- [ ] 多表JOIN成功
- [ ] GROUP BY分组成功
- [ ] 聚合函数成功（COUNT, SUM, AVG, MAX, MIN）
- [ ] HAVING过滤成功
- [ ] UNION成功（去重）
- [ ] UNION ALL成功（保留重复）
- [ ] 子查询成功（标量、IN、EXISTS、NOT EXISTS、嵌套）

### 批量执行
- [ ] 批量执行多条INSERT成功
- [ ] 批量执行混合语句正确处理（部分成功，部分失败）
- [ ] 批量执行包含查询的语句成功

---

## 📝 注意事项

1. **测试顺序**：请按照本指南的顺序执行测试，因为前面的操作会影响后面的测试结果
2. **数据依赖**：某些测试依赖于之前插入的数据，请确保按顺序执行
3. **SQL语法**：所有SQL语句必须以分号（;）结尾
4. **表名大小写**：表名不区分大小写（Students、students、STUDENTS都可以）
5. **错误处理**：所有错误情况都应该显示清晰的错误消息
6. **批量执行**：可以一次性粘贴多条SQL语句（用分号分隔），系统会自动分割并批量执行
7. **数据持久化**：测试过程中可以关闭并重新打开数据库，验证数据是否持久化

---

## 🔍 问题排查

如果测试失败，请检查：
1. 数据库是否正确打开
2. SQL语句语法是否正确（特别是分号）
3. 表结构是否正确创建
4. 约束是否正确定义
5. 错误消息是否清晰显示
6. 控制台是否有调试输出

---

**最后更新**：2026-01-16

