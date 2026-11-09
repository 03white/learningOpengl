@echo off
setlocal enabledelayedexpansion
cd /d "%~dp0"

rem ============================================================
rem  Subproject generator based on a selected template project.
rem
rem  Repository layout:
rem    lessions\gl-xx    LearnOpenGL tutorial chapters
rem    projects\<name>   own practice projects
rem
rem  The template CMakeLists.txt derives its target name from the
rem  directory name (get_filename_component ... NAME), so no file
rem  content needs rewriting -- copying the folder is enough.
rem  Resource paths come from ASSET_DIR (= CMAKE_CURRENT_SOURCE_DIR),
rem  so they follow the copy automatically as well.
rem  This script copies the full template and registers the new
rem  subproject in its group's CMakeLists.txt.
rem ============================================================

set "TEMPLATE="
set /p "TEMPLATE=Enter template project name (e.g. gl-05): "

if "!TEMPLATE!"=="" (
    echo [ERROR] Template project name cannot be empty.
    goto :end
)

rem --- Locate the template under lessions\ then projects\ ---
set "TEMPLATE_DIR="
if exist "lessions\!TEMPLATE!\" set "TEMPLATE_DIR=lessions\!TEMPLATE!"
if not defined TEMPLATE_DIR if exist "projects\!TEMPLATE!\" set "TEMPLATE_DIR=projects\!TEMPLATE!"

if not defined TEMPLATE_DIR (
    echo [ERROR] Template "!TEMPLATE!" not found under lessions\ or projects\.
    goto :end
)
echo   - Using template "!TEMPLATE_DIR!"

set "GROUP="
set /p "GROUP=Target group [lessions/projects] (default: projects): "
if "!GROUP!"=="" set "GROUP=projects"

if /I not "!GROUP!"=="lessions" if /I not "!GROUP!"=="projects" (
    echo [ERROR] Target group must be "lessions" or "projects".
    goto :end
)

set "PROJ="
set /p "PROJ=Enter new project name (e.g. gl-06): "

if "!PROJ!"=="" (
    echo [ERROR] Project name cannot be empty.
    goto :end
)

set "DEST=!GROUP!\!PROJ!"

if exist "!DEST!" (
    echo [ERROR] Directory "!DEST!" already exists. Aborting.
    goto :end
)

echo.
echo Creating subproject "!DEST!" from template "!TEMPLATE_DIR!" ...

rem --- Copy the full template project directory ---
xcopy "!TEMPLATE_DIR!\*" "!DEST!\" /E /I /Q /H /Y >nul
if errorlevel 1 (
    echo [ERROR] Failed to copy template project "!TEMPLATE_DIR!".
    if exist "!DEST!\" rmdir /S /Q "!DEST!"
    goto :end
)
echo   - Copied project files

rem --- Register in the group's CMakeLists.txt (dedup + newline-safe) ---
set "GROUP_CML=!GROUP!\CMakeLists.txt"
if exist "!GROUP_CML!" (
    powershell -NoProfile -Command "$p='!PROJ!'; $f='!GROUP_CML!'; $c=Get-Content -Raw $f; $line='add_subdirectory('+$p+')'; if($c -match [regex]::Escape($line)){ Write-Host ('  - '+$f+' already references '+$p+', skipped') } else { if($c -notmatch '(\r?\n)$'){ $c+=[Environment]::NewLine }; $c+=$line+[Environment]::NewLine; Set-Content -NoNewline -Encoding UTF8 $f $c; Write-Host ('  - Registered add_subdirectory('+$p+') in '+$f) }"
) else (
    echo   [WARN] !GROUP_CML! not found. Add manually: add_subdirectory(!PROJ!)
)

echo.
echo Done. Subproject "!DEST!" created.
echo Reconfigure: cmake -S . -B build

:end
endlocal
pause
