
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

		if (parameters.std.has_value())
			std::cout << "\tstd = " << std::setprecision(4) << parameters.std.value() << std::endl;
		if (parameters.radius.has_value())
			std::cout << "\tradius = " << std::setprecision(4) << parameters.radius.value() << std::endl;
		if (parameters.meank.has_value())
			std::cout << "\tmeanK = " << parameters.meank.value() << std::endl;

		if (parameters.boundary.has_value()) 
			std::cout << "\tboundary = " << parameters.boundary.value().getPoints().size() << " polygon vertexes" << std::endl;		
		else 
			std::cout << "\tboundary = auto" << std::endl;
		
        std::cout << "\tconcurrency = " << parameters.concurrency << std::endl;
        std::cout << "\tverbose = " << (parameters.verbose ? "yes" : "no") << std::endl;
		std::cout << std::endl;
		
		FPCFilter::Pipeline pipeline(parameters.input);

		if (parameters.isCropRequested)
		{

			std::cout << std::endl << " -> Cropping ";

			pipeline.crop(parameters.boundary.value());

			std::cout << "OK" << std::endl;

		} else		
			std::cout << std::endl << " -> Skipping crop" << std::endl;
		
		if (parameters.isSampleRequested)
		{

			std::cout << std::endl << " -> Sampling ";

			pipeline.sample(parameters.radius.value());

			std::cout << "OK" << std::endl;

		}
		else		
			std::cout << std::endl << " -> Skipping sampling" << std::endl;
		
		if (parameters.isFilterRequested)
		{

			std::cout << std::endl << " -> Statistical filtering ";

			pipeline.filter(parameters.std.value(), parameters.meank.value());

			std::cout << "OK" << std::endl;

		}
		else		
			std::cout << std::endl << " -> Skipping statistical filtering" << std::endl;
		
		std::cout << std::endl << " -> Writing output ";

		pipeline.write(parameters.output);

		std::cout << "OK" << std::endl;

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