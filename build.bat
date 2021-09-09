@echo off

call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat" x64
mkdir build
pushd build

cl -DFL_DEBUG -DFL_WIN32 -Zi ..\src\*.c user32.lib kernel32.lib ws2_32.lib

popd
