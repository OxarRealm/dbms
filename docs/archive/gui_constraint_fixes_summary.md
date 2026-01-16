# GUI约束功能修复总结

> **最后更新**：2026-01-15

---

## 🔧 修复内容

### 1. CHECK约束表达式AND/OR支持 ✅

**问题**：CHECK约束表达式 `> 0 AND < 100` 只识别了 `> 0`，没有识别 `< 100`。

**原因**：`ConstraintManager::evaluateCheckExpression` 函数只支持单个条件，不支持AND/OR组合表达式。

**修复**：
- 重构了 `evaluateCheckExpression` 函数，支持AND/OR组合表达式
- 添加了 `evaluateSingleCondition` 辅助函数，用于评估单个条件
- 支持表达式格式：`> 0 AND < 100`, `>= 0 OR <= 150` 等

**代码位置**：
- `src/core/constraint_manager.cpp` - `evaluateCheckExpression` 和 `evaluateSingleCondition`
- `include/core/constraint_manager.h` - 添加了 `evaluateSingleCondition` 声明

---

### 2. UPDATE时的检查约束 ✅

**问题**：UPDATE操作时，检查约束没有正确工作，成功插入了不符合CHECK约束的记录。

**原因**：UPDATE时的检查约束逻辑是正确的，但可能存在以下问题：
- 约束没有正确加载到 `ConstraintRegistry`
- 数据库名不匹配

**修复**：
- 确保在 `onEditTable` 中正确加载约束
- 确保数据库名格式一致（使用baseName）

**代码位置**：
- `src/gui/table_management_widget.cpp` - `onEditTable` 函数中添加了约束重新加载逻辑

---

### 3. GUI显示问题修复 ✅

#### 3.1 窗口大小问题
**问题**：对话框窗口太小，标题文字看不见。

**修复**：
- 调整了 `TableEditDialog` 的最小尺寸和默认尺寸
- 调整了主窗口的最小尺寸

**代码位置**：
- `src/gui/table_management_widget.cpp` - `TableEditDialog::setupUI` 和 `TableManagementWidget::setupUI`

#### 3.2 约束列表显示问题
**问题**：
- 外键约束设置后，在Constraints中没有显示，只有一个水平滑动条
- 显示窗口高度太小（Add/Edit/Remove上方的矩形窗口）

**修复**：
- 为 `QListWidget` 设置了最小高度（120）和最大高度（200）
- 确保 `updateConstraintLists` 正确更新列表内容

**代码位置**：
- `src/gui/table_management_widget.cpp` - `TableEditDialog::setupUI` 中设置列表高度
- `src/gui/table_management_widget.cpp` - `TableEditDialog::updateConstraintLists` 确保正确显示

#### 3.3 约束加载问题
**问题**：更新约束后，退出再进入Edit界面，约束信息没有显示。

**原因**：在 `TableEditDialog` 构造函数中，`m_databaseNameEdit` 可能还没有设置，导致约束无法加载。

**修复**：
- 在 `onEditTable` 中，设置 `databaseNameEdit` 后，重新加载约束
- 确保数据库名格式一致

**代码位置**：
- `src/gui/table_management_widget.cpp` - `onEditTable` 函数中添加了约束重新加载逻辑

---

### 4. 外键编辑对话框改进 ✅

**问题**：使用多个 `QInputDialog` 导致窗口太小，标题文字看不见。

**修复**：
- 创建了自定义对话框，使用 `QDialog` 和布局管理器
- 对话框包含所有必要的输入控件（字段选择、引用表选择、引用字段输入、ON DELETE/UPDATE动作选择）
- 设置了合适的窗口大小（最小400x300，默认450x350）

**代码位置**：
- `src/gui/table_management_widget.cpp` - `TableEditDialog::addForeignKey` 函数

---

### 5. CHECK约束表达式输入对话框改进 ✅

**问题**：使用 `QInputDialog::getText` 导致窗口太小，没有提示信息。

**修复**：
- 创建了自定义对话框，包含表达式输入框和提示信息
- 添加了示例提示：`Examples: > 0, < 100, >= 0 AND <= 150`
- 设置了合适的窗口大小（最小400x150，默认450x180）

**代码位置**：
- `src/gui/table_management_widget.cpp` - `TableEditDialog::addCheckConstraint` 函数

---

### 6. Table Information中添加查看约束按钮 ✅

**问题**：用户需要点击表，选择Edit，才能看到约束信息。

**修复**：
- 在 `Table Information` 区域添加了 "View Constraints" 按钮
- 点击按钮后，显示一个对话框，列出所有约束（外键、唯一、检查）
- 对话框包含三个分组框，分别显示不同类型的约束

**代码位置**：
- `src/gui/table_management_widget.cpp` - `TableManagementWidget::setupUI` 中添加按钮
- `src/gui/table_management_widget.cpp` - `TableManagementWidget::onViewConstraints` 函数
- `include/gui/table_management_widget.h` - 添加了 `onViewConstraints` 槽函数声明

---

## 📝 关于UNIQUE约束的说明

### Fields窗口中的UNIQUE vs Constraints窗口中的Unique Constraints

**两者都需要保留**，因为它们的功能不同：

#### 1. Fields窗口中的UNIQUE（字段级唯一约束）
- **作用范围**：单个字段的唯一性
- **用途**：标记某个字段（如Email）必须唯一
- **实现方式**：存储在 `TableMode.bUnique` 字段中
- **示例**：`Email` 字段设置为 `UNIQUE`，确保每个记录的Email值都不同

#### 2. Constraints窗口中的Unique Constraints（表级多字段唯一约束）
- **作用范围**：多个字段的组合唯一性
- **用途**：标记多个字段的组合必须唯一
- **实现方式**：存储在 `TableConstraints.uniqueConstraints` 中
- **示例**：`(CustomerID, ProductID)` 组合必须唯一，允许同一个CustomerID有多个ProductID，但同一个组合只能出现一次

**区别总结**：
- **字段级UNIQUE**：单个字段的值必须唯一
- **表级Unique Constraints**：多个字段的组合必须唯一

**实际应用场景**：
- 字段级UNIQUE：`Email` 字段必须唯一（每个用户只能有一个邮箱）
- 表级Unique Constraints：`(OrderID, ProductID)` 组合必须唯一（一个订单中，同一个产品只能出现一次）

---

## ✅ 测试建议

### 1. CHECK约束AND/OR表达式测试
- 创建表，添加CHECK约束：`> 0 AND < 100`
- 尝试插入值：`-5`（应该失败）
- 尝试插入值：`50`（应该成功）
- 尝试插入值：`150`（应该失败）

### 2. UPDATE检查约束测试
- 创建表，添加CHECK约束：`Price > 0`
- 插入记录：`Price = 10`
- 尝试UPDATE：`Price = -5`（应该失败）
- 尝试UPDATE：`Price = 20`（应该成功）

### 3. GUI显示测试
- 创建表，添加外键约束
- 退出Edit对话框
- 重新进入Edit对话框
- 验证约束信息是否正确显示

### 4. 查看约束按钮测试
- 选择表
- 点击 "View Constraints" 按钮
- 验证所有约束信息是否正确显示

---

## 🔍 代码变更文件列表

1. `src/core/constraint_manager.cpp` - CHECK约束表达式评估逻辑
2. `include/core/constraint_manager.h` - 添加 `evaluateSingleCondition` 声明
3. `src/gui/table_management_widget.cpp` - GUI显示和约束加载修复
4. `include/gui/table_management_widget.h` - 添加 `onViewConstraints` 槽函数

---

**最后更新**：2026-01-15

**维护者**：项目开发团队

**状态**：所有修复已完成 ✅

