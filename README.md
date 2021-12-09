# FPCFilter (Fast Point Cloud Filtering)

**FPCFilter** performs 3 types of processes: Crop, Sample and Filter. 

Here are the input parameters:

```
  -i, --input arg        Input point cloud
  -o, --output arg       Output point cloud
  -b, --boundary arg     Crop boundary (GeoJSON POLYGON)
  -s, --std arg          Standard deviation threshold
  -m, --meank arg        Mean number of neighbors
  -r, --radius arg       Sample radius
  -c, --concurrency arg  Max concurrency
  -v, --verbose          Verbose output
```

The relevant parameters for each process are:

- Crop: `-b, --boundary` 
- Sample: `-r, --radius`
- Filter: `-s, --std` and `-m, --meank`

The programs works like a PDAL pipeline: 

`load -> crop -> sample -> filter -> write` 

It will skip the stages not requested by the user

See PDAL documentation for more details: 
- Crop: http://pdal.io/stages/filters.crop.html#filters-crop
- Sample: http://pdal.io/stages/filters.sample.html#filters-sample
- Filter: http://pdal.io/stages/filters.outlier.html#statistical-method

-----------------------------------------------------------------------

It supports [PLY point clouds](https://en.wikipedia.org/wiki/PLY_(file_format)) in the following formats:

`binary little endian` with header:

```
property float32 x
property float32 y
property float32 z
property uint8 red
property uint8 green
property uint8 blue
property float32 nx
property float32 ny
property float32 nz
property uint8 views
end_header
```

and `ascii` with header

```
property float x
property float y
property float z
property uchar diffuse_red
property uchar diffuse_green
property uchar diffuse_blue
property uchar views
end_header
```

**FPCFilter** outputs a `binary little endian` PLY with the following header if the source has the normals (`nx`, `ny`, `nz`):

```
property float x
property float y
property float z
property float nx
property float ny
property float nz			
property uchar red
property uchar blue
property uchar green
property uchar views
end_header
```

without `nx`, `ny` and `nz` if the source file has not got them.

-----------------------------------------------------------------------

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

In order to build the tests call cmake with `-DBUILD_TESTING=1` and `-DCMAKE_BUILD_TYPE=Debug`

## Docker

Build the image with:

```bash
docker build . -t opendronemap/fpcfilter
```

Run the container with:

```bash
docker run -it -v "${PWD}":/data --rm opendronemap/fpcfilter -i /data/input.ply -o /data/out.ply
```

`PWD` is the current directory, change it accordingly to your needs. 

Use the image as the executable itself, call it without parameters to see the help.