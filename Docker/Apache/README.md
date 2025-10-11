# Links Legacy Apache Docker Container

This directory contains a Docker configuration for running the Links Legacy Apache web server with SSL support and automated debug/release build separation.

## Features

- **Debug/Release Build Separation**: Separate containers for debug and release builds with different ports
- **Automated Build System**: Integrated LegacyBuilder container for cross-platform compilation
- **AI Auto-Foldering**: Local Ollama integration for intelligent bookmark organization
- **Debug Logging**: Comprehensive logging in debug builds (release builds have zero logging overhead)
- **Simplified Setup**: Platform-specific environment templates with automatic configuration
- **SSL/HTTPS Support**: Self-signed certificates for secure connections
- **Static Content Support**: CSS, JavaScript, and images served correctly
- **Container Restart Persistence**: Reliable operation after restarts
- **Cross-Platform Support**: macOS, Linux, and Windows compatibility

## Prerequisites

- Docker Desktop installed and running
- Docker Compose available in PATH
- Project source code available (builds automatically when needed)
- **Ollama installed locally** for AI auto-foldering features
  - Install from https://ollama.ai
  - **Recommended model**: `ollama pull llama3.2:latest` (3x faster than llama2, better semantic understanding)
  - Alternative models: `llama2:latest`, `mistral:latest`, `llama3.1:8b`
  - Service automatically detected from containers via `host.docker.internal`

## Quick Start

### Using the Universal Launcher (Recommended)

**Unix Systems (macOS/Linux):**
```bash
# Start release container (default, port 4444)
./start-apache.sh

# Start debug container (port 4433)
./start-apache.sh debug

# Build and start release container
./start-apache.sh start --build

# Build and start debug container
./start-apache.sh debug --build
```

**Windows:**
```cmd
# Start release container (default, port 4444)
start-apache.bat

# Start debug container (port 4433)
start-apache.bat debug

# Build and start release container
start-apache.bat start --build
```

### Available Commands

All scripts support these commands:

#### Primary Commands
- `start` - Start the **release** container (default, port 4444, optimized)
- `debug` - Start the **debug** container (port 4433, with debug info)
- `build` - Build and start release container
- `stop` - Stop both debug and release containers
- `restart` - Restart the release container
- `logs` - Show release container logs
- `status` - Show release container status

#### Build Options
- `start --build` - Build release version and start container
- `debug --build` - Build debug version and start container

## Container Types

### Release Container (Default)
- **Port**: 4444 (HTTPS)
- **Binary**: Optimized release build (smaller size, no debug info)
- **URL**: https://localhost:4444/cgi-bin/links-legacy.exe?;key=;ch_folder=Ok
- **Use Case**: Production use, regular development

### Debug Container
- **Port**: 4433 (HTTPS)  
- **Binary**: Debug build (larger size, includes debug symbols)
- **URL**: https://localhost:4433/cgi-bin/links-legacy.exe?;key=;ch_folder=Ok
- **Use Case**: Debugging, troubleshooting, development testing

#### Windows Debug Environment (Special Feature)
On Windows, debug builds **automatically** create an **isolated debug environment**:

- **Zero Setup Required**: Auto-generates configuration from your existing `.env.windows`
- **Working Data Safe**: Your actual bookmarks remain completely untouched
- **Debug Copy**: Complete isolated copy automatically created and maintained
- **Fresh Environment**: Debug environment cleaned and refreshed on each build
- **Safe AI Testing**: Perfect for testing auto-foldering AI without any risk

**How it works:**
1. Detects your working environment from `.env.windows`
2. Auto-generates `.env.windows.debug` configuration  
3. Creates debug directory (e.g., `D:/Documents/Links-Debug`)
4. Copies all bookmark data safely to debug environment
5. Starts container using isolated debug environment

**Usage:** Just run `build-and-deploy.bat debug` and everything is handled automatically!

**Requirements:** Only need properly configured `.env.windows` with `HOST_LIBRARY_PATH` pointing to your actual bookmarks.

## AI Auto-Foldering Performance

The AI auto-foldering feature uses your local Ollama installation for intelligent bookmark organization:

### Model Performance (Tested Results)
| Model | Response Time | Quality | Best Use Case |
|-------|---------------|---------|---------------|
| **llama3.2:latest** ⭐ | ~4 seconds | Superior semantic grouping | **Production (recommended)** |
| llama2:latest | ~12 seconds | Good literal categorization | Proven reliability |
| llama3.1:8b | ~15+ seconds | Highest accuracy | Complex categorization |
| mistral:latest | ~8 seconds | Balanced performance | Alternative option |

**Example Test Results:**
- **Input**: GitHub repository, Stack Overflow question, Python tutorial
- **llama3.2**: Intelligently grouped as "Programming" + "Development Communities"
- **llama2**: Literally grouped as "GitHub Repositories" + "Websites"
- **Performance**: llama3.2 delivered 3x faster responses with better semantic understanding

**Model Switching**: Easy to change in `Sources/Controllers/Commands.cpp` if you prefer different models.

## Configuration

The system uses platform-specific environment templates that are automatically processed:

- **`.env.mac`** - macOS paths (`/Users/${USER}/...`)
- **`.env.linux`** - Linux paths (`/home/${USER}/...`)  
- **`.env.windows`** - Windows paths (`C:/Users/${USERNAME}/...`)

When you run a startup script, it automatically:
1. Detects your platform
2. Processes the appropriate template  
3. Substitutes your username
4. Generates the `.env` file
5. Validates paths exist
6. Starts the appropriate container

### Environment File (.env)

Edit the `.env` file to match your local setup:

```properties
# Simple .env file for links-legacy-apache
# Edit these paths to match your local setup

HOST_LIBRARY_PATH=/Users/dmitrymorozov/Documents/Links
HOST_PROJECT_PATH=/Users/dmitrymorozov/Projects/LegacyCGICMake/deploy/var/www/html
HOST_CGI_PATH=/Users/dmitrymorozov/Projects/LegacyCGICMake/deploy/usr/lib/cgi-bin
HOST_SOURCES_PATH=/Users/dmitrymorozov/Projects/library-everything/links/Sources

# Note: HOST_LIBRARY_PATH will be mounted to /var/www/html/users/links in the container
# This should point to your local links library directory
```

**Important**: The `HOST_LIBRARY_PATH` should point to your links library directory that contains subdirectories like `test/`. The application will automatically work within the `test/` subdirectory inside the container.

### Volume Mappings

The containers directly mount the following files and directories:

| Host Path | Container Path | Purpose |
|-----------|----------------|---------|
| `${HOST_PROJECT_PATH}` | `/var/www/html` | Web content |
| `${HOST_LIBRARY_PATH}` | `/var/www/html/users/links` | User data |
| `${HOST_CGI_PATH}/links-legacy-debug.exe` | `/usr/lib/cgi-bin/links-legacy.exe` | Debug CGI executable |
| `${HOST_CGI_PATH}/links-legacy-release.exe` | `/usr/lib/cgi-bin/links-legacy.exe` | Release CGI executable |
| `${HOST_SOURCES_PATH}/Content/Styles.css` | `/var/www/html/links/Styles.css` | CSS file |
| `${HOST_SOURCES_PATH}/Content/Common.js` | `/var/www/html/links/Common.js` | JavaScript file |
| `${HOST_SOURCES_PATH}/Content/legacy-images` | `/var/www/html/links/legacy-images` | Image directory |

**Note**: Each container type mounts its specific binary (debug or release) to avoid conflicts.

## Usage

### Starting the Container

```bash
# Start the container
docker-compose up -d

# Check container status
docker-compose ps
```

### Stopping the Container

```bash
# Stop the container
docker-compose down
```

### Restarting the Container

```bash
# Restart the container
docker-compose restart

# Or stop and start
docker-compose down && docker-compose up -d
```

### Viewing Logs

```bash
# Follow logs
docker-compose logs -f

# View recent logs
docker-compose logs --tail=50
```

## Accessing the Application

Once started, the Apache servers are available at:

### Release Version (Default)
- **HTTPS**: https://localhost:4444/cgi-bin/links-legacy.exe?;key=;ch_folder=Ok
- **Port**: 4444
- **Optimized**: Yes (smaller binary, no debug output)

### Debug Version  
- **HTTPS**: https://localhost:4433/cgi-bin/links-legacy.exe?;key=;ch_folder=Ok
- **Port**: 4433
- **Debug Info**: Yes (larger binary, includes debug symbols)

**Note**: You'll need to accept the self-signed certificate warning in your browser for both versions.

### Directory Structure

The application works with the following directory structure in the container:
- **Working directory**: `/var/www/html/users/links/test/` (where the application operates)
- **Library mount**: `/var/www/html/users/links/` (mounted from `HOST_LIBRARY_PATH`)
- **Web root**: `/var/www/html/` (mounted from `HOST_PROJECT_PATH`)

Your local `HOST_LIBRARY_PATH` should contain a `test/` subdirectory with your links data.

## Troubleshooting

### Common Issues

1. **Docker not running**
   ```
   [ERROR] Cannot connect to Docker daemon
   ```
   **Solution**: Start Docker Desktop

2. **Build failures**
   ```
   [ERROR] Build failed in container
   ```
   **Solution**: 
   - Check if CMake files exist in `Projects/LegacyCGICMake/`
   - Verify Docker has enough resources
   - Check build logs: `docker logs links-legacy-builder`

3. **Path not found errors**
   - Check if paths in `.env` file exist on your system
   - If using `build` command, paths will be created automatically
   - Verify the source code exists at expected locations

4. **503 Service Unavailable after restart**
   - This is usually resolved automatically by the `init.sh` script
   - The script cleans up Apache socket files on restart

5. **Permission issues on macOS/Linux**
   ```bash
   chmod +x init.sh
   chmod +x build-and-deploy.sh
   ```

### Debug Commands

```bash
# Check release container status
docker ps | grep release

# Check debug container status  
docker ps | grep debug

# View release container logs
docker logs links-legacy-apache-release

# View debug container logs
docker logs links-legacy-apache-debug

# Access release container shell
docker exec -it links-legacy-apache-release /bin/bash

# Access debug container shell
docker exec -it links-legacy-apache-debug /bin/bash

# Check if release CGI binary is mounted
docker exec links-legacy-apache-release ls -la /usr/lib/cgi-bin/

# Check if debug CGI binary is mounted
docker exec links-legacy-apache-debug ls -la /usr/lib/cgi-bin/

# Verify binary sizes (debug should be larger)
docker exec links-legacy-apache-release ls -lh /usr/lib/cgi-bin/links-legacy.exe
docker exec links-legacy-apache-debug ls -lh /usr/lib/cgi-bin/links-legacy.exe

# Manual build commands
./build-and-deploy.sh release  # Build release version
./build-and-deploy.sh debug    # Build debug version
```

## Debug Logging

### Debug vs Release Builds

The application includes comprehensive debug logging that **only** compiles in debug builds:

| Build Type | Logging | Performance | Use Case |
|------------|---------|-------------|----------|
| **Debug** | Comprehensive logging to `/var/log/links/debug.log` (mapped to host) | Slightly slower | Development, troubleshooting |
| **Release** | Zero logging (compiled out) | Maximum performance | Production use |

### Debug Log Location

**Inside Container**: `/var/log/links/debug.log`
**Windows Host**: `D:\Projects\library-everything\links\Docker\Apache\logs\debug.log`

**Accessing Debug Logs**:
```bash
# View debug logs from debug container
docker exec links-legacy-apache-debug cat /var/log/links/debug.log

# Monitor debug logs in real-time
docker exec links-legacy-apache-debug tail -f /var/log/links/debug.log

# Search for specific component logs
docker exec links-legacy-apache-debug grep "OllamaService" /var/log/links/debug.log
docker exec links-legacy-apache-debug grep "auto_folder" /var/log/links/debug.log
docker exec links-legacy-apache-debug grep "Exception" /var/log/links/debug.log
```

**Windows File Access**:
```powershell
# View debug logs directly on Windows
Get-Content "D:\Projects\library-everything\links\Docker\Apache\logs\debug.log"

# Monitor debug logs in real-time on Windows
Get-Content "D:\Projects\library-everything\links\Docker\Apache\logs\debug.log" -Wait

# Search for specific patterns in Windows
Select-String -Path "D:\Projects\library-everything\links\Docker\Apache\logs\debug.log" -Pattern "auto_folder"
```

### What Gets Logged (Debug Builds Only)

- 🔍 **Function Tracing**: Entry/exit for all auto-foldering methods
- 🌐 **Environment Detection**: Docker vs native execution detection
- 🤖 **AI Service**: Ollama availability, requests, responses, and timing
- 📁 **File Operations**: Directory creation, file moves, and permissions
- ⚠️ **Exception Handling**: Detailed error context and stack traces
- 📊 **Performance**: Response times and operation counts
- 🔧 **Configuration**: URL parsing, model selection, and environment variables

### Debug Build Commands

```bash
# Build debug version with comprehensive logging
./build-and-deploy.sh debug

# Access debug container and view logs
docker exec -it links-legacy-apache-debug /bin/bash
tail -f /var/log/links/debug.log

# Test auto-foldering with debug logging
curl -k "https://localhost:4433/cgi-bin/links-legacy.exe?;key=;auto_folder=1"
docker exec links-legacy-apache-debug cat /var/log/links/debug.log | grep "auto_folder"
```

**Note**: Release builds have **zero** logging overhead - all debug code is completely compiled out for maximum performance.

## File Structure

```
Docker/Apache/
├── .env                      # Generated environment file (platform-specific)
├── .env.example             # Manual environment template
├── .env.mac                 # macOS environment template
├── .env.linux               # Linux environment template  
├── .env.windows             # Windows environment template
├── docker-compose.yml       # Default Docker Compose (debug)
├── docker-compose.debug.yml # Debug container configuration
├── docker-compose.release.yml # Release container configuration
├── init.sh                  # Container initialization script
├── start-apache.sh          # Universal Unix launcher
├── start-apache.bat         # Universal Windows launcher
├── build-and-deploy.sh      # Automated build system (debug/release)
├── scripts/                 # Platform-specific scripts
│   ├── README.md            # Scripts documentation
│   ├── mac/                 # macOS scripts
│   │   └── start-apache.sh  # Updated with debug/release support
│   ├── linux/               # Linux scripts  
│   │   └── start-apache.sh  # Updated with debug/release support
│   └── windows/             # Windows scripts
│       ├── start-apache.bat
│       └── start-apache.ps1
├── Dockerfile               # Docker image definition
├── apache2.conf             # Apache configuration
├── build-image.sh           # Image build script
├── sites-available/         # Apache site configurations
├── sites-enabled/           # Enabled Apache sites
└── README.md                # This documentation

Docker/LegacyBuilder/         # Build automation
├── docker-compose.yml       # Builder container configuration
├── Dockerfile               # Ubuntu 24.04 + CMake + GCC
└── README.md                # Builder documentation
```

## How It Works

### Debug/Release Build System

The system provides separate build configurations:

1. **Debug Builds**: Include debug symbols (`_DEBUG` defined), larger binaries, port 4433
2. **Release Builds**: Optimized code (`_DEBUG` not defined), smaller binaries, port 4444  
3. **Separate Binaries**: `links-legacy-debug.exe` and `links-legacy-release.exe`
4. **Separate Containers**: Different Docker networks to avoid conflicts
5. **Port-Specific URLs**: Debug and release versions generate URLs with correct ports

### Automated Build System

The system includes comprehensive building capabilities:

1. **Build Detection**: Scripts automatically detect if the application needs building
2. **LegacyBuilder Integration**: Uses Ubuntu 24.04-based container with CMake and GCC
3. **Automatic Image Creation**: Builder images are created automatically when needed
4. **Debug/Release Modes**: Support for both debug and release configurations
5. **Binary Separation**: Creates separate binaries to avoid Docker volume caching issues
6. **Deployment Integration**: Built binaries are deployed to correct container paths

### Container Management Process

1. **Platform Detection**: Universal launchers detect your OS automatically
2. **Template Processing**: Each platform has its own environment template
3. **Variable Substitution**: Scripts substitute user variables (`${USER}` or `${USERNAME}`)
4. **Path Validation**: Scripts verify required directories exist
5. **Build Automation**: Automated building with debug/release support
6. **Container Orchestration**: Separate networks and containers for debug/release
7. **Volume Mount Management**: Proper binary mounting with conflict resolution
