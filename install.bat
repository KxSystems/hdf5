@echo off

SETLOCAL
IF NOT DEFINED Q_SHARED_LIB_DIR (
    IF NOT DEFINED QHOME (
        ECHO ERROR: Environment variable QHOME is NOT defined
        EXIT /B
    )
    SET Q_SHARED_LIB_DIR=%QHOME%\w64
)
IF NOT DEFINED Q_SCRIPT_DIR (
    IF NOT DEFINED QHOME (
        ECHO ERROR: Environment variable QHOME is NOT defined
        EXIT /B
    )
    SET Q_SCRIPT_DIR=%QHOME%
)

IF NOT EXIST %Q_SHARED_LIB_DIR% (
    ECHO ERROR: Library installation destination %Q_SHARED_LIB_DIR% does not exist
    EXIT /B
)

IF NOT EXIST %Q_SCRIPT_DIR% (
    ECHO ERROR: Script installation destination %Q_SCRIPT_DIR% does not exist
    EXIT /B
)

IF EXIST q (
    ECHO Copying q script to %Q_SCRIPT_DIR%
    COPY q\* %Q_SCRIPT_DIR%
    IF %ERRORLEVEL% NEQ 0 (
        ECHO ERROR: Copy failed
        EXIT /B %ERRORLEVEL%
    )
)

IF EXIST lib (
    ECHO Copying DLL to %Q_SHARED_LIB_DIR%
    COPY lib\* %Q_SHARED_LIB_DIR%
    IF %ERRORLEVEL% NEQ 0 (
        ECHO ERROR: Copy failed
        EXIT /B %ERRORLEVEL%
    )
)

ECHO Installation complete
ENDLOCAL
