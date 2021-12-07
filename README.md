# FPCFilter
Fast Point Cloud Filtering

## Building

Requirements:
 * OpenMP
 * cmake
 * g++
 
```bash
git clone https://github.com/OpenDroneMap/FPCFilter.git
cd FPCFilter
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release .. && make -j$(nproc)
```

On Windows you should install Visual Studio (the free Community Edition works great), Git and CMake. Then:

```
git clone https://github.com/OpenDroneMap/FPCFilter.git
cd FPCFilter
md build && cd build
cmake ..
cmake --build . --config Release --target ALL_BUILD -- /maxcpucount:14
```