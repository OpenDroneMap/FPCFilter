
#include <iostream>
#include <filesystem>
#include "pipeline.hpp"

#include <numeric>

#include "vendor/loguru/loguru.hpp"

namespace fs = std::filesystem;

namespace FPCFilter
{

	Pipeline::Pipeline(const std::string& source)
	{
		this->source = source;

		happly::PLYData ply(source, true);

		this->vrtX = std::make_unique<std::vector<float>>(ply.getElement(VRT).getProperty<float>("x"));
		this->vrtY = std::make_unique<std::vector<float>>(ply.getElement(VRT).getProperty<float>("y"));
		this->vrtZ = std::make_unique<std::vector<float>>(ply.getElement(VRT).getProperty<float>("z"));

		std::cout << " ?> Found " << this->vrtX->size() << " vertexes" << std::endl;

		this->indexes = std::make_unique<std::vector<size_t>>(this->vrtX->size());

		std::iota(this->indexes->begin(), this->indexes->end(), 0);

	}


	void Pipeline::crop(const Polygon& p)
	{
		auto newIndexes = std::make_unique<std::vector<size_t>>();
		auto newVrtX = std::make_unique<std::vector<float>>();
		auto newVrtY = std::make_unique<std::vector<float>>();
		auto newVrtZ = std::make_unique<std::vector<float>>();

		const auto size = this->vrtX->size();

		newIndexes->reserve(size);
		newVrtX->reserve(size);
		newVrtY->reserve(size);
		newVrtZ->reserve(size);

		for (size_t i = 0; i < size; i++) {
			const auto x = this->vrtX->at(i);
			const auto y = this->vrtY->at(i);
			if (p.inside(x, y))
			{
				newIndexes->push_back(i);
				newVrtX->push_back(x);
				newVrtY->push_back(y);
				const auto z = this->vrtZ->at(i);

				newVrtZ->push_back(z);
			}
		}

		newIndexes->shrink_to_fit();
		newVrtX->shrink_to_fit();
		newVrtY->shrink_to_fit();
		newVrtZ->shrink_to_fit();

		this->vrtX = std::move(newVrtX);
		this->vrtY = std::move(newVrtY);
		this->vrtZ = std::move(newVrtZ);
		this->indexes = std::move(newIndexes);

	}

	void Pipeline::sample(double radius)
	{
	}

	void Pipeline::filter(double std, int meank)
	{
	}

	template <class T>
	void copyProperty(happly::PLYData& source, happly::PLYData& dest, const std::string& prop, std::vector<size_t>& indexes)
	{

		if (source.getElement("vertex").hasProperty(prop))
		{
			const auto p = source.getElement("vertex").getProperty<T>(prop);

			std::vector<T> propOk;

			propOk.reserve(indexes.size());

			for (const size_t i : indexes)
				propOk.push_back(p[i]);

			dest.getElement("vertex").addProperty(prop, propOk);

		}
	}


	void Pipeline::write(const std::string& target)
	{

		if (fs::exists(target))
			fs::remove(target);

		happly::PLYData ply(source);

		happly::PLYData plyOut;

		plyOut.addElement(VRT, this->vrtX->size());

		plyOut.getElement(VRT).addProperty<float>("x", *this->vrtX);
		plyOut.getElement(VRT).addProperty<float>("y", *this->vrtY);
		plyOut.getElement(VRT).addProperty<float>("z", *this->vrtZ);

		copyProperty<float>(ply, plyOut, "nx", *this->indexes);
		copyProperty<float>(ply, plyOut, "ny", *this->indexes);
		copyProperty<float>(ply, plyOut, "nz", *this->indexes);

		copyProperty<unsigned char>(ply, plyOut, "red", *this->indexes);
		copyProperty<unsigned char>(ply, plyOut, "blue", *this->indexes);
		copyProperty<unsigned char>(ply, plyOut, "green", *this->indexes);
		copyProperty<unsigned char>(ply, plyOut, "views", *this->indexes);

		plyOut.write(target, happly::DataFormat::Binary);
	};

}