#include <gtest/gtest.h>
#include "../pipeline.hpp"
#include "testarea.h"

#define ASCII_PLY "https://raw.githubusercontent.com/DroneDB/test_data/master/point-clouds/brighton_reconstruction.ply"

// Demonstrate some basic assertions.
TEST(PlyFileTest, LoadAscii) {
  
    TestArea ta("PlyFileTest");

    ta.downloadTestAsset(ASCII_PLY, "ascii.ply");

    const auto path = (ta.getFolder() / "ascii.ply").generic_string();

    const FPCFilter::PlyFile ply(path);

    ASSERT_EQ(ply.points.size(), 8006);
    ASSERT_EQ(ply.extras.size(), 0);
  
  // Expect two strings not to be equal.
  //EXPECT_STRNE("hello", "world");
  // Expect equality.
  //EXPECT_EQ(7 * 6, 42);
}