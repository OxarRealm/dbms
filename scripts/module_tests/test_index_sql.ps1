# Test script for CREATE INDEX and DROP INDEX SQL statements
# This script tests the SQL interface for index management

Write-Host "==========================================" -ForegroundColor Cyan
Write-Host "Testing CREATE INDEX and DROP INDEX SQL" -ForegroundColor Cyan
Write-Host "==========================================" -ForegroundColor Cyan
Write-Host ""

$scriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$projectRoot = Split-Path -Parent $scriptDir
$projectRoot = Split-Path -Parent $projectRoot

$testDbName = "test_index_sql_db"
$testDbPath = Join-Path $projectRoot $testDbName

Write-Host "Test Database: $testDbPath" -ForegroundColor Yellow
Write-Host ""

# Clean up previous test database
if (Test-Path "$testDbPath.dbf") {
    Remove-Item "$testDbPath.*" -Force
    Write-Host "Cleaned up previous test database" -ForegroundColor Gray
}

Write-Host "Step 1: Creating test table..." -ForegroundColor Green
$createTableSQL = @"
CREATE TABLE TestTable (
    id int KEY NO_NULL VALID,
    name string NO_NULL VALID,
    age int NO_NULL VALID
) INTO $testDbName;
"@

Write-Host "SQL: $createTableSQL" -ForegroundColor Gray

# Note: This script should be run from the GUI or compiled executable
# Here we only show the test SQL statements
Write-Host ""
Write-Host "Step 2: Test CREATE INDEX statements..." -ForegroundColor Green
Write-Host ""

$createIndexSQLs = @(
    "CREATE INDEX ON TestTable(id) USING hash IN $testDbName;",
    "CREATE INDEX ON TestTable(name) USING btree IN $testDbName;",
    "CREATE INDEX ON TestTable(age) USING adjacent IN $testDbName;"
)

foreach ($sql in $createIndexSQLs) {
    Write-Host "SQL: $sql" -ForegroundColor Gray
}

Write-Host ""
Write-Host "Step 3: Test DROP INDEX statements..." -ForegroundColor Green
Write-Host ""

$dropIndexSQLs = @(
    "DROP INDEX ON TestTable(name) USING btree IN $testDbName;",
    "DROP INDEX ON TestTable(age) IN $testDbName;"
)

foreach ($sql in $dropIndexSQLs) {
    Write-Host "SQL: $sql" -ForegroundColor Gray
}

Write-Host ""
Write-Host "==========================================" -ForegroundColor Cyan
Write-Host "Test SQL statements generated" -ForegroundColor Cyan
Write-Host "Please execute these SQL statements in the GUI SQL Execution tab" -ForegroundColor Yellow
Write-Host "==========================================" -ForegroundColor Cyan









