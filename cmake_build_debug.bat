@echo off

mkdir build
cd build
cmake -DBUILD_TESTING=1 ..
cmake --build . --config Debug --target ALL_BUILD -- /maxcpucount:14