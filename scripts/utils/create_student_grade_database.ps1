# 学生成绩数据库生成脚本
# 用途：使用CMake编译并运行 create_student_grade_database.cpp，生成学生成绩数据库

$ErrorActionPreference = "Continue"

# 设置脚本目录
$ScriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$ProjectRoot = Split-Path -Parent (Split-Path -Parent $ScriptDir)

Write-Host "========================================" -ForegroundColor Cyan
Write-Host "  Student Grade Database Generator" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""

# 切换到项目根目录
Set-Location $ProjectRoot

# 检查源文件是否存在
$SourceFile = Join-Path $ScriptDir "create_student_grade_database.cpp"
if (-not (Test-Path $SourceFile)) {
    Write-Host "Error: Source file not found: $SourceFile" -ForegroundColor Red
    exit 1
}

# 检查build目录
if (-not (Test-Path (Join-Path $ProjectRoot "build"))) {
    Write-Host "Error: build directory not found, please configure CMake first" -ForegroundColor Red
    Write-Host "Run: mkdir build; cd build; cmake .." -ForegroundColor Yellow
    exit 1
}

Write-Host "[1] Checking build directory..." -ForegroundColor Yellow

# 设置输出可执行文件路径（假设编译到build目录）
$OutputExe = Join-Path $ProjectRoot "create_student_grade_database.exe"

# 尝试查找可能的位置
$PossiblePaths = @(
    $OutputExe,
    (Join-Path $ProjectRoot "build\create_student_grade_database.exe"),
    (Join-Path $ProjectRoot "build\Release\create_student_grade_database.exe"),
    (Join-Path $ProjectRoot "build\Debug\create_student_grade_database.exe"),
    (Join-Path $ProjectRoot "build\bin\Release\create_student_grade_database.exe"),
    (Join-Path $ProjectRoot "build\bin\Debug\create_student_grade_database.exe")
)

$FoundExe = $null
foreach ($path in $PossiblePaths) {
    if (Test-Path $path) {
        $FoundExe = $path
        Write-Host "  Found executable: $FoundExe" -ForegroundColor Green
        break
    }
}

# 如果找不到可执行文件，尝试编译
if (-not $FoundExe) {
    Write-Host "[2] Executable not found, attempting to compile..." -ForegroundColor Yellow
    Write-Host "  Note: Please add create_student_grade_database target to CMakeLists.txt" -ForegroundColor Gray
    Write-Host "  Or compile manually using the existing build system" -ForegroundColor Gray
    Write-Host ""
    Write-Host "  Alternatively, you can run SQL statements directly in the GUI:" -ForegroundColor Yellow
    Write-Host "  1. Open the GUI" -ForegroundColor Gray
    Write-Host "  2. Create a new database named 'student_grade_db'" -ForegroundColor Gray
    Write-Host "  3. Execute the SQL statements from:" -ForegroundColor Gray
    $SqlFile = Join-Path $ScriptDir "..\..\test_data\student_grade_db_setup.sql"
    Write-Host "     $SqlFile" -ForegroundColor Cyan
    exit 1
}

Write-Host "[2] Running database generator..." -ForegroundColor Yellow
Write-Host ""

# 运行程序
$RunResult = & $FoundExe 2>&1
Write-Host $RunResult

if ($LASTEXITCODE -eq 0) {
    Write-Host ""
    Write-Host "========================================" -ForegroundColor Green
    Write-Host "  Database created successfully!" -ForegroundColor Green
    Write-Host "========================================" -ForegroundColor Green
    Write-Host ""
    
    # 显示数据库文件位置
    $DbFile = Join-Path $ProjectRoot "student_grade_db.dbf"
    $DataFile = Join-Path $ProjectRoot "student_grade_db.dat"
    
    if (Test-Path $DbFile) {
        $ResolvedDbFile = Resolve-Path $DbFile
        $ResolvedDataFile = Resolve-Path $DataFile
        
        Write-Host "Database file location:" -ForegroundColor Cyan
        Write-Host "  $ResolvedDbFile" -ForegroundColor Yellow
        Write-Host "  $ResolvedDataFile" -ForegroundColor Yellow
        Write-Host ""
        
        $DbInfo = Get-Item $DbFile
        $DataInfo = Get-Item $DataFile
        Write-Host "File size:" -ForegroundColor Cyan
        Write-Host "  $($DbInfo.Name): $([math]::Round($DbInfo.Length / 1KB, 2)) KB" -ForegroundColor Gray
        Write-Host "  $($DataInfo.Name): $([math]::Round($DataInfo.Length / 1KB, 2)) KB" -ForegroundColor Gray
        Write-Host ""
        Write-Host "You can now open this database in the GUI!" -ForegroundColor Green
    }
} else {
    Write-Host ""
    Write-Host "Error: Program execution failed (exit code: $LASTEXITCODE)" -ForegroundColor Red
    exit 1
}

Write-Host ""
Write-Host "Done!" -ForegroundColor Green
