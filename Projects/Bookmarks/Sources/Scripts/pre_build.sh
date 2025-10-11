#!/bin/bash

# Update package list
sudo apt-get update

# Check if libxml2-dev is installed
if ! dpkg -l | grep -q libxml2-dev; then
    echo "libxml2-dev is not installed. Installing..."
    sudo apt-get install -y libxml2-dev
else
    echo "libxml2-dev is already installed."
fi

# Check if libssl-dev is installed
if ! dpkg -l | grep -q libssl-dev; then
    echo "libssl-dev is not installed. Installing..."
    sudo apt-get install -y libssl-dev
else
    echo "libssl-dev is already installed."
fi
