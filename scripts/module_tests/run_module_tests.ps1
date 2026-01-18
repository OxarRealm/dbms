# 模块测试运行脚本
# 用途：运行所有模块测试

Write-Host "运行模块测试..." -ForegroundColor Yellow

$ErrorActionPreference = "Continue"
$testResults = @()

# 测试结果目录
$resultsDir = "test_results\module_tests"
if (-not (Test-Path $resultsDir)) {
    New-Item -ItemType Directory -Path $resultsDir -Force | Out-Null
}

$timestamp = Get-Date -Format "yyyy-MM-dd_HH-mm-ss"
$logFile = Join-Path $resultsDir "module_tests_$timestamp.log"

Write-Host "测试日志: $logFile"

# 模块测试脚本列表
$testScripts = @(
    "run_test_index_integration.ps1"  # 索引功能集成测试
)

$totalTests = 0
$passedTests = 0
$failedTests = 0

foreach ($script in $testScripts) {
    $scriptPath = Join-Path $PSScriptRoot $script
    if (Test-Path $scriptPath) {
        Write-Host ""
        Write-Host "运行: $script" -ForegroundColor Cyan
        Write-Host "----------------------------------------" -ForegroundColor Cyan
        
        & $scriptPath
        $scriptExitCode = $LASTEXITCODE
        
        $totalTests++
        if ($scriptExitCode -eq 0) {
            $passedTests++
            Write-Host "✓ $script 通过" -ForegroundColor Green
        } else {
            $failedTests++
            Write-Host "✗ $script 失败 (退出码: $scriptExitCode)" -ForegroundColor Red
        }
    } else {
        Write-Host "警告: 测试脚本不存在: $scriptPath" -ForegroundColor Yellow
    }
}

Write-Host ""
Write-Host "========================================" -ForegroundColor Cyan
Write-Host "模块测试总结" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host "总测试数: $totalTests" -ForegroundColor White
Write-Host "通过: $passedTests" -ForegroundColor Green
Write-Host "失败: $failedTests" -ForegroundColor Red
Write-Host "测试结果将保存到: $resultsDir" -ForegroundColor Cyan
