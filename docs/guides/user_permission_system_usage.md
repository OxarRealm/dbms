# 用户权限管理系统使用指南

> 本文档详细说明如何使用GUI界面和SQL语句管理用户、角色和权限

---

## 目录

1. [GUI界面使用](#gui界面使用)
2. [SQL语句使用](#sql语句使用)
3. [测试示例](#测试示例)
4. [常见问题](#常见问题)

---

## GUI界面使用

### 1. 启动和登录

1. **启动程序**
   - 运行编译后的可执行文件
   - 程序启动时会自动显示登录对话框

2. **登录系统**
   - 默认管理员账户：`admin` / `admin`
   - 输入用户名和密码后点击 "Login"
   - 如果登录失败，会显示错误提示

3. **打开数据库**
   - 登录成功后，在主窗口中选择：`File` → `Open Database`
   - 选择数据库文件（例如：`Student.dbf`）
   - 数据库打开后，所有功能才能正常使用

### 2. 用户管理界面

#### 2.1 进入用户管理

- 在主窗口的Tab栏中，点击 **"User Management"** Tab
- 界面分为左右两部分：
  - **左侧**：用户列表和操作按钮
  - **右侧**：选中用户的详细信息

#### 2.2 创建用户

1. 点击 **"Create User"** 按钮
2. 在弹出的对话框中输入：
   - **Username**：用户名（例如：`student1`）
   - **Password**：密码（例如：`pass123`）
3. 点击 "OK" 确认
4. 如果创建成功，用户列表会自动刷新

**注意**：
- 用户名不能为空
- 密码不能为空
- 用户名不能重复（如果用户已存在，会显示错误提示）

#### 2.3 删除用户

1. 在用户列表中，**选择要删除的用户**
2. 点击 **"Delete User"** 按钮
3. 在确认对话框中点击 "Yes"
4. 用户删除后，其所有权限和角色关联也会被删除

#### 2.4 启用/禁用用户

1. 在用户列表中，**选择要操作的用户**
2. 点击 **"Enable User"** 或 **"Disable User"** 按钮
3. 操作成功后，用户状态会更新
4. 禁用的用户无法登录系统

#### 2.5 修改密码

1. 在用户列表中，**选择要修改密码的用户**
2. 点击 **"Change Password"** 按钮
3. 在弹出的对话框中输入新密码
4. 点击 "OK" 确认

#### 2.6 查看用户信息

- 在用户列表中**点击任意用户**，右侧会显示：
  - **用户名**：用户的名称
  - **状态**：Enabled（启用）或 Disabled（禁用）
  - **角色列表**：用户拥有的所有角色
  - **权限列表**：用户的所有权限（包括直接授予和通过角色继承的）
    - 显示格式：对象类型 | 对象名称 | 权限列表 | Grant Option

#### 2.7 管理用户角色

1. 在用户列表中，**选择要管理的用户**
2. 点击 **"Manage Roles"** 按钮
3. 在弹出的对话框中：
   - **左侧**：显示可用的角色列表（用户尚未拥有的角色）
   - **右侧**：显示用户当前拥有的角色
4. **授予角色**：
   - 在左侧选择角色
   - 点击 **"Grant Role →"** 按钮
5. **撤销角色**：
   - 在右侧选择角色
   - 点击 **"← Revoke Role"** 按钮
6. 操作完成后，点击 **"Close"** 关闭对话框
7. 用户信息会自动刷新，显示更新后的角色列表

#### 2.8 管理用户权限

1. 在用户列表中，**选择要管理的用户**
2. 点击 **"Manage Permissions"** 按钮
3. 在弹出的对话框中：
   - **权限列表表格**：显示用户当前的所有权限
   - **Grant Permission** 按钮：授予新权限
   - **Revoke Permission** 按钮：撤销权限

4. **授予权限**：
   - 点击 **"Grant Permission"** 按钮
   - 在弹出的对话框中：
     - **Object Type**：选择对象类型（Table 或 Database）
     - **Object Name**：选择或输入对象名称
       - 如果选择 Table，会显示所有表名供选择
       - 如果选择 Database，可以输入数据库名
     - **Permissions**：选择要授予的权限（可多选）
       - SELECT：查询权限
       - INSERT：插入权限
       - UPDATE：更新权限
       - DELETE：删除权限
       - ALTER：修改表结构权限
       - DROP：删除表权限
       - CREATE TABLE：创建表权限
       - ALL PRIVILEGES：所有权限
     - **With Grant Option**：勾选后，用户可以将此权限授予其他用户
   - 点击 "OK" 确认

5. **撤销权限**：
   - 在权限列表表格中，**选择要撤销的权限行**
   - 点击 **"Revoke Permission"** 按钮
   - 在确认对话框中点击 "Yes"
   - 权限撤销后，表格会自动刷新

6. 操作完成后，点击 **"Close"** 关闭对话框
7. 用户信息会自动刷新，显示更新后的权限列表

#### 2.9 刷新用户列表

- 点击 **"Refresh"** 按钮，可以手动刷新用户列表和用户信息

---

## SQL语句使用

### 1. 进入SQL执行界面

- 在主窗口的Tab栏中，点击 **"SQL Execution"** Tab
- 在SQL输入框中输入SQL语句
- 点击 **"Execute"** 按钮执行

### 2. 用户管理SQL语句

#### 2.1 创建用户

```sql
CREATE USER username IDENTIFIED BY 'password';
```

**示例**：
```sql
CREATE USER student1 IDENTIFIED BY 'pass123';
CREATE USER teacher1 IDENTIFIED BY 'teacher123';
```

#### 2.2 修改用户密码

```sql
ALTER USER username IDENTIFIED BY 'newpassword';
```

**示例**：
```sql
ALTER USER student1 IDENTIFIED BY 'newpass456';
```

#### 2.3 启用用户

```sql
ALTER USER username ENABLE;
```

**示例**：
```sql
ALTER USER student1 ENABLE;
```

#### 2.4 禁用用户

```sql
ALTER USER username DISABLE;
```

**示例**：
```sql
ALTER USER student1 DISABLE;
```

#### 2.5 删除用户

```sql
DROP USER username;
```

**示例**：
```sql
DROP USER student1;
```

### 3. 角色管理SQL语句

#### 3.1 创建角色

```sql
CREATE ROLE role_name;
```

**示例**：
```sql
CREATE ROLE student_role;
CREATE ROLE teacher_role;
CREATE ROLE admin_role;
```

#### 3.2 删除角色

```sql
DROP ROLE role_name;
```

**示例**：
```sql
DROP ROLE student_role;
```

### 4. 权限管理SQL语句

#### 4.1 授予表权限

```sql
GRANT permission_list ON TABLE table_name TO username;
```

**权限列表**：
- `SELECT`：查询权限
- `INSERT`：插入权限
- `UPDATE`：更新权限
- `DELETE`：删除权限
- `ALTER`：修改表结构权限
- `DROP`：删除表权限
- `ALL PRIVILEGES`：所有权限

**示例**：
```sql
-- 授予单个权限
GRANT SELECT ON TABLE Students TO student1;

-- 授予多个权限
GRANT SELECT, INSERT, UPDATE ON TABLE Students TO student1;

-- 授予所有权限
GRANT ALL PRIVILEGES ON TABLE Students TO student1;

-- 授予权限并允许转授（WITH GRANT OPTION）
GRANT SELECT, INSERT ON TABLE Students TO student1 WITH GRANT OPTION;
```

#### 4.2 授予数据库权限

```sql
GRANT permission_list ON DATABASE TO username;
```

**示例**：
```sql
-- 授予数据库的所有权限
GRANT ALL PRIVILEGES ON DATABASE TO admin;

-- 授予创建表权限
GRANT CREATE TABLE ON DATABASE TO teacher1;
```

#### 4.3 授予角色给用户

```sql
GRANT role_name TO username;
```

**示例**：
```sql
GRANT student_role TO student1;
GRANT teacher_role TO teacher1;
```

#### 4.4 撤销表权限

```sql
REVOKE permission_list ON TABLE table_name FROM username;
```

**示例**：
```sql
-- 撤销单个权限
REVOKE SELECT ON TABLE Students FROM student1;

-- 撤销多个权限
REVOKE SELECT, INSERT ON TABLE Students FROM student1;

-- 撤销所有权限
REVOKE ALL PRIVILEGES ON TABLE Students FROM student1;
```

#### 4.5 撤销数据库权限

```sql
REVOKE permission_list ON DATABASE FROM username;
```

**示例**：
```sql
REVOKE CREATE TABLE ON DATABASE FROM teacher1;
```

#### 4.6 撤销角色

```sql
REVOKE role_name FROM username;
```

**示例**：
```sql
REVOKE student_role FROM student1;
```

---

## 测试示例

### 示例1：为学生数据库创建用户和角色

假设你有一个 `Student` 数据库，包含 `Students` 和 `Grades` 两个表。

#### 步骤1：创建角色

```sql
-- 创建学生角色
CREATE ROLE student_role;

-- 创建教师角色
CREATE ROLE teacher_role;
```

#### 步骤2：为角色授予权限

```sql
-- 学生角色：只能查询和插入自己的成绩
GRANT SELECT ON TABLE Students TO student_role;
GRANT SELECT, INSERT ON TABLE Grades TO student_role;

-- 教师角色：可以管理所有表
GRANT ALL PRIVILEGES ON TABLE Students TO teacher_role;
GRANT ALL PRIVILEGES ON TABLE Grades TO teacher_role;
```

#### 步骤3：创建用户

```sql
-- 创建学生用户
CREATE USER student1 IDENTIFIED BY 'pass123';
CREATE USER student2 IDENTIFIED BY 'pass456';

-- 创建教师用户
CREATE USER teacher1 IDENTIFIED BY 'teacher123';
```

#### 步骤4：将角色授予用户

```sql
-- 为学生分配学生角色
GRANT student_role TO student1;
GRANT student_role TO student2;

-- 为教师分配教师角色
GRANT teacher_role TO teacher1;
```

#### 步骤5：验证权限

1. 在 **User Management** Tab 中，选择 `student1`
2. 查看右侧的权限列表，应该看到：
   - Table | Students | SELECT
   - Table | Grades | SELECT, INSERT

3. 选择 `teacher1`，应该看到：
   - Table | Students | ALL PRIVILEGES
   - Table | Grades | ALL PRIVILEGES

### 示例2：直接授予权限（不使用角色）

```sql
-- 创建用户
CREATE USER admin_user IDENTIFIED BY 'admin123';

-- 直接授予所有数据库权限
GRANT ALL PRIVILEGES ON DATABASE TO admin_user WITH GRANT OPTION;

-- 授予特定表的特定权限
GRANT SELECT, UPDATE ON TABLE Students TO admin_user;
```

### 示例3：用户状态管理

```sql
-- 禁用用户（用户无法登录）
ALTER USER student1 DISABLE;

-- 启用用户
ALTER USER student1 ENABLE;

-- 修改密码
ALTER USER student1 IDENTIFIED BY 'newpassword';
```

### 示例4：权限撤销

```sql
-- 撤销用户的表权限
REVOKE INSERT ON TABLE Grades FROM student1;

-- 撤销用户的角色
REVOKE student_role FROM student1;

-- 撤销所有权限
REVOKE ALL PRIVILEGES ON TABLE Students FROM student1;
```

---

## 常见问题

### Q1: 为什么创建用户后看不到权限？

**A**: 新创建的用户默认没有任何权限。你需要：
1. 直接授予权限：使用 `GRANT` 语句
2. 或者授予角色：先创建角色并授予权限，然后将角色授予用户

### Q2: 如何查看用户的所有权限（包括通过角色继承的）？

**A**: 在 **User Management** Tab 中，选择用户后，右侧的权限列表会显示：
- 直接授予的权限
- 通过角色继承的权限

### Q3: WITH GRANT OPTION 的作用是什么？

**A**: 如果授予权限时使用了 `WITH GRANT OPTION`，该用户可以将此权限授予其他用户。否则，用户只能使用权限，不能转授。

### Q4: 删除用户或角色后，权限会怎样？

**A**: 
- 删除用户：该用户的所有权限和角色关联都会被删除
- 删除角色：所有拥有该角色的用户都会失去该角色的权限

### Q5: SQL语句执行失败怎么办？

**A**: 检查以下几点：
1. SQL语法是否正确（注意大小写、标点符号）
2. 用户名、角色名、表名是否存在
3. 是否已经打开了数据库
4. 查看错误提示信息

### Q6: 如何批量管理用户？

**A**: 目前系统不支持批量操作。你可以：
1. 使用SQL语句批量创建用户（在SQL Execution Tab中执行多条语句）
2. 使用GUI界面逐个管理用户

---

## 注意事项

1. **数据库路径**：所有用户、角色和权限数据都存储在数据库目录下的 `.usr`、`.role`、`.usrr`、`.perm` 文件中
2. **权限检查**：目前系统已经实现了权限管理功能，但权限检查（在执行SQL时验证用户权限）功能尚未完全集成
3. **默认管理员**：系统启动时使用默认的 `admin` 账户，建议首次登录后修改密码
4. **密码安全**：密码使用SHA256哈希存储，不会明文保存

---

**最后更新时间**：2026-01-18

