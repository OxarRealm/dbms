# UNION功能测试用例

基于现有的Users表和products表数据。

## 测试用例1：基本UNION（去重）

**SQL语句：**
```sql
SELECT UserID FROM Users WHERE Age = 30 UNION SELECT UserID FROM Users WHERE Age = 28;
```

**预期输出：**
- 列名：UserID
- 行数：3
- 数据：
  - 2 (Jane Smith, Age=30)
  - 6 (Charlie, Age=30)
  - 7 (David, Age=28)
  - 9 (Oscar, Age=28)
- **注意**：如果UserID=7和UserID=9都返回，但由于UNION去重，如果UserID相同则只显示一次

**说明**：UNION会自动去除重复的行。

---

## 测试用例2：UNION ALL（保留重复）

**SQL语句：**
```sql
SELECT UserID FROM Users WHERE Age = 30 UNION ALL SELECT UserID FROM Users WHERE Age = 28;
```

**预期输出：**
- 列名：UserID
- 行数：4
- 数据：
  - 2 (Jane Smith, Age=30)
  - 6 (Charlie, Age=30)
  - 7 (David, Age=28)
  - 9 (Oscar, Age=28)
- **注意**：UNION ALL保留所有行，包括重复的

**说明**：UNION ALL不会去重，保留所有结果。

---

## 测试用例3：多列UNION

**SQL语句：**
```sql
SELECT UserID, UserName FROM Users WHERE Age = 24 UNION SELECT UserID, UserName FROM Users WHERE Age = 27;
```

**预期输出：**
- 列名：UserID, UserName
- 行数：4
- 数据：
  - 4, Alice (Age=24)
  - 8, Eve (Age=24)
  - 5, Bob (Age=27)
  - 9, Frank (Age=27)

**说明**：UNION支持多列，所有查询的列数必须相同。

---

## 测试用例4：多个UNION连接

**SQL语句：**
```sql
SELECT UserID FROM Users WHERE Age = 24 UNION SELECT UserID FROM Users WHERE Age = 27 UNION SELECT UserID FROM Users WHERE Age = 28;
```

**预期输出：**
- 列名：UserID
- 行数：5
- 数据：
  - 4 (Alice, Age=24)
  - 8 (Eve, Age=24)
  - 5 (Bob, Age=27)
  - 9 (Frank, Age=27)
  - 7 (David, Age=28)
  - 9 (Oscar, Age=28)
- **注意**：UserID=9如果同时满足Age=27和Age=28，UNION会去重，只显示一次

**说明**：支持多个SELECT语句用UNION连接。

---

## 测试用例5：UNION与WHERE条件组合

**SQL语句：**
```sql
SELECT UserID, UserName FROM Users WHERE Age >= 28 UNION SELECT UserID, UserName FROM Users WHERE UserID <= 2;
```

**预期输出：**
- 列名：UserID, UserName
- 行数：6
- 数据：
  - 2, Jane Smith (Age=30, >=28)
  - 6, Charlie (Age=30, >=28)
  - 7, David (Age=28, >=28)
  - 9, Oscar (Age=28, >=28)
  - 1, John Doe (UserID=1, <=2)
  - 2, Jane Smith (UserID=2, <=2)
- **注意**：UserID=2同时满足两个条件，UNION去重后只显示一次

**说明**：UNION可以与WHERE条件组合使用。

---

## 测试用例6：UNION ALL保留所有重复

**SQL语句：**
```sql
SELECT UserID FROM Users WHERE UserID IN (1, 2) UNION ALL SELECT UserID FROM Users WHERE UserID IN (2, 3);
```

**预期输出：**
- 列名：UserID
- 行数：4
- 数据：
  - 1
  - 2
  - 2 (重复，因为UNION ALL保留)
  - 3

**说明**：UNION ALL会保留所有行，包括完全重复的行。

---

## 测试用例7：UNION与SELECT *组合

**SQL语句：**
```sql
SELECT * FROM Users WHERE Age = 24 UNION SELECT * FROM Users WHERE Age = 27;
```

**预期输出：**
- 列名：UserID, UserName, Email, Age
- 行数：4
- 数据：
  - 4, Alice, alice@example.com, 24
  - 8, Eve, eve@example.com, 24
  - 5, Bob, bob@example.com, 27
  - 9, Frank, frank@example.com, 27

**说明**：UNION支持SELECT *，但所有查询的列数必须相同。

---

## 测试用例8：错误测试 - 列数不匹配

**SQL语句：**
```sql
SELECT UserID, UserName FROM Users UNION SELECT UserID FROM Users;
```

**预期输出：**
- 错误信息："UNION queries must have the same number of columns"

**说明**：UNION要求所有查询的列数必须相同。

---

## 测试用例9：错误测试 - 混合UNION和UNION ALL

**SQL语句：**
```sql
SELECT UserID FROM Users UNION ALL SELECT UserID FROM Users UNION SELECT UserID FROM Users;
```

**预期输出：**
- 错误信息："UNION and UNION ALL cannot be mixed"

**说明**：不能混合使用UNION和UNION ALL。

---

## 测试用例10：UNION与ORDER BY组合

**SQL语句：**
```sql
SELECT UserID, UserName FROM Users WHERE Age = 24 UNION SELECT UserID, UserName FROM Users WHERE Age = 27 ORDER BY UserID;
```

**预期输出：**
- 列名：UserID, UserName
- 行数：4
- 数据（按UserID排序）：
  - 4, Alice
  - 5, Bob
  - 8, Eve
  - 9, Frank

**说明**：UNION结果可以使用ORDER BY排序（ORDER BY在最后一个SELECT之后）。

---

## 测试数据参考

**Users表数据：**
- UserID=1, UserName=John Doe, Email=john@example.com, Age=26
- UserID=2, UserName=Jane Smith, Email=jane@example.com, Age=30
- UserID=3, UserName=Oscar, Email=oscar@example.com, Age=28
- UserID=4, UserName=Alice, Email=alice@example.com, Age=24
- UserID=5, UserName=Bob, Email=bob@example.com, Age=27
- UserID=6, UserName=Charlie, Email=charlie@example.com, Age=30
- UserID=7, UserName=David, Email=david@example.com, Age=28
- UserID=8, UserName=Eve, Email=eve@example.com, Age=24
- UserID=9, UserName=Frank, Email=frank@example.com, Age=27

**products表数据：**
- ProductID=1, ProductName=Laptop, Price=1000.5, UserID=1
- ProductID=2, ProductName=Mouse, Price=25.99, UserID=1
- ProductID=3, ProductName=Keyboard, Price=89.99, UserID=2
- ProductID=4, ProductName=Monitor, Price=299.99, UserID=2
- ProductID=5, ProductName=Headphones, Price=79.99, UserID=1

---

**最后更新**：2026-01-15

