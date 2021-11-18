
#include <iostream>
#include <filesystem>
#include "pipeline.hpp"
#include "vendor/loguru/loguru.hpp"

namespace fs = std::filesystem;

namespace FPCFilter
{

	void Pipeline::load() {

		LOG_F(INFO, "Loading %s", source.c_str());

		this->ply = std::make_unique<PlyFile>(this->source);

		this->isLoaded = true;

	}

	void Pipeline::crop(const Polygon& p)
	{

		if (!this->isLoaded)
		{
			this->ply = std::make_unique<PlyFile>(this->source, [&p](const float x, const float y, const float z)
				{
					return p.inside(x, y);
				});

			return;
		}

		throw NotImplementedException("Filtering loaded ply is not supported");
	}

	void Pipeline::sample(double radius)
	{

	}

	void Pipeline::filter(double std, int meank)
	{
	}


	void Pipeline::write(const std::string& target)
	{

		if (!this->isLoaded)
		{
			this->load();
		}

		if (fs::exists(target))
			fs::remove(target);

		// TODO
	};

}