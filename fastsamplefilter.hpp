#pragma once

#include <iostream>
#include <map>
#include <tuple>
#include <vector>

#include "ply.hpp"

namespace FPCFilter {

    class FastSampleFilter {
        using Voxel = std::tuple<int, int, int>;
        using Coord = std::tuple<double, double, double>;
        using CoordList = std::vector<Coord>;

    public:
        FastSampleFilter(double radius) {
            m_radius = radius;
            m_radiusSqr = radius * radius;
            m_cell = 2.0 * m_radius / std::sqrt(3.0);
        }

        void run(PlyFile& file) {
            
            auto points = file.points;
            auto extras = file.extras;
            
            const auto cnt = points.size();

            if (cnt == 0) 
                return;
            
            const auto hasNormals = file.hasNormals();

            m_originX = points[0].x;
            m_originY = points[0].y;
            m_originZ = points[0].z;

            if (hasNormals) {

                std::vector<PlyPoint> newPoints;
                newPoints.reserve(cnt);

                std::vector<PlyExtra> newExtras;
                newExtras.reserve(cnt);

                for (auto n = 0; n < cnt; n++) {

                    const auto& point = points[n];
                    const auto& extra = extras[n];

                    if (this->voxelize(point)) {
                        newPoints.push_back(point);
                        newExtras.push_back(extra);
                    }
                }

                newPoints.shrink_to_fit();
                newExtras.shrink_to_fit();

                file.points = newPoints;
                file.extras = newExtras;

            } else {

                auto res = std::remove_if(points.begin(), points.end(), [this](PlyPoint& point) { return !this->voxelize(point); });
                points.erase(res, points.end());
                
            }

        }


    private:
        double m_cell;
        double m_radius;
        double m_radiusSqr;
        double m_originX;
        double m_originY;
        double m_originZ;

        std::map<Voxel, CoordList> m_populatedVoxels;

        bool voxelize(const PlyPoint& point) {

            double x = point.x;
            double y = point.y;
            double z = point.z;

            // Get voxel indices for current point.
            Voxel v = std::make_tuple((int)(std::floor((x - m_originX) / m_cell)),
                                    (int)(std::floor((y - m_originY) / m_cell)),
                                    (int)(std::floor((z - m_originZ) / m_cell)));

            // Check current voxel before any of the neighbors. We will most often have
            // points that are too close in the point's enclosing voxel, thus saving
            // cycles.
            if (m_populatedVoxels.find(v) != m_populatedVoxels.end())
            {
                // Get list of coordinates in candidate voxel.
                CoordList coords = m_populatedVoxels[v];
                for (Coord const& coord : coords)
                {
                    // Compute Euclidean distance between current point and
                    // candidate voxel.
                    double xv = std::get<0>(coord);
                    double yv = std::get<1>(coord);
                    double zv = std::get<2>(coord);
                    double distSqr =
                        (xv - x) * (xv - x) + (yv - y) * (yv - y) + (zv - z) * (zv - z);

                    // If any point is closer than the minimum radius, we can
                    // immediately return false, as the minimum distance
                    // criterion is violated.
                    if (distSqr < m_radiusSqr)
                        return false;
                }
            }

            // Iterate over immediate neighbors of current voxel, computing minimum
            // distance between any already added point and the current point.
            for (int xi = std::get<0>(v) - 1; xi < std::get<0>(v) + 2; ++xi)
            {
                for (int yi = std::get<1>(v) - 1; yi < std::get<1>(v) + 2; ++yi)
                {
                    for (int zi = std::get<2>(v) - 1; zi < std::get<2>(v) + 2; ++zi)
                    {
                        Voxel candidate = std::make_tuple(xi, yi, zi);

                        // We have already visited the center voxel, and can skip it.
                        if (v == candidate)
                            continue;

                        // Check that candidate voxel is occupied.
                        if (m_populatedVoxels.find(candidate) ==
                            m_populatedVoxels.end())
                            continue;

                        // Get list of coordinates in candidate voxel.
                        CoordList coords = m_populatedVoxels[candidate];
                        for (Coord const& coord : coords)
                        {
                            // Compute Euclidean distance between current point and
                            // candidate voxel.
                            double xv = std::get<0>(coord);
                            double yv = std::get<1>(coord);
                            double zv = std::get<2>(coord);
                            double distSqr = (xv - x) * (xv - x) + (yv - y) * (yv - y) +
                                            (zv - z) * (zv - z);

                            // If any point is closer than the minimum radius, we can
                            // immediately return false, as the minimum distance
                            // criterion is violated.
                            if (distSqr < m_radiusSqr)
                                return false;
                        }
                    }
                }
            }

            Coord coord = std::make_tuple(x, y, z);
            if (m_populatedVoxels.find(v) != m_populatedVoxels.end())
            {
                m_populatedVoxels[v].push_back(coord);
            }
            else
            {
                CoordList coords;
                coords.push_back(coord);
                m_populatedVoxels.emplace(v, coords);
            }
            return true;
        }
    };

}