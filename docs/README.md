# 项目文档目录

本文档目录包含项目的所有文档，按类型分类组织。

## 📁 文档分类结构

```
docs/
├── core/                  # 核心文档（开发日志、迭代记录、项目报告）
├── development/           # 开发相关文档（进度跟踪、任务指南）
├── project/               # 项目相关文档（架构设计、初始化总结）
├── technical/             # 技术文档（技术分析、需求对齐、流程分析）
├── guides/                # 指南文档（环境配置、初始化指南）
└── README.md              # 本文档
```

---

## 📚 文档列表

### 🔹 core/ - 核心文档

核心项目文档，包括开发记录和报告：

- **[development_log.md](core/development_log.md)** - 开发日志（记录开发过程中的重要事件）
- **[iteration_records.md](core/iteration_records.md)** - 版本迭代记录
- **[project_report.md](core/project_report.md)** - 项目报告模板（按照课程要求结构）

### 🔹 development/ - 开发相关文档

开发过程中的进度跟踪和任务管理：

- **[cursor_context.md](development/cursor_context.md)** - 项目进度跟踪文档（用于跨对话上下文）
- **[project_task_guide.md](development/project_task_guide.md)** - 项目任务进度指南（**最重要**，全局开发路线图）

### 🔹 project/ - 项目相关文档

项目整体设计和总结：

- **[project_architecture.md](project/project_architecture.md)** - 项目架构设计文档
- **[project_initialization_summary.md](project/project_initialization_summary.md)** - 项目初始化总结

### 🔹 technical/ - 技术文档

技术分析、需求对齐和流程分析：

- **[ai_technology_analysis.md](technical/ai_technology_analysis.md)** - AI技术选项分析文档
- **[detailed_design_analysis.md](technical/detailed_design_analysis.md)** - 项目详细设计分析（实现难点、改进点、扩展功能）
- **[requirements_alignment_analysis.md](technical/requirements_alignment_analysis.md)** - 课程要求对齐分析
- **[user_workflow_analysis.md](technical/user_workflow_analysis.md)** - 用户操作流程分析

### 🔹 guides/ - 指南文档

环境配置、初始化和使用指南：

- **[environment_setup.md](guides/environment_setup.md)** - 环境配置详细指南
- **[initialization_guide.md](guides/initialization_guide.md)** - 初始化指南
- **[initialization_status.md](guides/initialization_status.md)** - 初始化状态总结
- **[preparation_checklist.md](guides/preparation_checklist.md)** - 准备工作清单
- **[summary_response.md](guides/summary_response.md)** - 需求响应总结

---

## 📖 重要文档说明

### cursor_context.md ⭐（最重要）
用于跟踪项目的最新进度和状态。当对话token达到限制需要开启新对话时，新对话可以参考此文档了解项目全貌。

**位置**：`development/cursor_context.md`

### project_task_guide.md ⭐（最重要）
全局任务规划和进度跟踪文档，包含：
- 7个开发阶段的详细任务清单
- 时间估算和优先级
- 下一步行动指引

**位置**：`development/project_task_guide.md`

### project_architecture.md
系统架构设计文档，包含：
- 系统整体架构
- 模块划分
- 数据结构设计
- 接口设计

**位置**：`project/project_architecture.md`

### development_log.md
开发日志，记录开发过程中的重要事件、决策和进展。

**位置**：`core/development_log.md`

---

## 📝 文档更新规范

根据 `.cursorrules` 中的文档更新规范：

### 更新时机
- 完成一个功能模块后
- 修复重要bug后
- 版本发布前

### 需要更新的文档
完成模块后，需要更新以下文档：
1. `development/cursor_context.md` - 项目进度跟踪（必须更新）
2. `core/development_log.md` - 开发日志
3. `core/iteration_records.md` - 版本迭代记录（版本发布时）
4. `project/project_architecture.md` - 项目架构设计（架构变更时）
5. `development/project_task_guide.md` - 任务进度指南（任务完成时）
6. `core/project_report.md` - 项目报告（完成功能模块后）
7. `README.md` - 文档目录说明（新增文档时）
8. 项目根目录 `README.md` - 项目主README（重要更新时）

### 更新流程
1. AI提示用户是否需要更新文档
2. 用户确认后，统一进行文档更新
3. 或用户主动要求更新文档时，立即执行更新
4. 确保所有相关文档同步更新，保持一致性

---

## 🔗 快速导航

### 开始开发
- [项目任务进度指南](development/project_task_guide.md) - 查看开发路线图
- [用户操作流程分析](technical/user_workflow_analysis.md) - 了解用户操作流程
- [项目架构设计](project/project_architecture.md) - 了解系统架构

### 技术参考
- [详细设计分析](technical/detailed_design_analysis.md) - 实现难点和解决方案
- [需求对齐分析](technical/requirements_alignment_analysis.md) - 课程要求对齐
- [AI技术分析](technical/ai_technology_analysis.md) - AI技术选项

### 环境配置
- [环境配置指南](guides/environment_setup.md) - 环境配置详细说明
- [初始化指南](guides/initialization_guide.md) - 初始化步骤

### 开发记录
- [开发日志](core/development_log.md) - 开发过程记录
- [版本迭代记录](core/iteration_records.md) - 版本历史
- [项目进度跟踪](development/cursor_context.md) - 当前进度

---

**最后更新**：2026-01-14（阶段4：查询实现已完成；阶段8.1：相邻索引实现已完成；阶段8.2：哈希索引实现已完成）