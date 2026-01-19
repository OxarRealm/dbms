# 文档目录

> 数据库管理系统 (DBMS) - 项目文档索引

---

## 📁 文档结构说明

### `/core` - 核心项目文档
项目核心文档，包括开发日志、迭代记录和项目报告。

- **development_log.md** - 开发日志，记录开发过程中的重要决策和问题解决
- **iteration_records.md** - 版本迭代记录，记录各版本的更新内容
- **project_report.md** - 项目报告，完整的系统设计和实现文档

---

### `/project` - 项目设计文档
项目整体设计和架构文档。

- **project_architecture.md** - 项目架构设计，系统整体架构和模块划分
- **project_introduction.md** - 项目介绍，项目背景和目标
- **project_initialization_summary.md** - 项目初始化总结

---

### `/development` - 开发过程文档
开发过程中的进度跟踪、任务规划和功能分析文档（活跃文档）。

- **cursor_context.md** - 项目进度跟踪文档，用于新对话时了解项目全貌
- **dbms_feature_analysis.md** - DBMS功能分析，功能清单和完成度统计
- **dbms_new_technology_discussion.md** - 数据库新技术讨论
- **future_work_plan.md** - 未来工作计划
- **project_task_guide.md** - 项目任务指南
- **gui_checklist.md** - GUI功能检查清单

---

### `/testing` - 测试文档
所有测试相关的文档，包括测试用例、测试指南和测试总结。

- **sql_execution_test_cases.md** - SQL执行测试用例（详细测试用例）
- **sql_comprehensive_test_guide.md** - SQL执行全面测试指南（场景化、连贯测试）
- **gui_comprehensive_test_guide.md** - GUI图形化操作全面测试指南（场景化、连贯测试）
- **drop_table_test_cases.md** - DROP TABLE功能测试用例
- **gui_constraint_testing_guide.md** - GUI约束功能测试指南
- **gui_constraint_ui_test_cases.md** - GUI约束编辑界面测试用例
- **constraint_testing_summary.md** - 约束功能测试总结
- **union_test_cases.md** - UNION功能测试用例
- **subquery_test_cases.md** - 子查询功能测试用例
- **permission_parser_test_guide.md** - 权限SQL解析器测试指南
- **user_permission_debug_guide.md** - 用户权限系统调试指南

---

### `/guides` - 使用指南
用户和开发者使用指南。

- **compilation_guide.md** - 编译指南，如何编译项目
- **environment_setup.md** - 环境设置指南
- **environment_verification.md** - 环境验证指南
- **initialization_guide.md** - 初始化指南
- **initialization_status.md** - 初始化状态
- **preparation_checklist.md** - 准备检查清单
- **final_preparation_checklist.md** - 最终准备检查清单
- **documentation_update_summary.md** - 文档更新总结
- **summary_response.md** - 总结响应
- **user_permission_system_guide.md** - 用户权限系统使用指南

---

### `/technical` - 技术分析文档
技术选型、需求分析和详细设计文档。

- **ai_technology_analysis.md** - AI技术分析
- **database_new_technology_selection.md** - 数据库新技术选型
- **detailed_design_analysis.md** - 详细设计分析
- **requirements_alignment_analysis.md** - 需求对齐分析
- **user_workflow_analysis.md** - 用户工作流分析

---

### `/presentation` - 演示文档
演示和展示相关文档。

- **demo_test_cases.md** - 演示测试用例

---

### `/archive` - 归档文档
已完成或不再使用的历史文档，保留作为参考。

- **constraint_implementation_progress.md** - 约束实现进度（已完成，已归档）
- **constraint_implementation_progress_full.md** - 约束实现完整进度（已完成，已归档）
- **constraint_implementation_status.md** - 约束实现状态（已完成，已归档）
- **gui_constraint_fixes_summary.md** - GUI约束修复总结（已完成，已归档）
- **update_check_constraint_analysis.md** - UPDATE CHECK约束问题分析（已完成，已归档）
- **test_coverage_analysis.md** - 测试覆盖分析（已完成，已归档）
- **test_verification_summary.md** - 测试验证总结（已完成，已归档）

---

## 📖 快速导航

### 对于新开发者
1. 阅读 `/project/project_introduction.md` 了解项目背景
2. 阅读 `/project/project_architecture.md` 了解系统架构
3. 阅读 `/development/cursor_context.md` 了解当前进度
4. 阅读 `/guides/compilation_guide.md` 学习如何编译项目

### 对于测试人员
1. 阅读 `/testing/gui_comprehensive_test_guide.md` 进行GUI功能测试
2. 阅读 `/testing/sql_comprehensive_test_guide.md` 进行SQL功能测试
3. 参考 `/testing/sql_execution_test_cases.md` 查看详细测试用例

### 对于用户
1. 阅读项目根目录的 `USER_GUIDE.md` 了解如何使用系统
2. 阅读 `/guides/environment_setup.md` 学习环境设置

---

## 📝 文档更新规范

### 更新时机
- 完成一个功能模块后，需要更新相关文档
- 修复重要bug后，更新开发日志
- 版本发布前，更新版本记录

### 需要更新的文档列表
- `development/cursor_context.md` - 项目进度跟踪（必须更新）
- `core/development_log.md` - 开发日志（记录开发过程和决策）
- `core/iteration_records.md` - 版本迭代记录（版本发布时更新）
- `project/project_architecture.md` - 项目架构设计（架构变更时更新）
- `development/project_task_guide.md` - 任务进度指南（任务完成时更新进度）
- `core/project_report.md` - 项目报告（完成功能模块后补充相应内容）
- `README.md` - 本文档（新增文档时更新）
- `USER_GUIDE.md` - 用户操作指南（功能变更时同步更新）

### 文档格式规范
- **日期格式**：使用YYYY-MM-DD格式（如2026-01-16）
- **标题层级**：日期使用##（二级标题），子章节使用###（三级标题）
- **列表格式**：使用统一的列表格式（- 或1.），保持一致性
- **更新记录**：在文档末尾或指定位置记录最后更新时间
- **格式一致性**：保持文档之间的格式风格统一，沿用历史格式排版

---

## 🔄 文档分类原则

### 分类标准
1. **按用途分类**：
   - `core/` - 核心项目文档（报告、日志）
   - `project/` - 项目设计文档（架构、介绍）
   - `development/` - 开发过程文档（进度、任务）
   - `testing/` - 测试文档（用例、指南）
   - `guides/` - 使用指南（编译、环境）
   - `technical/` - 技术分析文档（选型、设计）
   - `presentation/` - 演示文档（演示用例）
   - `archive/` - 归档文档（历史文档）

2. **归档原则**：
   - 已完成的功能实现进度文档 → `archive/`
   - 已完成的问题分析和修复总结 → `archive/`
   - 已完成的测试分析文档 → `archive/`
   - 测试用例和测试指南 → `testing/`
   - 活跃的开发文档 → `development/`

3. **模块化原则**：
   - 每个目录有明确的用途
   - 避免文档重复
   - 保持目录结构清晰

---

**最后更新**：2026-01-18

**最新更新（2026-01-18）**：
- ✅ 用户权限管理系统实现完成
  - 核心权限管理模块（UserManager, RoleManager, PermissionManager, SessionManager）
  - SQL解析器扩展（支持CREATE USER, ALTER USER, DROP USER, CREATE ROLE, DROP ROLE, GRANT, REVOKE）
  - DDL执行器集成（用户权限相关Handler）
  - GUI界面实现（登录、用户管理、角色管理、权限管理）
  - 关键Bug修复（REVOKE操作、内存和文件数据一致性等）
- ✅ 新增测试和指南文档
  - permission_parser_test_guide.md - 权限SQL解析器测试指南
  - user_permission_debug_guide.md - 用户权限系统调试指南
  - user_permission_system_guide.md - 用户权限系统使用指南
