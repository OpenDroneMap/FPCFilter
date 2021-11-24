#include <gtest/gtest.h>
#include "../pipeline.hpp"
#include "testarea.h"

#define ASCII_PLY "https://raw.githubusercontent.com/DroneDB/test_data/master/point-clouds/brighton_reconstruction.ply"
#define BINARY_PLY "https://github.com/DroneDB/test_data/raw/master/point-clouds/brighton_dense_input.ply"

// Demonstrate some basic assertions.
TEST(PlyFileTest, LoadAscii) {

	TestArea ta("PlyFileTest");

	const auto path = ta.downloadTestAsset(ASCII_PLY, "ascii.ply");

	const FPCFilter::PlyFile ply(path.generic_string());

	ASSERT_EQ(ply.points.size(), 8006);
	ASSERT_EQ(ply.extras.size(), 0);

}

// Demonstrate some basic assertions.
TEST(PlyFileTest, LoadBinary) {

	TestArea ta("PlyFileTest");

	const auto path = ta.downloadTestAsset(BINARY_PLY, "binary.ply");

	const FPCFilter::PlyFile ply(path.generic_string());

	ASSERT_EQ(ply.points.size(), 835777);
	ASSERT_EQ(ply.extras.size(), 835777);

}