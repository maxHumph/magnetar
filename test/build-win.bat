REM Test build script
@ECHO OFF
SetLocal EnableDelayedExtension

REM Glob c files
SET cFiles=
FOR /R %%f in (*.c) do (
    SET cFiles=!cFiles! %%f
)

REM echo "Files:" %cFiles%

SET assembly=test
SET compilerFlags=-g
REM -Wall -Werror
SET includeFlags=-Isrc -I../engine/src/
SET linkerFlags=-L../bin/ -lengine.lib
SET defines=-D_DEBUG -DMIMPORT

ECHO "Building %assembly%%..."
clang %cFiles% %compilerFlags% -o ../bin/%assembly%.exe %defines% %includeFlags% %linkerFlags%
