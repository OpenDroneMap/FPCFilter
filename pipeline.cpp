
#include <iostream>
#include <filesystem>
#include "pipeline.hpp"
#include "vendor/loguru/loguru.hpp"

namespace fs = std::filesystem;

namespace FPCFilter
{

	Pipeline::Pipeline(const std::string& source)
	{
		this->source = source;

		happly::PLYData ply(source, true);

		this->vrtX = ply.getElement(VRT).getProperty<float>("x");
		this->vrtY = ply.getElement(VRT).getProperty<float>("y");
		this->vrtZ = ply.getElement(VRT).getProperty<float>("z");

		std::cout << " ?> Found " << this->vrtX.size() << " vertexes" << std::endl;

		indexes.reserve(this->vrtX.size());
		for (size_t n = 0; n < this->vrtX.size(); n++)
			indexes.push_back(n);

	}


	void Pipeline::crop(const Polygon& p)
	{

		std::vector<size_t> newIndexes;
		std::vector<float> newVrtX;
		std::vector<float> newVrtY;
		std::vector<float> newVrtZ;

		const auto size = this->vrtX.size();

		newIndexes.reserve(size);
		newVrtX.reserve(size);
		newVrtY.reserve(size);
		newVrtZ.reserve(size);

		for (size_t i = 0; i < size; i++) {
			const auto x = this->vrtX[i];
			const auto y = this->vrtY[i];
			if (p.inside(x, y))
			{
				newIndexes.push_back(i);
				newVrtX.push_back(x);
				newVrtY.push_back(y);
				const auto z = this->vrtZ[i];

				newVrtZ.push_back(z);
			}
		}

		newIndexes.shrink_to_fit();
		newVrtX.shrink_to_fit();
		newVrtY.shrink_to_fit();
		newVrtZ.shrink_to_fit();

		this->vrtX = newVrtX;
		this->vrtY = newVrtY;
		this->vrtZ = newVrtZ;
		this->indexes = newIndexes;

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

		plyOut.addElement(VRT, this->vrtX.size());

		plyOut.getElement(VRT).addProperty<float>("x", this->vrtX);
		plyOut.getElement(VRT).addProperty<float>("y", this->vrtY);
		plyOut.getElement(VRT).addProperty<float>("z", this->vrtZ);

		copyProperty<float>(ply, plyOut, "nx", this->indexes);
		copyProperty<float>(ply, plyOut, "ny", this->indexes);
		copyProperty<float>(ply, plyOut, "nz", this->indexes);

		copyProperty<unsigned char>(ply, plyOut, "red", this->indexes);
		copyProperty<unsigned char>(ply, plyOut, "blue", this->indexes);
		copyProperty<unsigned char>(ply, plyOut, "green", this->indexes);
		copyProperty<unsigned char>(ply, plyOut, "views", this->indexes);

		plyOut.write(target, happly::DataFormat::Binary);
	};

}