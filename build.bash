#!/usr/bin/env bash

set -e  # Exit immediately if a command fails

echo -e "\n🔧 Starting Build Process..."

BUILD_DIR="build"

# Create build directory if it doesn't exist
if [ ! -d "$BUILD_DIR" ]; then
    echo "📁 Creating build directory..."
    mkdir "$BUILD_DIR"
fi

cd "$BUILD_DIR"

echo "⚙️  Running CMake..."
cmake .. -G "MinGW Makefiles"

echo "🔨 Running mingw32-make..."
mingw32-make

echo "🚀 Running the game..."
./GateSimulator.exe
