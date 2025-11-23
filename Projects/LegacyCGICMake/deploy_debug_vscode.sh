#!/bin/bash

# This script is used to deploy the project when it is built in Docker container.

# Check if the script is run as root
if [ "$EUID" -ne 0 ]; then
    echo "Please run as root"
    exit 1
fi

# Check if the deploy directory is passed as a parameter
if [ -z "$1" ]; then
    echo "Usage: $0 <deploy_directory>"
    exit 1
fi

DEPLOY_DIR="$1"
TARGET_DIR="$DEPLOY_DIR/var/www/html/links"
CGI_BIN_DIR="$DEPLOY_DIR/usr/lib/cgi-bin"
TEMP_DIR="$DEPLOY_DIR/var/www/html/temp/links"

# Check if TARGET_DIR exists, create if necessary
if [ ! -d "$TARGET_DIR" ]; then
    mkdir -p "$TARGET_DIR"
fi

# Check if CGI_BIN_DIR exists, create if necessary
if [ ! -d "$CGI_BIN_DIR" ]; then
    mkdir -p "$CGI_BIN_DIR"
fi

# Check if TEMP_DIR exists, create if necessary
if [ ! -d "$TEMP_DIR" ]; then
    mkdir -p "$TEMP_DIR"
    echo "Created temp directory: $TEMP_DIR"
fi

# Set proper permissions for temp directory
# Make it writable by the web server (dmitry user)
chown -R dmitry:dmitry "$TEMP_DIR"
chmod -R 755 "$TEMP_DIR"
echo "Set permissions for temp directory: $TEMP_DIR (owner: dmitry)"

if [ -f "links-legacy.exe" ]; then
	cp "links-legacy.exe" "$CGI_BIN_DIR/links-legacy-debug.exe"
	echo "Deployed debug binary: links-legacy.exe -> $CGI_BIN_DIR/links-legacy-debug.exe"
else
    echo "Error: links-legacy.exe not found."
    exit 1
fi

# Access rights to "/var/www/html" must be provided to the user under which the script is running.
mkdir -p "$TARGET_DIR/legacy-images"

if [ -d /mnt/links/Sources/Content/legacy-images ]; then
	cp -R /mnt/links/Sources/Content/legacy-images "$TARGET_DIR"
else
	echo "Error: /mnt/links/Sources/Content/legacy-images not found."
	exit 1
fi

if [ -f /mnt/links/Sources/Content/Styles.css ]; then
    cp /mnt/links/Sources/Content/Styles.css "$TARGET_DIR"
else
    echo "Error: /mnt/links/Sources/Content/Styles.css not found."
    exit 1
fi

if [ -f /mnt/links/Sources/Content/Common.js ]; then
    cp /mnt/links/Sources/Content/Common.js "$TARGET_DIR"
else
    echo "Error: /mnt/links/Sources/Content/Common.js not found."
    exit 1
fi
