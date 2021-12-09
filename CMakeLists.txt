cmake_minimum_required(VERSION 3.10)
project(FPCFilter VERSION 0.1 LANGUAGES CXX)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(OpenMP REQUIRED)
file(GLOB SOURCES "*.cpp")
add_executable(FPCFilter ${SOURCES})

if(OpenMP_CXX_FOUND)
    target_link_libraries(FPCFilter PUBLIC OpenMP::OpenMP_CXX)
endif()

configure_file(FPCFilter.h.in FPCFilter.h)
target_include_directories(FPCFilter PUBLIC "${PROJECT_BINARY_DIR}")
target_include_directories(FPCFilter PRIVATE "vendor")

target_compile_definitions(FPCFilter PUBLIC "$<$<CONFIG:DEBUG>:DEBUG>")

install(TARGETS FPCFilter RUNTIME DESTINATION bin)

if(BUILD_TESTING)
    add_subdirectory("test")
endif()
