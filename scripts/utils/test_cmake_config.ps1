# CMake Configuration Test Script
# Purpose: Test if CMake configuration is correct

Write-Host "========================================" -ForegroundColor Cyan
Write-Host "  CMake Configuration Test" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""

# Check CMake
try {
    $null = cmake --version 2>&1
} catch {
    Write-Host "Error: CMake not found, please install CMake and add to PATH" -ForegroundColor Red
    Write-Host "If CMake was just installed, please restart terminal" -ForegroundColor Yellow
    exit 1
}

# Create build directory
$buildDir = "build"
if (-not (Test-Path $buildDir)) {
    New-Item -ItemType Directory -Path $buildDir | Out-Null
    Write-Host "Created build directory: $buildDir" -ForegroundColor Green
}

# Enter build directory
Push-Location $buildDir

try {
    Write-Host "Running CMake configuration..." -ForegroundColor Yellow
    Write-Host ""
    
    # Run CMake configuration
    $cmakeOutput = cmake .. 2>&1 | Tee-Object -Variable cmakeOutputVar
    
    # Check configuration result
    if ($LASTEXITCODE -eq 0) {
        Write-Host ""
        Write-Host "========================================" -ForegroundColor Green
        Write-Host "  CMake configuration successful OK" -ForegroundColor Green
        Write-Host "========================================" -ForegroundColor Green
        
        # Check if Qt was found
        $outputString = $cmakeOutputVar -join "`n"
        if ($outputString -match "Found Qt") {
            Write-Host "Qt was found correctly" -ForegroundColor Green
        } else {
            Write-Host "Warning: Qt may not be found, check Qt path in CMakeLists.txt" -ForegroundColor Yellow
        }
        
        Write-Host ""
        Write-Host "Next steps:" -ForegroundColor Cyan
        Write-Host "  1. Check CMake output to confirm Qt version" -ForegroundColor White
        Write-Host "  2. If configuration is correct, you can start development" -ForegroundColor White
        Write-Host "  3. If there are errors, check Qt path in CMakeLists.txt" -ForegroundColor White
    } else {
        Write-Host ""
        Write-Host "========================================" -ForegroundColor Red
        Write-Host "  CMake configuration failed X" -ForegroundColor Red
        Write-Host "========================================" -ForegroundColor Red
        Write-Host ""
        Write-Host "Common issues:" -ForegroundColor Yellow
        Write-Host "  1. Qt not found: Check CMAKE_PREFIX_PATH in CMakeLists.txt" -ForegroundColor White
        Write-Host "  2. Compiler not found: Ensure MSVC is in PATH" -ForegroundColor White
        Write-Host "  3. Other errors: Check error messages above" -ForegroundColor White
        Pop-Location
        exit 1
    }
} catch {
    Write-Host ""
    Write-Host "Error occurred: $_" -ForegroundColor Red
    Pop-Location
    exit 1
} finally {
    Pop-Location
}