#!/bin/bash
# Automated build and deploy script for links-legacy application
# This script:
# 1. Starts the legacy builder container
# 2. Builds the specified version (debug/release)
# 3. Stops the builder container
# 4. Deploys the binary and static content
# 5. Starts the Apache container with appropriate configuration

set -e

# Parse command line arguments
BUILD_TYPE="Release"  # Default to Release
CONTAINER_SUFFIX=""   # Default container name
if [ "$1" = "debug" ]; then
    BUILD_TYPE="Debug"
    CONTAINER_SUFFIX="-debug"
    echo "🐛 Building DEBUG version with debug output enabled"
elif [ "$1" = "release" ] || [ -z "$1" ]; then
    BUILD_TYPE="Release"
    CONTAINER_SUFFIX=""
    echo "🚀 Building RELEASE version without debug output"
else
    echo "Usage: $0 [debug|release]"
    echo "  debug   - Build debug version with debug output"
    echo "  release - Build release version without debug output (default)"
    exit 1
fi

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
CYAN='\033[0;36m'
NC='\033[0m' # No Color

# Get script directory and project root
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/../.." && pwd)"
LEGACY_BUILDER_DIR="$PROJECT_ROOT/Docker/LegacyBuilder"
LEGACY_CMAKE_DIR="$PROJECT_ROOT/Projects/LegacyCGICMake"

echo -e "${CYAN}🔨 Starting automated build and deploy process for ${BUILD_TYPE} build...${NC}"

# Function to check if Docker is running
check_docker() {
    if ! docker info >/dev/null 2>&1; then
        # Check if Docker Desktop context is active but not running
        local current_context=$(docker context show 2>/dev/null)
        if [ "$current_context" = "desktop-linux" ] || [ "$current_context" = "desktop-windows" ]; then
            echo -e "${RED}❌ Docker Desktop is not running.${NC}"
            echo -e "${YELLOW}Please start Docker Desktop and try again, or switch to system Docker:${NC}"
            echo -e "${CYAN}   docker context use default${NC}"
        else
            echo -e "${RED}❌ Docker is not running. Please start Docker and try again.${NC}"
        fi
        exit 1
    fi
    
    # Display which Docker is being used
    local current_context=$(docker context show 2>/dev/null)
    if [ "$current_context" = "desktop-linux" ] || [ "$current_context" = "desktop-windows" ]; then
        echo -e "${CYAN}ℹ️  Using Docker Desktop${NC}"
    else
        echo -e "${CYAN}ℹ️  Using system Docker (socket: /var/run/docker.sock)${NC}"
    fi
}

# Function to build the application
build_application() {
    echo -e "${BLUE}📦 Starting legacy builder container...${NC}"
    
    cd "$LEGACY_BUILDER_DIR"
    
    # Check if builder image exists, build it if not
    if ! docker image inspect links-legacy-builder:latest >/dev/null 2>&1; then
        echo -e "${YELLOW}🔧 Builder image not found. Building links-legacy-builder...${NC}"
        docker build -t links-legacy-builder:latest .
        if [ $? -ne 0 ]; then
            echo -e "${RED}❌ Failed to build links-legacy-builder image!${NC}"
            exit 1
        fi
        echo -e "${GREEN}✅ Builder image built successfully!${NC}"
    fi
    
    # Start the builder container using docker run instead of docker-compose
    # (docker-compose has issues with Docker Desktop on Linux)
    echo -e "${YELLOW}🚀 Starting container...${NC}"
    
    # Remove any existing container
    docker rm -f links-legacy-builder 2>/dev/null || true
    
    # Start the container
    docker run -d \
        --name links-legacy-builder \
        -p 2500:22 \
        -v "$PROJECT_ROOT:/mnt/links" \
        links-legacy-builder:latest
    
    if [ $? -ne 0 ]; then
        echo -e "${RED}❌ Failed to start builder container!${NC}"
        exit 1
    fi
    
    # Wait for container to be ready and verify it's running
    echo -e "${YELLOW}⏳ Waiting for builder container to be ready...${NC}"
    sleep 5
    
    # Check if container is actually running
    if ! docker ps | grep -q links-legacy-builder; then
        echo -e "${RED}❌ Builder container failed to start or exited immediately!${NC}"
        echo -e "${YELLOW}Checking container logs:${NC}"
        docker logs links-legacy-builder 2>&1 || echo "No logs available"
        echo -e "${YELLOW}Checking container status:${NC}"
        docker ps -a | grep links-legacy-builder || echo "Container not found"
        docker rm -f links-legacy-builder 2>/dev/null
        exit 1
    fi
    
    # Build the application inside the container
    echo -e "${BLUE}🔧 Building ${BUILD_TYPE} version...${NC}"
    docker exec links-legacy-builder bash -c "
        cd /mnt/links/Projects/LegacyCGICMake && 
        rm -f CMakeCache.txt &&
        cmake -DCMAKE_BUILD_TYPE=${BUILD_TYPE} . &&
        cmake --build . --config ${BUILD_TYPE}
    "
    
    if [ $? -eq 0 ]; then
        echo -e "${GREEN}✅ Build completed successfully!${NC}"
    else
        echo -e "${RED}❌ Build failed!${NC}"
        docker stop links-legacy-builder
        docker rm links-legacy-builder
        exit 1
    fi
    
    # Stop the builder container
    echo -e "${BLUE}🛑 Stopping builder container...${NC}"
    docker stop links-legacy-builder
    docker rm links-legacy-builder
}

# Function to deploy the built application
deploy_application() {
    echo -e "${BLUE}📤 Deploying application...${NC}"
    
    cd "$LEGACY_CMAKE_DIR"
    
    # Check if the binary was built in the root directory
    if [ ! -f "links-legacy.exe" ]; then
        echo -e "${RED}❌ Built binary not found at links-legacy.exe${NC}"
        exit 1
    fi
    
    # Make sure deploy directories exist
    mkdir -p "deploy/var/www/html/links"
    mkdir -p "deploy/usr/lib/cgi-bin"
    mkdir -p "deploy/var/www/html/temp"
    
    # Set permissions for temp directory (writable by Apache)
    chmod 777 "deploy/var/www/html/temp"
    
    # Copy the binary to the deploy directory with appropriate name
    if [ "$BUILD_TYPE" = "Debug" ]; then
        cp "links-legacy.exe" "deploy/usr/lib/cgi-bin/links-legacy-debug.exe"
        chmod +x "deploy/usr/lib/cgi-bin/links-legacy-debug.exe"
        echo -e "${GREEN}   ✓ Deployed debug binary to deploy/usr/lib/cgi-bin/links-legacy-debug.exe${NC}"
    else
        cp "links-legacy.exe" "deploy/usr/lib/cgi-bin/links-legacy-release.exe"
        chmod +x "deploy/usr/lib/cgi-bin/links-legacy-release.exe"
        echo -e "${GREEN}   ✓ Deployed release binary to deploy/usr/lib/cgi-bin/links-legacy-release.exe${NC}"
    fi
    
    # Deploy static content
    echo -e "${YELLOW}📂 Deploying static content...${NC}"
    
    # Deploy CSS and JS files
    if [ -f "../../Sources/Content/Styles.css" ]; then
        cp "../../Sources/Content/Styles.css" "deploy/var/www/html/links/"
        echo -e "${GREEN}   ✓ Deployed Styles.css${NC}"
    fi
    
    if [ -f "../../Sources/Content/Common.js" ]; then
        cp "../../Sources/Content/Common.js" "deploy/var/www/html/links/"
        echo -e "${GREEN}   ✓ Deployed Common.js${NC}"
    fi
    
    # Deploy images
    if [ -d "../../Sources/Content/legacy-images" ]; then
        cp -r "../../Sources/Content/legacy-images" "deploy/var/www/html/links/"
        echo -e "${GREEN}   ✓ Deployed legacy-images${NC}"
    fi
    
        echo -e "${GREEN}✅ Application deployed successfully!${NC}"
}

# Function to start the appropriate Apache container
start_apache_container() {
    echo -e "${BLUE}🚀 Starting Apache container for ${BUILD_TYPE} build...${NC}"
    
    cd "$SCRIPT_DIR"
    
    # Check and build Apache image if needed
    check_and_build_apache_image
    
    # Stop any existing containers first
    echo -e "${YELLOW}🛑 Stopping any existing containers...${NC}"
    # Stop by name
    docker stop links-legacy-apache-debug 2>/dev/null || true
    docker rm links-legacy-apache-debug 2>/dev/null || true
    docker stop links-legacy-apache-release 2>/dev/null || true
    docker rm links-legacy-apache-release 2>/dev/null || true
    
    # Also stop any containers using ports 4433 or 4444
    CONTAINERS_ON_4433=$(docker ps -q --filter "publish=4433")
    if [ -n "$CONTAINERS_ON_4433" ]; then
        echo -e "${YELLOW}   Stopping containers on port 4433...${NC}"
        docker stop $CONTAINERS_ON_4433 2>/dev/null || true
        docker rm $CONTAINERS_ON_4433 2>/dev/null || true
    fi
    
    CONTAINERS_ON_4444=$(docker ps -q --filter "publish=4444")
    if [ -n "$CONTAINERS_ON_4444" ]; then
        echo -e "${YELLOW}   Stopping containers on port 4444...${NC}"
        docker stop $CONTAINERS_ON_4444 2>/dev/null || true
        docker rm $CONTAINERS_ON_4444 2>/dev/null || true
    fi
    
    # Generate appropriate .env file and source it
    if [[ "$OSTYPE" == "darwin"* ]]; then
        ./scripts/mac/start-apache.sh env-only
    elif [[ "$OSTYPE" == "linux-gnu"* ]]; then
        ./scripts/linux/start-apache.sh env-only
    else
        echo -e "${YELLOW}⚠️  Platform detection failed, using universal launcher...${NC}"
        ./start-apache.sh env-only
    fi
    
    # Source the generated .env file
    if [ -f ".env" ]; then
        set -a
        source .env
        set +a
    fi
    
    if [ "$BUILD_TYPE" = "Debug" ]; then
        echo -e "${YELLOW}🐛 Starting debug container on port 4433...${NC}"
        docker run -d \
            --name links-legacy-apache-debug \
            -p 4433:443 \
            --add-host="host.docker.internal:host-gateway" \
            -v "${HOST_PROJECT_PATH}:/var/www/html" \
            -v "${HOST_LIBRARY_PATH}:/var/www/html/users/links/test" \
            -v "${HOST_CGI_PATH}/links-legacy-debug.exe:/usr/lib/cgi-bin/links-legacy.exe:ro" \
            -v "${HOST_SOURCES_PATH}/Content/Styles.css:/var/www/html/links/Styles.css:ro" \
            -v "${HOST_SOURCES_PATH}/Content/Common.js:/var/www/html/links/Common.js:ro" \
            -v "${HOST_SOURCES_PATH}/Content/legacy-images:/var/www/html/links/legacy-images:ro" \
            -v "${HOST_LOG_PATH}:/var/log/links" \
            -v "$SCRIPT_DIR/init.sh:/init.sh:ro" \
            -e BUILD_TYPE=Debug \
            links-legacy-apache:debug \
            /init.sh apachectl -D FOREGROUND
        
        if [ $? -eq 0 ]; then
            echo -e "${GREEN}✅ Debug container started!${NC}"
            echo -e "${CYAN}🌐 Debug application available at: https://localhost:4433/cgi-bin/links-legacy.exe?;key=;ch_folder=Ok${NC}"
        else
            echo -e "${RED}❌ Failed to start debug container!${NC}"
            exit 1
        fi
    else
        echo -e "${YELLOW}🚀 Starting release container on port 4444...${NC}"
        docker run -d \
            --name links-legacy-apache-release \
            -p 4444:443 \
            -v "${HOST_PROJECT_PATH}:/var/www/html" \
            -v "${HOST_PROJECT_PATH}/../temp:/var/www/html/temp" \
            -v "${HOST_LIBRARY_PATH}:/var/www/html/users/links/test" \
            -v "${HOST_CGI_PATH}/links-legacy-release.exe:/usr/lib/cgi-bin/links-legacy.exe:ro" \
            -v "${HOST_SOURCES_PATH}/Content/Styles.css:/var/www/html/links/Styles.css:ro" \
            -v "${HOST_SOURCES_PATH}/Content/Common.js:/var/www/html/links/Common.js:ro" \
            -v "${HOST_SOURCES_PATH}/Content/legacy-images:/var/www/html/links/legacy-images:ro" \
            -v "$SCRIPT_DIR/init.sh:/init.sh:ro" \
            -e BUILD_TYPE=Release \
            links-legacy-apache:release \
            /init.sh apachectl -D FOREGROUND
        
        if [ $? -eq 0 ]; then
            echo -e "${GREEN}✅ Release container started!${NC}"
            echo -e "${CYAN}🌐 Release application available at: https://localhost:4444/cgi-bin/links-legacy.exe?;key=;ch_folder=Ok${NC}"
        else
            echo -e "${RED}❌ Failed to start release container!${NC}"
            exit 1
        fi
    fi
}

# Function to check and build Apache image if needed
check_and_build_apache_image() {
    echo -e "${BLUE}🔍 Checking for Apache image...${NC}"
    
    if [ "$BUILD_TYPE" = "Debug" ]; then
        IMAGE_TAG="links-legacy-apache:debug"
        TARGET="debug"
    else
        IMAGE_TAG="links-legacy-apache:release"
        TARGET="base"
    fi
    
    # Check if image exists, build if not
    if ! docker image inspect "$IMAGE_TAG" >/dev/null 2>&1; then
        echo -e "${YELLOW}🔧 Apache image $IMAGE_TAG not found. Building...${NC}"
        docker build --target "$TARGET" \
            --build-arg USER_UID=$(id -u) \
            --build-arg USER_GID=$(id -g) \
            -t "$IMAGE_TAG" .
        if [ $? -ne 0 ]; then
            echo -e "${RED}❌ Failed to build $IMAGE_TAG image!${NC}"
            exit 1
        fi
        echo -e "${GREEN}✅ Apache image $IMAGE_TAG built successfully!${NC}"
    else
        echo -e "${GREEN}✅ Apache image $IMAGE_TAG already exists${NC}"
    fi
}

# Main function
main() {
    check_docker
    build_application
    deploy_application
    start_apache_container
    
    echo -e "${GREEN}🎉 Build and deploy process completed!${NC}"
    if [ "$BUILD_TYPE" = "Debug" ]; then
        echo -e "${CYAN}🌐 Debug application available at: https://localhost:4433/cgi-bin/links-legacy.exe?;key=;ch_folder=Ok${NC}"
    else
        echo -e "${CYAN}🌐 Release application available at: https://localhost:4444/cgi-bin/links-legacy.exe?;key=;ch_folder=Ok${NC}"
    fi
}

# Run main function
main
