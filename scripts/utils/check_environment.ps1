# 环境检查脚本
# 用途：检查开发环境是否配置正确

Write-Host "========================================" -ForegroundColor Cyan
Write-Host "  Development Environment Check" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""

$allOK = $true

# 1. Check CMake
Write-Host "[1/4] Checking CMake..." -ForegroundColor Yellow
try {
    $cmakeVersion = cmake --version 2>&1 | Select-Object -First 1
    Write-Host "  OK CMake installed: $cmakeVersion" -ForegroundColor Green
} catch {
    Write-Host "  X CMake not found, please ensure CMake is installed and in PATH" -ForegroundColor Red
    Write-Host "    Tip: If CMake was just installed, please restart terminal" -ForegroundColor Yellow
    $allOK = $false
}

# 2. Check Qt
Write-Host "`n[2/4] Checking Qt..." -ForegroundColor Yellow
try {
    $qtVersion = qmake --version 2>&1 | Select-Object -First 1
    Write-Host "  OK Qt installed: $qtVersion" -ForegroundColor Green
    
    # Try to find Qt path
    $qtPath = $qtVersion | Select-String -Pattern "in (.+)$" | ForEach-Object { $_.Matches[0].Groups[1].Value }
    if ($qtPath) {
        Write-Host "  Qt path: $qtPath" -ForegroundColor Cyan
    }
} catch {
    Write-Host "  X Qt not found" -ForegroundColor Red
    $allOK = $false
}

# 3. Check MSVC compiler
Write-Host "`n[3/4] Checking MSVC compiler..." -ForegroundColor Yellow
try {
    $clPath = where.exe cl.exe 2>&1
    if ($LASTEXITCODE -eq 0) {
        Write-Host "  OK MSVC compiler found: $clPath" -ForegroundColor Green
    } else {
        Write-Host "  X MSVC compiler not found" -ForegroundColor Red
        $allOK = $false
    }
} catch {
    Write-Host "  X MSVC compiler not found" -ForegroundColor Red
    $allOK = $false
}

# 4. Check Git (optional)
Write-Host "`n[4/4] Checking Git (optional)..." -ForegroundColor Yellow
try {
    $gitVersion = git --version 2>&1
    Write-Host "  OK Git installed: $gitVersion" -ForegroundColor Green
} catch {
    Write-Host "  ! Git not found (optional)" -ForegroundColor Yellow
}

Write-Host "`n========================================" -ForegroundColor Cyan
if ($allOK) {
    Write-Host "  Environment check passed OK" -ForegroundColor Green
    Write-Host "  Ready to configure project" -ForegroundColor Green
} else {
    Write-Host "  Environment check failed X" -ForegroundColor Red
    Write-Host "  Please fix the issues above" -ForegroundColor Red
}
Write-Host "========================================" -ForegroundColor Cyan
