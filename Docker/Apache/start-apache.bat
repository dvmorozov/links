@echo off
REM Universal launcher script for links-legacy-apache container
REM Automatically detects Windows and uses appropriate script

echo 🪟 Detected Windows

REM Get script directory
set "SCRIPT_DIR=%~dp0"

REM Check for PowerShell flag
if "%1"=="--powershell" (
    echo 🚀 Launching PowerShell script...
    shift
    powershell -ExecutionPolicy Bypass -File "%SCRIPT_DIR%scripts\windows\start-apache.ps1" %*
) else (
    echo 🚀 Launching batch script...
    call "%SCRIPT_DIR%scripts\windows\start-apache.bat" %*
)
