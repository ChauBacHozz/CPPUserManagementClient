@echo off
echo Starting execution of multiple programs...

rmdir /s/q build
mkdir build

cd build
cmake .. -G "MinGW Makefiles"
mingw32-make
.\main.exe

echo Done execution
pause