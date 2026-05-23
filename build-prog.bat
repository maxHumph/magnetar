@ECHO OFF
REM Build project

ECHO "Building full project.."

PUSHD engine
CALL build.bat
POPD
IF %ERRORLEVEL% NEQ 0 (echo Error:%ERROR_LEVEL% && exit)

PUSHD test
CALL build.bat
POPD
IF %ERRORLEVEL% NEQ 0 (echo Error:%ERROR_LEVEL% && exit)

ECHO "Build successful."
