# Links Legacy Docker Setup

Enhanced Docker setup for the Links Legacy application with support for debug and release builds.

## Quick Start

### Prerequisites

- Docker and Docker Compose installed
- At least 4GB of available RAM
- Windows, Linux, or macOS

### Windows Quick Start

```cmd
cd Docker\Apache

# Debug build (default)
.\build-and-deploy.bat debug

# Release build  
.\build-and-deploy.bat release

# Clean environment and build
.\build-and-deploy.bat debug --clean
```

### Linux/macOS Quick Start

```bash
cd Docker/Apache

# Debug build (default)
./build-and-deploy.sh debug

# Release build
./build-and-deploy.sh release

# Clean environment and build
./build-and-deploy.sh debug --clean
```

## Build Types

### Debug Build
- **Port**: 4433
- **Container**: `links-legacy-apache-debug`
- **Binary**: `links-legacy-debug.exe`
- **Features**: Includes debugging tools (gdb, build tools)
- **URL**: https://localhost:4433/cgi-bin/links-legacy.exe?;key=;ch_folder=Ok

### Release Build
- **Port**: 4444
- **Container**: `links-legacy-apache-release`
- **Binary**: `links-legacy-release.exe`
- **Features**: Optimized production build
- **URL**: https://localhost:4444/cgi-bin/links-legacy.exe?;key=;ch_folder=Ok

## Available Commands

### Build and Deploy Script

```cmd
# Windows
.\build-and-deploy.bat [debug|release] [--clean] [--skip-build] [--skip-apache] [--help]

# Linux/macOS  
./build-and-deploy.sh [debug|release] [--clean] [--skip-build] [--skip-apache] [--help]
```

**Options:**
- `debug|release`: Choose build type (default: debug)
- `--clean`: Remove all containers and images before building
- `--skip-build`: Skip build process, only deploy and start Apache
- `--skip-apache`: Build and deploy, but don't start Apache container
- `--help`: Show help message

### PowerShell Script (Windows)

```powershell
# Basic usage
.\scripts\windows\start-apache.ps1 start debug

# Build and start with clean environment
.\scripts\windows\start-apache.ps1 start release -Build -Clean

# Install auto-start service (requires Admin)
.\scripts\windows\start-apache.ps1 install-autostart debug
```

**Commands:**
- `start`: Start Apache container
- `build`: Build and deploy application
- `stop`: Stop containers
- `restart`: Restart container
- `logs`: Show container logs
- `status`: Show container status
- `clean`: Clean all containers and images
- `install-autostart`: Install Windows auto-start service
- `uninstall-autostart`: Remove auto-start service

## Examples

### Development Workflow

```cmd
# Start fresh debug environment
.\build-and-deploy.bat debug --clean

# View logs
docker logs links-legacy-apache-debug -f

# Stop when done
docker stop links-legacy-apache-debug
```

### Production Deployment

```cmd
# Build and deploy release version
.\build-and-deploy.bat release

# Install auto-start (Windows, requires Admin)
.\scripts\windows\start-apache.ps1 install-autostart release
```

### Clean Rebuild

```cmd
# Clean all containers and images, then rebuild
.\build-and-deploy.bat debug --clean
```

## Troubleshooting

### Common Issues

1. **Docker not running**: Ensure Docker Desktop is started
2. **Port conflicts**: Debug uses 4433, Release uses 4444
3. **Permission errors**: Run PowerShell as Administrator for auto-start features
4. **Build failures**: Use `--clean` option to remove old artifacts

### Useful Commands

```cmd
# Check container status
docker ps

# View all images
docker images | findstr links-legacy

# Clean everything (nuclear option)
docker system prune -a -f
```

### SSL Certificate Warning

The application uses a self-signed SSL certificate. Accept the browser security warning to proceed.

## File Structure

- `build-and-deploy.bat/sh`: Main build script
- `scripts/windows/start-apache.ps1`: Enhanced PowerShell script
- `docker-compose.debug.yml`: Debug configuration
- `docker-compose.release.yml`: Release configuration
- `Dockerfile`: Multi-stage build with debug/release targets

## Environment Variables

Environment variables are automatically generated from `.env.windows`, `.env.linux`, or `.env.mac` templates based on your system.