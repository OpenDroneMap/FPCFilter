
#pragma once

#include <iostream>
#include <functional>
#include <filesystem>
#include "vendor/octree.hpp"
#include "vendor/happly.hpp"
#include "ply.hpp"
#include "common.hpp"

namespace fs = std::filesystem;

namespace FPCFilter
{

	class Pipeline {

		std::unique_ptr<PlyFile> ply;

		std::string source;
		bool isLoaded = false;

	public:

		Pipeline(const std::string& source) : source(source) {}

		void load();
		void crop(const Polygon& p);
		void sample(double radius);
		void filter(double std, int meank);

		void write(const std::string& output);

	};



}