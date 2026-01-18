# 运行所有测试脚本
# 用途：执行项目中的所有测试（单元测试、模块测试、集成测试等）

Write-Host "========================================" -ForegroundColor Cyan
Write-Host "  智能音乐播放管理系统 - 测试套件" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""

$ErrorActionPreference = "Stop"
$testResults = @()

# 创建测试结果目录
$testResultsDir = "test_results"
if (-not (Test-Path $testResultsDir)) {
    New-Item -ItemType Directory -Path $testResultsDir | Out-Null
}

$timestamp = Get-Date -Format "yyyy-MM-dd_HH-mm-ss"
$logFile = Join-Path $testResultsDir "test_run_$timestamp.log"

function Write-Log {
    param([string]$Message, [string]$Color = "White")
    Write-Host $Message -ForegroundColor $Color
    Add-Content -Path $logFile -Value $Message
}

Write-Log "测试开始时间: $(Get-Date)" "Green"

# 1. 单元测试
Write-Log "`n[1/5] 运行单元测试..." "Yellow"
try {
    if (Test-Path "scripts\unit_tests\run_unit_tests.ps1") {
        & "scripts\unit_tests\run_unit_tests.ps1"
        $testResults += @{Type="单元测试"; Status="通过"}
        Write-Log "✓ 单元测试完成" "Green"
    } else {
        Write-Log "⚠ 单元测试脚本不存在，跳过" "Yellow"
    }
} catch {
    $testResults += @{Type="单元测试"; Status="失败"}
    Write-Log "✗ 单元测试失败: $_" "Red"
}

# 2. 模块测试
Write-Log "`n[2/5] 运行模块测试..." "Yellow"
try {
    if (Test-Path "scripts\module_tests\run_module_tests.ps1") {
        & "scripts\module_tests\run_module_tests.ps1"
        $testResults += @{Type="模块测试"; Status="通过"}
        Write-Log "✓ 模块测试完成" "Green"
    } else {
        Write-Log "⚠ 模块测试脚本不存在，跳过" "Yellow"
    }
} catch {
    $testResults += @{Type="模块测试"; Status="失败"}
    Write-Log "✗ 模块测试失败: $_" "Red"
}

# 3. 集成测试
Write-Log "`n[3/5] 运行集成测试..." "Yellow"
try {
    if (Test-Path "scripts\integration_tests\run_integration_tests.ps1") {
        & "scripts\integration_tests\run_integration_tests.ps1"
        $testResults += @{Type="集成测试"; Status="通过"}
        Write-Log "✓ 集成测试完成" "Green"
    } else {
        Write-Log "⚠ 集成测试脚本不存在，跳过" "Yellow"
    }
} catch {
    $testResults += @{Type="集成测试"; Status="失败"}
    Write-Log "✗ 集成测试失败: $_" "Red"
}

# 4. 边界测试
Write-Log "`n[4/5] 运行边界测试..." "Yellow"
try {
    if (Test-Path "scripts\boundary_tests\run_boundary_tests.ps1") {
        & "scripts\boundary_tests\run_boundary_tests.ps1"
        $testResults += @{Type="边界测试"; Status="通过"}
        Write-Log "✓ 边界测试完成" "Green"
    } else {
        Write-Log "⚠ 边界测试脚本不存在，跳过" "Yellow"
    }
} catch {
    $testResults += @{Type="边界测试"; Status="失败"}
    Write-Log "✗ 边界测试失败: $_" "Red"
}

# 5. 性能测试
Write-Log "`n[5/5] 运行性能测试..." "Yellow"
try {
    if (Test-Path "scripts\performance_tests\run_performance_tests.ps1") {
        & "scripts\performance_tests\run_performance_tests.ps1"
        $testResults += @{Type="性能测试"; Status="通过"}
        Write-Log "✓ 性能测试完成" "Green"
    } else {
        Write-Log "⚠ 性能测试脚本不存在，跳过" "Yellow"
    }
} catch {
    $testResults += @{Type="性能测试"; Status="失败"}
    Write-Log "✗ 性能测试失败: $_" "Red"
}

# 输出测试摘要
Write-Log "`n========================================" "Cyan"
Write-Log "  测试摘要" "Cyan"
Write-Log "========================================" "Cyan"

foreach ($result in $testResults) {
    $statusColor = if ($result.Status -eq "通过") { "Green" } else { "Red" }
    Write-Log "$($result.Type): $($result.Status)" $statusColor
}

$passed = ($testResults | Where-Object { $_.Status -eq "通过" }).Count
$total = $testResults.Count
Write-Log "`n总计: $passed/$total 通过" $(if ($passed -eq $total) { "Green" } else { "Yellow" })
Write-Log "测试结束时间: $(Get-Date)" "Green"
Write-Log "`n测试日志已保存到: $logFile" "Cyan"

Write-Host ""
