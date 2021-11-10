@echo off

mkdir build
cd build
cmake ..
cmake --build . --config Debug --target ALL_BUILD -- /maxcpucount:14