# 索引功能集成测试脚本
# 用途：测试索引创建、使用、智能推荐等完整功能

Write-Host "========================================" -ForegroundColor Cyan
Write-Host "  索引功能集成测试" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""

$ErrorActionPreference = "Continue"
$testDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$projectRoot = Split-Path -Parent (Split-Path -Parent $testDir)
$testFile = Join-Path $testDir "test_index_integration.cpp"

# 检查测试文件是否存在
if (-not (Test-Path $testFile)) {
    Write-Host "错误: 测试文件不存在: $testFile" -ForegroundColor Red
    exit 1
}

# 检查CMake配置
if (-not (Test-Path (Join-Path $projectRoot "build"))) {
    Write-Host "错误: build目录不存在，请先运行CMake配置" -ForegroundColor Red
    Write-Host "运行: mkdir build; cd build; cmake .." -ForegroundColor Yellow
    exit 1
}

# 编译测试程序
Write-Host "[1/3] 编译测试程序..." -ForegroundColor Yellow

$exeFile = Join-Path $projectRoot "build\bin\Release\test_index_integration.exe"

# 使用CMake编译（需要先在CMakeLists.txt中添加target）
Write-Host "正在编译..." -ForegroundColor Gray
Push-Location (Join-Path $projectRoot "build")

# 尝试使用CMake编译（如果target存在）
cmake --build . --config Release --target test_index_integration 2>&1 | Out-Null

if ($LASTEXITCODE -ne 0 -or -not (Test-Path $exeFile)) {
    Write-Host "CMake target不存在，需要先在CMakeLists.txt中添加test_index_integration target" -ForegroundColor Yellow
    Write-Host "或者直接运行测试程序（如果已编译）" -ForegroundColor Yellow
    Pop-Location
    # 检查是否已有可执行文件
    if (-not (Test-Path $exeFile)) {
        Write-Host "错误: 无法编译测试程序，请检查CMakeLists.txt配置" -ForegroundColor Red
        exit 1
    }
} else {
    Pop-Location
}

# 检查可执行文件
if (-not (Test-Path $exeFile)) {
    Write-Host "错误: 可执行文件未生成: $exeFile" -ForegroundColor Red
    exit 1
}

Write-Host "编译成功!" -ForegroundColor Green
Write-Host ""

# 运行测试
Write-Host "[2/3] 运行测试..." -ForegroundColor Yellow
Write-Host ""

Push-Location $projectRoot
$testOutput = & $exeFile
$exitCode = $LASTEXITCODE
Pop-Location

Write-Host $testOutput

Write-Host ""
Write-Host "[3/3] 检查生成的文件..." -ForegroundColor Yellow

# 显示生成的数据库文件位置
$dbPath = Join-Path $projectRoot "test_index_db"
$dbfFile = $dbPath + ".dbf"
$datFile = $dbPath + ".dat"
if (Test-Path $dbfFile) {
    Write-Host ""
    Write-Host "测试数据库文件已生成：" -ForegroundColor Green
    Write-Host "  - $dbfFile" -ForegroundColor Green
    Write-Host "  - $datFile" -ForegroundColor Green
    Write-Host ""
    $resolvedPath = Resolve-Path $dbfFile
    Write-Host "文件位置: $resolvedPath" -ForegroundColor Cyan
    Write-Host ""
    Write-Host "现在可以在GUI中打开这个数据库测试索引功能！" -ForegroundColor Green
} else {
    Write-Host "警告: 数据库文件未找到" -ForegroundColor Yellow
}

Write-Host ""
Write-Host "========================================" -ForegroundColor Cyan
if ($exitCode -eq 0) {
    Write-Host "测试通过!" -ForegroundColor Green
} else {
    Write-Host "测试失败 (退出码: $exitCode)" -ForegroundColor Red
}
Write-Host "========================================" -ForegroundColor Cyan

exit $exitCode
