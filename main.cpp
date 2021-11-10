// Your First C++ Program

#include <iostream>
#include <filesystem>
#include <omp.h>
#include "FPCFilter.h"
#include "vendor/octree.hpp"
#include "vendor/happly.hpp"
#include "vendor/json.hpp"
#include "vendor/cxxopts.hpp"

namespace fs = std::filesystem;

#define MAX 10
#define DEFAULT_STD_DEV "2.5"
#define DEFAULT_SAMPLE_RADIUS "0"

#ifdef DEBUG
#define DEFAULT_VERBOSE "true"
#else
#define DEFAULT_VERBOSE "false"
#endif


int main(const int argc, char** argv)
{
	std::cout << " *** FPCFilter - v" << FPCFilter_VERSION_MAJOR << "." << FPCFilter_VERSION_MINOR << " ***" << std::endl;

	cxxopts::Options options("FPCFilter", "Fast Point Cloud Filtering");

	options.show_positional_help();

	options.add_options()
		//("d,debug", "Enable debugging") // a bool parameter
		("i,input", "Input point cloud", cxxopts::value<std::string>())
		("o,output", "Output point cloud", cxxopts::value<std::string>())
		("s,std", "Standard deviation", cxxopts::value<double>()->default_value(DEFAULT_STD_DEV))
		("r,radius", "Sample radius", cxxopts::value<double>()->default_value(DEFAULT_SAMPLE_RADIUS))
		("c,concurrency", "Max concurrency", cxxopts::value<int>()->default_value("0"))
		("v,verbose", "Verbose output", cxxopts::value<bool>()->default_value(DEFAULT_VERBOSE));

	options.parse_positional({ "input", "output" });

	try {

		const auto result = options.parse(argc, argv);

		if (!result.count("input") || !result.count("output"))
		{
			std::cout << options.help() << std::endl;
			return 0;
		}

		std::cout << std::endl << "?> Parameters:" << std::endl;

		const auto inputFile = result["input"].as<std::string>();

		if (!fs::exists(inputFile))
		{
			std::cout << "!> Input file '" << inputFile << "' does not exist" << std::endl;
			return 0;
		}

		std::cout << "\tinput = " << inputFile << std::endl;

		const auto outputFile = result["output"].as<std::string>();

		// Delete existing output file
		if (fs::exists(outputFile)) fs::remove(outputFile);
		
		std::cout << "\toutput = " << outputFile << std::endl;

		const auto std = result["std"].as<double>();

		if (std < 0)
		{
			std::cout << "!> Standard deviation cannot be less than 0" << std::endl;
			return 0;
		}

		std::cout << "\tstd = " << std::setprecision(4) << std << std::endl;

		const auto radius = result["radius"].as<double>();

		if (radius < 0)
		{
			std::cout << "!> Radius cannot be less than 0" << std::endl;
			return 0;
		}

		std::cout << "\tradius = " << std::setprecision(4) << radius << std::endl;

		auto concurrency = result["concurrency"].as<int>();

		if (concurrency == 0)
		{
			concurrency = std::max(omp_get_num_procs(), 1);
		}
		else if (concurrency < 0)
		{
			std::cout << "!> Concurrency cannot be less than 0" << std::endl;
			return 0;
		}

		std::cout << "\tconcurrency = " << concurrency << std::endl;

		const auto verbose = result["verbose"].as<bool>();

		std::cout << "\tverbose = " << (verbose ? "yes" : "no") << std::endl;
		
		//octree::Octree<Point3f> octree;
		//octree::OctreeParams params;

		// Construct a data object by reading from file
		happly::PLYData plyIn(inputFile);

		const auto elements = plyIn.getElementNames();

		for (const auto& element : elements) {
			std::cout << element << std::endl;

			const auto properties = plyIn.getElement(element).getPropertyNames();

			for (const auto& prop : properties) {
				std::cout << '\t' << prop << std::endl;
			}
		}

		plyIn.write(outputFile, happly::DataFormat::Binary);

	}
	catch (const std::runtime_error& e) {
		std::cerr << e.what() << std::endl;
		exit(EXIT_FAILURE);
	}

	return 0;
}