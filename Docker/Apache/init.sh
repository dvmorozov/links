#!/bin/bash
# Ultra-minimal initialization - clean up Apache files and start Apache
rm -f /var/run/apache2/apache2.pid
rm -f /var/run/apache2/*.sock
rm -rf /var/run/apache2/socks
mkdir -p /var/run/apache2/socks

# Ensure log directory exists and has proper permissions
mkdir -p /var/log/links
chmod 777 /var/log/links

# Ensure temp directory exists and has proper permissions
mkdir -p /var/www/html/temp
chmod -R 777 /var/www/html/temp

exec "$@"
