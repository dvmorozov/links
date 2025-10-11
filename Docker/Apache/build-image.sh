#!/bin/bash

# Simple launcher for build-image.sh script
# Forwards all arguments to the actual build script

# Change to script directory and execute the build script
cd "$(dirname "$0")"
exec "./scripts/build-image.sh" "$@"
