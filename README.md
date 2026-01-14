# 智能音乐播放管理系统

> 数据库新技术实践课程设计项目

## 项目简介

本项目是一个基于C++和Qt开发的智能音乐播放管理系统，实现了自定义的数据库管理系统（DBMS），包含表结构管理、数据操作、SQL查询以及AI智能推荐等功能。

## 技术栈

- **编程语言**：C++
- **GUI框架**：Qt (C++)
- **开发环境**：VSCode (Cursor)
- **构建工具**：CMake
- **版本控制**：Git

## 核心功能

### 1. 数据存储结构的设计与管理
- 表构造模式存储（.dbf文件）
- 记录数据存储（.dat文件）
- 支持多表存储和管理

### 2. 数据定义语言（DDL）
- CREATE TABLE - 创建表
- EDIT TABLE - 编辑表结构
- RENAME TABLE - 重命名表
- DROP TABLE - 删除表

### 3. 数据操纵语言（DML）
- INSERT - 插入记录
- DELETE - 删除记录
- UPDATE - 更新记录

### 4. 数据库查询
- SELECT 单表查询
- SELECT 多表查询
- SELECT 连接查询（JOIN）

### 5. AI智能推荐系统
- 基于协同过滤的音乐推荐
- 基于内容相似度的推荐
- 个性化推荐结果展示

## 项目结构

```
database-design/
├── src/                    # 源代码目录
│   ├── core/              # 核心功能（文件操作、数据结构）
│   ├── ddl/               # DDL实现
│   ├── dml/               # DML实现
│   ├── query/             # 查询实现
│   ├── sql_parser/        # SQL解析
│   ├── ai/                # AI推荐算法
│   └── gui/               # Qt界面
├── include/                # 头文件目录
├── resources/              # 资源文件
├── tests/                  # 测试代码
├── build/                  # 构建输出
├── examples/               # 示例文件
├── data/                   # 数据文件（.dbf, .dat）
└── docs/                   # 文档目录
```

## 数据库设计

### 核心数据表

1. **Users（用户表）**
   - UserID, UserName, RegisterTime

2. **Songs（歌曲表）**
   - SongID, SongName, Artist, Genre, Year, Duration

3. **PlayRecords（播放记录表）**
   - RecordID, UserID, SongID, PlayTime, PlayCount

4. **Playlists（歌单表）**
   - PlaylistID, UserID, PlaylistName, CreateTime

5. **PlaylistSongs（歌单-歌曲关联表）**
   - RelationID, PlaylistID, SongID

## 编译和运行

### 环境要求
- C++14 或 C++17 编译器（推荐C++17）
- Qt 5.15+ 或 Qt 6.x（当前使用Qt 5.15.2）
- CMake 3.10+（当前使用CMake 4.2.1）
- MSVC编译器（Windows）或 GCC/Clang（Linux/Mac）

### 编译步骤

```bash
# 创建构建目录
mkdir build
cd build

# 生成构建文件
cmake ..

# 编译
cmake --build .

# 运行
./database-design  # Linux/Mac
database-design.exe  # Windows
```

## 使用说明

### 1. 创建数据库表

```sql
CREATE TABLE Songs (
    SongID int KEY NO_NULL VALID,
    SongName char[100] NOT_KEY NO_NULL VALID,
    Artist char[50] NOT_KEY NULL VALID,
    Genre char[30] NOT_KEY NULL VALID
) INTO MusicDB;
```

### 2. 插入数据

```sql
INSERT INTO Songs VALUES (1, 'Bohemian Rhapsody', 'Queen', 'Rock') IN MusicDB;
```

### 3. 查询数据

```sql
SELECT * FROM Songs WHERE Genre='Rock';
```

### 4. 智能推荐

在GUI界面中选择用户和查询条件，系统会自动推荐相似歌曲。

## 开发日志

详细开发过程请参考 [docs/core/development_log.md](docs/core/development_log.md)

## 项目架构

详细架构设计请参考 [docs/project/project_architecture.md](docs/project/project_architecture.md)

## 版本历史

版本迭代记录请参考 [docs/core/iteration_records.md](docs/core/iteration_records.md)

## 课程报告

项目报告请参考 [docs/core/project_report.md](docs/core/project_report.md)

## 贡献者

- 吕金鸣 (23013085) - 组长
- 李雨宣 (23013083)

## 许可证

本项目为课程设计项目，仅供学习使用。

## 参考资料

- 《数据库管理系统内部结构及其C语言实现》，唐常杰，电子科技大学出版社，1995
- Qt官方文档：https://doc.qt.io/
- C++参考：https://en.cppreference.com/

---

## 当前状态

- ✅ 项目初始化完成
- ✅ 开发环境配置完成（CMake 4.2.1, Qt 5.15.2, MSVC编译器）
- ✅ CMake配置验证通过
- ✅ **阶段1：核心数据结构与文件存储已完成**
  - ✅ 任务1.1：定义核心数据结构（36测试通过）
  - ✅ 任务1.2：实现.dbf文件读写（30测试通过）
  - ✅ 任务1.3：实现.dat文件读写（38测试通过）
  - ✅ 任务1.4：基础文件I/O封装（41测试通过）
  - **总计：145个测试全部通过**
- ✅ **阶段2：DDL实现已完成**
  - ✅ 任务2.1：SQL解析器基础框架（78测试通过）
  - ✅ 任务2.2：CREATE TABLE实现（24测试通过）
  - ✅ 任务2.3：EDIT TABLE实现（21测试通过）
  - ✅ 任务2.4：RENAME TABLE实现（23测试通过）
  - ✅ 任务2.5：DROP TABLE实现（27测试通过）
  - ✅ 任务2.6：DDL执行器整合（38测试通过）
  - **总计：211个测试全部通过**
- ✅ **阶段3：DML实现已完成**
  - ✅ 任务3.1：INSERT实现（43测试通过）
  - ✅ 任务3.2：DELETE实现（38测试通过）
  - ✅ 任务3.3：UPDATE实现（43测试通过）
  - ✅ 任务3.4：DML执行器整合（42测试通过）
  - **总计：166个测试全部通过**
- ✅ **阶段4：查询实现已完成**
  - ✅ 任务4.1：SELECT单表查询（30测试通过）
  - ✅ 任务4.2：SELECT多表查询（50测试通过）
  - ✅ 任务4.3：SELECT连接查询（57测试通过）
  - ✅ 任务4.4：查询执行器整合（24测试通过）
  - **总计：161个测试全部通过**
- ✅ **阶段8.1：相邻索引实现已完成**
  - ✅ 任务8.1：相邻索引实现（28测试通过）
  - **完成时间**：2026-01-14
- ✅ **阶段8.2：哈希索引实现已完成**
  - ✅ 任务8.2：哈希索引实现（29测试通过）
  - **完成时间**：2026-01-14
- ✅ **阶段8.3：智能索引建议系统已完成**
  - ✅ 任务8.3：智能索引建议系统（14测试通过）
  - **完成时间**：2026-01-14
- ✅ **阶段8：数据库新技术实现全部完成**
  - **技术方案**：方案A（索引技术 + 智能推荐）
  - **完成时间**：2026-01-14（3天完成）
  - **测试总计**：71个测试全部通过
  - **详细分析**：`docs/technical/database_new_technology_selection.md`
- ✅ **阶段5.1：GUI最小可运行程序已完成**
  - ✅ 主窗口类实现（MainWindow）
  - ✅ 应用程序入口实现（main.cpp）
  - ✅ 基础UI组件（标签、按钮、菜单栏、状态栏）
  - ✅ 字体设置（英文使用Segoe UI）
  - ✅ 编译问题修复（parser_select.cpp、Qt MOC、编码问题）
  - **完成时间**：2026-01-14
  - **状态**：GUI能够正常显示 ✅
- ✅ **阶段5.2：GUI主界面布局已完成**
  - ✅ 标签页系统（5个标签页：Table Management, Index Management, Data Operation, SQL Execution, Guide）
  - ✅ 所有文字改为英文，字体使用Segoe UI
  - ✅ 状态栏显示数据库名称和当前时间
  - ✅ 全局键盘快捷键（Ctrl+Q, F1, Ctrl+Tab等）
  - ✅ 窗口标题包含版本号（v0.6.1）
  - ✅ Guide标签页显示USER_GUIDE.md内容
  - **完成时间**：2026-01-14
  - **状态**：主界面布局已完成 ✅
- ✅ **阶段5.3：数据库管理和表结构管理界面已完成**
  - ✅ 数据库管理功能（Create Database, Open Database）
  - ✅ 表列表显示和表信息显示
  - ✅ 创建表功能（TableEditDialog，支持字段定义）
  - ✅ 编辑表功能（修改表结构）
  - ✅ 删除表功能（带确认对话框）
  - ✅ 刷新表列表功能
  - ✅ 界面优化（居中显示、按钮文本、关闭事件处理）
  - **完成时间**：2026-01-14
  - **状态**：数据库管理和表结构管理界面已完成 ✅

**项目总进度**：72%完成（阶段1-4和阶段8全部完成，754个测试全部通过；阶段5.1-5.3已完成；阶段5.4-7待开发）

---

**最后更新时间**：2026-01-14
