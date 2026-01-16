# SQL执行测试用例

本文档提供了SQL执行界面的全面测试用例。

## 测试环境设置

1. 使用 `文件 -> 创建数据库` 或 `文件 -> 打开数据库` 创建或打开数据库
2. 确保数据库已加载（检查状态栏）
3. 导航到"SQL执行"标签页

## 测试用例分类

### 1. DDL（数据定义语言）测试

#### 测试用例 1.1: CREATE TABLE
**SQL语句：**
```sql
CREATE TABLE Users (
    UserID int KEY NO_NULL VALID,
    UserName char[50] NOT_KEY NULL VALID,
    Email char[100] NOT_KEY NULL VALID,
    Age int NOT_KEY NULL VALID
) INTO test_db;
```

**预期结果：**
- 成功消息框："DDL statement executed successfully"
- 状态标签："DDL statement executed successfully"
- 表"Users"应出现在表管理标签页中

**注意事项：**
- 表名不区分大小写（Users、users、USERS都可以）
- 必须以分号（;）结尾

---

#### 测试用例 1.2: CREATE TABLE - 表名大小写不敏感
**SQL语句：**
```sql
CREATE TABLE products (
    ProductID int KEY NO_NULL VALID,
    ProductName char[100] NOT_KEY NULL VALID,
    Price float NOT_KEY NULL VALID,
    UserID int NOT_KEY NULL VALID
) INTO test_db;
```

**预期结果：**
- 成功消息框
- 表可以以"Products"、"products"或"PRODUCTS"查询
- 表包含4个字段：ProductID, ProductName, Price, UserID

---

#### 测试用例 1.3: EDIT TABLE
**SQL语句：**
```sql
EDIT TABLE Users (
    Phone char[20] NOT_KEY NULL VALID
) IN test_db;
```

**预期结果：**
- 成功消息框
- 表"Users"应有一个新的"Phone"字段

---

#### 测试用例 1.4: RENAME TABLE
**SQL语句：**
```sql
RENAME TABLE Users Customers IN test_db;
```

**预期结果：**
- 成功消息框
- 表"Users"应被重命名为"Customers"
- 旧名称"Users"应不再存在

---

#### 测试用例 1.5: DROP TABLE
**SQL语句：**
```sql
DROP TABLE Customers IN test_db;
```

**预期结果：**
- 成功消息框
- 表"Customers"应被删除

---

#### 测试用例 1.6: CREATE TABLE - 缺少分号（错误）
**SQL语句：**
```sql
CREATE TABLE TestTable (
    ID int KEY NO_NULL VALID
) INTO test_db
```

**预期结果：**
- 错误消息框："Expected ';', but got: [EOF or next token]"
- 状态标签："Error: ..."

---

### 2. DML（数据操作语言）测试

#### 测试用例 2.1: INSERT INTO
**SQL语句：**
```sql
INSERT INTO Users VALUES ('1', 'John Doe', 'john@example.com', '25') IN test_db;
```

**预期结果：**
- 成功消息框："DML statement executed successfully, 1 row(s) affected"
- 状态标签："DML statement executed successfully, 1 row(s) affected"
- 记录应出现在数据操作标签页中

---

#### 测试用例 2.2: INSERT 多条记录（批量执行）
**SQL语句：**
```sql
INSERT INTO Users VALUES ('2', 'Jane Smith', 'jane@example.com', '30') IN test_db;
INSERT INTO Users VALUES ('3', 'Bob Johnson', 'bob@example.com', '28') IN test_db;
```

**预期结果：**
- 批量执行成功消息框："Batch execution completed successfully. Total statements: 2, Successful: 2, Total rows affected: 2"
- 状态标签："Batch execution: 2 statement(s) executed successfully, 2 row(s) affected"
- Users表中共有3条记录（包括之前插入的John Doe）
- **注意**：系统会自动按分号分割多条SQL语句并批量执行

---

#### 测试用例 2.3: UPDATE
**SQL语句：**
```sql
UPDATE Users (
    SET Age='26'
    WHERE UserID='1'
) IN test_db;
```

**预期结果：**
- 成功消息框："DML statement executed successfully, 1 row(s) affected"
- UserID=1的Age应更新为26

---

#### 测试用例 2.4: DELETE
**SQL语句：**
```sql
DELETE FROM Users WHERE UserID='3' IN test_db;
```

**预期结果：**
- 成功消息框："DML statement executed successfully, 1 row(s) affected"
- UserID=3的记录应被删除

---

#### 测试用例 2.5: UPDATE - 无匹配记录
**SQL语句：**
```sql
UPDATE Users (
    SET Age='99'
    WHERE UserID='999'
) IN test_db;
```

**预期结果：**
- 成功消息框："DML statement executed successfully, 0 row(s) affected (no matching records found)"
- 没有记录被更新
- **注意**：这是标准SQL行为，UPDATE语句执行成功但WHERE条件没有匹配到任何记录

---

### 3. 查询（SELECT）测试

#### 测试用例 3.1: SELECT * FROM 单表
**SQL语句：**
```sql
SELECT * FROM Users;
```

**预期结果：**
- 成功消息框："Query executed successfully. X row(s) returned."
- 结果表显示所有列和行
- 状态标签："Query executed successfully. X row(s) returned."

**注意事项：**
- 表名不区分大小写
- 分号（;）是必需的

---

#### 测试用例 3.2: SELECT 特定字段
**SQL语句：**
```sql
SELECT UserName, Email FROM Users;
```

**预期结果：**
- 成功消息框
- 结果表仅显示UserName和Email列

---

#### 测试用例 3.3: SELECT 带WHERE子句
**SQL语句：**
```sql
SELECT * FROM Users WHERE UserID='1';
```

**预期结果：**
- 成功消息框
- 结果表仅显示UserID='1'的记录

---

#### 测试用例 3.4: SELECT - 表名大小写不敏感
**SQL语句：**
```sql
SELECT * FROM users;
```

**预期结果：**
- 成功消息框
- 与"SELECT * FROM Users;"结果相同

---

#### 测试用例 3.5: SELECT - 表不存在（错误）
**SQL语句：**
```sql
SELECT * FROM NonExistentTable;
```

**预期结果：**
- 错误消息框："Table 'NonExistentTable' does not exist"或类似
- 状态标签："Error: ..."

---

#### 测试用例 3.6: SELECT - 缺少分号（错误）
**SQL语句：**
```sql
SELECT * FROM Users
```

**预期结果：**
- 错误消息框："Expected ';', but got: [EOF or next token]"
- 状态标签："Error: ..."

---

### 3. 查询（SELECT）测试

**前置条件**：在执行以下测试用例之前，请确保已经创建了`Products`表（测试用例1.2，包含UserID字段），并插入了以下示例数据：

```sql
-- 插入Products表示例数据（可以批量执行）
-- 注意：Products表现在包含4个字段：ProductID, ProductName, Price, UserID
INSERT INTO Products VALUES ('1', 'Laptop', '1000.50', '1') IN test_db;
INSERT INTO Products VALUES ('2', 'Mouse', '25.99', '1') IN test_db;
INSERT INTO Products VALUES ('3', 'Keyboard', '89.99', '2') IN test_db;
INSERT INTO Products VALUES ('4', 'Monitor', '299.99', '2') IN test_db;
INSERT INTO Products VALUES ('5', 'Headphones', '79.99', '1') IN test_db;
```

**注意**：
- 这些INSERT语句可以批量执行（一次性粘贴多条语句，用分号分隔）
- UserID字段用于JOIN查询，关联到Users表的UserID

---

#### 测试用例 3.7: SELECT - 多表查询
**SQL语句：**
```sql
SELECT * FROM Users, Products;
```

**预期结果：**
- 成功消息框
- 结果表显示两个表的笛卡尔积

---

#### 测试用例 3.8: SELECT - INNER JOIN查询
**SQL语句：**
```sql
SELECT * FROM Users INNER JOIN Products ON Users.UserID=Products.UserID;
```

**预期结果：**
- 成功消息框
- 结果表显示连接后的数据（Users表和Products表通过UserID字段连接）
- 应该显示匹配的记录，例如UserID=1的用户对应ProductID=1,2,5的产品
- **注意**：INNER JOIN只返回两个表中都有匹配的记录

---

#### 测试用例 3.9: SELECT - LEFT JOIN查询
**SQL语句：**
```sql
SELECT * FROM Users LEFT JOIN Products ON Users.UserID=Products.UserID;
```

**预期结果：**
- 成功消息框
- 结果表显示左连接后的数据（Users表的所有记录，以及匹配的Products记录）
- 如果Users表中的某条记录在Products表中没有匹配，Products字段显示为空值
- **注意**：LEFT JOIN返回左表（Users）的所有记录，即使右表（Products）中没有匹配

---

#### 测试用例 3.10: SELECT - RIGHT JOIN查询
**SQL语句：**
```sql
SELECT * FROM Users RIGHT JOIN Products ON Users.UserID=Products.UserID;
```

**预期结果：**
- 成功消息框
- 结果表显示右连接后的数据（Products表的所有记录，以及匹配的Users记录）
- 如果Products表中的某条记录在Users表中没有匹配，Users字段显示为空值
- **注意**：RIGHT JOIN返回右表（Products）的所有记录，即使左表（Users）中没有匹配

---

#### 测试用例 3.11: SELECT - JOIN查询（指定字段）
**SQL语句：**
```sql
SELECT Users.UserName, Products.ProductName, Products.Price FROM Users INNER JOIN Products ON Users.UserID=Products.UserID;
```

**预期结果：**
- 成功消息框
- 结果表仅显示指定的字段：UserName, ProductName, Price
- 字段名使用"TableName.FieldName"格式

---

#### 测试用例 3.12: SELECT - JOIN查询（带WHERE子句）
**SQL语句：**
```sql
SELECT * FROM Users INNER JOIN Products ON Users.UserID=Products.UserID WHERE Users.UserID='1';
```

**预期结果：**
- 成功消息框
- 结果表显示连接后的数据，但只包含UserID='1'的记录
- **注意**：WHERE子句在JOIN之后应用，用于过滤连接结果

---

#### 测试用例 3.13: SELECT - 多表JOIN查询
**前置条件**：创建一个Orders表用于测试多表JOIN：
```sql
CREATE TABLE Orders (
    OrderID int KEY NO_NULL VALID,
    UserID int NOT_KEY NULL VALID,
    ProductID int NOT_KEY NULL VALID,
    Quantity int NOT_KEY NULL VALID
) INTO test_db;

INSERT INTO Orders VALUES ('1', '1', '1', '2') IN test_db;
INSERT INTO Orders VALUES ('2', '1', '2', '1') IN test_db;
INSERT INTO Orders VALUES ('3', '2', '3', '1') IN test_db;
```

**SQL语句：**
```sql
SELECT Users.UserName, Products.ProductName, Orders.Quantity FROM Users INNER JOIN Products ON Users.UserID=Products.UserID INNER JOIN Orders ON Products.ProductID=Orders.ProductID;
```

**预期结果：**
- 成功消息框
- 结果表显示三个表连接后的数据
- 应该显示用户、产品和订单的关联信息

---

#### 测试用例 3.14: SELECT - JOIN查询（表名大小写不敏感）
**SQL语句：**
```sql
SELECT * FROM users INNER JOIN products ON users.UserID=products.UserID;
```

**预期结果：**
- 成功消息框
- 与"SELECT * FROM Users INNER JOIN Products ON Users.UserID=Products.UserID;"结果相同
- **注意**：表名不区分大小写

---

### 4. 错误处理测试

#### 测试用例 4.1: 空SQL语句
**SQL语句：**
```
(空)
```

**预期结果：**
- 信息消息框："Please enter a SQL statement."
- 不执行

---

#### 测试用例 4.2: 无效SQL语法
**SQL语句：**
```sql
SELECT * FRM Users;
```

**预期结果：**
- 错误消息框："Expected 'FROM', but got: FRM"或类似
- 状态标签："Error: ..."

---

#### 测试用例 4.3: 缺少数据库
**操作：**
1. 关闭数据库（如果已打开）
2. 尝试执行任何SQL

**预期结果：**
- 信息消息框："Please create or open a database first..."
- 不执行

---

#### 测试用例 4.4: DDL中无效的表名
**SQL语句：**
```sql
CREATE TABLE 123Invalid (
    ID int KEY NO_NULL VALID
) INTO test_db;
```

**预期结果：**
- 错误消息框："Expected identifier, but got: 123Invalid"或类似

---

### 5. 批量执行测试

#### 测试用例 5.1: 批量执行多条INSERT语句
**SQL语句：**
```sql
INSERT INTO Users VALUES ('4', 'Alice', 'alice@example.com', '22') IN test_db;
INSERT INTO Users VALUES ('5', 'Bob', 'bob@example.com', '25') IN test_db;
INSERT INTO Users VALUES ('6', 'Charlie', 'charlie@example.com', '30') IN test_db;
```

**预期结果：**
- 批量执行成功消息框，显示总语句数、成功数、受影响行数
- 所有3条记录都成功插入

---

#### 测试用例 5.2: 批量执行混合语句（部分成功，部分失败）
**SQL语句：**
```sql
INSERT INTO Users VALUES ('7', 'David', 'david@example.com', '28') IN test_db;
INSERT INTO Users VALUES ('1', 'Duplicate', 'duplicate@example.com', '99') IN test_db;
INSERT INTO Users VALUES ('8', 'Eve', 'eve@example.com', '24') IN test_db;
```

**预期结果：**
- 批量执行完成但包含错误的消息框
- 显示：Total statements: 3, Successful: 2, Failed: 1
- 错误信息：Statement 2: Primary key constraint violation: Field (UserID) value already exists
- 第1条和第3条记录成功插入，第2条因主键冲突失败

---

#### 测试用例 5.3: 批量执行包含查询的语句
**SQL语句：**
```sql
INSERT INTO Users VALUES ('9', 'Frank', 'frank@example.com', '27') IN test_db;
SELECT * FROM Users WHERE UserID='9';
```

**预期结果：**
- 批量执行成功消息框
- 结果表显示UserID='9'的记录
- 消息框显示："Last query returned 1 row(s)."

---

### 6. 消息框一致性测试

#### 测试用例 6.1: 所有消息框使用Segoe UI字体
**操作：**
- 执行各种SQL语句（成功和错误情况）
- 检查所有消息框

**预期结果：**
- 所有消息框以Segoe UI字体显示文本
- 所有对话框外观一致

---

#### 测试用例 6.2: 成功消息始终显示
**操作：**
- 执行DDL、DML和查询语句
- 检查成功消息框

**预期结果：**
- 每次成功执行都显示消息框
- DDL："DDL statement executed successfully"
- DML："DML statement executed successfully, X row(s) affected"
- 查询："Query executed successfully. X row(s) returned."

---

#### 测试用例 6.3: 错误消息始终显示
**操作：**
- 执行无效的SQL语句
- 检查错误消息框

**预期结果：**
- 每个错误都显示带错误图标的消息框
- 错误消息清晰且描述性强

---

## 测试执行检查清单

### 基础功能
- [ ] DDL语句正确工作（CREATE, EDIT, RENAME, DROP TABLE）
- [ ] DML语句正确工作（INSERT, UPDATE, DELETE）
- [ ] 查询语句正确工作（SELECT）
- [ ] 表名不区分大小写
- [ ] 所有语句都需要分号（;）
- [ ] 所有成功情况都显示消息框
- [ ] 所有错误情况都显示消息框
- [ ] 所有消息框使用Segoe UI字体
- [ ] 错误消息为英文
- [ ] 成功消息为英文
- [ ] 状态标签正确更新
- [ ] 结果表正确显示
- [ ] 批量执行功能正常工作
- [ ] 批量执行时正确分割多条SQL语句
- [ ] 批量执行时部分失败不影响其他语句

### JOIN查询功能
- [ ] INNER JOIN正确工作
- [ ] LEFT JOIN正确工作
- [ ] RIGHT JOIN正确工作
- [ ] JOIN查询支持指定字段（TableName.FieldName）
- [ ] JOIN查询支持WHERE子句
- [ ] 多表JOIN查询正确工作
- [ ] JOIN查询中表名大小写不敏感

### 新实现功能检查清单
- [x] ORDER BY排序（已实现）✅
- [x] DISTINCT去重（已实现）✅
- [x] LIMIT分页（已实现）✅
- [x] 比较运算符（>, <, >=, <=, !=）（已实现）✅
- [x] 复杂WHERE条件（AND, OR, NOT）（已实现）✅
- [x] LIKE模式匹配（已实现）✅
- [x] IN子句（已实现）✅
- [x] BETWEEN范围查询（已实现）✅

---

### 7. 新实现功能测试（ORDER BY, DISTINCT, LIMIT, 复杂WHERE条件）

#### 测试用例 7.1: ORDER BY - 单字段升序排序
**SQL语句：**
```sql
SELECT * FROM Users ORDER BY Age ASC;
```

**预期结果：**
- 成功消息框
- 结果表按Age字段升序排列（从小到大）

---

#### 测试用例 7.2: ORDER BY - 单字段降序排序
**SQL语句：**
```sql
SELECT * FROM Users ORDER BY Age DESC;
```

**预期结果：**
- 成功消息框
- 结果表按Age字段降序排列（从大到小）

---

#### 测试用例 7.3: ORDER BY - 多字段排序
**SQL语句：**
```sql
SELECT * FROM Users ORDER BY Age ASC, UserID DESC;
```

**预期结果：**
- 成功消息框
- 结果表先按Age升序，Age相同时按UserID降序排列

---

#### 测试用例 7.4: DISTINCT - 去重
**SQL语句：**
```sql
SELECT DISTINCT Age FROM Users;
```

**预期结果：**
- 成功消息框
- 结果表只显示唯一的Age值（去除重复）

---

#### 测试用例 7.5: LIMIT - 限制返回行数
**SQL语句：**
```sql
SELECT * FROM Users LIMIT 3;
```

**预期结果：**
- 成功消息框
- 结果表最多返回3行记录

---

#### 测试用例 7.6: LIMIT - 与ORDER BY组合
**SQL语句：**
```sql
SELECT * FROM Users ORDER BY Age DESC LIMIT 2;
```

**预期结果：**
- 成功消息框
- 结果表按Age降序排列，只返回前2条记录

---

#### 测试用例 7.7: WHERE - 大于运算符（>）
**SQL语句：**
```sql
SELECT * FROM Users WHERE Age > '25';
```

**预期结果：**
- 成功消息框
- 结果表只显示Age大于25的记录

---

#### 测试用例 7.8: WHERE - 小于运算符（<）
**SQL语句：**
```sql
SELECT * FROM Users WHERE Age < '30';
```

**预期结果：**
- 成功消息框
- 结果表只显示Age小于30的记录

---

#### 测试用例 7.9: WHERE - 大于等于运算符（>=）
**SQL语句：**
```sql
SELECT * FROM Users WHERE Age >= '26';
```

**预期结果：**
- 成功消息框
- 结果表只显示Age大于等于26的记录

---

#### 测试用例 7.10: WHERE - 小于等于运算符（<=）
**SQL语句：**
```sql
SELECT * FROM Users WHERE Age <= '28';
```

**预期结果：**
- 成功消息框
- 结果表只显示Age小于等于28的记录

---

#### 测试用例 7.11: WHERE - 不等于运算符（!=）
**SQL语句：**
```sql
SELECT * FROM Users WHERE Age != '26';
```

**预期结果：**
- 成功消息框
- 结果表只显示Age不等于26的记录

---

#### 测试用例 7.12: WHERE - AND逻辑运算符
**SQL语句：**
```sql
SELECT * FROM Users WHERE Age > '25' AND Age < '30';
```

**预期结果：**
- 成功消息框
- 结果表只显示Age大于25且小于30的记录

---

#### 测试用例 7.13: WHERE - OR逻辑运算符
**SQL语句：**
```sql
SELECT * FROM Users WHERE Age = '26' OR Age = '30';
```

**预期结果：**
- 成功消息框
- 结果表显示Age等于26或30的记录

---

#### 测试用例 7.14: WHERE - NOT逻辑运算符
**SQL语句：**
```sql
SELECT * FROM Users WHERE NOT Age = '26';
```

**预期结果：**
- 成功消息框
- 结果表显示Age不等于26的记录（与!=效果相同）

---

#### 测试用例 7.15: WHERE - 复杂AND/OR组合
**SQL语句：**
```sql
SELECT * FROM Users WHERE (Age > '25' AND Age < '30') OR UserID = '1';
```

**预期结果：**
- 成功消息框
- 结果表显示Age在25-30之间或UserID为1的记录

---

#### 测试用例 7.16: WHERE - LIKE模式匹配（前缀匹配）
**SQL语句：**
```sql
SELECT * FROM Users WHERE UserName LIKE 'John%';
```

**预期结果：**
- 成功消息框
- 结果表显示UserName以"John"开头的记录

---

#### 测试用例 7.17: WHERE - LIKE模式匹配（后缀匹配）
**SQL语句：**
```sql
SELECT * FROM Users WHERE Email LIKE '%@example.com';
```

**预期结果：**
- 成功消息框
- 结果表显示Email以"@example.com"结尾的记录

---

#### 测试用例 7.18: WHERE - LIKE模式匹配（包含匹配）
**SQL语句：**
```sql
SELECT * FROM Users WHERE UserName LIKE '%Smith%';
```

**预期结果：**
- 成功消息框
- 结果表显示UserName包含"Smith"的记录

---

#### 测试用例 7.19: WHERE - IN子句
**SQL语句：**
```sql
SELECT * FROM Users WHERE Age IN ('26', '30', '28');
```

**预期结果：**
- 成功消息框
- 结果表显示Age为26、30或28的记录

---

#### 测试用例 7.20: WHERE - BETWEEN范围查询
**SQL语句：**
```sql
SELECT * FROM Users WHERE Age BETWEEN '25' AND '30';
```

**预期结果：**
- 成功消息框
- 结果表显示Age在25到30之间（包含边界）的记录

---

#### 测试用例 7.21: 组合使用 - DISTINCT + ORDER BY + LIMIT
**SQL语句：**
```sql
SELECT DISTINCT Age FROM Users ORDER BY Age DESC LIMIT 3;
```

**预期结果：**
- 成功消息框
- 结果表显示唯一的Age值，按降序排列，最多返回3条

---

#### 测试用例 7.22: 组合使用 - 复杂WHERE + ORDER BY
**SQL语句：**
```sql
SELECT * FROM Users WHERE Age > '25' AND Email LIKE '%@example.com' ORDER BY Age ASC;
```

**预期结果：**
- 成功消息框
- 结果表显示Age大于25且Email以"@example.com"结尾的记录，按Age升序排列

---

### 8. GROUP BY和聚合函数测试

**前置条件**：确保Users表和Products表已创建并包含数据（参考测试用例1.1、1.2、2.1-2.2和3.7的前置条件）。

#### 测试用例 8.1: COUNT(*) - 统计所有记录数
**SQL语句：**
```sql
SELECT COUNT(*) FROM Users;
```

**预期结果：**
- 成功消息框
- 结果表显示一行一列：COUNT(*) = 当前Users表的记录数（例如：2，如果只插入了2条记录）

---

#### 测试用例 8.2: COUNT(Field) - 统计非空字段数
**SQL语句：**
```sql
SELECT COUNT(Age) FROM Users;
```

**预期结果：**
- 成功消息框
- 结果表显示一行一列：COUNT(Age) = 非空Age字段的数量

---

#### 测试用例 8.3: SUM - 求和
**SQL语句：**
```sql
SELECT SUM(Price) FROM Products;
```

**预期结果：**
- 成功消息框
- 结果表显示一行一列：SUM(Price) = 所有产品价格的总和（例如：1495.46）

---

#### 测试用例 8.4: AVG - 平均值
**SQL语句：**
```sql
SELECT AVG(Age) FROM Users;
```

**预期结果：**
- 成功消息框
- 结果表显示一行一列：AVG(Age) = 所有用户年龄的平均值

---

#### 测试用例 8.5: MAX - 最大值
**SQL语句：**
```sql
SELECT MAX(Price) FROM Products;
```

**预期结果：**
- 成功消息框
- 结果表显示一行一列：MAX(Price) = 最高价格（例如：1000.50）

---

#### 测试用例 8.6: MIN - 最小值
**SQL语句：**
```sql
SELECT MIN(Age) FROM Users;
```

**预期结果：**
- 成功消息框
- 结果表显示一行一列：MIN(Age) = 最小年龄

---

#### 测试用例 8.7: GROUP BY - 单字段分组
**SQL语句：**
```sql
SELECT UserID, COUNT(*) FROM Products GROUP BY UserID;
```

**预期结果：**
- 成功消息框
- 结果表显示按UserID分组的记录数和每个UserID的产品数量
- 例如：UserID=1有3个产品，UserID=2有2个产品

---

#### 测试用例 8.8: GROUP BY - 多字段分组
**SQL语句：**
```sql
SELECT UserID, Age, COUNT(*) FROM Users GROUP BY UserID, Age;
```

**预期结果：**
- 成功消息框
- 结果表显示按UserID和Age组合分组的记录数

---

#### 测试用例 8.9: GROUP BY + 聚合函数组合
**SQL语句：**
```sql
SELECT UserID, COUNT(*), SUM(Price), AVG(Price) FROM Products GROUP BY UserID;
```

**预期结果：**
- 成功消息框
- 结果表显示每个UserID的产品数量、总价和平均价格

---

#### 测试用例 8.10: HAVING - 过滤分组结果
**SQL语句：**
```sql
SELECT UserID, COUNT(*) FROM Products GROUP BY UserID HAVING COUNT(*) > 2;
```

**预期结果：**
- 成功消息框
- 结果表只显示产品数量大于2的UserID（例如：UserID=1）

---

#### 测试用例 8.11: HAVING - 使用聚合函数条件
**SQL语句：**
```sql
SELECT UserID, AVG(Price) FROM Products GROUP BY UserID HAVING AVG(Price) > 100;
```

**预期结果：**
- 成功消息框
- 结果表只显示平均价格大于100的UserID

---

#### 测试用例 8.12: HAVING - 复杂条件
**SQL语句：**
```sql
SELECT UserID, COUNT(*), SUM(Price) FROM Products GROUP BY UserID HAVING COUNT(*) > 1 AND SUM(Price) > 50;
```

**预期结果：**
- 成功消息框
- 结果表显示满足两个条件的UserID分组

---

#### 测试用例 8.13: GROUP BY + WHERE + HAVING组合
**SQL语句：**
```sql
SELECT UserID, COUNT(*) FROM Products WHERE Price > 50 GROUP BY UserID HAVING COUNT(*) >= 1;
```

**预期结果：**
- 成功消息框
- 结果表显示Price>50的产品，按UserID分组，且分组后数量>=1

---

#### 测试用例 8.14: GROUP BY + ORDER BY组合
**SQL语句：**
```sql
SELECT UserID, COUNT(*) FROM Products GROUP BY UserID ORDER BY COUNT(*) DESC;
```

**预期结果：**
- 成功消息框
- 结果表按产品数量降序排列

---

### 9. FULL OUTER JOIN测试

**前置条件**：确保Users表和Products表已创建并包含数据。

#### 测试用例 9.1: FULL OUTER JOIN - 基本用法
**SQL语句：**
```sql
SELECT * FROM Users FULL OUTER JOIN Products ON Users.UserID = Products.UserID;
```

**预期结果：**
- 成功消息框
- 结果表显示所有Users记录和所有Products记录
- 如果Users表中某条记录在Products表中没有匹配，Products字段显示为空
- 如果Products表中某条记录在Users表中没有匹配，Users字段显示为空

---

#### 测试用例 9.2: FULL JOIN - OUTER关键字可选
**SQL语句：**
```sql
SELECT * FROM Users FULL JOIN Products ON Users.UserID = Products.UserID;
```

**预期结果：**
- 成功消息框
- 与FULL OUTER JOIN结果相同

---

#### 测试用例 9.3: FULL OUTER JOIN + WHERE条件
**SQL语句：**
```sql
SELECT * FROM Users FULL OUTER JOIN Products ON Users.UserID = Products.UserID WHERE Users.Age > 25;
```

**预期结果：**
- 成功消息框
- 结果表显示FULL OUTER JOIN的结果，但只包含Age>25的Users记录

---

### 10. NATURAL JOIN测试

**前置条件**：确保Users表和Products表已创建并包含数据，且两个表有共同的字段名（如UserID）。

#### 测试用例 10.1: NATURAL JOIN - 基本用法
**SQL语句：**
```sql
SELECT * FROM Users NATURAL JOIN Products;
```

**预期结果：**
- 成功消息框
- 结果表显示Users和Products通过共同字段（UserID）自动连接的结果
- 只返回两个表中都有匹配的记录（类似INNER JOIN）

---

#### 测试用例 10.2: NATURAL LEFT JOIN
**SQL语句：**
```sql
SELECT * FROM Users NATURAL LEFT JOIN Products;
```

**预期结果：**
- 成功消息框
- 结果表显示Users表的所有记录，以及匹配的Products记录

---

#### 测试用例 10.3: NATURAL RIGHT JOIN
**SQL语句：**
```sql
SELECT * FROM Users NATURAL RIGHT JOIN Products;
```

**预期结果：**
- 成功消息框
- 结果表显示Products表的所有记录，以及匹配的Users记录

---

#### 测试用例 10.4: NATURAL INNER JOIN
**SQL语句：**
```sql
SELECT * FROM Users NATURAL INNER JOIN Products;
```

**预期结果：**
- 成功消息框
- 结果表显示两个表中都有匹配的记录

---

#### 测试用例 10.5: NATURAL FULL JOIN
**SQL语句：**
```sql
SELECT * FROM Users NATURAL FULL JOIN Products;
```

**预期结果：**
- 成功消息框
- 结果表显示所有匹配和未匹配的记录

---

### 11. UNION测试

**前置条件**：确保Users表已创建并包含数据。

#### 测试用例 11.1: UNION - 基本用法（去重）
**SQL语句：**
```sql
SELECT UserID FROM Users WHERE Age = 24 UNION SELECT UserID FROM Users WHERE Age = 27;
```

**预期结果：**
- 成功消息框
- 结果表显示Age=24或Age=27的UserID，自动去除重复

---

#### 测试用例 11.2: UNION ALL - 保留重复
**SQL语句：**
```sql
SELECT UserID FROM Users WHERE Age = 24 UNION ALL SELECT UserID FROM Users WHERE Age = 27;
```

**预期结果：**
- 成功消息框
- 结果表显示Age=24或Age=27的UserID，保留所有重复

---

#### 测试用例 11.3: UNION - 多列
**SQL语句：**
```sql
SELECT UserID, UserName FROM Users WHERE Age = 24 UNION SELECT UserID, UserName FROM Users WHERE Age = 27;
```

**预期结果：**
- 成功消息框
- 结果表显示两列：UserID和UserName，来自Age=24或Age=27的记录

---

#### 测试用例 11.4: UNION - 多个查询
**SQL语句：**
```sql
SELECT UserID FROM Users WHERE Age = 24 UNION SELECT UserID FROM Users WHERE Age = 27 UNION SELECT UserID FROM Users WHERE Age = 28;
```

**预期结果：**
- 成功消息框
- 结果表显示Age为24、27或28的UserID

---

#### 测试用例 11.5: UNION + ORDER BY
**SQL语句：**
```sql
SELECT UserID, UserName FROM Users WHERE Age = 24 UNION SELECT UserID, UserName FROM Users WHERE Age = 27 ORDER BY UserID;
```

**预期结果：**
- 成功消息框
- 结果表按UserID排序显示

---

#### 测试用例 11.6: UNION - 列数不匹配（错误）
**SQL语句：**
```sql
SELECT UserID, UserName FROM Users UNION SELECT UserID FROM Users;
```

**预期结果：**
- 错误消息框："UNION queries must have the same number of columns"

---

### 12. 子查询测试

**前置条件**：确保Users表和Products表已创建并包含数据。

#### 测试用例 12.1: 标量子查询 - 等值比较
**SQL语句：**
```sql
SELECT * FROM Users WHERE UserID = (SELECT UserID FROM Users WHERE UserName = 'John Doe');
```

**预期结果：**
- 成功消息框
- 结果表显示UserID等于子查询返回值的记录（John Doe的记录）

---

#### 测试用例 12.2: 标量子查询 - 大于比较
**SQL语句：**
```sql
SELECT * FROM Users WHERE Age > (SELECT Age FROM Users WHERE UserName = 'John Doe');
```

**预期结果：**
- 成功消息框
- 结果表显示Age大于John Doe的Age的所有用户

---

#### 测试用例 12.3: IN子查询
**SQL语句：**
```sql
SELECT * FROM Users WHERE UserID IN (SELECT UserID FROM Products);
```

**预期结果：**
- 成功消息框
- 结果表显示UserID在Products表中出现的所有用户

---

#### 测试用例 12.4: EXISTS子查询
**SQL语句：**
```sql
SELECT * FROM Users WHERE EXISTS (SELECT * FROM Products WHERE Products.UserID = Users.UserID);
```

**预期结果：**
- 成功消息框
- 结果表显示在Products表中有对应记录的用户（关联子查询）

---

#### 测试用例 12.5: NOT EXISTS子查询
**SQL语句：**
```sql
SELECT * FROM Users WHERE NOT EXISTS (SELECT * FROM Products WHERE Products.UserID = Users.UserID);
```

**预期结果：**
- 成功消息框
- 结果表显示在Products表中没有对应记录的用户

---

#### 测试用例 12.6: 子查询与聚合函数
**SQL语句：**
```sql
SELECT * FROM Users WHERE Age > (SELECT AVG(Age) FROM Users);
```

**预期结果：**
- 成功消息框
- 结果表显示Age大于平均年龄的用户

---

#### 测试用例 12.7: 嵌套子查询
**SQL语句：**
```sql
SELECT * FROM Users WHERE UserID = (SELECT UserID FROM Products WHERE Price = (SELECT MAX(Price) FROM Products));
```

**预期结果：**
- 成功消息框
- 结果表显示拥有最高价格产品的用户

---

#### 测试用例 12.8: 子查询与ORDER BY组合
**SQL语句：**
```sql
SELECT * FROM Users WHERE Age >= (SELECT MIN(Age) FROM Users) ORDER BY Age;
```

**预期结果：**
- 成功消息框
- 结果表显示所有用户（因为MIN(Age)是最小值），按Age排序

---

### 13. 组合功能测试

#### 测试用例 13.1: JOIN + WHERE + ORDER BY + LIMIT
**SQL语句：**
```sql
SELECT Users.UserName, Products.ProductName, Products.Price FROM Users INNER JOIN Products ON Users.UserID = Products.UserID WHERE Products.Price > 50 ORDER BY Products.Price DESC LIMIT 3;
```

**预期结果：**
- 成功消息框
- 结果表显示价格>50的产品，按价格降序排列，只返回前3条

---

#### 测试用例 13.2: GROUP BY + HAVING + ORDER BY
**SQL语句：**
```sql
SELECT UserID, COUNT(*), AVG(Price) FROM Products GROUP BY UserID HAVING COUNT(*) > 1 ORDER BY AVG(Price) DESC;
```

**预期结果：**
- 成功消息框
- 结果表显示产品数量>1的用户，按平均价格降序排列

---

#### 测试用例 13.3: UNION + ORDER BY + LIMIT
**SQL语句：**
```sql
SELECT UserID, UserName FROM Users WHERE Age = 24 UNION SELECT UserID, UserName FROM Users WHERE Age = 27 ORDER BY UserID LIMIT 2;
```

**预期结果：**
- 成功消息框
- 结果表显示UNION结果，按UserID排序，只返回前2条

---

#### 测试用例 13.4: 子查询 + JOIN + WHERE
**SQL语句：**
```sql
SELECT * FROM Users INNER JOIN Products ON Users.UserID = Products.UserID WHERE Products.Price > (SELECT AVG(Price) FROM Products);
```

**预期结果：**
- 成功消息框
- 结果表显示价格高于平均价格的产品及其用户信息

---

### 14. 未实现功能（未来版本）
- [x] GROUP BY分组（已实现 ✅）
- [x] 聚合函数（COUNT, SUM, AVG, MAX, MIN）（已实现 ✅）
- [x] HAVING子句（已实现 ✅）
- [x] FULL OUTER JOIN（已实现 ✅）
- [x] NATURAL JOIN（已实现 ✅）
- [x] 子查询（已实现 ✅）
- [x] UNION并集（已实现 ✅）

## 注意事项

1. **分号要求**：所有SQL语句必须以分号（;）结尾。这是严格要求。

2. **大小写敏感性**：
   - 表名不区分大小写（与SQL Server对齐）
   - 关键字不区分大小写（SELECT、select、Select都可以）

3. **消息框**：
   - 所有消息框使用Segoe UI字体
   - 成功和错误情况都显示消息框以保持一致性
   - 消息框标题和内容为英文

4. **错误消息**：所有错误消息为英文且描述性强。

5. **已实现的JOIN类型**：
   - INNER JOIN：内连接，只返回匹配的记录
   - LEFT JOIN：左连接，返回左表的所有记录
   - RIGHT JOIN：右连接，返回右表的所有记录
   - FULL OUTER JOIN：全外连接，返回所有匹配和未匹配的记录
   - **注意**：NATURAL JOIN当前未实现

6. **已实现的新功能**（v0.7.0）：
   - ✅ ORDER BY（排序）- 支持单字段和多字段排序，ASC/DESC
   - ✅ DISTINCT（去重）
   - ✅ LIMIT（分页）
   - ✅ 比较运算符（>, <, >=, <=, !=）
   - ✅ 复杂WHERE条件（AND, OR, NOT，支持括号优先级）
   - ✅ LIKE模式匹配（支持%通配符，前缀匹配、后缀匹配、包含匹配，大小写敏感）
   - ✅ IN子句（支持值列表查询和子查询）
   - ✅ BETWEEN范围查询（支持数值和字符串范围查询，包含边界）
   - ✅ GROUP BY（分组）- 支持单字段和多字段分组
   - ✅ 聚合函数（COUNT, SUM, AVG, MAX, MIN）- 支持COUNT(*), COUNT(Field), SUM, AVG, MAX, MIN
   - ✅ HAVING子句（分组过滤）- 支持对聚合函数结果和分组字段的过滤，支持复杂条件
   - ✅ FULL OUTER JOIN（全外连接）- 返回所有匹配和未匹配的记录
   - ✅ NATURAL JOIN（自然连接）- 支持NATURAL JOIN, NATURAL LEFT JOIN, NATURAL RIGHT JOIN, NATURAL INNER JOIN, NATURAL FULL JOIN
   - ✅ UNION（并集操作）- 支持UNION（去重）和UNION ALL（保留重复），支持多个UNION连接，支持ORDER BY和LIMIT
   - ✅ 子查询（嵌套查询）- 支持标量子查询、IN子查询、EXISTS/NOT EXISTS子查询、关联子查询、嵌套子查询

7. **测试用例执行顺序建议**：
   - 建议按照测试用例编号顺序执行，确保前置条件满足
   - DDL测试（1.x）→ DML测试（2.x）→ 基础查询测试（3.x）→ 错误处理测试（4.x）→ 批量执行测试（5.x）→ 消息框测试（6.x）→ 新功能测试（7.x-13.x）
   - 每个测试用例的前置条件已在相应章节说明
