#pragma once

#include <iostream>
#include <map>
#include <tuple>
#include <vector>

#include "ply.hpp"
#include "vendor/nanoflann.hpp"

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

    class FastOutlierFilter {

        typedef nanoflann::KDTreeSingleIndexAdaptor<nanoflann::L2_Simple_Adaptor<
            double, PointCloud, double>, PointCloud, -1, std::size_t> KDTree;

        double m_multiplier;
        int m_meanK;

        std::unique_ptr<KDTree> tree;

    public:
        FastOutlierFilter(double std, int meanK) {
            m_multiplier = std;
            m_meanK = meanK;
        }

        void knnSearch(PlyPoint& point, size_t k,
            std::vector<size_t>& indices, std::vector<double>& sqr_dists) const
        {
            nanoflann::KNNResultSet<double, size_t, size_t> resultSet(k);

            resultSet.init(&indices.front(), &sqr_dists.front());

            std::array<double, 3> pt = {point.x, point.y, point.z};
            tree->findNeighbors(resultSet, &pt[0], nanoflann::SearchParams(10));

        }

        void run(PlyFile& file) {

            PointCloud pointCloud(file.points);

            tree = std::make_unique<KDTree>(3, pointCloud, nanoflann::KDTreeSingleIndexAdaptorParams(100));
            tree->buildIndex();

            size_t np = file.points.size();

            std::vector<PlyPoint> newPoints;
            newPoints.reserve(np);

            std::vector<PlyExtra> newExtras;
            newExtras.reserve(np);

            std::vector<size_t> inliers, outliers;

            std::vector<double> distances(np, 0.0);

            // we increase the count by one because the query point itself will
            // be included with a distance of 0
            size_t count = m_meanK + 1;
            std::vector<size_t> indices(count);
            std::vector<double> sqr_dists(count);
            for (size_t i = 0; i < np; ++i)
            {
                knnSearch(file.points[i], count, indices, sqr_dists);

                for (size_t j = 1; j < count; ++j)
                {
                    double delta = std::sqrt(sqr_dists[j]) - distances[i];
                    distances[i] += (delta / j);
                }
                indices.clear(); indices.resize(count);
                sqr_dists.clear(); sqr_dists.resize(count);
            }

            size_t n(0);
            double M1(0.0);
            double M2(0.0);
            for (auto const& d : distances)
            {
                size_t n1(n);
                n++;
                double delta = d - M1;
                double delta_n = delta / n;
                M1 += delta_n;
                M2 += delta * delta_n * n1;
            }
            double mean = M1;
            double variance = M2 / (n - 1.0);
            double stdev = std::sqrt(variance);

            double threshold = mean + m_multiplier * stdev;

            if (file.hasNormals()) {
                for (size_t i = 0; i < np; ++i)
                {
                    if (distances[i] < threshold) {

                        const auto pt = file.points[i];
                        const auto xs = file.extras[i];

                        newPoints.emplace_back(pt.x, pt.y, pt.z, pt.red, pt.green, pt.blue, pt.views);
                        newExtras.emplace_back(xs.nx, xs.ny, xs.nz);
                    } 
                    
                }

                newPoints.shrink_to_fit();
                newExtras.shrink_to_fit();

                file.points = newPoints;
                file.extras = newExtras;

            }
            else {

                for (size_t i = 0; i < np; ++i)
                {
                    if (distances[i] < threshold) {
                        const auto pt = file.points[i];
                        newPoints.emplace_back(pt.x, pt.y, pt.z, pt.red, pt.green, pt.blue, pt.views);
                    }

                }

                newPoints.shrink_to_fit();

                file.points = newPoints;

            }
            
        }


    };

}