# 用户权限管理系统使用指南

## 概述

用户权限管理系统已实现核心框架。本文档说明当前实现状态和使用方法。

## 当前实现状态

### 已完成 ✅
1. **核心数据结构定义**（`include/core/user_mode.h`）
   - UserInfo、RoleInfo、PermissionInfo、UserRoleInfo结构体
   - 权限类型、对象类型常量定义

2. **管理器类头文件**：
   - `UserManager` - 用户管理（头文件已创建）
   - `RoleManager` - 角色管理（头文件已创建）
   - `PermissionManager` - 权限管理（头文件已创建）
   - `SessionManager` - 会话管理（已实现基础功能）

3. **存储管理器**：
   - `UserStorageManager` - 用户权限存储管理（路径方法已实现）

4. **SQL解析器扩展**（2026-01-18完成）✅
   - Token扩展：添加USER, ROLE, PRIVILEGES, IDENTIFIED, BY, GRANT, REVOKE, TO, FROM, WITH, OPTION, ALTER, DATABASE, ON等关键字
   - AST节点扩展：添加CreateUserNode, AlterUserNode, DropUserNode, CreateRoleNode, DropRoleNode, GrantNode, RevokeNode
   - Parser方法实现：
     - `parseCreateUser()` - 解析CREATE USER语句
     - `parseAlterUser()` - 解析ALTER USER语句
     - `parseDropUser()` - 解析DROP USER语句
     - `parseCreateRole()` - 解析CREATE ROLE语句
     - `parseDropRole()` - 解析DROP ROLE语句
     - `parseGrant()` - 解析GRANT语句（支持权限授予和角色授予）
     - `parseRevoke()` - 解析REVOKE语句（支持权限撤销和角色撤销）
   - 支持SQL语句格式：
     - `CREATE USER username IDENTIFIED BY 'password';`
     - `ALTER USER username IDENTIFIED BY 'newpassword';`
     - `ALTER USER username ENABLE/DISABLE;`
     - `DROP USER username;`
     - `CREATE ROLE role_name;`
     - `DROP ROLE role_name;`
     - `GRANT privilege_list ON object_type object_name TO username [WITH GRANT OPTION];`
     - `GRANT ROLE role_name TO username;` (注意：必须使用ROLE关键字)
     - `REVOKE privilege_list ON object_type object_name FROM username;`
     - `REVOKE ROLE role_name FROM username;` 或 `REVOKE role_name FROM username;` (两种格式都支持)
   - **实现文件**：`src/sql_parser/parser_user_permission.cpp`

5. **Parser主流程扩展**：
   - `parse()`方法支持GRANT和REVOKE语句
   - `parseDDL()`方法支持CREATE USER, ALTER USER, DROP USER, CREATE ROLE, DROP ROLE

### 待实现 ⏳
1. UserManager、RoleManager、PermissionManager的完整实现（.cpp文件）
2. 密码加密功能实现（使用Qt QCryptographicHash）
3. 文件存储实现（.usr, .role, .perm文件读写）
4. Handler实现（CREATE USER、GRANT等语句的执行处理器）
5. DDLExecutor集成（将权限相关语句集成到DDL执行器）
6. 权限检查中间件集成（在QueryExecutor、DDLExecutor、DMLExecutor中添加权限检查）
7. GUI界面（登录界面、用户管理界面）

## 数据结构

### 用户信息（UserInfo）
- `userName`: 用户名（最大32字符）
- `passwordHash`: 密码哈希（SHA256，64字符）
- `createTime`: 创建时间
- `lastLoginTime`: 最后登录时间
- `status`: 状态（1=启用, 0=禁用）

### 角色信息（RoleInfo）
- `roleName`: 角色名（最大32字符）
- `description`: 角色描述
- `createTime`: 创建时间

### 权限信息（PermissionInfo）
- `userName`: 用户名或角色名
- `objectType`: 对象类型（1=表, 2=数据库, 3=系统）
- `objectName`: 对象名（表名或数据库名）
- `permissionType`: 权限类型（SELECT, INSERT等）
- `isRole`: 是否为角色权限
- `withGrantOption`: 是否有GRANT OPTION
- `grantTime`: 授予时间
- `grantedBy`: 授予者

## 权限类型

- **表级权限**：SELECT, INSERT, UPDATE, DELETE, ALTER, DROP
- **数据库级权限**：CREATE TABLE, DROP DATABASE, CREATE INDEX
- **系统级权限**：CREATE USER, GRANT OPTION, ALL PRIVILEGES

## 示例用户和角色配置

### 为Student和Grade数据库创建用户和角色

#### 1. 创建超级管理员（admin）
```sql
CREATE USER admin IDENTIFIED BY 'admin';
GRANT ALL PRIVILEGES ON DATABASE TO admin WITH GRANT OPTION;
```

#### 2. 创建角色
```sql
-- 学生角色：只能查看Student表
CREATE ROLE student_role;
GRANT SELECT ON Students TO student_role;

-- 教师角色：可以查看和修改Grade表
CREATE ROLE teacher_role;
GRANT SELECT, INSERT, UPDATE ON Grades TO teacher_role;
GRANT SELECT ON Students TO teacher_role;

-- 管理员角色：所有表的全部权限
CREATE ROLE admin_role;
GRANT ALL PRIVILEGES ON Students TO admin_role;
GRANT ALL PRIVILEGES ON Grades TO admin_role;
```

#### 3. 创建用户并分配角色
```sql
-- 学生用户：只能查看
CREATE USER student1 IDENTIFIED BY 'student123';
GRANT ROLE student_role TO student1;  -- 注意：必须使用ROLE关键字

-- 教师用户：可以修改成绩
CREATE USER teacher1 IDENTIFIED BY 'teacher123';
GRANT ROLE teacher_role TO teacher1;  -- 注意：必须使用ROLE关键字

-- 另一个管理员用户
CREATE USER admin2 IDENTIFIED BY 'admin456';
GRANT ROLE admin_role TO admin2;  -- 注意：必须使用ROLE关键字
```

#### 4. 直接授予权限（不使用角色）
```sql
-- 给予student2只读权限
CREATE USER student2 IDENTIFIED BY 'student456';
GRANT SELECT ON Students TO student2;
GRANT SELECT ON Grades TO student2;
```

## 后续实现计划

### 阶段1：核心实现（3-4天）
1. 实现UserManager的完整功能（密码加密、用户CRUD、文件存储）
2. 实现RoleManager的完整功能
3. 实现PermissionManager的完整功能
4. 实现文件存储（.usr, .role, .perm文件读写）

### 阶段2：SQL集成（2-3天）
1. 扩展SQL解析器支持CREATE USER、GRANT等语句
2. 在QueryExecutor中集成权限检查

### 阶段3：GUI界面（3-4天）
1. 实现登录界面（LoginDialog）
2. 实现用户管理标签页（UserManagementWidget）
3. 在主窗口集成登录逻辑

### 阶段4：权限控制显示（2-3天）
1. 在表管理界面添加权限控制显示
2. 在数据操作界面添加权限控制显示
3. 在SQL执行界面添加权限错误提示

## 使用说明（待完成实现后）

### 1. 登录系统
- 启动应用时显示登录界面
- 输入用户名和密码进行认证
- 登录成功后进入主界面

### 2. 创建用户和角色
- 通过"User Management"标签页创建用户
- 通过"Roles"子界面创建角色
- 通过SQL语句执行CREATE USER、CREATE ROLE

### 3. 分配权限
- 通过"Permissions"子界面分配权限
- 通过SQL语句执行GRANT、REVOKE

### 4. 权限检查
- 所有DDL、DML、查询操作前自动检查权限
- 权限不足时显示错误信息
- 界面按钮根据权限动态启用/禁用

## 注意事项

1. **默认用户**：首次启动时自动创建`admin`用户（密码：`admin`），拥有所有权限
2. **权限继承**：用户通过角色继承权限，角色权限优先级高于直接授予的用户权限
3. **GRANT OPTION**：拥有GRANT OPTION的用户可以将自己的权限授予其他用户
4. **文件存储**：用户、角色、权限信息存储在`.usr`、`.role`、`.perm`文件中

## 文件结构

```
数据库目录/
  ├── database.dbf        # 表结构文件
  ├── database.dat        # 数据文件
  ├── database.idx        # 索引文件
  ├── database.cst        # 约束文件
  ├── database.usr        # 用户文件 [新增]
  ├── database.role       # 角色文件 [新增]
  └── database.perm       # 权限文件 [新增]
```

---

## 实现记录

### 2026-01-18 - SQL解析器扩展完成

**实现内容**：
1. ✅ **Token扩展**：添加权限相关关键字
   - USER, ROLE, PRIVILEGES, IDENTIFIED, BY
   - GRANT, REVOKE, TO, FROM, WITH, OPTION
   - ALTER, DATABASE, ON
   - **修改文件**：`include/sql_parser/token.h`, `src/sql_parser/token.cpp`

2. ✅ **AST节点扩展**：添加权限相关节点类型
   - CreateUserNode, AlterUserNode, DropUserNode
   - CreateRoleNode, DropRoleNode
   - GrantNode, RevokeNode
   - **修改文件**：`include/sql_parser/ast_node.h`, `src/sql_parser/ast_node.cpp`

3. ✅ **Parser解析方法实现**
   - `parseCreateUser()` - CREATE USER语句解析
   - `parseAlterUser()` - ALTER USER语句解析
   - `parseDropUser()` - DROP USER语句解析
   - `parseCreateRole()` - CREATE ROLE语句解析
   - `parseDropRole()` - DROP ROLE语句解析
   - `parseGrant()` - GRANT语句解析（支持权限和角色授予）
   - `parseRevoke()` - REVOKE语句解析（支持权限和角色撤销）
   - **新增文件**：`src/sql_parser/parser_user_permission.cpp`

4. ✅ **Parser主流程集成**
   - `parse()`方法支持GRANT和REVOKE语句
   - `parseDDL()`方法支持CREATE/ALTER/DROP USER和CREATE/DROP ROLE
   - **修改文件**：`include/sql_parser/parser.h`, `src/sql_parser/parser.cpp`

5. ✅ **CMakeLists.txt更新**
   - 添加`parser_user_permission.cpp`到构建列表

**支持的SQL语法**：
- ✅ `CREATE USER username IDENTIFIED BY 'password';`
- ✅ `ALTER USER username IDENTIFIED BY 'newpassword';`
- ✅ `ALTER USER username ENABLE;`
- ✅ `ALTER USER username DISABLE;`
- ✅ `DROP USER username;`
- ✅ `CREATE ROLE role_name;`
- ✅ `DROP ROLE role_name;`
- ✅ `GRANT SELECT, INSERT ON TABLE table_name TO username;`
- ✅ `GRANT ALL PRIVILEGES ON TABLE table_name TO username WITH GRANT OPTION;`
- ✅ `GRANT ALL PRIVILEGES ON DATABASE TO username;`
- ✅ `GRANT ROLE role_name TO username;` (注意：必须使用ROLE关键字)
- ✅ `REVOKE SELECT, INSERT ON TABLE table_name FROM username;`
- ✅ `REVOKE ROLE role_name FROM username;` 或 `REVOKE role_name FROM username;` (两种格式都支持)

**测试状态**：解析器已实现，可以通过Parser解析权限相关SQL语句。待Handler实现后可通过SQL执行测试。

---

**最后更新时间**：2026-01-18  
**状态**：SQL解析器扩展完成 ✅，Handler实现待完成 ⏳

