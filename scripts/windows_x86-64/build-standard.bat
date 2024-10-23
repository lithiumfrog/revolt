@echo off
setlocal

:: root dir
cd /d %~dp0..\..
@REM echo dir = %cd%

:: bin dir
if not exist bin mkdir bin
if not exist bin\windows_x86-64 mkdir bin\windows_x86-64
if not exist bin\windows_x86-64\int-standard mkdir bin\windows_x86-64\int-standard

:: delete intermediates
del /q bin\windows_x86-64\int-standard\*.obj

:: flags
set COMPILER_FLAGS=-std:c++latest ^
                    /nologo ^
                    -WX ^
                    -W4 ^
                    -EHa ^
                    -Zi ^
                    -wd4100 ^
                    -wd4189 ^
                    -D_CRT_SECURE_NO_WARNINGS ^
                    -DSTANDARD_PLATFORM_WINDOWS=1 ^
                    -DSTANDARD_DEBUG_ASSERTS=1 ^
                    -DSTANDARD_DEBUG_BASE_ADDRESS_FOR_ALLOCATIONS=1 ^
                    -DSTANDARD_DEBUG_PROFILING=1 ^
                    -DSTANDARD_DEBUG_FILE_LINE_TRACKING=1 ^
                    -Isrc\standard ^
                    -Isrc\vendor\khronos

set LINKER_FLAGS=-subsystem:console ^
                 -incremental:no ^
                 -opt:ref ^
                 -DEBUG ^
                 user32.lib ^
                 gdi32.lib ^
                 OpenGL32.lib

:: compile
echo -- standard --
for /r src\standard %%f in (*.cpp) do (
    cl /c %COMPILER_FLAGS% /Fd"bin\windows_x86-64\int-standard\vc.pdb" /Fo"bin\windows_x86-64\int-standard\\" %%f
)

:: lib
lib /nologo /OUT:bin\windows_x86-64\standard.lib bin\windows_x86-64\int-standard\*.obj
if %ERRORLEVEL% EQU 0 ( echo "standard.lib" )

:: clean up
endlocal
exit /b 0
