#!/bin/bash

# Script di test locale per il progetto cpp-sdl-audio
# Simula l'ambiente CI per test locali

set -e

echo "🏗️  Building project..."
cmake --build build --config Release

echo "🧪 Building tests..."
cmake --build build --target sdl_appTests

echo "🚀 Running tests..."
./build/sdl_appTests --reporter=console

echo "✅ All tests completed successfully!"
echo ""
echo "📊 Test Summary:"
./build/sdl_appTests --list-test-names-only | wc -l | xargs echo "  - Test cases:"
./build/sdl_appTests --reporter=compact | grep "assertions" | tail -1

echo ""
echo "🎉 Ready for GitHub push!"
