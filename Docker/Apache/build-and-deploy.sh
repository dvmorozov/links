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
        echo -e "${RED}❌ Docker is not running. Please start Docker and try again.${NC}"
        exit 1
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
    
    # Create a temporary .env file with UID for docker-compose
    # Start the builder container
    docker-compose up -d links-legacy-builder
    
    # Wait for container to be ready
    echo -e "${YELLOW}⏳ Waiting for builder container to be ready...${NC}"
    sleep 5
    
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
        docker-compose down
        exit 1
    fi
    
    # Stop the builder container
    echo -e "${BLUE}🛑 Stopping builder container...${NC}"
    docker-compose down
}

# Function to deploy the built application
deploy_application() {
    echo -e "${BLUE}📤 Deploying application...${NC}"
    
    cd "$LEGACY_CMAKE_DIR"
    
    # Check if the binary was built
    if [ ! -f "deploy/usr/lib/cgi-bin/links-legacy.exe" ]; then
        echo -e "${RED}❌ Built binary not found at deploy/usr/lib/cgi-bin/links-legacy.exe${NC}"
        exit 1
    fi
    
    # Make sure deploy directories exist
    mkdir -p "deploy/var/www/html/links"
    mkdir -p "deploy/usr/lib/cgi-bin"
    
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
    docker-compose -f docker-compose.debug.yml down 2>/dev/null || true
    docker-compose -f docker-compose.yml down 2>/dev/null || true
    docker-compose -f docker-compose.release.yml down 2>/dev/null || true
    
    # Generate appropriate .env file
    if [[ "$OSTYPE" == "darwin"* ]]; then
        ./scripts/mac/start-apache.sh env-only
    elif [[ "$OSTYPE" == "linux-gnu"* ]]; then
        ./scripts/linux/start-apache.sh env-only
    else
        echo -e "${YELLOW}⚠️  Platform detection failed, using universal launcher...${NC}"
        ./start-apache.sh env-only
    fi
    
    if [ "$BUILD_TYPE" = "Debug" ]; then
        echo -e "${YELLOW}🐛 Starting debug container on port 4433...${NC}"
        docker-compose -f docker-compose.debug.yml up -d
        echo -e "${GREEN}✅ Debug container started!${NC}"
        echo -e "${CYAN}🌐 Debug application available at: https://localhost:4433/cgi-bin/links-legacy.exe?;key=;ch_folder=Ok${NC}"
    else
        echo -e "${YELLOW}🚀 Starting release container on port 4444...${NC}"
        docker-compose -f docker-compose.release.yml up -d
        echo -e "${GREEN}✅ Release container started!${NC}"
        echo -e "${CYAN}🌐 Release application available at: https://localhost:4444/cgi-bin/links-legacy.exe?;key=;ch_folder=Ok${NC}"
    fi
}

# Function to check and build Apache image if needed
check_and_build_apache_image() {
    echo -e "${BLUE}🔍 Checking for Apache image...${NC}"
    
    if [ "$BUILD_TYPE" = "Debug" ]; then
        COMPOSE_FILE="docker-compose.debug.yml"
        SERVICE_NAME="links-legacy-apache-debug"
        IMAGE_TAG="links-legacy-apache:debug"
        TARGET="debug"
    else
        COMPOSE_FILE="docker-compose.release.yml"
        SERVICE_NAME="links-legacy-apache-release"
        # For release, try to determine what image name docker-compose would create
        IMAGE_TAG="links-legacy-apache:latest"
        TARGET="base"
    fi
    
    # For debug build, check the specific image tag since docker-compose.yml references it
    if [ "$BUILD_TYPE" = "Debug" ]; then
        if ! docker image inspect "$IMAGE_TAG" >/dev/null 2>&1; then
            echo -e "${YELLOW}🔧 Apache image $IMAGE_TAG not found. Building...${NC}"
            docker build --target "$TARGET" -t "$IMAGE_TAG" .
            if [ $? -ne 0 ]; then
                echo -e "${RED}❌ Failed to build $IMAGE_TAG image!${NC}"
                exit 1
            fi
            echo -e "${GREEN}✅ Apache image $IMAGE_TAG built successfully!${NC}"
        else
            echo -e "${GREEN}✅ Apache image $IMAGE_TAG already exists${NC}"
        fi
    else
        # For release, use docker-compose build to ensure correct image naming
        echo -e "${YELLOW}🔧 Building Apache release image using docker-compose...${NC}"
        docker-compose -f "$COMPOSE_FILE" build "$SERVICE_NAME"
        if [ $? -ne 0 ]; then
            echo -e "${RED}❌ Failed to build Apache release image!${NC}"
            exit 1
        fi
        echo -e "${GREEN}✅ Apache release image built successfully!${NC}"
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
