# SQL解析器权限相关语句简单测试脚本
# 用途：编译并运行Parser权限相关语句简单测试程序

Write-Host "========================================" -ForegroundColor Cyan
Write-Host "  SQL Parser User Permission Simple Test" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""

$ErrorActionPreference = "Continue"
$testDir = Split-Path -Parent $MyInvocation.MyCommand.Path
# 往上三级：sql_parser -> unit_tests -> scripts -> projectRoot
$projectRoot = Split-Path -Parent (Split-Path -Parent (Split-Path -Parent $testDir))
$testFile = Join-Path $testDir "test_parser_user_permission_simple.cpp"
$tokenCpp = Join-Path $projectRoot "src\sql_parser\token.cpp"
$lexerCpp = Join-Path $projectRoot "src\sql_parser\lexer.cpp"
$parserCpp = Join-Path $projectRoot "src\sql_parser\parser.cpp"
$parserPermissionCpp = Join-Path $projectRoot "src\sql_parser\parser_user_permission.cpp"
$astNodeCpp = Join-Path $projectRoot "src\sql_parser\ast_node.cpp"
$outputDir = Join-Path $projectRoot "test_results\unit_tests"
$outputExe = Join-Path $outputDir "test_parser_user_permission_simple.exe"

# 创建输出目录
if (-not (Test-Path $outputDir)) {
    New-Item -ItemType Directory -Path $outputDir -Force | Out-Null
}

Write-Host "Test file: $testFile" -ForegroundColor Yellow
Write-Host "Output directory: $outputDir" -ForegroundColor Yellow
Write-Host ""

# 检查测试文件是否存在
if (-not (Test-Path $testFile)) {
    Write-Host "Error: Test file not found: $testFile" -ForegroundColor Red
    exit 1
}

# 检查必要文件是否存在
if (-not (Test-Path $parserPermissionCpp)) {
    Write-Host "Error: Parser permission file not found: $parserPermissionCpp" -ForegroundColor Red
    exit 1
}

# 编译测试程序
Write-Host "Compiling test program..." -ForegroundColor Yellow

# 检查编译器
$gppCheck = Get-Command g++ -ErrorAction SilentlyContinue
$clCheck = Get-Command cl.exe -ErrorAction SilentlyContinue
$compileSuccess = $false

if ($gppCheck) {
    Write-Host "Using compiler: g++" -ForegroundColor Cyan
    & g++ -std=c++17 -Wall -I"$projectRoot\include" "$testFile" "$tokenCpp" "$lexerCpp" "$parserCpp" "$parserPermissionCpp" "$astNodeCpp" -o "$outputExe" 2>&1 | Write-Host
    if ($LASTEXITCODE -eq 0) {
        $compileSuccess = $true
    }
} elseif ($clCheck) {
    Write-Host "Using compiler: MSVC (cl.exe)" -ForegroundColor Cyan
    # MSVC编译命令
    & cl.exe /EHsc /std:c++17 /I"$projectRoot\include" "$testFile" "$tokenCpp" "$lexerCpp" "$parserCpp" "$parserPermissionCpp" "$astNodeCpp" /Fe:"$outputExe" /Fo:"$outputDir\test_parser_user_permission_simple.obj" 2>&1 | Write-Host
    if ($LASTEXITCODE -eq 0) {
        $compileSuccess = $true
    }
}

if (-not $compileSuccess) {
    Write-Host "Compilation failed!" -ForegroundColor Red
    Write-Host "Note: This test requires a C++ compiler (g++ or MSVC)" -ForegroundColor Yellow
    Write-Host "If compilation fails, you can test the parser through the GUI instead." -ForegroundColor Yellow
    exit 1
}

Write-Host "Compilation successful" -ForegroundColor Green

# 运行测试程序
Write-Host ""
Write-Host "Running test program..." -ForegroundColor Yellow

if (-not (Test-Path $outputExe)) {
    Write-Host "Error: Executable not found: $outputExe" -ForegroundColor Red
    exit 1
}

Push-Location $outputDir
$testResult = & .\test_parser_user_permission_simple.exe 2>&1
$runResult = $LASTEXITCODE
Pop-Location

Write-Host $testResult

if ($runResult -eq 0) {
    Write-Host ""
    Write-Host "========================================" -ForegroundColor Green
    Write-Host "  Test Completed!" -ForegroundColor Green
    Write-Host "========================================" -ForegroundColor Green
    exit 0
} else {
    Write-Host ""
    Write-Host "========================================" -ForegroundColor Red
    Write-Host "  Test Failed! Exit code: $runResult" -ForegroundColor Red
    Write-Host "========================================" -ForegroundColor Red
    exit 1
}




