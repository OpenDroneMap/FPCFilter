
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

file(GLOB TEST_SOURCES "*.cpp")

enable_testing()

add_executable(
  fpcfilter_test
  ${TEST_SOURCES}
)

set_target_properties(fpcfilter_test PROPERTIES CXX_STANDARD 17)

set(EXE_EXT ".run")
if (WIN32)
    set(EXE_EXT ".exe")
endif()

target_link_libraries(
  fpcfilter_test
  gtest_main
)

add_subdirectory(vendor/curly)
target_link_libraries(fpcfilter_test curly.hpp)

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../vendor")

#target_link_libraries(fpcfilter_test curly.hpp)

include(GoogleTest)
gtest_discover_tests(fpcfilter_test)