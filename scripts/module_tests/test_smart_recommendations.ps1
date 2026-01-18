# Smart Recommendations Test Script
# Purpose: Compile and run smart recommendations test program

Write-Host "========================================" -ForegroundColor Cyan
Write-Host "  Smart Recommendations Test" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""

$ErrorActionPreference = "Continue"
$testDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$projectRoot = Split-Path -Parent (Split-Path -Parent $testDir)
$testFile = Join-Path $testDir "test_smart_recommendations.cpp"

# Source files
$indexAdvisorCpp = Join-Path $projectRoot "src\index\index_advisor.cpp"
$adjacentIndexCpp = Join-Path $projectRoot "src\core\adjacent_index.cpp"
$hashIndexCpp = Join-Path $projectRoot "src\core\hash_index.cpp"
$btreeIndexCpp = Join-Path $projectRoot "src\core\btree_index.cpp"
$dataManagerCpp = Join-Path $projectRoot "src\core\data_manager.cpp"
$tableManagerCpp = Join-Path $projectRoot "src\core\table_manager.cpp"
$fileManagerCpp = Join-Path $projectRoot "src\core\file_manager.cpp"
$indexManagerCpp = Join-Path $projectRoot "src\core\index_manager.cpp"
$indexStorageCpp = Join-Path $projectRoot "src\core\index_storage.cpp"

# Include directories
$includeDir = Join-Path $projectRoot "include"

# Output directory
$outputDir = Join-Path $projectRoot "test_results\module_tests"
if (-not (Test-Path $outputDir)) {
    New-Item -ItemType Directory -Path $outputDir -Force | Out-Null
}

$exeFile = Join-Path $outputDir "test_smart_recommendations.exe"

Write-Host "Compiling test program..." -ForegroundColor Yellow

# Compile
$compileCmd = "g++ -std=c++14 -I`"$includeDir`" -o `"$exeFile`" `"$testFile`" `"$indexAdvisorCpp`" `"$adjacentIndexCpp`" `"$hashIndexCpp`" `"$btreeIndexCpp`" `"$dataManagerCpp`" `"$tableManagerCpp`" `"$fileManagerCpp`" `"$indexManagerCpp`" `"$indexStorageCpp`" 2>&1"

try {
    $compileOutput = Invoke-Expression $compileCmd
    if ($LASTEXITCODE -ne 0) {
        Write-Host "Compilation failed!" -ForegroundColor Red
        Write-Host $compileOutput
        exit 1
    }
    Write-Host "Compilation successful!" -ForegroundColor Green
} catch {
    Write-Host "Compilation error: $_" -ForegroundColor Red
    exit 1
}

Write-Host ""
Write-Host "Running test program..." -ForegroundColor Yellow
Write-Host ""

# Run test
try {
    & $exeFile
    $exitCode = $LASTEXITCODE
    
    Write-Host ""
    if ($exitCode -eq 0) {
        Write-Host "All tests passed!" -ForegroundColor Green
    } else {
        Write-Host "Some tests failed!" -ForegroundColor Red
    }
    
    exit $exitCode
} catch {
    Write-Host "Test execution error: $_" -ForegroundColor Red
    exit 1
}




