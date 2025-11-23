#!/bin/bash
# Universal launcher script for links-legacy-apache container
# Automatically detects platform and uses appropriate script

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# Detect platform
if [[ "$OSTYPE" == "darwin"* ]]; then
    PLATFORM="mac"
    echo "🍎 Detected macOS"
elif [[ "$OSTYPE" == "linux-gnu"* ]]; then
    PLATFORM="linux"
    echo "🐧 Detected Linux"
else
    echo "❌ Unsupported platform: $OSTYPE"
    echo "Please use the platform-specific scripts in scripts/ directory"
    exit 1
fi

# Execute platform-specific script
PLATFORM_SCRIPT="$SCRIPT_DIR/scripts/$PLATFORM/start-apache.sh"

if [ -f "$PLATFORM_SCRIPT" ]; then
    echo "🚀 Launching platform-specific script..."
    exec "$PLATFORM_SCRIPT" "$@"
else
    echo "❌ Platform script not found: $PLATFORM_SCRIPT"
    exit 1
fi
