# Windows PowerShell script for links-legacy-apache container

param(
    [Parameter(Position=0)]
    [ValidateSet("start", "stop", "restart", "logs", "status", "build", "install-autostart", "uninstall-autostart")]
    [string]$Action = "start",
    
    [Parameter(Position=1)]
    [switch]$Build
)

Write-Host "Windows Links Legacy Apache Script" -ForegroundColor Cyan

# Function to check if Docker is running
function Test-DockerRunning {
    try {
        docker info 2>$null | Out-Null
        return $true
    } catch {
        return $false
    }
}

# Function to wait for Docker to be ready
function Wait-ForDocker {
    param([int]$TimeoutMinutes = 5)
    
    $timeout = (Get-Date).AddMinutes($TimeoutMinutes)
    Write-Host "Waiting for Docker to be ready..." -ForegroundColor Yellow
    
    while ((Get-Date) -lt $timeout) {
        if (Test-DockerRunning) {
            Write-Host "Docker is ready!" -ForegroundColor Green
            return $true
        }
        Start-Sleep -Seconds 10
    }
    
    Write-Host "Timeout waiting for Docker to be ready" -ForegroundColor Red
    return $false
}

# Get script directory and project directory
$ScriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$ProjectDir = Resolve-Path (Join-Path $ScriptDir "../..")

Write-Host "Generating .env file for Windows..." -ForegroundColor Yellow

# Generate .env from Windows template
$templateContent = Get-Content (Join-Path $ProjectDir ".env.windows") -Raw
$envContent = $templateContent -replace "\\`${USERNAME}", $env:USERNAME
$envContent | Set-Content (Join-Path $ProjectDir ".env")

# Change to project directory
Set-Location $ProjectDir

# Handle actions
switch ($Action) {
    "start" {
        # Check if Docker is running, wait if needed
        if (-not (Test-DockerRunning)) {
            Write-Host "Docker is not running. Waiting for it to start..." -ForegroundColor Yellow
            if (-not (Wait-ForDocker)) {
                Write-Host "Docker is not available. Please start Docker Desktop manually." -ForegroundColor Red
                exit 1
            }
        }
        
        Write-Host "Starting Apache container..." -ForegroundColor Green
        docker-compose down --remove-orphans 2>$null
        docker-compose up -d
        Write-Host "Container started successfully!" -ForegroundColor Green
        Write-Host "Access the application at: https://localhost:4433/cgi-bin/links-legacy.exe" -ForegroundColor Cyan
    }
    "stop" {
        Write-Host "Stopping Apache container..." -ForegroundColor Red
        docker-compose down
        Write-Host "Container stopped successfully!" -ForegroundColor Green
    }
    "status" {
        Write-Host "Container status:" -ForegroundColor Cyan
        docker-compose ps
    }
    "install-autostart" {
        Write-Host "Installing auto-start service..." -ForegroundColor Yellow
        
        # Check if running as administrator
        if (-NOT ([Security.Principal.WindowsPrincipal] [Security.Principal.WindowsIdentity]::GetCurrent()).IsInRole([Security.Principal.WindowsBuiltInRole] "Administrator")) {
            Write-Host "Error: This command requires administrator privileges. Please run PowerShell as Administrator." -ForegroundColor Red
            exit 1
        }
        
        $taskName = "LinksLegacyApache-AutoStart"
        $scriptPath = $MyInvocation.MyCommand.Path
        
        # Create a scheduled task that runs at startup
        $action = New-ScheduledTaskAction -Execute "PowerShell.exe" -Argument "-ExecutionPolicy Bypass -File `"$scriptPath`" start"
        $trigger = New-ScheduledTaskTrigger -AtStartup
        $principal = New-ScheduledTaskPrincipal -UserId "SYSTEM" -LogonType ServiceAccount -RunLevel Highest
        $settings = New-ScheduledTaskSettingsSet -AllowStartIfOnDemand -DontStopIfGoingOnBatteries -StartWhenAvailable
        
        # Add a delay to ensure Docker is fully started
        $trigger.Delay = "PT2M"  # 2 minutes delay
        
        try {
            Register-ScheduledTask -TaskName $taskName -Action $action -Trigger $trigger -Principal $principal -Settings $settings -Force
            Write-Host "Auto-start service installed successfully!" -ForegroundColor Green
            Write-Host "Task name: $taskName" -ForegroundColor Cyan
            Write-Host "The container will start automatically 2 minutes after system boot." -ForegroundColor Cyan
        } catch {
            Write-Host "Error installing auto-start service: $($_.Exception.Message)" -ForegroundColor Red
            exit 1
        }
    }
    "uninstall-autostart" {
        Write-Host "Uninstalling auto-start service..." -ForegroundColor Yellow
        
        # Check if running as administrator
        if (-NOT ([Security.Principal.WindowsPrincipal] [Security.Principal.WindowsIdentity]::GetCurrent()).IsInRole([Security.Principal.WindowsBuiltInRole] "Administrator")) {
            Write-Host "Error: This command requires administrator privileges. Please run PowerShell as Administrator." -ForegroundColor Red
            exit 1
        }
        
        $taskName = "LinksLegacyApache-AutoStart"
        
        try {
            Unregister-ScheduledTask -TaskName $taskName -Confirm:$false
            Write-Host "Auto-start service uninstalled successfully!" -ForegroundColor Green
        } catch {
            Write-Host "Error uninstalling auto-start service: $($_.Exception.Message)" -ForegroundColor Red
            Write-Host "The task may not exist or may already be removed." -ForegroundColor Yellow
        }
    }
    default {
        Write-Host "Usage: .\start-apache.ps1 {start|stop|status|install-autostart|uninstall-autostart} [-Build]" -ForegroundColor White
        Write-Host ""
        Write-Host "Commands:" -ForegroundColor White
        Write-Host "  start              - Start the Apache container (default)" -ForegroundColor White
        Write-Host "  stop               - Stop the Apache container" -ForegroundColor White
        Write-Host "  status             - Show container status" -ForegroundColor White
        Write-Host "  install-autostart  - Install auto-start service (requires admin)" -ForegroundColor White
        Write-Host "  uninstall-autostart- Uninstall auto-start service (requires admin)" -ForegroundColor White
        Write-Host ""
        Write-Host "Auto-start setup:" -ForegroundColor Yellow
        Write-Host "  Run install-autostart as Administrator to enable automatic container startup after system reboot." -ForegroundColor Yellow
        exit 1
    }
}

