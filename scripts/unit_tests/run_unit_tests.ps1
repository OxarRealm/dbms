# 单元测试运行脚本
# 用途：运行所有单元测试

Write-Host "运行单元测试..." -ForegroundColor Yellow

$ErrorActionPreference = "Continue"
$testResults = @()

# 测试结果目录
$resultsDir = "test_results\unit_tests"
if (-not (Test-Path $resultsDir)) {
    New-Item -ItemType Directory -Path $resultsDir -Force | Out-Null
}

$timestamp = Get-Date -Format "yyyy-MM-dd_HH-mm-ss"
$logFile = Join-Path $resultsDir "unit_tests_$timestamp.log"

Write-Host "测试日志: $logFile"

# 运行所有单元测试
$testScripts = @(
    # Core模块测试
    "core\run_test_table_mode.ps1",
    "core\run_test_table_manager.ps1",
    "core\run_test_data_manager.ps1",
    "core\run_test_file_manager.ps1",
    "core\run_test_constraint_manager.ps1",
    
    # SQL Parser模块测试
    "sql_parser\run_test_lexer.ps1",
    "sql_parser\run_test_parser.ps1",
    "sql_parser\run_test_parser_select.ps1",
    "sql_parser\run_test_parser_where.ps1",
    "sql_parser\run_test_token.ps1",
    "sql_parser\run_test_ast_node.ps1",
    
    # DDL模块测试
    "ddl\run_test_create_table_handler.ps1",
    "ddl\run_test_edit_table_handler.ps1",
    "ddl\run_test_rename_table_handler.ps1",
    "ddl\run_test_drop_table_handler.ps1",
    "ddl\run_test_ddl_executor.ps1",
    
    # DML模块测试
    "dml\run_test_insert_handler.ps1",
    "dml\run_test_delete_handler.ps1",
    "dml\run_test_update_handler.ps1",
    "dml\run_test_dml_executor.ps1",
    
    # Query模块测试
    "query\run_test_select_handler.ps1",
    "query\run_test_query_executor.ps1",
    
    # Index模块测试
    "index\run_test_adjacent_index.ps1",
    "index\run_test_hash_index.ps1",
    "index\run_test_index_advisor.ps1"
)

$totalTests = 0
$passedTests = 0
$failedTests = 0

foreach ($script in $testScripts) {
    $scriptPath = Join-Path $PSScriptRoot $script
    if (Test-Path $scriptPath) {
        Write-Host ""
        Write-Host "Running: $script" -ForegroundColor Cyan
        Write-Host "----------------------------------------" -ForegroundColor Cyan
        
        Push-Location (Split-Path $scriptPath)
        $result = & ".\$(Split-Path -Leaf $scriptPath)" 2>&1
        $exitCode = $LASTEXITCODE
        Pop-Location
        
        Write-Host $result
        
        $totalTests++
        if ($exitCode -eq 0) {
            $passedTests++
            Write-Host "✓ PASSED" -ForegroundColor Green
        } else {
            $failedTests++
            Write-Host "✗ FAILED (Exit code: $exitCode)" -ForegroundColor Red
        }
        
        # 记录到日志文件
        Add-Content -Path $logFile -Value "`n=== $script ==="
        Add-Content -Path $logFile -Value $result
        Add-Content -Path $logFile -Value "Exit code: $exitCode"
    } else {
        Write-Host "Warning: Test script not found: $scriptPath" -ForegroundColor Yellow
    }
}

Write-Host ""
Write-Host "========================================" -ForegroundColor Cyan
Write-Host "  单元测试汇总" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host "总计: $totalTests 个测试套件" -ForegroundColor White
Write-Host "通过: $passedTests 个" -ForegroundColor Green
Write-Host "失败: $failedTests 个" -ForegroundColor $(if ($failedTests -eq 0) { "Green" } else { "Red" })
Write-Host "测试日志: $logFile" -ForegroundColor Cyan

if ($failedTests -eq 0) {
    Write-Host ""
    Write-Host "✓ 所有单元测试通过！" -ForegroundColor Green
    exit 0
} else {
    Write-Host ""
    Write-Host "✗ 有单元测试失败！" -ForegroundColor Red
    exit 1
}
