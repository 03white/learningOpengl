@echo off
setlocal enabledelayedexpansion
cd /d "%~dp0"

rem Template subproject (its include/lib/CMakeLists.txt/main.cpp are copied)
set "TEMPLATE=gl-sample"

set "PROJ="
set /p "PROJ=Enter new project name (e.g. gl-02): "

if "!PROJ!"=="" (
    echo [ERROR] Project name cannot be empty.
    goto :end
)

if exist "!PROJ!" (
    echo [ERROR] Directory "!PROJ!" already exists. Aborting.
    goto :end
)

if not exist "%TEMPLATE%" (
    echo [ERROR] Template "%TEMPLATE%" not found.
    goto :end
)

echo.
echo Creating subproject "!PROJ!" ...

rem Copy template contents (skip build folder by copying items individually)
mkdir "!PROJ!"
xcopy "%TEMPLATE%\include" "!PROJ!\include\" /E /I /Q >nul
xcopy "%TEMPLATE%\lib"     "!PROJ!\lib\"     /E /I /Q >nul
copy  "%TEMPLATE%\CMakeLists.txt" "!PROJ!\CMakeLists.txt" >nul
copy  "%TEMPLATE%\main.cpp"       "!PROJ!\main.cpp"       >nul

echo   - Copied include / lib / CMakeLists.txt / main.cpp
echo.
echo Done. Subproject "!PROJ!" created.
echo Remember to add: add_subdirectory(!PROJ!)  to the root CMakeLists.txt
echo Then reconfigure: cmake -S . -B build -G Ninja

:end
endlocal
pause
