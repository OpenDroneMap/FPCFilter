
#pragma once

#include <iostream>
#include <functional>
#include <filesystem>
#include <map>

#include "vendor/octree.hpp"
#include "vendor/happly.hpp"
#include "ply.hpp"
#include "common.hpp"

#include "fastsamplefilter.hpp"
#include "fastoutlierfilter.hpp"
#include "vendor/loguru/loguru.hpp"

namespace fs = std::filesystem;

namespace FPCFilter
{

	class Pipeline
	{

		std::unique_ptr<PlyFile> ply;

		std::string source;
		bool isLoaded = false;

	public:
		Pipeline(const std::string &source) : source(source) {}

		void Pipeline::load()
		{

			LOG_F(INFO, "Loading %s", source.c_str());

			this->ply = std::make_unique<PlyFile>(this->source);

			this->isLoaded = true;
		}

		void Pipeline::crop(const Polygon &p)
		{

			if (!this->isLoaded)
			{
				this->ply = std::make_unique<PlyFile>(this->source, [&p](const float x, const float y, const float z)
													  { return p.inside(x, y); });

				this->isLoaded = true;

				return;
			}

			throw NotImplementedException("Filtering loaded ply is not supported");
		}

		void Pipeline::sample(double radius)
		{
			if (!this->isLoaded)
				this->load();

			FastSampleFilter filter(radius);

			filter.run(*this->ply);
		}

		void Pipeline::filter(double std, int meank)
		{
			if (!this->isLoaded)
				this->load();

			FastOutlierFilter filter(std, meank);

			filter.run(*this->ply);
		}

		void Pipeline::write(const std::string &target)
		{

			if (!this->isLoaded)
				this->load();

			if (fs::exists(target))
				fs::remove(target);

			std::ofstream writer(target, std::ofstream::binary);

			if (!writer.is_open())
				throw std::invalid_argument(std::string("Cannot open file ") + target);

			this->ply->write(writer);

			writer.close();
		};
	};

}