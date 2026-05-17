@echo off
SETLOCAL EnableDelayedExpansion

:: --- CONFIGURATION ---
SET "BINARY_PATH=code/vip/programs/output/build/vip_system.elf"
SET "INTERFACE=interface/cmsis-dap.cfg"
SET "TARGET=target/rp2040.cfg"
:: ---------------------

echo ===================================================
echo             RP2040 AUTO FLASH SCRIPT               
echo ===================================================

:: 1. Verify the file exists before launching OpenOCD
:: (Replacing forward slashes with backslashes just for the Windows file check)
SET "WIN_PATH=%BINARY_PATH:/=\%"
if not exist "%WIN_PATH%" (
    color 0C
    echo [ERROR] Cannot find target binary at:
    echo        %WIN_PATH%
    echo.
    echo Please ensure your build succeeded before running this script.
    goto :END
)

:: 2. Run OpenOCD with the optimized reset flags to prevent the multidrop error
echo [INFO] Found binary. Initializing OpenOCD flashing...
echo.

openocd -f %INTERFACE% -f %TARGET% -c "reset_config none separate" -c "program %BINARY_PATH% verify reset exit"

echo.
echo ===================================================
echo [SUCCESS] Flash routine complete.
echo ===================================================

:END
pause