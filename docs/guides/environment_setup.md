# 开发环境配置指南

> 智能音乐播放管理系统 - 开发环境配置文档

## ✅ 环境检查结果

### 已安装的工具

- ✅ **Qt**: Qt 5.15.2 (conda环境: C:/Users/Oscar/anaconda3/Library/lib)
- ✅ **CMake**: 已下载（需要添加到PATH或重新启动终端）
- ✅ **MSVC编译器**: 已安装 (Visual Studio 2022 BuildTools)

### 需要配置的项目

1. **CMake添加到PATH**：CMake命令未找到，需要添加到系统PATH或重新启动终端
2. **Qt路径配置**：需要确认Qt安装路径并配置CMakeLists.txt
3. **CMake配置测试**：测试CMake是否能找到Qt
4. **Git仓库初始化**：首次提交（可选）

---

## 🔧 环境配置步骤

### 步骤1：确认CMake安装

**问题**：CMake命令未在PATH中找到

**解决方案**：

**方案A：重新启动终端**
- 如果CMake已安装但当前终端未识别，关闭并重新打开PowerShell/终端
- CMake安装程序通常会提示添加到PATH，重启终端后生效

**方案B：手动添加到PATH**
- 找到CMake安装路径（通常在 `C:\Program Files\CMake\bin`）
- 添加到系统PATH环境变量
- 重新打开终端

**验证**：
```powershell
cmake --version
# 应该显示：cmake version 3.x.x
```

---

### 步骤2：确认Qt安装路径

**当前状态**：
- conda环境中有Qt 5.15.2：`C:/Users/Oscar/anaconda3/Library`
- 如果您单独下载了Qt，需要找到其安装路径

**查找Qt安装路径**：

1. **如果使用conda环境的Qt**（推荐用于开发）：
   ```
   Qt路径: C:/Users/Oscar/anaconda3/Library
   ```

2. **如果单独安装了Qt**，常见路径：
   ```
   C:\Qt\5.15.2\msvc2019_64
   C:\Qt\6.5.0\msvc2019_64
   C:\Qt\Qt5.15.2\5.15.2\msvc2019_64
   ```

3. **查找方法**：
   - 检查 `C:\Qt` 目录
   - 检查 `C:\Program Files\Qt` 目录
   - 在开始菜单搜索Qt并查看安装路径

---

### 步骤3：配置CMakeLists.txt

根据找到的Qt路径，更新 `CMakeLists.txt` 中的Qt配置。

**如果使用conda环境的Qt**：
```cmake
set(CMAKE_PREFIX_PATH "C:/Users/Oscar/anaconda3/Library" CACHE PATH "Qt installation path")
```

**如果使用单独安装的Qt**：
```cmake
set(CMAKE_PREFIX_PATH "C:/Qt/5.15.2/msvc2019_64" CACHE PATH "Qt installation path")
# 或
set(CMAKE_PREFIX_PATH "C:/Qt/6.5.0/msvc2019_64" CACHE PATH "Qt installation path")
```

---

### 步骤4：测试CMake配置

在项目根目录执行：

```powershell
# 创建build目录
mkdir build
cd build

# 运行CMake配置
cmake ..

# 检查输出
# 应该看到：
# - Found Qt5: TRUE (或 Found Qt6: TRUE)
# - Qt version: 5.15.2 (或相应版本)
# - Configuring done
# - Generating done
```

**如果出现错误**：
- Qt未找到：检查CMAKE_PREFIX_PATH路径是否正确
- 编译器未找到：确保MSVC在PATH中
- 其他错误：查看错误信息并相应调整

---

### 步骤5：Git仓库初始化（可选但推荐）

如果还没有初始化Git仓库，可以现在初始化：

```powershell
# 初始化Git仓库（如果还没有）
git init

# 添加所有文件
git add .

# 创建初始提交
git commit -m "feat: 项目初始化，创建项目结构和基础文档

- 创建完整的项目文件夹结构
- 创建所有必需的文档（.cursorrules, README, 开发日志等）
- 创建测试脚本结构
- 配置CMakeLists.txt
- 配置开发环境"
```

---

## 📋 初始化检查清单

完成以下检查清单，确保环境配置正确：

### 基础环境
- [ ] CMake已安装并可在命令行使用（`cmake --version`）
- [ ] Qt已安装并找到路径
- [ ] MSVC编译器可用（`cl.exe` 可找到）
- [ ] 确认Qt版本（Qt5 或 Qt6）

### 项目配置
- [ ] CMakeLists.txt中的Qt路径已正确配置
- [ ] CMake配置测试通过（`cmake ..` 无错误）
- [ ] 能够生成构建文件

### 可选配置
- [ ] Git仓库已初始化（如果需要版本控制）
- [ ] 首次提交已完成（如果需要）

---

## 🚨 常见问题

### 问题1：CMake命令未找到

**解决方法**：
1. 确认CMake已安装
2. 重启终端（CMake安装后需要重启）
3. 手动添加到PATH（如上述步骤1）

### 问题2：Qt未找到

**解决方法**：
1. 确认Qt安装路径
2. 检查CMakeLists.txt中的CMAKE_PREFIX_PATH
3. 路径使用正斜杠 `/` 或转义的反斜杠 `\\`
4. 确保路径指向Qt安装目录（包含bin、lib等子目录）

### 问题3：编译器未找到

**解决方法**：
1. 确保Visual Studio BuildTools已安装
2. 使用"Developer Command Prompt for VS"启动终端
3. 或手动设置MSVC环境变量

### 问题4：Qt版本不匹配

**解决方法**：
- 如果使用Qt6，需要修改CMakeLists.txt：
  ```cmake
  find_package(Qt6 COMPONENTS Core Widgets REQUIRED)
  target_link_libraries(${PROJECT_NAME} Qt6::Core Qt6::Widgets)
  ```

---

## ✅ 配置完成后

配置完成后，您可以：

1. **开始开发**：按照 `docs/project_task_guide.md` 开始阶段1的开发
2. **测试构建**：虽然还没有源代码，但可以测试CMake配置是否正确
3. **查看文档**：
   - `docs/user_workflow_analysis.md` - 了解用户操作流程
   - `docs/detailed_design_analysis.md` - 了解实现难点
   - `docs/project_task_guide.md` - 查看开发路线图

---

**最后更新**：2025-01-12
