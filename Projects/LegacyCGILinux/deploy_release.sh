#!/bin/bash

cp /home/dmitry/VSProjects/LegacyCGILinux/bin/x64/Release/links-legacy.exe /usr/lib/cgi-bin/
# Access rights to "/var/www/html" must be provided to the user under which the script is running.
mkdir -p /var/www/html/links/legacy-images
cp -R /home/dmitry/VSProjects/LegacyCGILinux/Sources/Content/legacy-images /var/www/html/links
cp /home/dmitry/VSProjects/LegacyCGILinux/Sources/Content/Styles.css /var/www/html/links
cp /home/dmitry/VSProjects/LegacyCGILinux/Sources/Content/Common.js /var/www/html/links
