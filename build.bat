@echo off

call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat" x64
mkdir build
pushd build

cl -Fe:fl_server_proxy.exe -MT -DFL_LOG_EXTRA -DFL_DEBUG -DFL_WIN32 -DFL_DEBUG_ERRORS_ONLY -Zi ..\src\*.c user32.lib kernel32.lib ws2_32.lib

popd
