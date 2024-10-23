@echo off
setlocal

:: root dir
cd /d %~dp0..\..
@REM echo dir = %cd%

:: bin dir
if not exist bin mkdir bin
if not exist bin\windows_x86-64 mkdir bin\windows_x86-64
if not exist bin\windows_x86-64\int-ut mkdir bin\windows_x86-64\int-ut

:: delete intermediates
del /q bin\windows_x86-64\int-ut\*.obj

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
                 -DEBUG ^
                 user32.lib ^
                 gdi32.lib ^
                 OpenGL32.lib ^
                 bin\windows_x86-64\standard.lib

:: compile
echo -- ut --
for /r src\ut %%f in (*.cpp) do (
    cl /c %COMPILER_FLAGS% /Fd"bin\windows_x86-64\int-ut\vc.pdb" /Fo"bin\windows_x86-64\int-ut\\" %%f
    if %ERRORLEVEL% NEQ 0 (
        echo "Error: Compilation failed for %%f"
        exit /b 1
    )
)

:: link
link /nologo /OUT:bin\windows_x86-64\ut.exe bin\windows_x86-64\int-ut\*.obj %LINKER_FLAGS%
if %ERRORLEVEL% NEQ 0 (
    echo "Error: Linking failed."
    exit /b 1
) else ( echo "ut.exe" )

:: clean up
endlocal
exit /b 0
