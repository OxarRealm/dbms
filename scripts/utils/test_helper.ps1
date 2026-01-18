# 测试辅助函数
# 用途：提供测试工具函数

function Assert-Equal {
    param(
        [object]$Expected,
        [object]$Actual,
        [string]$Message = ""
    )
    
    if ($Expected -ne $Actual) {
        Write-Error "断言失败: $Message`n期望: $Expected`n实际: $Actual"
        return $false
    }
    return $true
}

function Assert-True {
    param(
        [bool]$Condition,
        [string]$Message = ""
    )
    
    if (-not $Condition) {
        Write-Error "断言失败: $Message`n条件应为真"
        return $false
    }
    return $true
}

function Assert-False {
    param(
        [bool]$Condition,
        [string]$Message = ""
    )
    
    if ($Condition) {
        Write-Error "断言失败: $Message`n条件应为假"
        return $false
    }
    return $true
}

function Measure-ExecutionTime {
    param(
        [scriptblock]$ScriptBlock
    )
    
    $stopwatch = [System.Diagnostics.Stopwatch]::StartNew()
    $result = & $ScriptBlock
    $stopwatch.Stop()
    
    return @{
        Result = $result
        ElapsedMilliseconds = $stopwatch.ElapsedMilliseconds
    }
}

function Test-FileExists {
    param([string]$FilePath)
    return Test-Path $FilePath
}

function Clean-TestFiles {
    param([string]$Directory)
    
    if (Test-Path $Directory) {
        Get-ChildItem -Path $Directory -Filter "*.dbf" | Remove-Item -Force
        Get-ChildItem -Path $Directory -Filter "*.dat" | Remove-Item -Force
        Write-Host "清理测试文件: $Directory" -ForegroundColor Yellow
    }
}

Export-ModuleMember -Function Assert-Equal, Assert-True, Assert-False, Measure-ExecutionTime, Test-FileExists, Clean-TestFiles
