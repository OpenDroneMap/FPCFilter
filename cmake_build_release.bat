@echo off

mkdir build
cd build
cmake ..
cmake --build . --config Release --target ALL_BUILD -- /maxcpucount:14