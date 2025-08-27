
#pragma once

#include <iostream>
#include <filesystem>
#include <vector>
#include "FPCFilter.h"

namespace FPCFilter {

	class PlyPoint {
	public:
		float x;
		float y;
		float z;

		uint8_t red;
		uint8_t blue;
		uint8_t green;

		uint8_t views;

		PlyPoint(float x, float y, float z, uint8_t red, uint8_t green, uint8_t blue, uint8_t views) : x(x), y(y), z(z), red(red), green(green), blue(blue), views(views) {}

	};

	class PlyExtra {
	public:
		float nx;
		float ny;
		float nz;

		PlyExtra(float nx, float ny, float nz) : nx(nx), ny(ny), nz(nz) {}
	};

	class PlyFile {

		std::string getVertexLine(std::ifstream& reader) {

			std::string line;

			// Skip comments
			do {
				
				std::getline(reader, line);

				if (line.find("element") == 0)
					return line;
				else if (line.find("comment") == 0)
					continue;
				else
					throw std::invalid_argument("Invalid PLY file");

			} while (true);
		}

		int getVertexCount(const std::string& line) {

			// Split line into tokens
			std::vector<std::string> tokens;

			std::istringstream iss(line);
			std::string token;
			while (std::getline(iss, token, ' '))
				tokens.push_back(token);

			if (tokens.size() != 3)
				throw std::invalid_argument("Invalid PLY file");

			if (tokens[0] != "element" && tokens[1] != "vertex")
				throw std::invalid_argument("Invalid PLY file");

			return std::stoi(tokens[2]);
		}

	public:
		std::vector<PlyExtra> extras;
		std::vector<PlyPoint> points;

        bool hasNormals() {
            return !extras.empty();
        }

		PlyFile(const std::string& path, const std::function<bool(const float x, const float y, const float z)> filter = nullptr) {

			std::ifstream reader(path);

			if (!reader.is_open())
				throw std::invalid_argument(std::string("Cannot open file ") + path);

			std::string line;

			std::getline(reader, line);
			if (line != "ply")
				throw std::invalid_argument("Invalid PLY file");

			std::getline(reader, line);

			// We are reading an ascii ply
			if (line == "format ascii 1.0") {

				/*
				Expected header:
					element vertex 62217
					property float x
					property float y
					property float z
					property uchar diffuse_red
					property uchar diffuse_green
					property uchar diffuse_blue
					property uchar views
					end_header
				*/

				const auto vertexLine = getVertexLine(reader);
				const auto count = getVertexCount(vertexLine);

				std::getline(reader, line);
				if (line != "property float x")
					throw std::invalid_argument("Invalid PLY file (expected 'property float x')");

				std::getline(reader, line);
				if (line != "property float y")
					throw std::invalid_argument("Invalid PLY file (expected 'property float y')");

				std::getline(reader, line);
				if (line != "property float z")
					throw std::invalid_argument("Invalid PLY file (expected 'property float z')");

				std::getline(reader, line);
				if (line != "property uchar diffuse_red" && line != "property uchar red")
					throw std::invalid_argument("Invalid PLY file (expected 'property uchar diffuse_red' or 'property uchar red')");

				std::getline(reader, line);
				if (line != "property uchar diffuse_green" && line != "property uchar green")
					throw std::invalid_argument("Invalid PLY file (expected 'property uchar diffuse_green' or 'property uchar green')");

				std::getline(reader, line);
				if (line != "property uchar diffuse_blue" && line != "property uchar blue")
					throw std::invalid_argument("Invalid PLY file (expected 'property uchar diffuse_blue' or 'property uchar blue')");

				std::getline(reader, line);
				if (line != "property uchar views")
					throw std::invalid_argument("Invalid PLY file (expected 'property uchar views')");

				std::getline(reader, line);
				if (line != "end_header")
					throw std::invalid_argument("Invalid PLY file (expected 'end_header')");

				points.reserve(count);

				if (filter) {

					// Read points
					for (auto i = 0; i < count; i++) {

						float x, y, z;
						int red, green, blue;
						int views;

						reader >> x >> y >> z >> red >> green >> blue >> views;

						if (filter(x, y, z)) 
							points.emplace_back(x, y, z, red, green, blue, views);						
					}
				}
				else {

					// Read points
					for (auto i = 0; i < count; i++) {

						float x, y, z;
						int red, green, blue;
						int views;

						reader >> x >> y >> z >> red >> green >> blue >> views;

						points.emplace_back(x, y, z, red, green, blue, views);						
					}
				}				


			// Otherwise it's a binary ply
			} else if (line == "format binary_little_endian 1.0") {

				/*
				Expected header:
					element vertex 2751907
					property float32 x
					property float32 y
					property float32 z
					property uint8 red
					property uint8 green
					property uint8 blue
					property float32 nx
					property float32 ny
					property float32 nz
					property uint8 views
					end_header
				*/

				const auto vertexLine = getVertexLine(reader);
				const auto count = getVertexCount(vertexLine);

				std::getline(reader, line);
				if (line != "property float32 x")
					throw std::invalid_argument("Invalid PLY file (expected 'property float32 x')");

				std::getline(reader, line);
				if (line != "property float32 y")
					throw std::invalid_argument("Invalid PLY file (expected 'property float32 y')");

				std::getline(reader, line);
				if (line != "property float32 z")
					throw std::invalid_argument("Invalid PLY file (expected 'property float32 z')");

				std::getline(reader, line);
				if (line != "property uint8 red")
					throw std::invalid_argument("Invalid PLY file (expected 'property uint8 red')");

				std::getline(reader, line);
				if (line != "property uint8 green")
					throw std::invalid_argument("Invalid PLY file (expected 'property uint8 green')");

				std::getline(reader, line);
				if (line != "property uint8 blue")
					throw std::invalid_argument("Invalid PLY file (expected 'property uint8 blue')");

				std::getline(reader, line);
				if (line != "property float32 nx")
					throw std::invalid_argument("Invalid PLY file (expected 'property float32 nx')");

				std::getline(reader, line);
				if (line != "property float32 ny")
					throw std::invalid_argument("Invalid PLY file (expected 'property float32 ny')");

				std::getline(reader, line);
				if (line != "property float32 nz")
					throw std::invalid_argument("Invalid PLY file (expected 'property float32 nz')");

				std::getline(reader, line);
				if (line != "property uint8 views")
					throw std::invalid_argument("Invalid PLY file (expected 'property uint8 views')");

				std::getline(reader, line);
				if (line != "end_header")
					throw std::invalid_argument("Invalid PLY file (expected 'end_header')");

				reader = std::ifstream(path, std::ifstream::binary);
				
				for (auto n = 0; n < 14; n++)
					std::getline(reader, line);

				points.reserve(count);
				extras.reserve(count);

				if (filter) {

					// Read points
					for (auto i = 0; i < count; i++) {

						float x, y, z;
						float nx, ny, nz;
						uint8_t red, green, blue;
						uint8_t views;

						reader.read(reinterpret_cast<char*>(&x), sizeof(float));
						reader.read(reinterpret_cast<char*>(&y), sizeof(float));
						reader.read(reinterpret_cast<char*>(&z), sizeof(float));

						reader.read(reinterpret_cast<char*>(&red), sizeof(uint8_t));
						reader.read(reinterpret_cast<char*>(&green), sizeof(uint8_t));
						reader.read(reinterpret_cast<char*>(&blue), sizeof(uint8_t));

						reader.read(reinterpret_cast<char*>(&nx), sizeof(float));
						reader.read(reinterpret_cast<char*>(&ny), sizeof(float));
						reader.read(reinterpret_cast<char*>(&nz), sizeof(float));

						reader.read(reinterpret_cast<char*>(&views), sizeof(uint8_t));

						if (filter(x, y, z)) {
							points.emplace_back(x, y, z, red, green, blue, views);
							extras.emplace_back(nx, ny, nz);
						}
					}

				}
				else {

					// Read points
					for (auto i = 0; i < count; i++) {

						float x, y, z;
						float nx, ny, nz;
						uint8_t red, green, blue;
						uint8_t views;

						reader.read(reinterpret_cast<char*>(&x), sizeof(float));
						reader.read(reinterpret_cast<char*>(&y), sizeof(float));
						reader.read(reinterpret_cast<char*>(&z), sizeof(float));

						reader.read(reinterpret_cast<char*>(&red), sizeof(uint8_t));
						reader.read(reinterpret_cast<char*>(&green), sizeof(uint8_t));
						reader.read(reinterpret_cast<char*>(&blue), sizeof(uint8_t));

						reader.read(reinterpret_cast<char*>(&nx), sizeof(float));
						reader.read(reinterpret_cast<char*>(&ny), sizeof(float));
						reader.read(reinterpret_cast<char*>(&nz), sizeof(float));

						reader.read(reinterpret_cast<char*>(&views), sizeof(uint8_t));

						points.emplace_back(x, y, z, red, green, blue, views);
						extras.emplace_back(nx, ny, nz);

					}
				}

			} else
				throw std::invalid_argument("Invalid PLY file");
					
			reader.close();

		}

		void write(std::ostream& o) {

			const auto cnt = this->points.size();

			o << "ply" << std::endl;
			o << "format binary_little_endian 1.0" << std::endl;
			o << "comment Generated by FPCFilter v" << FPCFilter_VERSION_MAJOR << "." << FPCFilter_VERSION_MINOR << std::endl;
			o << "element vertex " << cnt << std::endl;

			o << "property float x" << std::endl;
			o << "property float y" << std::endl;
			o << "property float z" << std::endl;

			const auto hasNormals = this->hasNormals();

			if (hasNormals)
			{
				o << "property float nx" << std::endl;
				o << "property float ny" << std::endl;
				o << "property float nz" << std::endl;
			}

			o << "property uchar red" << std::endl;
			o << "property uchar blue" << std::endl;
			o << "property uchar green" << std::endl;
			o << "property uchar views" << std::endl;

			o << "end_header" << std::endl;

			if (hasNormals)
			{
				for (auto n = 0; n < cnt; n++)
				{
					const auto point = this->points[n];
					const auto extra = this->extras[n];

                    o.write(reinterpret_cast<const char*>(&point.x), sizeof(float));
                    o.write(reinterpret_cast<const char*>(&point.y), sizeof(float));
                    o.write(reinterpret_cast<const char*>(&point.z), sizeof(float));

                    o.write(reinterpret_cast<const char*>(&extra.nx), sizeof(float));
                    o.write(reinterpret_cast<const char*>(&extra.ny), sizeof(float));
                    o.write(reinterpret_cast<const char*>(&extra.nz), sizeof(float));

                    o.write(reinterpret_cast<const char*>(&point.red), sizeof(uint8_t));
                    o.write(reinterpret_cast<const char*>(&point.blue), sizeof(uint8_t));
                    o.write(reinterpret_cast<const char*>(&point.green), sizeof(uint8_t));

                    o.write(reinterpret_cast<const char*>(&point.views), sizeof(uint8_t));

				}

			} else
			{
				for (auto n = 0; n < cnt; n++)
				{
					const auto point = this->points[n];

                    o.write(reinterpret_cast<const char*>(&point.x), sizeof(float));
                    o.write(reinterpret_cast<const char*>(&point.y), sizeof(float));
                    o.write(reinterpret_cast<const char*>(&point.z), sizeof(float));

                    o.write(reinterpret_cast<const char*>(&point.red), sizeof(uint8_t));
                    o.write(reinterpret_cast<const char*>(&point.blue), sizeof(uint8_t));
                    o.write(reinterpret_cast<const char*>(&point.green), sizeof(uint8_t));

                    o.write(reinterpret_cast<const char*>(&point.views), sizeof(uint8_t));

				}
			}            

		}
	};
}