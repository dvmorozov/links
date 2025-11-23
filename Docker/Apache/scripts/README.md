# Platform-Specific Scripts

This directory contains platform-specific startup scripts for the Links Legacy Apache container.

## Quick Start

### Universal Launchers (Recommended)

Use these scripts from the main Docker/Apache directory:

**Unix Systems (macOS/Linux):**
```bash
./start-apache.sh [start|stop|restart|logs|status]
```

**Windows:**
```cmd
# Using batch script (default)
start-apache.bat [start|stop|restart|logs|status]

# Using PowerShell script
start-apache.bat --powershell [start|stop|restart|logs|status]
```

### Direct Platform Scripts

You can also run platform-specific scripts directly:

**macOS:**
```bash
./scripts/mac/start-apache.sh [start|stop|restart|logs|status]
```

**Linux:**
```bash
./scripts/linux/start-apache.sh [start|stop|restart|logs|status]
```

**Windows (Batch):**
```cmd
scripts\windows\start-apache.bat [start|stop|restart|logs|status]
```

**Windows (PowerShell):**
```powershell
.\scripts\windows\start-apache.ps1 [-Action start|stop|restart|logs|status]
```

## How It Works

1. **Environment Detection**: Scripts automatically detect the platform and use appropriate paths
2. **Template Processing**: Each platform has its own `.env.{platform}` template with platform-specific paths
3. **Variable Substitution**: Scripts substitute `${USER}` (Unix) or `${USERNAME}` (Windows) with actual username
4. **Path Validation**: Scripts check if required directories exist before starting
5. **Docker Management**: Scripts handle Docker Compose operations with proper error handling

## Environment Templates

### `.env.mac` (macOS)
- Uses `/Users/${USER}/` path convention
- Designed for macOS file system structure

### `.env.linux` (Linux)  
- Uses `/home/${USER}/` path convention
- Designed for Linux file system structure

### `.env.windows` (Windows)
- Uses `C:/Users/${USERNAME}/` path convention
- Compatible with both Command Prompt and PowerShell

## Features

- ✅ **Platform auto-detection**
- ✅ **Path validation with warnings**
- ✅ **User variable substitution**
- ✅ **Colored output for better UX**
- ✅ **Error handling and graceful fallbacks**
- ✅ **Multiple Windows script options (batch/PowerShell)**
- ✅ **Container lifecycle management**

## Commands

All scripts support these commands:

- `start` - Start the Apache container (default action)
- `stop` - Stop the Apache container  
- `restart` - Restart the Apache container
- `logs` - Show container logs (follow mode)
- `status` - Show container status

## Examples

```bash
# Start container (default)
./start-apache.sh

# Stop container
./start-apache.sh stop

# View logs
./start-apache.sh logs

# Check status
./start-apache.sh status
```

## Troubleshooting

### Path Issues
If you see path validation warnings:
1. Make sure you've built the project in `Projects/LegacyCGICMake/`
2. Adjust paths in the appropriate `.env.{platform}` file
3. Ensure the CGI binary exists: `links-legacy.exe`

### Windows Execution Policy
If PowerShell scripts fail to run:
```powershell
Set-ExecutionPolicy -ExecutionPolicy RemoteSigned -Scope CurrentUser
```

### Permission Issues (Unix)
Make scripts executable:
```bash
chmod +x start-apache.sh
chmod +x scripts/mac/start-apache.sh
chmod +x scripts/linux/start-apache.sh
```
