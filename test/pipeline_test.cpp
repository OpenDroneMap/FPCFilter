#include <gtest/gtest.h>
#include "../pipeline.hpp"
#include "testarea.h"

#define ASCII_PLY "https://raw.githubusercontent.com/DroneDB/test_data/master/point-clouds/brighton_reconstruction.ply"
#define BINARY_PLY "https://github.com/DroneDB/test_data/raw/master/point-clouds/brighton_dense_input.ply"

#define ABS_ERROR 0.00001

// Demonstrate some basic assertions.
TEST(PlyFileTest, LoadAscii) {

	TestArea ta("PlyFileTest");

	const auto path = ta.downloadTestAsset(ASCII_PLY, "ascii.ply");

	const FPCFilter::PlyFile ply(path.generic_string());

	ASSERT_EQ(ply.points.size(), 8006);
	ASSERT_EQ(ply.extras.size(), 0);

	EXPECT_NEAR(ply.points[0].x, 14.5421934, ABS_ERROR);
	EXPECT_NEAR(ply.points[0].y, 12.4072504, ABS_ERROR);
	EXPECT_NEAR(ply.points[0].z, 163.061676, ABS_ERROR);
	ASSERT_EQ(ply.points[0].red, 91);
	ASSERT_EQ(ply.points[0].green, 105);
	ASSERT_EQ(ply.points[0].blue, 44);
	ASSERT_EQ(ply.points[0].views, 6);

	EXPECT_NEAR(ply.points[1].x, 16.865696763391075, ABS_ERROR);
	EXPECT_NEAR(ply.points[1].y, 18.324702430434996, ABS_ERROR);
	EXPECT_NEAR(ply.points[1].z, 163.01406518646994, ABS_ERROR);
	ASSERT_EQ(ply.points[1].red, 69);
	ASSERT_EQ(ply.points[1].green, 56);
	ASSERT_EQ(ply.points[1].blue, 47);
	ASSERT_EQ(ply.points[1].views, 2);

	EXPECT_NEAR(ply.points[2].x, -26.34021437480597, ABS_ERROR);
	EXPECT_NEAR(ply.points[2].y, -20.93062454025772, ABS_ERROR);
	EXPECT_NEAR(ply.points[2].z, 162.80065028537288, ABS_ERROR);
	ASSERT_EQ(ply.points[2].red, 58);
	ASSERT_EQ(ply.points[2].green, 48);
	ASSERT_EQ(ply.points[2].blue, 79);
	ASSERT_EQ(ply.points[2].views, 2);

}

// Demonstrate some basic assertions.
TEST(PlyFileTest, LoadBinary) {

	TestArea ta("PlyFileTest");

	const auto path = ta.downloadTestAsset(BINARY_PLY, "binary.ply");

	const FPCFilter::PlyFile ply(path.generic_string());

	ASSERT_EQ(ply.points.size(), 835777);
	ASSERT_EQ(ply.extras.size(), 835777);

	EXPECT_NEAR(ply.points[0].x, 9.66503811, ABS_ERROR);
	EXPECT_NEAR(ply.points[0].y, 23.4589748, ABS_ERROR);
	EXPECT_NEAR(ply.points[0].z, 163.194290, ABS_ERROR);
	ASSERT_EQ(ply.points[0].red, 30);
	ASSERT_EQ(ply.points[0].green, 41);
	ASSERT_EQ(ply.points[0].blue, 68);
	ASSERT_EQ(ply.points[0].views, 3);
	EXPECT_NEAR(ply.extras[0].nx, -0.0256650653, ABS_ERROR);
	EXPECT_NEAR(ply.extras[0].ny, -0.160922706, ABS_ERROR);
	EXPECT_NEAR(ply.extras[0].nz, 0.986633241, ABS_ERROR);


	EXPECT_NEAR(ply.points[1].x, 9.70740223, ABS_ERROR);
	EXPECT_NEAR(ply.points[1].y, 23.4105225, ABS_ERROR);
	EXPECT_NEAR(ply.points[1].z, 163.194122, ABS_ERROR);
	ASSERT_EQ(ply.points[1].red, 29);
	ASSERT_EQ(ply.points[1].green, 41);
	ASSERT_EQ(ply.points[1].blue, 67);
	ASSERT_EQ(ply.points[1].views, 3);
	EXPECT_NEAR(ply.extras[1].nx, 0.0179924276, ABS_ERROR);
	EXPECT_NEAR(ply.extras[1].ny, -0.192628577, ABS_ERROR);
	EXPECT_NEAR(ply.extras[1].nz, 0.981106758, ABS_ERROR);

	EXPECT_NEAR(ply.points[2].x, 9.75507355, ABS_ERROR);
	EXPECT_NEAR(ply.points[2].y, 23.3601894, ABS_ERROR);
	EXPECT_NEAR(ply.points[2].z, 163.211090, ABS_ERROR);
	ASSERT_EQ(ply.points[2].red, 29);
	ASSERT_EQ(ply.points[2].green, 42);
	ASSERT_EQ(ply.points[2].blue, 67);
	ASSERT_EQ(ply.points[2].views, 3);
	EXPECT_NEAR(ply.extras[2].nx, -0.0121546201, ABS_ERROR);
	EXPECT_NEAR(ply.extras[2].ny, 0.0412164144, ABS_ERROR);
	EXPECT_NEAR(ply.extras[2].nz, 0.999076307, ABS_ERROR);

}