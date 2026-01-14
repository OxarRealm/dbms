# AdjacentIndex Test Script
# Purpose: Compile and run AdjacentIndex test program

Write-Host "========================================" -ForegroundColor Cyan
Write-Host "  AdjacentIndex Test" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""

$ErrorActionPreference = "Continue"
$testDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$unitTestsDir = Split-Path -Parent $testDir
$projectRoot = Split-Path -Parent (Split-Path -Parent $unitTestsDir)
$testFile = Join-Path $testDir "test_adjacent_index.cpp"
$adjacentIndexCpp = Join-Path $projectRoot "src\core\adjacent_index.cpp"
$dataManagerCpp = Join-Path $projectRoot "src\core\data_manager.cpp"
$tableManagerCpp = Join-Path $projectRoot "src\core\table_manager.cpp"
$fileManagerCpp = Join-Path $projectRoot "src\core\file_manager.cpp"
$outputDir = Join-Path $projectRoot "test_results\unit_tests"
$outputExe = Join-Path $outputDir "test_adjacent_index.exe"

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
    & g++ -std=c++17 -Wall -I"$projectRoot\include" "$testFile" "$adjacentIndexCpp" "$dataManagerCpp" "$tableManagerCpp" "$fileManagerCpp" -o "$outputExe" 2>&1 | Write-Host
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
$testResult = & .\test_adjacent_index.exe 2>&1
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

