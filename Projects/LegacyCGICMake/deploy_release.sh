#!/bin/bash

# This script is used to deploy the project for release.

DEPLOY_DIR="$1"
SOURCE_BIN="links-legacy.exe"  # Binary built in current directory
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

if [ -f "$SOURCE_BIN" ]; then
	cp "$SOURCE_BIN" "$CGI_BIN_DIR/links-legacy-release.exe"
	echo "Deployed release binary: $SOURCE_BIN -> $CGI_BIN_DIR/links-legacy-release.exe"
else
    echo "Error: $SOURCE_BIN not found."
    exit 1
fi

# Access rights to "/var/www/html" must be provided to the user under which the script is running.
mkdir -p "$TARGET_DIR/legacy-images"

if [ -d ../../Sources/Content/legacy-images ]; then
	cp -R ../../Sources/Content/legacy-images "$TARGET_DIR"
else
	echo "Error: ../../Sources/Content/legacy-images not found."
	exit 1
fi

if [ -f ../../Sources/Content/Styles.css ]; then
    cp ../../Sources/Content/Styles.css "$TARGET_DIR"
else
    echo "Error: ../../Sources/Content/Styles.css not found."
    exit 1
fi

if [ -f ../../Sources/Content/Common.js ]; then
    cp ../../Sources/Content/Common.js "$TARGET_DIR"
else
    echo "Error: ../../Sources/Content/Common.js not found."
    exit 1
fi
