FROM ubuntu:24.04

# Prevent interactive prompts during package installation
ENV DEBIAN_FRONTEND=noninteractive

# Install Valgrind and common C/C++ build tools
RUN apt-get update && apt-get install -y \
    valgrind \
    build-essential \
    gdb \
    cmake \
    && rm -rf /var/lib/apt/lists/*

# Set the working directory inside the container
WORKDIR /app
