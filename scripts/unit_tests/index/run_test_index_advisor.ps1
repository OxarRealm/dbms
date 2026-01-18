# IndexAdvisor Test Script
# Purpose: Compile and run IndexAdvisor test program

Write-Host "========================================" -ForegroundColor Cyan
Write-Host "  IndexAdvisor Test" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""

$ErrorActionPreference = "Continue"
$testDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$unitTestsDir = Split-Path -Parent $testDir
$projectRoot = Split-Path -Parent (Split-Path -Parent $unitTestsDir)
$testFile = Join-Path $testDir "test_index_advisor.cpp"
$indexAdvisorCpp = Join-Path $projectRoot "src\index\index_advisor.cpp"
$adjacentIndexCpp = Join-Path $projectRoot "src\core\adjacent_index.cpp"
$hashIndexCpp = Join-Path $projectRoot "src\core\hash_index.cpp"
$queryExecutorCpp = Join-Path $projectRoot "src\query\query_executor.cpp"
$selectHandlerCpp = Join-Path $projectRoot "src\query\select_handler.cpp"
$ddlExecutorCpp = Join-Path $projectRoot "src\ddl\ddl_executor.cpp"
$dmlExecutorCpp = Join-Path $projectRoot "src\dml\dml_executor.cpp"
$createTableHandlerCpp = Join-Path $projectRoot "src\ddl\create_table_handler.cpp"
$editTableHandlerCpp = Join-Path $projectRoot "src\ddl\edit_table_handler.cpp"
$renameTableHandlerCpp = Join-Path $projectRoot "src\ddl\rename_table_handler.cpp"
$dropTableHandlerCpp = Join-Path $projectRoot "src\ddl\drop_table_handler.cpp"
$insertHandlerCpp = Join-Path $projectRoot "src\dml\insert_handler.cpp"
$deleteHandlerCpp = Join-Path $projectRoot "src\dml\delete_handler.cpp"
$updateHandlerCpp = Join-Path $projectRoot "src\dml\update_handler.cpp"
$dataManagerCpp = Join-Path $projectRoot "src\core\data_manager.cpp"
$tableManagerCpp = Join-Path $projectRoot "src\core\table_manager.cpp"
$fileManagerCpp = Join-Path $projectRoot "src\core\file_manager.cpp"
$parserCpp = Join-Path $projectRoot "src\sql_parser\parser.cpp"
$lexerCpp = Join-Path $projectRoot "src\sql_parser\lexer.cpp"
$tokenCpp = Join-Path $projectRoot "src\sql_parser\token.cpp"
$astNodeCpp = Join-Path $projectRoot "src\sql_parser\ast_node.cpp"
$outputDir = Join-Path $projectRoot "test_results\unit_tests"
$outputExe = Join-Path $outputDir "test_index_advisor.exe"

# Create output directory
if (-not (Test-Path $outputDir)) {
    New-Item -ItemType Directory -Path $outputDir -Force | Out-Null
}

Write-Host "Test file: $testFile" -ForegroundColor Yellow
Write-Host "Output directory: $outputDir" -ForegroundColor Yellow
Write-Host ""

# Check if test file exists
if (-not (Test-Path $testFile)) {
    Write-Host "Error: Test file not found: $testFile" -ForegroundColor Red
    exit 1
}

# Compile test program
Write-Host "Compiling test program..." -ForegroundColor Yellow

# Check compiler
$gppCheck = Get-Command g++ -ErrorAction SilentlyContinue
$compileSuccess = $false

if ($gppCheck) {
    Write-Host "Using compiler: g++" -ForegroundColor Cyan
    $sourceFiles = @(
        "$testFile",
        "$indexAdvisorCpp",
        "$adjacentIndexCpp",
        "$hashIndexCpp",
        "$queryExecutorCpp",
        "$selectHandlerCpp",
        "$ddlExecutorCpp",
        "$dmlExecutorCpp",
        "$createTableHandlerCpp",
        "$editTableHandlerCpp",
        "$renameTableHandlerCpp",
        "$dropTableHandlerCpp",
        "$insertHandlerCpp",
        "$deleteHandlerCpp",
        "$updateHandlerCpp",
        "$dataManagerCpp",
        "$tableManagerCpp",
        "$fileManagerCpp",
        "$parserCpp",
        "$lexerCpp",
        "$tokenCpp",
        "$astNodeCpp"
    )
    & g++ -std=c++17 -Wall -I"$projectRoot\include" $sourceFiles -o "$outputExe" 2>&1 | Write-Host
    if ($LASTEXITCODE -eq 0) {
        $compileSuccess = $true
    }
}

if (-not $compileSuccess) {
    Write-Host "Compilation failed!" -ForegroundColor Red
    exit 1
}

Write-Host "Compilation successful" -ForegroundColor Green

# Run test program
Write-Host ""
Write-Host "Running test program..." -ForegroundColor Yellow

if (-not (Test-Path $outputExe)) {
    Write-Host "Error: Executable not found: $outputExe" -ForegroundColor Red
    exit 1
}

Push-Location $outputDir
$testResult = & .\test_index_advisor.exe 2>&1
$runResult = $LASTEXITCODE
Pop-Location

Write-Host $testResult

if ($runResult -eq 0) {
    Write-Host ""
    Write-Host "========================================" -ForegroundColor Green
    Write-Host "  Test PASSED!" -ForegroundColor Green
    Write-Host "========================================" -ForegroundColor Green
    exit 0
} else {
    Write-Host ""
    Write-Host "========================================" -ForegroundColor Red
    Write-Host "  Test FAILED! Exit code: $runResult" -ForegroundColor Red
    Write-Host "========================================" -ForegroundColor Red
    exit 1
}

