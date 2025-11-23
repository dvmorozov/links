# Stage 1: Base image with Apache and OpenSSL
# Step 1: Set the base image
FROM ubuntu:24.04 AS base

# Step 2: Set labels
LABEL version=latest
LABEL name=links-legacy-apache
LABEL org.opencontainers.image.version=24.04
LABEL org.opencontainers.image.ref.name=ubuntu

# Step 3: Set build arguments (these are often used to define environment-specific configurations)
ARG LAUNCHPAD_BUILD_ARCH
ARG RELEASE
ARG USER_UID=1000
ARG USER_GID=1000

# Step 4: Install required packages and clean up
RUN apt-get update && \
    apt-get install -y apache2 openssl && \
    apt-get clean

# Step 5: Add users and groups with matching UID/GID from host
# Delete existing ubuntu user/group if they conflict, then create dmitry user
RUN (getent group ${USER_GID} && groupdel $(getent group ${USER_GID} | cut -d: -f1) || true) && \
    (getent passwd ${USER_UID} && userdel -r $(getent passwd ${USER_UID} | cut -d: -f1) || true) && \
    groupadd -g ${USER_GID} dmitry && \
    useradd -u ${USER_UID} -g dmitry -m dmitry && \
    echo "User ID: $(id -u dmitry)" && \
    echo "Group ID: $(id -g dmitry)"

# Step 6: Configure Apache
RUN a2enmod cgid && \
    a2enmod ssl && \
    a2ensite default-ssl && \
    a2enconf serve-cgi-bin && \
    echo "ServerName localhost" >> /etc/apache2/apache2.conf && \
    touch /usr/lib/cgi-bin/links-legacy.exe && \
    chmod +x /usr/lib/cgi-bin/links-legacy.exe

# Step 7: Copy configuration files
COPY apache2.conf /etc/apache2/apache2.conf
COPY sites-available/default-ssl.conf /etc/apache2/sites-available/default-ssl.conf
COPY sites-enabled/default-ssl.conf /etc/apache2/sites-enabled/default-ssl.conf

# Step 8: Expose port for Apache
EXPOSE 443/tcp

# Step 9: Set the default command to run Apache in the foreground
CMD ["apachectl", "-D", "FOREGROUND"]

# Stage 2: Debug image based on the base image
FROM base AS debug

# Install additional packages for debugging C++ CGI module
RUN apt-get update && \
    apt-get install -y gdb gdbserver build-essential cmake curl && \
    apt-get clean

# Expose port 443 for HTTPS
EXPOSE 443

# Start Apache in the foreground
CMD ["apachectl", "-D", "FOREGROUND"]