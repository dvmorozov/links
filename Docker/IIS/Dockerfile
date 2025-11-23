# Use the official Windows Server Core image with IIS
FROM mcr.microsoft.com/windows/servercore:ltsc2019

# Install IIS
RUN powershell -Command \
    Add-WindowsFeature Web-Server

# Remove default website
RUN powershell -Command \
    Remove-Item -Recurse C:\inetpub\wwwroot\*

# Copy content to the IIS root directory
# COPY ./wwwroot /inetpub/wwwroot

# Expose port 80
EXPOSE 80

# Start IIS
CMD ["powershell", "Start-Service", "w3svc", ";", "while ($true) { Start-Sleep -Seconds 3600 }"]
