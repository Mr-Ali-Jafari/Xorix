#!/bin/bash
set -e

echo "Updating package lists..."
sudo apt-get update

echo "Installing dependencies for 32-bit kernel compilation..."
sudo apt-get install -y build-essential gcc-multilib g++-multilib nasm binutils grub2-common grub-pc-bin xorriso

echo "All dependencies installed successfully!"
