# GUI约束编辑界面测试用例

> 数据库管理系统 - GUI约束编辑功能测试用例文档
>
> 本文档提供GUI约束编辑界面的详细测试用例，确保GUI操作与SQL执行功能对齐。

---

## 📋 测试环境准备

### 前置条件
1. 已编译并运行DBMS应用程序
2. 创建或打开一个测试数据库（例如：`test_db`）
3. 确保数据库中没有表（或可以删除现有表）

---

## 🧪 测试用例1：创建表并添加外键约束

### 测试目标
验证通过GUI创建表并添加外键约束的功能。

### 测试步骤

#### 步骤1.1：创建被引用表（Departments）
1. 打开应用程序，进入 **Table Management** 标签页
2. 点击 **Create** 按钮
3. 在 **Fields** 标签页中：
   - 添加字段：`DeptID` (int, KEY, NO_NULL, VALID)
   - 添加字段：`DeptName` (string, NOT_KEY, NULL, VALID)
4. 表名输入：`Departments`
5. 点击 **Create** 按钮

**预期结果**：
- 表创建成功，显示 "Table created successfully."
- 表出现在左侧列表中

#### 步骤1.2：插入测试数据
1. 进入 **Data Management** 标签页
2. 选择表 `Departments`
3. 点击 **Insert** 按钮
4. 插入记录：
   - `DeptID`: `1`
   - `DeptName`: `Engineering`
5. 点击 **OK**

**预期结果**：
- 记录插入成功
- 表中显示新插入的记录

#### 步骤1.3：创建引用表（Employees）并添加外键约束
1. 返回 **Table Management** 标签页
2. 点击 **Create** 按钮
3. 在 **Fields** 标签页中：
   - 添加字段：`EmpID` (int, KEY, NO_NULL, VALID)
   - 添加字段：`EmpName` (string, NOT_KEY, NULL, VALID)
   - 添加字段：`DeptID` (int, NOT_KEY, NULL, VALID)
4. 切换到 **Constraints** 标签页
5. 在 **Foreign Key Constraints** 区域：
   - 点击 **Add** 按钮
   - 选择字段：`DeptID`
   - 选择引用表：`Departments`
   - 输入引用字段：`DeptID`
   - 选择 ON DELETE 动作：`RESTRICT`
   - 选择 ON UPDATE 动作：`RESTRICT`
   - 点击 **OK**
6. 表名输入：`Employees`
7. 点击 **Create** 按钮

**预期结果**：
- 表创建成功
- 外键约束显示在列表中：`Unnamed: DeptID -> Departments.DeptID (ON DELETE RESTRICT, ON UPDATE RESTRICT)`

#### 步骤1.4：测试外键约束
1. 进入 **Data Management** 标签页
2. 选择表 `Employees`
3. 点击 **Insert** 按钮
4. 尝试插入记录：
   - `EmpID`: `1`
   - `EmpName`: `Alice`
   - `DeptID`: `99` (不存在的部门ID)
5. 点击 **OK**

**预期结果**：
- 插入失败，显示错误消息：`Foreign key constraint violation: value '99' not found in referenced table 'Departments' field 'DeptID'`

6. 再次点击 **Insert** 按钮
7. 插入记录：
   - `EmpID`: `1`
   - `EmpName`: `Alice`
   - `DeptID`: `1` (存在的部门ID)
8. 点击 **OK**

**预期结果**：
- 插入成功

---

## 🧪 测试用例2：创建表并添加唯一约束（多字段）

### 测试目标
验证通过GUI创建表并添加多字段唯一约束的功能。

### 测试步骤

#### 步骤2.1：创建表（Orders）
1. 在 **Table Management** 标签页，点击 **Create** 按钮
2. 在 **Fields** 标签页中：
   - 添加字段：`OrderID` (int, KEY, NO_NULL, VALID)
   - 添加字段：`CustomerID` (int, NOT_KEY, NO_NULL, VALID)
   - 添加字段：`ProductID` (int, NOT_KEY, NO_NULL, VALID)
   - 添加字段：`Quantity` (int, NOT_KEY, NULL, VALID)
3. 切换到 **Constraints** 标签页
4. 在 **Unique Constraints** 区域：
   - 点击 **Add** 按钮
   - 输入字段名（逗号分隔）：`CustomerID, ProductID`
   - 点击 **OK**
5. 表名输入：`Orders`
6. 点击 **Create** 按钮

**预期结果**：
- 表创建成功
- 唯一约束显示在列表中：`Unnamed: (CustomerID, ProductID)`

#### 步骤2.2：测试多字段唯一约束
1. 进入 **Data Management** 标签页
2. 选择表 `Orders`
3. 点击 **Insert** 按钮
4. 插入记录：
   - `OrderID`: `1`
   - `CustomerID`: `100`
   - `ProductID`: `200`
   - `Quantity`: `5`
5. 点击 **OK**

**预期结果**：
- 插入成功

6. 再次点击 **Insert** 按钮
7. 尝试插入记录（违反唯一约束）：
   - `OrderID`: `2`
   - `CustomerID`: `100` (与第一条记录相同)
   - `ProductID`: `200` (与第一条记录相同)
   - `Quantity`: `3`
8. 点击 **OK**

**预期结果**：
- 插入失败，显示错误消息：`Unique constraint violation: combination of fields (CustomerID, ProductID) already exists`

9. 再次点击 **Insert** 按钮
10. 插入记录（不违反唯一约束）：
    - `OrderID`: `3`
    - `CustomerID`: `100` (相同)
    - `ProductID`: `201` (不同)
    - `Quantity`: `2`
11. 点击 **OK**

**预期结果**：
- 插入成功（因为组合不同）

---

## 🧪 测试用例3：创建表并添加检查约束

### 测试目标
验证通过GUI创建表并添加检查约束的功能。

### 测试步骤

#### 步骤3.1：创建表（Products）
1. 在 **Table Management** 标签页，点击 **Create** 按钮
2. 在 **Fields** 标签页中：
   - 添加字段：`ProductID` (int, KEY, NO_NULL, VALID)
   - 添加字段：`ProductName` (string, NOT_KEY, NULL, VALID)
   - 添加字段：`Price` (float, NOT_KEY, NULL, VALID)
   - 添加字段：`Stock` (int, NOT_KEY, NULL, VALID)
3. 切换到 **Constraints** 标签页
4. 在 **Check Constraints** 区域：
   - 点击 **Add** 按钮
   - 选择字段：`Price`
   - 输入表达式：`> 0`
   - 点击 **OK**
   - 再次点击 **Add** 按钮
   - 选择字段：`Stock`
   - 输入表达式：`>= 0`
   - 点击 **OK**
5. 表名输入：`Products`
6. 点击 **Create** 按钮

**预期结果**：
- 表创建成功
- 检查约束显示在列表中：
  - `Unnamed: Price > 0`
  - `Unnamed: Stock >= 0`

#### 步骤3.2：测试检查约束
1. 进入 **Data Management** 标签页
2. 选择表 `Products`
3. 点击 **Insert** 按钮
4. 尝试插入记录（违反Price约束）：
   - `ProductID`: `1`
   - `ProductName`: `Product1`
   - `Price`: `-10` (负数，违反约束)
   - `Stock`: `100`
5. 点击 **OK**

**预期结果**：
- 插入失败，显示错误消息：`Check constraint violation: value '-10' does not satisfy constraint '> 0' on field 'Price'`

6. 再次点击 **Insert** 按钮
7. 尝试插入记录（违反Stock约束）：
   - `ProductID`: `2`
   - `ProductName`: `Product2`
   - `Price`: `10.5`
   - `Stock`: `-5` (负数，违反约束)
8. 点击 **OK**

**预期结果**：
- 插入失败，显示错误消息：`Check constraint violation: value '-5' does not satisfy constraint '>= 0' on field 'Stock'`

9. 再次点击 **Insert** 按钮
10. 插入记录（满足所有约束）：
    - `ProductID`: `3`
    - `ProductName`: `Product3`
    - `Price`: `15.99`
    - `Stock`: `50`
11. 点击 **OK**

**预期结果**：
- 插入成功

#### 步骤3.3：测试UPDATE时的检查约束
1. 选择已插入的记录（ProductID=3）
2. 点击 **Edit** 按钮
3. 修改 `Price` 为：`-5`
4. 点击 **OK**

**预期结果**：
- 更新失败，显示错误消息：`Check constraint violation: value '-5' does not satisfy constraint '> 0' on field 'Price'`

5. 再次点击 **Edit** 按钮
6. 修改 `Price` 为：`20.5`
7. 点击 **OK**

**预期结果**：
- 更新成功

---

## 🧪 测试用例4：编辑表的约束

### 测试目标
验证通过GUI编辑表并修改约束的功能。

### 测试步骤

#### 步骤4.1：编辑表并添加约束
1. 在 **Table Management** 标签页，选择表 `Products`
2. 点击 **Edit** 按钮
3. 切换到 **Constraints** 标签页
4. 在 **Check Constraints** 区域：
   - 点击 **Add** 按钮
   - 选择字段：`Price`
   - 输入表达式：`< 1000`
   - 点击 **OK**
5. 点击 **Edit** 按钮

**预期结果**：
- 表更新成功
- 检查约束列表显示两个Price约束：
  - `Unnamed: Price > 0`
  - `Unnamed: Price < 1000`

#### 步骤4.2：测试新添加的约束
1. 进入 **Data Management** 标签页
2. 选择表 `Products`
3. 点击 **Insert** 按钮
4. 尝试插入记录（违反新约束）：
   - `ProductID`: `4`
   - `ProductName`: `Product4`
   - `Price`: `1500` (大于1000，违反约束)
   - `Stock`: `10`
5. 点击 **OK**

**预期结果**：
- 插入失败，显示错误消息：`Check constraint violation: value '1500' does not satisfy constraint '< 1000' on field 'Price'`

#### 步骤4.3：删除约束
1. 返回 **Table Management** 标签页
2. 选择表 `Products`
3. 点击 **Edit** 按钮
4. 切换到 **Constraints** 标签页
5. 在 **Check Constraints** 区域：
   - 选择约束：`Unnamed: Price < 1000`
   - 点击 **Remove** 按钮
6. 点击 **Edit** 按钮

**预期结果**：
- 表更新成功
- 检查约束列表中只剩下：`Unnamed: Price > 0`

7. 进入 **Data Management** 标签页
8. 尝试插入记录：
   - `ProductID`: `5`
   - `ProductName`: `Product5`
   - `Price`: `1500` (现在应该可以插入，因为约束已删除)
   - `Stock`: `20`
9. 点击 **OK**

**预期结果**：
- 插入成功（因为 `< 1000` 约束已被删除）

---

## 🧪 测试用例5：约束持久化测试

### 测试目标
验证约束在程序退出后重新进入时仍然有效。

### 测试步骤

#### 步骤5.1：创建带约束的表
1. 在 **Table Management** 标签页，创建一个新表 `TestTable`
2. 添加字段：
   - `ID` (int, KEY, NO_NULL, VALID)
   - `Value` (int, NOT_KEY, NULL, VALID)
3. 添加检查约束：
   - 字段：`Value`
   - 表达式：`> 0 AND < 100`
4. 创建表

**预期结果**：
- 表创建成功
- 约束已保存

#### 步骤5.2：退出并重新进入
1. 关闭应用程序
2. 重新打开应用程序
3. 打开相同的数据库

**预期结果**：
- 数据库打开成功
- 表 `TestTable` 仍然存在

#### 步骤5.3：验证约束仍然有效
1. 进入 **Data Management** 标签页
2. 选择表 `TestTable`
3. 点击 **Insert** 按钮
4. 尝试插入记录（违反约束）：
   - `ID`: `1`
   - `Value`: `-5`
5. 点击 **OK**

**预期结果**：
- 插入失败，显示检查约束违反错误

6. 再次点击 **Insert** 按钮
7. 插入记录（满足约束）：
   - `ID`: `2`
   - `Value`: `50`
8. 点击 **OK**

**预期结果**：
- 插入成功

#### 步骤5.4：验证约束在GUI中可见
1. 进入 **Table Management** 标签页
2. 选择表 `TestTable`
3. 点击 **Edit** 按钮
4. 切换到 **Constraints** 标签页

**预期结果**：
- 检查约束列表显示：`Unnamed: Value > 0 AND < 100`
- 约束信息已正确加载

---

## 🧪 测试用例6：外键级联操作测试

### 测试目标
验证通过GUI创建的外键约束支持级联操作。

### 测试步骤

#### 步骤6.1：创建带CASCADE外键的表
1. 在 **Table Management** 标签页，点击 **Create** 按钮
2. 创建表 `Employees2`：
   - 字段：`EmpID` (int, KEY, NO_NULL, VALID)
   - 字段：`EmpName` (string, NOT_KEY, NULL, VALID)
   - 字段：`DeptID` (int, NOT_KEY, NULL, VALID)
3. 切换到 **Constraints** 标签页
4. 添加外键约束：
   - 字段：`DeptID`
   - 引用表：`Departments`
   - 引用字段：`DeptID`
   - ON DELETE：`CASCADE`
   - ON UPDATE：`RESTRICT`
5. 创建表

**预期结果**：
- 表创建成功
- 外键约束显示：`Unnamed: DeptID -> Departments.DeptID (ON DELETE CASCADE, ON UPDATE RESTRICT)`

#### 步骤6.2：插入测试数据
1. 进入 **Data Management** 标签页
2. 选择表 `Employees2`
3. 插入记录：
   - `EmpID`: `1`
   - `EmpName`: `Bob`
   - `DeptID`: `1`
4. 插入记录：
   - `EmpID`: `2`
   - `EmpName`: `Charlie`
   - `DeptID`: `1`

**预期结果**：
- 两条记录都插入成功

#### 步骤6.3：测试CASCADE删除
1. 进入 **SQL Execution** 标签页
2. 执行SQL：
   ```sql
   DELETE FROM Departments WHERE DeptID=1 IN test_db;
   ```
3. 点击 **Execute**

**预期结果**：
- 删除成功
- 显示受影响的行数

4. 返回 **Data Management** 标签页
5. 选择表 `Employees2`

**预期结果**：
- 表中没有记录（两条记录都被级联删除）

---

## 🧪 测试用例7：综合约束测试

### 测试目标
验证表可以同时拥有多种类型的约束。

### 测试步骤

#### 步骤7.1：创建带多种约束的表
1. 在 **Table Management** 标签页，点击 **Create** 按钮
2. 创建表 `ComprehensiveTable`：
   - 字段：`ID` (int, KEY, NO_NULL, VALID)
   - 字段：`Email` (string, NOT_KEY, NO_NULL, VALID, UNIQUE)
   - 字段：`Age` (int, NOT_KEY, NULL, VALID, DEFAULT: 18)
   - 字段：`DeptID` (int, NOT_KEY, NULL, VALID)
3. 切换到 **Constraints** 标签页
4. 添加外键约束：
   - 字段：`DeptID`
   - 引用表：`Departments`（如果存在）
   - 引用字段：`DeptID`
   - ON DELETE：`SET NULL`
5. 添加唯一约束：
   - 字段：`ID, Email`（多字段唯一约束）
6. 添加检查约束：
   - 字段：`Age`
   - 表达式：`> 0 AND < 150`
7. 创建表

**预期结果**：
- 表创建成功
- 所有约束都显示在对应的列表中

#### 步骤7.2：测试所有约束
1. 进入 **Data Management** 标签页
2. 选择表 `ComprehensiveTable`
3. 测试各种约束场景：
   - 测试字段级UNIQUE约束（Email）
   - 测试DEFAULT值（Age留空）
   - 测试检查约束（Age值）
   - 测试外键约束（DeptID）
   - 测试多字段唯一约束（ID, Email组合）

**预期结果**：
- 所有约束都正确工作
- 违反约束的操作被阻止并显示相应错误

---

## ✅ 测试检查清单

### 功能检查
- [ ] 可以在GUI中创建表并添加外键约束
- [ ] 可以在GUI中创建表并添加唯一约束（单字段和多字段）
- [ ] 可以在GUI中创建表并添加检查约束
- [ ] 可以在GUI中编辑表并修改约束
- [ ] 可以在GUI中删除约束
- [ ] 约束在程序退出后重新进入时仍然有效
- [ ] 约束在GUI中正确显示
- [ ] 所有约束在INSERT/UPDATE/DELETE操作时正确检查

### 界面检查
- [ ] Constraints标签页正确显示
- [ ] 约束列表正确显示约束信息
- [ ] 添加/编辑/删除按钮正常工作
- [ ] 约束编辑对话框正确显示和关闭
- [ ] 错误消息清晰明确

### 数据一致性检查
- [ ] 约束保存到.cst文件
- [ ] 约束从.cst文件正确加载
- [ ] 约束在内存中正确注册
- [ ] 约束查询使用正确的数据库名

---

## 📝 测试结果记录

### 测试日期：_________

### 测试人员：_________

### 测试结果：

| 测试用例 | 测试步骤 | 结果 | 备注 |
|---------|---------|------|------|
| 测试用例1 | 步骤1.1-1.4 | ☐ 通过 ☐ 失败 | |
| 测试用例2 | 步骤2.1-2.2 | ☐ 通过 ☐ 失败 | |
| 测试用例3 | 步骤3.1-3.3 | ☐ 通过 ☐ 失败 | |
| 测试用例4 | 步骤4.1-4.3 | ☐ 通过 ☐ 失败 | |
| 测试用例5 | 步骤5.1-5.4 | ☐ 通过 ☐ 失败 | |
| 测试用例6 | 步骤6.1-6.3 | ☐ 通过 ☐ 失败 | |
| 测试用例7 | 步骤7.1-7.2 | ☐ 通过 ☐ 失败 | |

### 发现的问题：

1. 
2. 
3. 

---

## 🔧 故障排查

### 问题1：约束列表为空
**可能原因**：
- 约束未正确保存到.cst文件
- 约束未正确从.cst文件加载
- 数据库名不一致

**解决方法**：
- 检查.cst文件是否存在
- 检查数据库名是否正确
- 查看控制台错误消息

### 问题2：约束编辑对话框无法打开
**可能原因**：
- 字段列表为空
- 表列表为空（对于外键）

**解决方法**：
- 确保先添加字段
- 确保存在其他表（对于外键约束）

### 问题3：约束不生效
**可能原因**：
- 约束未正确注册到ConstraintRegistry
- 约束未正确保存到文件
- 数据库打开时约束未加载

**解决方法**：
- 检查约束是否正确注册
- 检查.cst文件内容
- 重新打开数据库

---

**最后更新**：2026-01-15

**维护者**：项目开发团队

**状态**：测试用例已创建 ✅

