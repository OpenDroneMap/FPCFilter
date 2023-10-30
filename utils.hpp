
#pragma once

#ifdef DEBUG
#define DEFAULT_VERBOSE "true"
#else
#define DEFAULT_VERBOSE "false"
#endif

#include <memory>
#include <string>
#include <stdexcept>

namespace FPCFilter {
    struct PointCloud {
        std::vector<PlyPoint> &pts;

        PointCloud(std::vector<PlyPoint>& points) : pts(points) {
            this->pts = points;
        }

        // Must return the number of data points
        inline size_t kdtree_get_point_count() const { return pts.size(); }

        // Returns the dim'th component of the idx'th point in the class:
        // Since this is inlined and the "dim" argument is typically an immediate value, the
        //  "if/else's" are actually solved at compile time.
        inline float kdtree_get_pt(const size_t idx, const size_t dim) const
        {
            if (dim == 0) return pts[idx].x;
            else if (dim == 1) return pts[idx].y;
            else return pts[idx].z;
        }

        double kdtree_distance(const float* p1, const size_t p2_idx,
            size_t /*numDims*/) const
        {
            double d0 = p1[0] - pts[p2_idx].x;
            double d1 = p1[1] - pts[p2_idx].y;
            double d2 = p1[2] - pts[p2_idx].z;

            return (d0 * d0 + d1 * d1 + d2 * d2);
        }


        // Optional bounding-box computation: return false to default to a standard bbox computation loop.
        //   Return true if the BBOX was already computed by the class and returned in "bb" so it can be avoided to redo it again.
        //   Look at bb.size() to find out the expected dimensionality (e.g. 2 or 3 for point clouds)
        template <class BBOX>
        bool kdtree_get_bbox(BBOX& /* bb */) const { return false; }
    };
}

// Ref https://stackoverflow.com/a/26221725
template<typename ... Args>
std::string string_format(const std::string& format, Args ... args)
{
	int size_s = std::snprintf(nullptr, 0, format.c_str(), args ...) + 1; // Extra space for '\0'
	if (size_s <= 0) { throw std::runtime_error("Error during formatting."); }
	auto size = static_cast<size_t>(size_s);
	auto buf = std::make_unique<char[]>(size);
	std::snprintf(buf.get(), size, format.c_str(), args ...);
	return std::string(buf.get(), buf.get() + size - 1); // We don't want the '\0' inside
}

