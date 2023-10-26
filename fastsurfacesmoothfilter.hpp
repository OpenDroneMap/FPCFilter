#pragma once

#include <iostream>
#include <map>
#include <tuple>
#include <vector>
#include <random>
#include <memory>
#include <chrono>
#include <omp.h>

#include "ply.hpp"
#include "vendor/nanoflann.hpp"
#include "utils.hpp"

namespace FPCFilter {

    class FastSurfaceSmoothFilter {
        typedef nanoflann::KDTreeSingleIndexAdaptor<nanoflann::L2_Simple_Adaptor<
            double, PointCloud, double>, PointCloud, 3, std::size_t> KDTree;

        std::ostream& log;
        bool isVerbose;

        double radius, binRadiusSq;
        
        std::unique_ptr<KDTree> tree;

        const nanoflann::SearchParams params;

    public:
        FastSurfaceSmoothFilter(double radius, double binScale, std::ostream& logstream, bool isVerbose) : radius(radius), log(logstream), isVerbose(isVerbose), params(10) {
            binRadiusSq = binScale * binScale * radius * radius;
        }

        void radiusSearch(PlyPoint& point, double radius, std::vector<size_t> &indices) const {
            std::vector<std::pair<size_t, double>> indices_dists;
            std::array<double, 3> pt = {point.x, point.y, point.z};
            const size_t nMatches = tree->radiusSearch(&pt[0], radius, indices_dists, this->params);
            indices.resize(nMatches);
            for(auto i = 0; i < nMatches; i++) {
                indices[i] = indices_dists[i].first;
            }
        }

        void run(PlyFile& file) {
            PointCloud pointCloud(file.points);
            size_t np = pointCloud.pts.size();
            if (np == 0) {
                return;
            }

            const auto hasNormals = file.hasNormals();

            if (!hasNormals) {
                std::cerr << " !> Normals are required for surface smoothing, skip" << std::endl;
                return;
            }
            auto points = file.points;
            auto extras = file.extras;

            auto start = std::chrono::steady_clock::now();
        
            tree = std::make_unique<KDTree>(3, pointCloud, nanoflann::KDTreeSingleIndexAdaptorParams(100));
            tree->buildIndex();
            if (this->isVerbose) {
                const std::chrono::duration<double> diff = std::chrono::steady_clock::now() - start;
                std::cout << " ?> Done building index in " << diff.count() << "s" << std::endl;
            }
            
            std::vector<double> dists(np);

            #pragma omp parallel for
            for (auto n =0; n < np; n++) {
                std::vector<size_t> indices;
                PlyPoint& point = points[n];
                radiusSearch(point, radius, indices);
                if (indices.empty()) {
                    continue;
                }
                dists[n] = surface_median(points, point, extras[n], indices);
            }
            
            for (auto n =0; n < np; n++) {
                points[n].x += extras[n].nx * dists[n];
                points[n].y += extras[n].ny * dists[n];
                points[n].z += extras[n].nz * dists[n];
            }
        }

    private:
        double surface_median(const std::vector<PlyPoint> &points, const PlyPoint &pt, const PlyExtra &normal, const std::vector<size_t> &indices) const {
            std::vector<double> proj_dist;
            proj_dist.reserve(indices.size());
            for (auto i : indices) {
                auto dist = normal_distance(normal, pt, points[i]);
                if (dist.second <= binRadiusSq) {
                    proj_dist.push_back(dist.first);
                }
            }
            if (proj_dist.size() % 2 == 0) {
                std::nth_element(proj_dist.begin(), proj_dist.begin() + proj_dist.size() / 2, proj_dist.end());
                std::nth_element(proj_dist.begin(), proj_dist.begin() + proj_dist.size() / 2 - 1, proj_dist.end());
                return (proj_dist[proj_dist.size() / 2] + proj_dist[proj_dist.size() / 2 - 1]) / 2;
            } else {
                std::nth_element(proj_dist.begin(), proj_dist.begin() + proj_dist.size() / 2, proj_dist.end());
                return proj_dist[proj_dist.size() / 2];
            }
        }

        std::pair<double, double> normal_distance(const PlyExtra& normal, const PlyPoint& pt1, const PlyPoint &pt2) const {
            double dx = pt2.x - pt1.x;
            double dy = pt2.y - pt1.y;
            double dz = pt2.z - pt1.z;
            
            double projection_dist = normal.nx * dx + normal.ny * dy + normal.nz * dz; // distance along normal direction
            double ortho_distsq = dx * dx + dy * dy + dz * dz - projection_dist * projection_dist; // orthogonal distance to p1

            return std::make_pair(projection_dist, ortho_distsq);
            
        }

    };
    
}