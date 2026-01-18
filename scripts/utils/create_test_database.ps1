# 创建测试数据库脚本
# 用途：编译并运行create_test_database.cpp，生成测试数据库文件

param(
    [string]$DatabaseName = "demo_db",
    [string]$OutputDir = "."
)

Write-Host "========================================" -ForegroundColor Cyan
Write-Host "  测试数据库生成工具" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""

# 检查CMake配置
if (-not (Test-Path "build")) {
    Write-Host "错误: build目录不存在，请先运行CMake配置" -ForegroundColor Red
    Write-Host "运行: mkdir build; cd build; cmake .." -ForegroundColor Yellow
    exit 1
}

# 编译测试数据库生成工具
Write-Host "[1/3] 编译测试数据库生成工具..." -ForegroundColor Yellow

$sourceFile = "scripts/utils/create_test_database.cpp"
$exeFile = "build/bin/Release/create_test_database.exe"

# 检查源文件是否存在
if (-not (Test-Path $sourceFile)) {
    Write-Host "错误: 源文件不存在: $sourceFile" -ForegroundColor Red
    exit 1
}

# 使用CMake编译
Write-Host "正在编译..." -ForegroundColor Gray
cd build
cmake --build . --config Release --target create_test_database 2>&1 | Out-Null

if ($LASTEXITCODE -ne 0) {
    # 如果CMake target不存在，手动编译
    Write-Host "CMake target不存在，尝试手动编译..." -ForegroundColor Yellow
    
    $includeDirs = @(
        "../include",
        "../include/core",
        "../include/ddl",
        "../include/dml",
        "../include/query",
        "../include/sql_parser"
    )
    
    $includeFlags = $includeDirs | ForEach-Object { "-I$_" }
    $includeFlags = $includeFlags -join " "
    
    $sourceFiles = @(
        "../src/core/table_manager.cpp",
        "../src/core/data_manager.cpp",
        "../src/core/file_manager.cpp"
    )
    
    $compileCmd = "cl.exe /EHsc /std:c++17 /utf-8 $includeFlags $sourceFile $($sourceFiles -join ' ') /Fe:$exeFile /Fo:build/obj/Release/create_test_database.obj /link /OUT:$exeFile"
    
    Write-Host "编译命令: $compileCmd" -ForegroundColor Gray
    Invoke-Expression $compileCmd
    
    if ($LASTEXITCODE -ne 0) {
        Write-Host "错误: 编译失败" -ForegroundColor Red
        cd ..
        exit 1
    }
}

cd ..

# 检查可执行文件
if (-not (Test-Path $exeFile)) {
    Write-Host "错误: 可执行文件未生成: $exeFile" -ForegroundColor Red
    Write-Host "提示: 可能需要手动编译，或使用GUI创建数据库" -ForegroundColor Yellow
    exit 1
}

# 运行测试数据库生成工具
Write-Host "[2/3] 运行测试数据库生成工具..." -ForegroundColor Yellow
Write-Host ""

& $exeFile

if ($LASTEXITCODE -ne 0) {
    Write-Host "错误: 测试数据库生成失败" -ForegroundColor Red
    exit 1
}

# 检查生成的文件
Write-Host ""
Write-Host "[3/3] 检查生成的文件..." -ForegroundColor Yellow

$dbfFile = "$DatabaseName.dbf"
$datFile = "$DatabaseName.dat"

if ((Test-Path $dbfFile) -and (Test-Path $datFile)) {
    Write-Host "✓ 测试数据库文件已生成：" -ForegroundColor Green
    Write-Host "  - $dbfFile" -ForegroundColor Green
    Write-Host "  - $datFile" -ForegroundColor Green
    Write-Host ""
    Write-Host "文件位置: $(Resolve-Path $dbfFile)" -ForegroundColor Cyan
    Write-Host ""
    Write-Host "现在可以在GUI中打开这个数据库进行测试！" -ForegroundColor Green
} else {
    Write-Host "警告: 数据库文件未找到" -ForegroundColor Yellow
    Write-Host "  - $dbfFile: $(if (Test-Path $dbfFile) { '存在' } else { '不存在' })" -ForegroundColor Yellow
    Write-Host "  - $datFile: $(if (Test-Path $datFile) { '存在' } else { '不存在' })" -ForegroundColor Yellow
}

Write-Host ""
Write-Host "========================================" -ForegroundColor Cyan




