
#include <iostream>
#include <filesystem>
#include "FPCFilter.h"
#include "pipeline.hpp"
#include "parameters.hpp"

int main(const int argc, char** argv)
{
	std::cout << " *** FPCFilter - v" << FPCFilter_VERSION_MAJOR << "." << FPCFilter_VERSION_MINOR << " ***" << std::endl << std::endl;

	try {

		FPCFilter::Parameters parameters(argc, argv);

        std::cout << "?> Parameters:" << std::endl;
        std::cout << "\tinput = " << parameters.input << std::endl;
        std::cout << "\toutput = " << parameters.output << std::endl;

		if (!parameters.stats.empty()) std::cout << "\tstats = " << parameters.stats << std::endl;
		nlohmann::json stats = nlohmann::json::object();

		if (parameters.std.has_value())
			std::cout << "\tstd = " << std::setprecision(4) << parameters.std.value() << std::endl;
		if (parameters.radius.has_value())
			std::cout << "\tradius = " << std::setprecision(4) << parameters.radius.value() << std::endl;
		if (parameters.meank.has_value())
			std::cout << "\tmeanK = " << parameters.meank.value() << std::endl;
		if (parameters.smooth_radius.has_value()) 
			std::cout << "\tSmoothRadius = " << std::setprecision(4) << parameters.smooth_radius.value() << std::endl;

		if (parameters.boundary.has_value()) 
			std::cout << "\tboundary = " << parameters.boundary.value().getPoints().size() << " polygon vertexes" << std::endl;		
		else 
			std::cout << "\tboundary = auto" << std::endl;
		
        std::cout << "\tconcurrency = " << parameters.concurrency << std::endl;
        std::cout << "\tverbose = " << (parameters.verbose ? "yes" : "no") << std::endl;
		std::cout << std::endl;

		std::cout << " -> Setting num_threads to " << parameters.concurrency << std::endl;
		omp_set_num_threads(parameters.concurrency);

		const auto pipelineStart = std::chrono::steady_clock::now();

		FPCFilter::Pipeline pipeline(parameters.input, std::cout, parameters.verbose, &stats);

		if (parameters.isCropRequested)
		{

			std::cout << std::endl << " -> Cropping" << std::endl << std::endl;;

			const auto start = std::chrono::steady_clock::now();

			pipeline.crop(parameters.boundary.value());

			const std::chrono::duration<double> diff = std::chrono::steady_clock::now() - start;

			std::cout << " -> Done cropping in " << diff.count() << "s" << std::endl;

		} else		
			std::cout << std::endl << " ?> Skipping crop" << std::endl;
		
		if (parameters.isSampleRequested)
		{

			std::cout << std::endl << " -> Sampling" << std::endl << std::endl;

			const auto start = std::chrono::steady_clock::now();

			pipeline.sample(parameters.radius.value());

			const std::chrono::duration<double> diff = std::chrono::steady_clock::now() - start;

			std::cout << " ?> Done in " << diff.count() << "s" << std::endl;

		}
		else		
			std::cout << std::endl << " ?> Skipping sampling" << std::endl;
		
		if (parameters.isFilterRequested)
		{

			std::cout << std::endl << " -> Statistical filtering" << std::endl << std::endl;;

			const auto start = std::chrono::steady_clock::now();

			pipeline.filter(parameters.std.value(), parameters.meank.value());

			const std::chrono::duration<double> diff = std::chrono::steady_clock::now() - start;

			std::cout << " ?> Done in " << diff.count() << "s" << std::endl;

		}
		else		
			std::cout << std::endl << " ?> Skipping statistical filtering" << std::endl;

		if (parameters.isSmoothRequested)
		{
			std::cout << std::endl << " -> Smoothing " << std::endl << std::endl;;

			const auto start = std::chrono::steady_clock::now();

			pipeline.smooth(parameters.smooth_radius.value());

			const std::chrono::duration<double> diff = std::chrono::steady_clock::now() - start;

			std::cout << " ?> Done in " << diff.count() << "s" << std::endl;
		} 
		else
			std::cout << std::endl << " ?> Skipping Z smoothing" << std::endl;

		{
			std::cout << std::endl << " -> Writing output" << std::endl << std::endl;

			const auto start = std::chrono::steady_clock::now();

			pipeline.write(parameters.output);

			const std::chrono::duration<double> diff = std::chrono::steady_clock::now() - start;

			std::cout << " ?> Done in " << diff.count() << "s" << std::endl;
		}

		const std::chrono::duration<double> pipelineDiff = std::chrono::steady_clock::now() - pipelineStart;

		std::cout << std::endl << " ?> Pipeline done in " << pipelineDiff.count() << "s" << std::endl << std::endl;

		if (!parameters.stats.empty()){
			std::ofstream o(parameters.stats);
			o << stats;
			o.close();
		}
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