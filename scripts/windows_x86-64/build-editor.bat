@echo off
setlocal

:: root dir
cd /d %~dp0..\..
@REM echo dir = %cd%

:: bin dir
if not exist bin mkdir bin
if not exist bin\windows_x86-64 mkdir bin\windows_x86-64
if not exist bin\windows_x86-64\int-editor mkdir bin\windows_x86-64\int-editor

:: delete intermediates
del /q bin\windows_x86-64\int-editor\*.obj

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
                    -Isrc\standard

set LINKER_FLAGS=-subsystem:console ^
                 -incremental:no ^
                 -opt:ref ^
                 user32.lib ^
                 gdi32.lib ^
                 OpenGL32.lib ^
                 bin\windows_x86-64\standard.lib

:: compile
echo -- editor --
for /r src\editor %%f in (*.cpp) do (
    cl /c %COMPILER_FLAGS% /Fd"bin\windows_x86-64\int-editor\vc.pdb" /Fo"bin\windows_x86-64\int-editor\\" %%f
    if %ERRORLEVEL% NEQ 0 (
        echo "Error: Compilation failed for %%f"
        exit /b 1
    )
)

:: link
link /nologo /OUT:bin\windows_x86-64\editor.exe bin\windows_x86-64\int-editor\*.obj %LINKER_FLAGS%
if %ERRORLEVEL% NEQ 0 (
    echo "Error: Linking failed."
    exit /b 1
) else ( echo "editor.exe" )

:: clean up
endlocal
exit /b 0
