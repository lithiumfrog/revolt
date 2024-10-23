@echo off

:: Set up Visual Studio environment
call "C:\Program Files\Microsoft Visual Studio\2022\Preview\VC\Auxiliary\Build\vcvars64.bat" || (echo "vcvars64.bat not found" & goto :build_failed)

echo.
echo --------------------------- STARTING BUILD ---------------------------
echo.
call build-standard.bat
echo.
call build-editor.bat
echo.
call build-ut.bat
echo.
echo --------------------------- BUILD COMPLETE ---------------------------
echo.
goto :eof

:build_failed
:: Enhanced FAIL message with ASCII Art
echo.
echo  *******************************************************************************
echo  *     ______     ______     __         ______     __         ______           *
echo  *    /\  ___\   /\  ___\   /\ \       /\  __ \   /\ \       /\  ___\          *
echo  *    \ \ \____  \ \  __\   \ \ \____  \ \ \/\ \  \ \ \____  \ \___  \         *
echo  *     \ \_____\  \ \_____\  \ \_____\  \ \_____\  \ \_____\  \/\_____\        *
echo  *      \/_____/   \/_____/   \/_____/   \/_____/   \/_____/   \/_____/        *
echo  *                                                                           *
echo  *                     BUILD FAILED - CHECK ERRORS ABOVE                     *
echo  *******************************************************************************
echo.
echo "The build failed! Please check the logs for more details."
goto :eof
