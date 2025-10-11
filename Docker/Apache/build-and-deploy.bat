@echo off
REM Enhanced build and deploy script for Windows
REM Supports debug/release builds and clean option

setlocal enabledelayedexpansion

REM Default values
set "BUILD_TYPE=debug"
set "SKIP_BUILD=false"
set "SKIP_APACHE=false"
set "CLEAN=false"

REM Parse command line arguments
:parse_args
if "%1"=="debug" (
    set "BUILD_TYPE=debug"
    shift
    goto parse_args
)
if "%1"=="release" (
    set "BUILD_TYPE=release"
    shift
    goto parse_args
)
if "%1"=="--clean" (
    set "CLEAN=true"
    shift
    goto parse_args
)
if "%1"=="--skip-build" (
    set "SKIP_BUILD=true"
    shift
    goto parse_args
)
if "%1"=="--skip-apache" (
    set "SKIP_APACHE=true"
    shift
    goto parse_args
)
if "%1"=="--help" goto show_help
if "%1"=="-h" goto show_help
if "%1"=="/?" goto show_help
if not "%1"=="" (
    echo [ERROR] Unknown option: %1
    echo Use --help for usage information
    exit /b 1
)

echo [BUILD] Starting %BUILD_TYPE% build and deploy process...

REM Get script directory and project root
set "SCRIPT_DIR=%~dp0"
set "PROJECT_ROOT=%SCRIPT_DIR%..\.."
set "LEGACY_BUILDER_DIR=%PROJECT_ROOT%\Docker\LegacyBuilder"
set "LEGACY_CMAKE_DIR=%PROJECT_ROOT%\Projects\LegacyCGICMake"

REM Load environment configuration
call :load_environment
if !errorlevel! neq 0 exit /b !errorlevel!

REM Check if Docker is running
docker info >nul 2>&1
if !errorlevel! neq 0 (
    echo [ERROR] Docker is not running. Please start Docker and try again.
    exit /b 1
)

REM Check if Ollama container is running and start if needed
call :check_and_start_ollama
if !errorlevel! neq 0 exit /b !errorlevel!

REM Clean containers and images if requested
if "!CLEAN!"=="true" (
    call :clean_environment
    if !errorlevel! neq 0 exit /b !errorlevel!
)

REM Build application if not skipped
if "!SKIP_BUILD!"=="false" (
    call :build_application
    if !errorlevel! neq 0 exit /b !errorlevel!
    
    call :deploy_application
    if !errorlevel! neq 0 exit /b !errorlevel!
) else (
    echo [SKIP] Skipping build process...
    call :deploy_application
    if !errorlevel! neq 0 exit /b !errorlevel!
)

REM Start Apache container if not skipped
if "!SKIP_APACHE!"=="false" (
    call :build_and_start_apache
    if !errorlevel! neq 0 exit /b !errorlevel!
) else (
    echo [SKIP] Skipping Apache container startup...
)

echo [SUCCESS] %BUILD_TYPE% build and deploy process completed!

if "!SKIP_APACHE!"=="false" (
    if "!BUILD_TYPE!"=="debug" (
        if defined HOST_LIBRARY_PATH_DEBUG (
            echo [INFO] Debug application available at: https://localhost:4433/cgi-bin/links-legacy.exe?;key=;ch_folder=Ok
            echo [INFO] Using isolated debug environment: !HOST_LIBRARY_PATH_DEBUG!
            echo [INFO] Working environment preserved at: !HOST_LIBRARY_PATH_WORKING!
            echo [INFO] Debug configuration: .env.windows.debug (auto-generated if needed)
        ) else (
            echo [INFO] Debug application available at: https://localhost:4433/cgi-bin/links-legacy.exe?;key=;ch_folder=Ok
        )
    ) else (
        echo [INFO] Release application available at: https://localhost:4444/cgi-bin/links-legacy.exe?;key=;ch_folder=Ok
    )
)

endlocal
goto :eof

:clean_environment
echo [CLEAN] Cleaning containers and images...

REM Stop and remove containers
docker stop links-legacy-apache-debug links-legacy-apache-release links-legacy-builder 2>nul
docker rm links-legacy-apache-debug links-legacy-apache-release links-legacy-builder 2>nul

REM Remove images
docker rmi links-legacy-apache:debug links-legacy-apache:release links-legacy-builder:latest 2>nul

REM Clean up Docker system
docker system prune -f >nul 2>&1

echo [SUCCESS] Environment cleaned!
goto :eof

:build_application
echo [BUILDER] Starting legacy builder container...

cd /d "%LEGACY_BUILDER_DIR%"

REM Check if builder image exists, build it if not
docker image inspect links-legacy-builder:latest >nul 2>&1
if !errorlevel! neq 0 (
    echo [BUILD] Building links-legacy-builder image...
    docker build -t links-legacy-builder:latest .
    if !errorlevel! neq 0 (
        echo [ERROR] Failed to build links-legacy-builder image!
        exit /b 1
    )
    echo [SUCCESS] Builder image built successfully!
)

REM Start the builder container
docker-compose up -d links-legacy-builder

REM Wait for container to be ready
echo [WAIT] Waiting for builder container to be ready...
timeout /t 5 /nobreak >nul

REM Build the application inside the container
if "!BUILD_TYPE!"=="debug" (
    echo [BUILD] Building debug version...
    docker exec links-legacy-builder bash -c "cd /mnt/links/Projects/LegacyCGICMake && rm -f CMakeCache.txt && cmake -DCMAKE_BUILD_TYPE=Debug . && cmake --build . --config Debug"
) else (
    echo [BUILD] Building release version...
    docker exec links-legacy-builder bash -c "cd /mnt/links/Projects/LegacyCGICMake && rm -f CMakeCache.txt && cmake -DCMAKE_BUILD_TYPE=Release . && cmake --build . --config Release"
)

if !errorlevel! equ 0 (
    echo [SUCCESS] %BUILD_TYPE% build completed successfully!
) else (
    echo [ERROR] %BUILD_TYPE% build failed!
    docker-compose down
    exit /b 1
)

REM Stop the builder container
echo [STOP] Stopping builder container...
docker-compose down

goto :eof

:deploy_application
echo [DEPLOY] Deploying %BUILD_TYPE% application...

cd /d "%LEGACY_CMAKE_DIR%"

REM Check if the binary was built
if not exist "links-legacy.exe" (
    echo [ERROR] Built binary not found at links-legacy.exe
    exit /b 1
)

REM Make sure deploy directories exist
if not exist "deploy\var\www\html\links" mkdir "deploy\var\www\html\links"
if not exist "deploy\usr\lib\cgi-bin" mkdir "deploy\usr\lib\cgi-bin"

REM Copy binary with appropriate name
if "!BUILD_TYPE!"=="debug" (
    copy "links-legacy.exe" "deploy\usr\lib\cgi-bin\links-legacy-debug.exe" >nul
    echo    [OK] Deployed debug binary
) else (
    copy "links-legacy.exe" "deploy\usr\lib\cgi-bin\links-legacy-release.exe" >nul
    echo    [OK] Deployed release binary
)

REM Deploy static content
echo [DEPLOY] Deploying static content...

if exist "..\..\Sources\Content\Styles.css" (
    copy "..\..\Sources\Content\Styles.css" "deploy\var\www\html\links\" >nul
    echo    [OK] Deployed Styles.css
)

if exist "..\..\Sources\Content\Common.js" (
    copy "..\..\Sources\Content\Common.js" "deploy\var\www\html\links\" >nul
    echo    [OK] Deployed Common.js
)

if exist "..\..\Sources\Content\legacy-images" (
    xcopy "..\..\Sources\Content\legacy-images" "deploy\var\www\html\links\legacy-images\" /E /I /Y >nul
    echo    [OK] Deployed legacy-images
)

echo [SUCCESS] %BUILD_TYPE% application deployed successfully!
goto :eof

:build_and_start_apache
echo [APACHE] Building and starting Apache container...

cd /d "%SCRIPT_DIR%"

REM Build the appropriate Apache image
if "!BUILD_TYPE!"=="debug" (
    docker build --target debug -t links-legacy-apache:debug .
    if !errorlevel! neq 0 (
        echo [ERROR] Failed to build Apache debug image!
        exit /b 1
    )
    echo [SUCCESS] Apache debug image built successfully!
    
    echo [CLEANUP] Stopping existing debug container...
    docker stop links-legacy-apache-debug >nul 2>&1
    docker rm links-legacy-apache-debug >nul 2>&1
    
    if defined HOST_LIBRARY_PATH_DEBUG (
        echo [APACHE] Using Windows debug environment configuration...
        docker-compose --env-file .env.windows.debug -f docker-compose.debug.windows.yml up -d
    ) else (
        echo [APACHE] Using standard debug configuration...
        docker-compose -f docker-compose.debug.yml up -d
    )
) else (
    docker build --target base -t links-legacy-apache:release .
    if !errorlevel! neq 0 (
        echo [ERROR] Failed to build Apache release image!
        exit /b 1
    )
    echo [SUCCESS] Apache release image built successfully!
    
    echo [CLEANUP] Stopping existing release container...
    docker stop links-legacy-apache-release >nul 2>&1
    docker rm links-legacy-apache-release >nul 2>&1
    
    docker-compose -f docker-compose.release.yml up -d
)

if !errorlevel! equ 0 (
    echo [SUCCESS] Apache %BUILD_TYPE% container started successfully!
) else (
    echo [ERROR] Failed to start Apache %BUILD_TYPE% container!
    exit /b 1
)

goto :eof

:show_help
echo Usage: %0 [debug^|release] [--clean] [--skip-build] [--skip-apache] [--help]
echo.
echo Build Types:
echo   debug       Build and deploy debug version (default, port 4433)
echo               On Windows: Creates isolated debug environment with copy of working data
echo   release     Build and deploy release version (port 4444)
echo.
echo Options:
echo   --clean         Clean all containers and images before building
echo   --skip-build    Skip the build process, only deploy and start Apache
echo   --skip-apache   Build and deploy, but don't start Apache container
echo   --help, -h, /?  Show this help message
echo.
echo Debug Environment (Windows):
echo   Debug builds automatically create and manage a separate debug environment:
echo   - Automatically generates debug configuration from .env.windows
echo   - Copies all files from working environment (safe, isolated testing)
echo   - Stores them in separate debug environment directory
echo   - Cleans debug environment on each build for consistency
echo   - No manual setup required - everything is automated
echo.
echo Examples:
echo   %0 debug           Build debug with isolated environment
echo   %0 release --clean Clean environment and build release version
echo   %0 --skip-build    Just start Apache with existing binaries
exit /b 0

:load_environment
echo [ENV] Loading environment configuration...

REM Detect Windows and load appropriate environment
if "%BUILD_TYPE%"=="debug" (
    echo [DEBUG-ENV] Setting up debug environment...
    call :setup_debug_environment
    if !errorlevel! neq 0 exit /b !errorlevel!
) else (
    echo [ENV] Loading standard Windows environment...
    set "STD_ENV_FILE=%SCRIPT_DIR%.env.windows"
    for /f "tokens=1,2 delims==" %%a in ('type "!STD_ENV_FILE!" ^| findstr /v "^#" ^| findstr /v "^$"') do (
        set "%%a=%%b"
    )
)

goto :eof

:check_and_start_ollama
echo [OLLAMA] Checking if Ollama container is running...

REM Check if backupseeker-ollama container is running
docker ps --format "table {{.Names}}" | findstr /i "backupseeker-ollama" >nul 2>&1
if !errorlevel! equ 0 (
    echo [OLLAMA] Ollama container is already running
    goto :eof
)

echo [OLLAMA] Ollama container not running, attempting to start...

REM Try to start existing backupseeker-ollama container
docker start backupseeker-ollama >nul 2>&1
if !errorlevel! equ 0 (
    echo [OLLAMA] Successfully started existing Ollama container
    
    REM Wait a few seconds for the container to fully start
    echo [OLLAMA] Waiting for Ollama service to be ready...
    timeout /t 5 /nobreak >nul
    goto :eof
)

echo [WARNING] Could not start Ollama container. Checking if image exists...

REM Check if backupseeker-ollama image exists
docker images --format "table {{.Repository}}:{{.Tag}}" | findstr /i "backupseeker-ollama" >nul 2>&1
if !errorlevel! neq 0 (
    echo [ERROR] Ollama image 'backupseeker-ollama' not found!
    echo [ERROR] Please ensure the Ollama container is built and available.
    echo [ERROR] The application requires Ollama for AI features to work.
    exit /b 1
)

echo [ERROR] Ollama image exists but container failed to start!
echo [ERROR] Please check the container manually: docker logs backupseeker-ollama
exit /b 1

:setup_debug_environment
echo [DEBUG-ENV] Setting up debug environment...

REM Load debug environment configuration if it exists
set "DEBUG_ENV_FILE=%SCRIPT_DIR%.env.windows.debug"
if exist "!DEBUG_ENV_FILE!" goto load_debug_config

echo [DEBUG-ENV] Creating debug configuration from template...
set "ENV_FILE=%SCRIPT_DIR%.env.windows"
if not exist "!ENV_FILE!" (
    echo [ERROR] Standard Windows environment file (!ENV_FILE!) not found!
    echo [ERROR] Please ensure .env.windows exists before running debug build.
    exit /b 1
)

for /f "tokens=1,2 delims==" %%a in ('type "!ENV_FILE!" ^| findstr /v "^#" ^| findstr /v "^$"') do (
    if "%%a"=="HOST_LIBRARY_PATH" set "WORKING_PATH=%%b"
    if "%%a"=="HOST_PROJECT_PATH" set "PROJECT_PATH=%%b"
    if "%%a"=="HOST_CGI_PATH" set "CGI_PATH=%%b"
    if "%%a"=="HOST_SOURCES_PATH" set "SOURCES_PATH=%%b"
    if "%%a"=="HOST_LOG_PATH" set "LOG_PATH=%%b"
)

set "DEBUG_OUTPUT_FILE=%SCRIPT_DIR%.env.windows.debug"
echo # Auto-generated Windows DEBUG environment configuration > "!DEBUG_OUTPUT_FILE!"
echo # Generated from .env.windows on %date% %time% >> "!DEBUG_OUTPUT_FILE!"
echo. >> "!DEBUG_OUTPUT_FILE!"
echo # Working environment ^(your actual bookmark data - DO NOT MODIFY^) >> "!DEBUG_OUTPUT_FILE!"
echo HOST_LIBRARY_PATH_WORKING=!WORKING_PATH! >> "!DEBUG_OUTPUT_FILE!"
echo. >> "!DEBUG_OUTPUT_FILE!"
echo # Debug environment ^(isolated copy for safe debugging^) >> "!DEBUG_OUTPUT_FILE!"
echo HOST_LIBRARY_PATH_DEBUG=!WORKING_PATH!-Debug >> "!DEBUG_OUTPUT_FILE!"
echo. >> "!DEBUG_OUTPUT_FILE!"
echo # Standard paths >> "!DEBUG_OUTPUT_FILE!"
echo HOST_PROJECT_PATH=!PROJECT_PATH! >> "!DEBUG_OUTPUT_FILE!"
echo HOST_CGI_PATH=!CGI_PATH! >> "!DEBUG_OUTPUT_FILE!"
echo HOST_SOURCES_PATH=!SOURCES_PATH! >> "!DEBUG_OUTPUT_FILE!"
echo. >> "!DEBUG_OUTPUT_FILE!"
echo # Debug log directory ^(mapped from container for Windows access^) >> "!DEBUG_OUTPUT_FILE!"
echo HOST_LOG_PATH=!LOG_PATH! >> "!DEBUG_OUTPUT_FILE!"
echo. >> "!DEBUG_OUTPUT_FILE!"
echo PLATFORM=windows >> "!DEBUG_OUTPUT_FILE!"

echo [DEBUG-ENV] Auto-generated debug configuration: .env.windows.debug

:load_debug_config
echo [DEBUG-ENV] Loading debug environment configuration...
set "DEBUG_CONFIG_FILE=%SCRIPT_DIR%.env.windows.debug"
for /f "tokens=1,2 delims==" %%a in ('type "!DEBUG_CONFIG_FILE!" ^| findstr /v "^#" ^| findstr /v "^$"') do (
    set "%%a=%%b"
)

REM Validate configuration
if "!HOST_LIBRARY_PATH_WORKING!"=="" (
    echo [ERROR] HOST_LIBRARY_PATH_WORKING not defined in debug configuration!
    echo [ERROR] Please check .env.windows.debug file.
    exit /b 1
)

if "!HOST_LIBRARY_PATH_DEBUG!"=="" (
    echo [ERROR] HOST_LIBRARY_PATH_DEBUG not defined in debug configuration!
    echo [ERROR] Please check .env.windows.debug file.
    exit /b 1
)

if not exist "!HOST_LIBRARY_PATH_WORKING!" (
    echo [ERROR] Working library path does not exist: !HOST_LIBRARY_PATH_WORKING!
    echo [ERROR] Please ensure your working environment is properly configured.
    echo [ERROR] Check the HOST_LIBRARY_PATH in .env.windows file.
    exit /b 1
)

echo [DEBUG-ENV] Configuration validated:
echo    Working: !HOST_LIBRARY_PATH_WORKING!
echo    Debug:   !HOST_LIBRARY_PATH_DEBUG!

REM Copy files from working to debug environment
call :copy_debug_environment
if !errorlevel! neq 0 exit /b !errorlevel!

goto :eof

:copy_debug_environment
echo [DEBUG-ENV] Preparing isolated debug environment...

REM Create debug environment directory if it doesn't exist
if not exist "!HOST_LIBRARY_PATH_DEBUG!" (
    echo [DEBUG-ENV] Creating debug directory: !HOST_LIBRARY_PATH_DEBUG!
    mkdir "!HOST_LIBRARY_PATH_DEBUG!" 2>nul
    if !errorlevel! neq 0 (
        echo [ERROR] Failed to create debug directory!
        echo [ERROR] Please check permissions for: !HOST_LIBRARY_PATH_DEBUG!
        exit /b 1
    )
)

REM Create log directory if it doesn't exist
if not exist "!HOST_LOG_PATH!" (
    echo [DEBUG-ENV] Creating log directory: !HOST_LOG_PATH!
    mkdir "!HOST_LOG_PATH!" 2>nul
    if !errorlevel! neq 0 (
        echo [ERROR] Failed to create log directory!
        echo [ERROR] Please check permissions for: !HOST_LOG_PATH!
        exit /b 1
    )
) else (
    echo [DEBUG-ENV] Log directory already exists: !HOST_LOG_PATH!
)

REM Clean up log files to start fresh
echo [DEBUG-ENV] Cleaning up log files for fresh start...
if exist "!HOST_LOG_PATH!\auto_organize.log" (
    echo [DEBUG-ENV] Removing auto_organize.log
    del "!HOST_LOG_PATH!\auto_organize.log" 2>nul
)
if exist "!HOST_LOG_PATH!\debug.log" (
    echo [DEBUG-ENV] Removing debug.log
    del "!HOST_LOG_PATH!\debug.log" 2>nul
)
if exist "!HOST_LOG_PATH!\llm_json.log" (
    echo [DEBUG-ENV] Removing llm_json.log
    del "!HOST_LOG_PATH!\llm_json.log" 2>nul
)
echo [DEBUG-ENV] Log cleanup completed

REM Clean debug environment (remove all contents)
echo [DEBUG-ENV] Cleaning previous debug data...
if exist "!HOST_LIBRARY_PATH_DEBUG!\*" (
    del /s /q "!HOST_LIBRARY_PATH_DEBUG!\*" >nul 2>&1
    for /d %%d in ("!HOST_LIBRARY_PATH_DEBUG!\*") do rmdir /s /q "%%d" >nul 2>&1
)

REM Count files to copy for progress indication
echo [DEBUG-ENV] Analyzing working environment...
set "FILE_COUNT=0"
for /r "!HOST_LIBRARY_PATH_WORKING!" %%f in (*) do set /a FILE_COUNT+=1

if !FILE_COUNT! equ 0 (
    echo [WARNING] No files found in working environment: !HOST_LIBRARY_PATH_WORKING!
    echo [WARNING] Debug environment will be empty.
) else (
    echo [DEBUG-ENV] Found !FILE_COUNT! files to copy
)

REM Copy all files from working to debug environment
echo [DEBUG-ENV] Copying working environment to debug environment...
echo    Source: !HOST_LIBRARY_PATH_WORKING!
echo    Target: !HOST_LIBRARY_PATH_DEBUG!

xcopy "!HOST_LIBRARY_PATH_WORKING!\*" "!HOST_LIBRARY_PATH_DEBUG!\" /E /I /Y /H /K /Q >nul 2>&1
if !errorlevel! neq 0 (
    echo [ERROR] Failed to copy files to debug environment!
    echo [ERROR] Please check:
    echo [ERROR]   - Source exists: !HOST_LIBRARY_PATH_WORKING!
    echo [ERROR]   - Target writable: !HOST_LIBRARY_PATH_DEBUG!
    echo [ERROR]   - No files in use by other applications
    exit /b 1
)

REM Verify copy was successful
set "COPIED_COUNT=0"
for /r "!HOST_LIBRARY_PATH_DEBUG!" %%f in (*) do set /a COPIED_COUNT+=1

echo [SUCCESS] Debug environment ready!
echo [DEBUG-ENV] Copied !COPIED_COUNT! files to isolated debug environment
echo [DEBUG-ENV] Working environment remains untouched at: !HOST_LIBRARY_PATH_WORKING!
echo [DEBUG-ENV] Debug environment ready at: !HOST_LIBRARY_PATH_DEBUG!

goto :eof