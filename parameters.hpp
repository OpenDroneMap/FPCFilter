#pragma once

#include <iostream>
#include <fstream>
#include <filesystem>
#include <omp.h>
#include "common.hpp"
#include "vendor/cxxopts.hpp"
#include "utils.hpp"
#include "vendor/json.hpp"

#define DEFAULT_STD_DEV "2.5"
#define DEFAULT_MEANK "16"
#define DEFAULT_SAMPLE_RADIUS "0"

namespace fs = std::filesystem;

namespace FPCFilter
{

	class Parameters
	{

		std::optional<Polygon> extractPolygon(const std::string& boundary)
		{

			Polygon polygon;

			std::ifstream i(boundary);
			nlohmann::json j;
			i >> j;

			const auto features = j["features"];

			if (features.empty())
				return std::nullopt;

			for (const auto& f : features)
			{
				const auto geometry = f["geometry"];

				if (geometry["type"] != "Polygon")
					continue;

				const auto coordinates = geometry["coordinates"][0];

				// Add the points
				for (auto& coord : coordinates)
					polygon.addPoint(coord[0], coord[1]);

				return polygon;
			}

			return std::nullopt;
		}

	public:
		std::string input;
		std::string output;
		std::string stats;
		
		bool isCropRequested = false;
		std::optional<Polygon> boundary;
		
		bool isFilterRequested = false;
		std::optional<double> std;
		std::optional<int> meank;

		bool isSampleRequested = false;
		std::optional<double> radius;

		bool isSmoothRequested = false;
		std::optional<double> smooth_radius;

		int concurrency;
		bool verbose;
		

		Parameters(const int argc, char** argv)
		{

			cxxopts::Options options("FPCFilter", "Fast Point Cloud Filtering");

			options.show_positional_help();

			options.add_options()
				("i,input", "Input point cloud", cxxopts::value<std::string>())
				("o,output", "Output point cloud", cxxopts::value<std::string>())
				("j,stats", "Output statistics file (JSON)", cxxopts::value<std::string>())
				("b,boundary", "Crop boundary (GeoJSON POLYGON)", cxxopts::value<std::string>())
				("s,std", "Standard deviation threshold", cxxopts::value<double>())
				("m,meank", "Mean number of neighbors", cxxopts::value<int>())
				("r,radius", "Sample radius", cxxopts::value<double>())
				("smooth_radius", "Sample radius for smoothing", cxxopts::value<double>())
				("smooth_surface", "Use surface smooth method, else z smooth", cxxopts::value<bool>())
				("c,concurrency", "Max concurrency", cxxopts::value<int>())
				("v,verbose", "Verbose output", cxxopts::value<bool>());

			options.parse_positional({ "input", "output" });

			const auto result = options.parse(argc, argv);

			if (!result.count("input") || !result.count("output"))
				throw std::invalid_argument(options.help());

			input = result["input"].as<std::string>();

			if (input.empty())
				throw std::invalid_argument("Input file is empty");

			if (!fs::exists(input))
				throw std::invalid_argument(string_format("Input file '%s' does not exist", input.c_str()));

			output = result["output"].as<std::string>();

			if (output.empty())
				throw std::invalid_argument("Output file is empty");

			stats = result.count("stats") ? result["stats"].as<std::string>() : "";

			if (result.count("std") && result.count("meank")) {

				std = result["std"].as<double>();

				if (std < 0)
					throw std::invalid_argument("Standard deviation threshold cannot be less than 0");

				meank = result["meank"].as<int>();

				if (meank < 1)
					throw std::invalid_argument("Mean number of neighbors cannot be less than 1");

				isFilterRequested = std > 0 && meank > 1;
			}

			if (result.count("radius")) {

				radius = result["radius"].as<double>();

				if (radius < 0)
					throw std::invalid_argument("Sample radius cannot be less than 0");

				isSampleRequested = true;

			}

			if (result.count("smooth_radius")) {
				smooth_radius = result["smooth_radius"].as<double>();

				if (smooth_radius < 0)
					 throw std::invalid_argument("Sample radius cannot be less than 0");
				
				isSmoothRequested = true;
			}

			
			if (result.count("concurrency")) {

				concurrency = result["concurrency"].as<int>();

				if (concurrency < 1)
					throw std::invalid_argument("Concurrency cannot be less than 1");
				
			} else 
				concurrency = std::max(omp_get_num_procs(), 1);
			

			verbose = result.count("verbose") != 0;

			if (result.count("boundary")) {

				const auto boundaryFile = result["boundary"].as<std::string>();

				boundary = extractPolygon(boundaryFile);

				if (!boundary.has_value())
					throw std::invalid_argument(string_format("Boundary file '%s' does not contain a valid GeoJSON POLYGON", boundaryFile.c_str()));

				isCropRequested = true;
				
			}

		}
	};

}