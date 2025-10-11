# Use the official Ubuntu 24.04 as a base image
FROM ubuntu:24.04

# Update the package list and install necessary packages
RUN apt-get update && \
    apt-get install -y \
    build-essential \
    cmake \
    gcc \
    g++ \
    openssh-server \
    npm

# Install dependencies for VS Code server
RUN apt-get install -y \
    curl \
    wget \
    libx11-xcb1 \
    libxcomposite1 \
    libxcursor1 \
    libxdamage1 \
    libxi6 \
    libxtst6 \
    libnss3 \
    libxrandr2 \
    libasound2-data \
    libasound2-dev \
    libpangocairo-1.0-0 \
    libatk1.0-0 \
    libatk-bridge2.0-0 \
    libgtk-3-0 \
    libxss1 \
    libgbm1 \
    libxshmfence1 \
    libglu1-mesa && \
    apt-get clean

# Install basic development tools
RUN apt-get install -y \
    wget \
    curl && \
    apt-get clean

# Create a directory for the SSH daemon
RUN mkdir /var/run/sshd

# Set up passwordless SSH for root (not recommended for production)
RUN echo 'root:root' | chpasswd

# Allow root login via SSH
RUN sed -i 's/#PermitRootLogin prohibit-password/PermitRootLogin yes/' /etc/ssh/sshd_config

# Allow password authentication
RUN sed -i 's/#PasswordAuthentication yes/PasswordAuthentication yes/' /etc/ssh/sshd_config

# Expose port 22 for SSH
EXPOSE 22

# Create /mnt/links directory
RUN mkdir -p /mnt/links

# Start the SSH daemon
CMD ["/usr/sbin/sshd", "-D"]
