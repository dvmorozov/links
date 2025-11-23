#!/bin/bash
# Linux startup script for links-legacy-apache container

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_DIR="$(cd "$SCRIPT_DIR/../.." && pwd)"

echo "🐧 Starting Links Legacy Apache on Linux..."

# Function to substitute environment variables in template
substitute_env_vars() {
    local input_file="$1"
    local output_file="$2"
    
    # Use envsubst to substitute variables, with fallback if not available
    if command -v envsubst >/dev/null 2>&1; then
        envsubst < "$input_file" > "$output_file"
    else
        # Fallback: use sed for basic substitution
        sed "s/\${USER}/$USER/g" "$input_file" > "$output_file"
    fi
}

# Generate .env from Linux template
echo "📝 Generating .env file for Linux (user: $USER)..."
substitute_env_vars "$PROJECT_DIR/.env.linux" "$PROJECT_DIR/.env"

# Validate paths exist
echo "🔍 Validating paths..."
ENV_FILE="$PROJECT_DIR/.env"

# Source the .env file to get the variables
if [ -f "$ENV_FILE" ]; then
    set -a
    source "$ENV_FILE"
    set +a
else
    echo "❌ Error: .env file not found at $ENV_FILE"
    exit 1
fi

# Handle command line arguments
ACTION="${1:-start}"
BUILD_FLAG="${2:-}"

# Skip validation if env-only mode (paths will be created by build script)
if [ "$ACTION" != "env-only" ]; then
    # Check if required directories exist
    MISSING_PATHS=()

    if [ ! -d "$(eval echo $HOST_LIBRARY_PATH)" ]; then
        MISSING_PATHS+=("HOST_LIBRARY_PATH: $(eval echo $HOST_LIBRARY_PATH)")
    fi

    if [ ! -d "$(eval echo $HOST_PROJECT_PATH)" ]; then
        MISSING_PATHS+=("HOST_PROJECT_PATH: $(eval echo $HOST_PROJECT_PATH)")
    fi

    if [ ! -d "$(eval echo $HOST_CGI_PATH)" ]; then
        MISSING_PATHS+=("HOST_CGI_PATH: $(eval echo $HOST_CGI_PATH)")
    fi

    if [ ! -f "$(eval echo $HOST_CGI_PATH)/links-legacy.exe" ]; then
        MISSING_PATHS+=("CGI Binary: $(eval echo $HOST_CGI_PATH)/links-legacy.exe")
    fi

    if [ ! -d "$(eval echo $HOST_SOURCES_PATH)" ]; then
        MISSING_PATHS+=("HOST_SOURCES_PATH: $(eval echo $HOST_SOURCES_PATH)")
    fi

    if [ ${#MISSING_PATHS[@]} -gt 0 ]; then
        echo "⚠️  Warning: Some paths don't exist:"
        for path in "${MISSING_PATHS[@]}"; do
            echo "   - $path"
        done
        echo ""
        echo "💡 Make sure to:"
        echo "   1. Build the project in Projects/LegacyCGICMake/"
        echo "   2. Adjust paths in .env.linux if needed"
        echo ""
        read -p "Continue anyway? (y/N): " -n 1 -r
        echo
        if [[ ! $REPLY =~ ^[Yy]$ ]]; then
            exit 1
        fi
    fi
fi

case "$ACTION" in
    start)
        # Check if --build flag is passed
        if [ "$BUILD_FLAG" = "--build" ] || [ "$2" = "--build" ]; then
            echo "🔨 Building application before starting..."
            cd "$PROJECT_DIR"
            ./build-and-deploy.sh --skip-apache
            cd "$PROJECT_DIR"
        fi
        
        echo "🚀 Starting Apache container..."
        docker-compose -f docker-compose.release.yml down --remove-orphans 2>/dev/null || true
        docker-compose -f docker-compose.debug.yml down --remove-orphans 2>/dev/null || true
        docker-compose -f docker-compose.release.yml up -d
        echo "✅ Container started successfully!"
        echo "🌐 Access the application at: https://localhost:4444/cgi-bin/links-legacy.exe?;key=;ch_folder=Ok"
        ;;
    debug)
        # Check if --build flag is passed
        if [ "$BUILD_FLAG" = "--build" ] || [ "$2" = "--build" ]; then
            echo "🔨 Building debug application before starting..."
            cd "$PROJECT_DIR"
            ./build-and-deploy.sh debug --skip-apache
            cd "$PROJECT_DIR"
        fi
        
        echo "🐛 Starting Debug Apache container..."
        docker-compose -f docker-compose.release.yml down --remove-orphans 2>/dev/null || true
        docker-compose -f docker-compose.debug.yml down --remove-orphans 2>/dev/null || true
        docker-compose -f docker-compose.debug.yml up -d
        echo "✅ Debug container started successfully!"
        echo "🌐 Access the debug application at: https://localhost:4433/cgi-bin/links-legacy.exe?;key=;ch_folder=Ok"
        ;;
    build)
        echo "🔨 Building and deploying application..."
        cd "$PROJECT_DIR"
        ./build-and-deploy.sh
        ;;
    stop)
        echo "🛑 Stopping Apache container..."
        docker-compose -f docker-compose.release.yml down
        docker-compose -f docker-compose.debug.yml down 2>/dev/null || true
        echo "✅ Container stopped successfully!"
        ;;
    restart)
        # Check if --build flag is passed
        if [ "$BUILD_FLAG" = "--build" ] || [ "$2" = "--build" ]; then
            echo "🔨 Building application before restarting..."
            cd "$PROJECT_DIR"
            ./build-and-deploy.sh --skip-apache
            cd "$PROJECT_DIR"
        fi
        
        echo "🔄 Restarting Apache container..."
        docker-compose -f docker-compose.release.yml restart
        echo "✅ Container restarted successfully!"
        ;;
    logs)
        echo "📋 Showing container logs..."
        docker-compose -f docker-compose.release.yml logs -f
        ;;
    env-only)
        echo "📝 Generated .env file for Linux (environment setup only)"
        echo "✅ Environment configuration completed!"
        ;;
    status)
        echo "📊 Container status:"
        docker-compose -f docker-compose.release.yml ps
        ;;
    *)
        echo "Usage: $0 {start|debug|build|stop|restart|logs|status|env-only} [--build]"
        echo ""
        echo "Commands:"
        echo "  start    - Start the Apache container (Release version, default)"
        echo "  debug    - Start the Apache container (Debug version)"
        echo "  build    - Build application and start Apache container"
        echo "  stop     - Stop the Apache container"
        echo "  restart  - Restart the Apache container"
        echo "  logs     - Show container logs"
        echo "  status   - Show container status"
        echo "  env-only - Generate .env file without starting container"
        echo ""
        echo "Options:"
        echo "  --build - Build the application before start/restart"
        exit 1
        ;;
esac
