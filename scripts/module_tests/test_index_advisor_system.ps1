# 智能索引建议系统测试脚本
# 测试IndexAdvisor的完整功能

Write-Host "========================================" -ForegroundColor Cyan
Write-Host "智能索引建议系统测试" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""

# 设置路径
$scriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$projectRoot = Split-Path -Parent (Split-Path -Parent $scriptDir)
$buildDir = Join-Path $projectRoot "build"
$testExe = Join-Path $buildDir "test_index_advisor_system.exe"

# 检查构建目录
if (-not (Test-Path $buildDir)) {
    Write-Host "错误: 构建目录不存在: $buildDir" -ForegroundColor Red
    Write-Host "请先运行 cmake 和构建项目" -ForegroundColor Yellow
    exit 1
}

# 检查测试数据库是否存在
$testDb = Join-Path $projectRoot "student_grade_db.dbf"
if (-not (Test-Path $testDb)) {
    Write-Host "警告: 测试数据库不存在: $testDb" -ForegroundColor Yellow
    Write-Host "请先创建 student_grade_db 数据库" -ForegroundColor Yellow
    Write-Host ""
}

# 编译测试程序
Write-Host "[1/3] 编译测试程序..." -ForegroundColor Green
Push-Location $buildDir
try {
    cmake --build . --config Release --target test_index_advisor_system 2>&1 | Out-Null
    if ($LASTEXITCODE -ne 0) {
        Write-Host "编译失败，尝试重新配置..." -ForegroundColor Yellow
        cmake .. 2>&1 | Out-Null
        cmake --build . --config Release --target test_index_advisor_system 2>&1 | Out-Null
    }
} catch {
    Write-Host "编译错误: $_" -ForegroundColor Red
    Pop-Location
    exit 1
}
Pop-Location

# 检查可执行文件
if (-not (Test-Path $testExe)) {
    Write-Host "错误: 测试程序不存在: $testExe" -ForegroundColor Red
    Write-Host "编译可能失败，请检查错误信息" -ForegroundColor Yellow
    exit 1
}

# 运行测试
Write-Host "[2/3] 运行测试..." -ForegroundColor Green
Write-Host ""
Push-Location $projectRoot
try {
    & $testExe
    $exitCode = $LASTEXITCODE
} catch {
    Write-Host "运行错误: $_" -ForegroundColor Red
    $exitCode = 1
} finally {
    Pop-Location
}

# 输出结果
Write-Host ""
Write-Host "[3/3] 测试完成" -ForegroundColor Green
if ($exitCode -eq 0) {
    Write-Host "测试通过！" -ForegroundColor Green
} else {
    Write-Host "测试失败（退出码: $exitCode）" -ForegroundColor Red
}

exit $exitCode




