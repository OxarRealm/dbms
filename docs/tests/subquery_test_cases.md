# 子查询功能测试用例

基于现有的Users表和products表数据。

## 测试用例1：标量子查询（等值比较）

**SQL语句：**
```sql
SELECT * FROM Users WHERE UserID = (SELECT UserID FROM Users WHERE UserName = 'John Doe');
```

**预期输出：**
- 列名：UserID, UserName, Email, Age
- 行数：1
- 数据：
  - 1, John Doe, john@example.com, 26

**说明**：子查询返回John Doe的UserID（1），主查询查找UserID=1的用户。

---

## 测试用例2：标量子查询（大于比较）

**SQL语句：**
```sql
SELECT * FROM Users WHERE Age > (SELECT Age FROM Users WHERE UserName = 'John Doe');
```

**预期输出：**
- 列名：UserID, UserName, Email, Age
- 行数：5
- 数据：
  - 2, Jane Smith, jane@example.com, 30
  - 5, Bob, bob@example.com, 27
  - 6, Charlie, charlie@example.com, 30
  - 7, David, david@example.com, 28
  - 9, Frank, frank@example.com, 27

**说明**：子查询返回John Doe的Age（26），主查询查找Age>26的用户。

---

## 测试用例3：IN子查询

**SQL语句：**
```sql
SELECT * FROM Users WHERE UserID IN (SELECT UserID FROM products);
```

**预期输出：**
- 列名：UserID, UserName, Email, Age
- 行数：2
- 数据：
  - 1, John Doe, john@example.com, 26
  - 2, Jane Smith, jane@example.com, 30

**说明**：子查询返回products表中的所有UserID（1, 2），主查询查找这些UserID对应的用户。

---

## 测试用例4：IN子查询（多行结果）

**SQL语句：**
```sql
SELECT * FROM Users WHERE Age IN (SELECT DISTINCT Age FROM Users WHERE Age >= 28);
```

**预期输出：**
- 列名：UserID, UserName, Email, Age
- 行数：5
- 数据：
  - 2, Jane Smith, jane@example.com, 30
  - 3, Oscar, oscar@example.com, 28
  - 5, Bob, bob@example.com, 27
  - 6, Charlie, charlie@example.com, 30
  - 7, David, david@example.com, 28
  - 9, Frank, frank@example.com, 27

**说明**：子查询返回Age>=28的所有不同Age值（30, 28, 27），主查询查找这些Age的用户。

---

## 测试用例5：EXISTS子查询

**SQL语句：**
```sql
SELECT * FROM Users WHERE EXISTS (SELECT * FROM products WHERE products.UserID = Users.UserID);
```

**预期输出：**
- 列名：UserID, UserName, Email, Age
- 行数：2
- 数据：
  - 1, John Doe, john@example.com, 26
  - 2, Jane Smith, jane@example.com, 30

**说明**：EXISTS子查询检查是否存在products记录，其UserID与Users表的UserID匹配。返回有产品的用户。

**注意**：此测试用例需要关联子查询（子查询引用外部查询的字段），当前实现可能不支持，如果失败可以跳过。

---

## 测试用例6：NOT EXISTS子查询

**SQL语句：**
```sql
SELECT * FROM Users WHERE NOT EXISTS (SELECT * FROM products WHERE products.UserID = Users.UserID);
```

**预期输出：**
- 列名：UserID, UserName, Email, Age
- 行数：7
- 数据：所有没有产品的用户（UserID不是1或2的用户）

**说明**：NOT EXISTS子查询检查是否不存在products记录，其UserID与Users表的UserID匹配。返回没有产品的用户。

**注意**：此测试用例需要关联子查询，如果当前实现不支持，可以跳过。

---

## 测试用例7：子查询与普通条件组合

**SQL语句：**
```sql
SELECT * FROM Users WHERE Age > (SELECT Age FROM Users WHERE UserName = 'John Doe') AND UserID IN (SELECT UserID FROM products);
```

**预期输出：**
- 列名：UserID, UserName, Email, Age
- 行数：1
- 数据：
  - 2, Jane Smith, jane@example.com, 30

**说明**：组合使用标量子查询和IN子查询，查找Age>26且UserID在products表中的用户。

---

## 测试用例8：子查询与聚合函数

**SQL语句：**
```sql
SELECT * FROM Users WHERE Age > (SELECT AVG(Age) FROM Users);
```

**预期输出：**
- 列名：UserID, UserName, Email, Age
- 行数：5
- 数据：Age大于平均年龄的用户

**说明**：子查询计算Users表的平均年龄，主查询查找Age大于平均年龄的用户。

**注意**：需要先计算平均年龄（约27.3），然后查找Age>27.3的用户。

---

## 测试用例9：多层嵌套子查询（如果支持）

**SQL语句：**
```sql
SELECT * FROM Users WHERE UserID = (SELECT UserID FROM products WHERE Price = (SELECT MAX(Price) FROM products));
```

**预期输出：**
- 列名：UserID, UserName, Email, Age
- 行数：1
- 数据：
  - 1, John Doe, john@example.com, 26

**说明**：嵌套子查询：内层查询找到最高价格（1000.5），中层查询找到该价格的UserID（1），主查询查找该UserID的用户。

**注意**：如果当前实现不支持嵌套子查询，可以跳过此测试用例。

---

## 测试用例10：子查询与ORDER BY组合

**SQL语句：**
```sql
SELECT * FROM Users WHERE Age >= (SELECT MIN(Age) FROM Users) ORDER BY Age;
```

**预期输出：**
- 列名：UserID, UserName, Email, Age
- 行数：9（所有用户，因为MIN(Age)=24）
- 数据：按Age升序排序的所有用户

**说明**：子查询找到最小年龄（24），主查询查找Age>=24的用户并按Age排序。

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

