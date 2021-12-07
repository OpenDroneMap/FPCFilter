
#pragma once

#include <iostream>
#include <fstream>
#include <optional>
#include "vendor/json.hpp"

namespace FPCFilter
{

	template <class T> 
    class PointXYZ {
    public:
        T x, y, z;
        PointXYZ(T x, T y, T z) : x(x), y(y), z(z) {}

        bool operator==(const PointXYZ &other) const {
            return x == other.x && y == other.y && z == other.z;
        }

        bool operator!=(const PointXYZ &other) const {
            return !(*this == other);
        }

        bool operator<(const PointXYZ &other) const {
            return std::tie(x, y, z) < std::tie(other.x, other.y, other.z);
        }

        bool operator>(const PointXYZ &other) const {
            return std::tie(x, y, z) > std::tie(other.x, other.y, other.z);
        }

        bool operator<=(const PointXYZ &other) const {
            return std::tie(x, y, z) <= std::tie(other.x, other.y, other.z);
        }

        bool operator>=(const PointXYZ &other) const {
            return std::tie(x, y, z) >= std::tie(other.x, other.y, other.z);
        }

    };

	// Point class (x, y)
	class PointXY
	{
	public:
		float x;
		float y;
		PointXY(float x, float y) : x(x), y(y) {}
	};

	class Polygon
	{

	private:
		std::vector<PointXY> points;

	public:
		Polygon() {}
		Polygon(std::vector<PointXY> points) : points(points) {}
		Polygon(std::vector<float> x, std::vector<float> y)
		{
			for (int i = 0; i < x.size(); i++)
			{
				this->points.push_back(PointXY(x[i], y[i]));
			}
		}

		std::vector<PointXY> getPoints()
		{
			return this->points;
		}

		void addPoint(PointXY point)
		{
			this->points.push_back(point);
		}

		void addPoint(float x, float y)
		{
			this->points.push_back(PointXY(x, y));
		}

		bool inside(float x, float y) const
		{
			// ray-casting algorithm based on
			// https://wrf.ecse.rpi.edu/Research/Short_Notes/pnpoly.html/pnpoly.html

			auto inside = false;

			size_t i;
			size_t j;

			for (i = 0, j = points.size() - 1; i < points.size(); j = i++)
			{
				const auto xi = points[i].x;
				const auto yi = points[i].y;
				const auto xj = points[j].x;
				const auto yj = points[j].y;

				const auto intersect = ((yi > y) != (yj > y)) && (x < (xj - xi)* (y - yi) / (yj - yi) + xi);

				if (intersect)
					inside = !inside;
			}

			return inside;
		};

		bool inside(const PointXY& point) const
		{
			return inside(point.x, point.y);
		};
	};

	class NotImplementedException : public std::exception
	{
	public:
		NotImplementedException(const char* message) : message(message) {}
		const char* what() const noexcept
		{
			return message;
		}

	private:
		const char* message;
	};

}