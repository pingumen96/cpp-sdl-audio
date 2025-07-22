@echo off
REM Script di test locale per il progetto cpp-sdl-audio
REM Simula l'ambiente CI per test locali

echo 🏗️  Building project...
cmake --build build --config Release
if %ERRORLEVEL% neq 0 (
    echo ❌ Build failed!
    exit /b 1
)

echo 🧪 Building tests...
cmake --build build --target sdl_appTests
if %ERRORLEVEL% neq 0 (
    echo ❌ Test build failed!
    exit /b 1
)

echo 🚀 Running tests...
.\build\sdl_appTests.exe --reporter=console
if %ERRORLEVEL% neq 0 (
    echo ❌ Tests failed!
    exit /b 1
)

echo ✅ All tests completed successfully!
echo.
echo 📊 Test Summary:
.\build\sdl_appTests.exe --reporter=compact | findstr "assertions"

echo.
echo 🎉 Ready for GitHub push!
