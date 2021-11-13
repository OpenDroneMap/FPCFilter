
#include <iostream>
#include <filesystem>
#include <omp.h>
#include "common.h"
#include "FPCFilter.h"
#include "parameters.h"
#include "vendor/octree.hpp"
#include "vendor/happly.hpp"

namespace fs = std::filesystem;

int main(const int argc, char** argv)
{
	std::cout << " *** FPCFilter - v" << FPCFilter_VERSION_MAJOR << "." << FPCFilter_VERSION_MINOR << " ***" << std::endl << std::endl;


	try {

		FPCFilter::Parameters parameters(argc, argv);

        std::cout << "?> Parameters:" << std::endl;
        std::cout << "\tinput = " << parameters.input << std::endl;
        std::cout << "\toutput = " << parameters.output << std::endl;
        std::cout << "\tstd = " << std::setprecision(4) << parameters.std << std::endl;
        std::cout << "\tradius = " << std::setprecision(4) << parameters.radius << std::endl;
        
		if (parameters.boundary.has_value()) 
			std::cout << "\tboundary = " << parameters.boundary.value().getPoints().size() << " polygon vertexes" << std::endl;		
		else 
			std::cout << "\tboundary = auto" << std::endl;
		
        std::cout << "\tconcurrency = " << parameters.concurrency << std::endl;
        std::cout << "\tverbose = " << (parameters.verbose ? "yes" : "no") << std::endl;
		std::cout << std::endl;

        // Delete existing output file
        if (fs::exists(parameters.output)) fs::remove(parameters.output);

		/*
		 * -> Workflow:
		 * 0) Load input
		 * 1) Crop out of boundary points
		 * 2) Sample point cloud (reduce density using 'radius' parameter)
		 * 3) Filter out outliers using statistical filter
		 * 4) Write output
		 */

		//octree::Octree<Point3f> octree;
		//octree::OctreeParams params;

		// Construct a data object by reading from file
		happly::PLYData plyIn(parameters.input);

		const auto elements = plyIn.getElementNames();

		std::cout << " ?> Mesh contents: " << std::endl << std::endl;

		for (const auto& element : elements) {
			std::cout << '\t' << element << std::endl;

			const auto properties = plyIn.getElement(element).getPropertyNames();

			for (const auto& prop : properties) {
				std::cout << "\t\t" << prop << std::endl;
			}
		}

		std::cout << std::endl;

		const std::vector<std::array<double, 3>> vertexes = plyIn.getVertexPositions();

		std::cout << " ?> Found " << vertexes.size() << " vertexes" << std::endl;

		plyIn.write(parameters.output, happly::DataFormat::Binary);

	}
	catch(const std::invalid_argument& e) {
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}
	catch(const std::exception& e) {
		std::cerr << "Error: " << e.what() << std::endl;
		return EXIT_FAILURE;
	}
	
	return EXIT_SUCCESS;
}