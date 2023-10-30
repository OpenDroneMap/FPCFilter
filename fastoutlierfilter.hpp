#pragma once

#include <iostream>
#include <map>
#include <tuple>
#include <vector>
#include <random>
#include <memory>
#include <chrono>

#include "ply.hpp"
#include "vendor/nanoflann.hpp"
#include "utils.hpp"

namespace FPCFilter {
    class FastOutlierFilter {

        typedef nanoflann::KDTreeSingleIndexAdaptor<nanoflann::L2_Simple_Adaptor<
            double, PointCloud, double>, PointCloud, -1, std::size_t> KDTree;

        double multiplier;
        int meanK;

        std::ostream& log;
        bool isVerbose;

        std::unique_ptr<KDTree> tree;

        const nanoflann::SearchParams params;
        nlohmann::json *stats;

    public:
        FastOutlierFilter(double std, int meanK, std::ostream &logstream, bool isVerbose, nlohmann::json *stats) : 
            multiplier(std), meanK(meanK), isVerbose(isVerbose), log(logstream), params(nanoflann::SearchParams(10)), stats(stats) {}

        void knnSearch(PlyPoint& point, size_t k,
            std::vector<size_t>& indices, std::vector<double>& sqr_dists) const
        {
            nanoflann::KNNResultSet<double, size_t, size_t> resultSet(k);

            resultSet.init(&indices.front(), &sqr_dists.front());

            std::array<double, 3> pt = {point.x, point.y, point.z};
            tree->findNeighbors(resultSet, &pt[0], this->params);

        }

        void run(PlyFile& file) {

            PointCloud pointCloud(file.points);

            auto start = std::chrono::steady_clock::now();

            tree = std::make_unique<KDTree>(3, pointCloud, nanoflann::KDTreeSingleIndexAdaptorParams(100));
            tree->buildIndex();

            if (this->isVerbose) {
                const std::chrono::duration<double> diff = std::chrono::steady_clock::now() - start;
                std::cout << " ?> Done building index in " << diff.count() << "s" << std::endl;
            }

            size_t np = file.points.size();

            // Compute neighbor median distance over closest neighbors
            // This could be part of a separate pipeline item

            std::vector<size_t> indices;
            std::vector<double> sqr_dists;
            size_t SAMPLES = std::min<size_t>(np, 10000);

            size_t count = 3;
            std::vector<double> distances(SAMPLES, 0.0);

            std::unordered_map<uint64_t, size_t> dist_map;

            std::vector<double> all_distances;
            std::random_device rd;
            std::mt19937_64 gen(rd());
            std::uniform_int_distribution<size_t> randomDis(
                0, np - 1
            );

            #pragma omp parallel private (indices, sqr_dists)
            {
                indices.resize(count);
                sqr_dists.resize(count);

                #pragma omp for
                for (long long i = 0; i < SAMPLES; ++i)
                {
                    const size_t idx = randomDis(gen);
                    knnSearch(file.points[idx], count, indices, sqr_dists);

                    double sum = 0.0;
                    for (size_t j = 1; j < count; ++j)
                    {
                        sum += std::sqrt(sqr_dists[j]);
                    }
                    sum /= count;

                    #pragma omp critical
                    {
                        uint64_t k = std::ceil(sum * 100);
                        if (dist_map.find(k) == dist_map.end()){
                            dist_map[k] = 1;
                        }else{
                            dist_map[k] += 1;
                        }
                    }
                    indices.clear(); indices.resize(count);
                    sqr_dists.clear(); sqr_dists.resize(count);
                }
            }

            uint64_t max_val = std::numeric_limits<uint64_t>::min();
            int d = 0;
            for (auto it : dist_map){
                if (it.second > max_val){
                    d = it.first;
                    max_val = it.second;
                }
            }

            double spacing = static_cast<double>(d) / 100.0;
            (*stats)["spacing"] = spacing;

            std::cout << " -> Spacing estimation completed (" << spacing << " meters)" << std::endl << std::endl;

            // Outlier filtering

            std::vector<PlyPoint> newPoints;
            newPoints.reserve(np);

            std::vector<PlyExtra> newExtras;
            newExtras.reserve(np);

            std::vector<size_t> inliers, outliers;
            distances.resize(np, 0.0);

            // we increase the count by one because the query point itself will
            // be included with a distance of 0
            count = (size_t)meanK + 1;

            indices.clear();
            sqr_dists.clear();

            start = std::chrono::steady_clock::now();

            #pragma omp parallel private (indices, sqr_dists)
            {
                indices.resize(count);
                sqr_dists.resize(count);

                // We are using 'long long' instead of size_t (unsigned long long) because OpenMP parallel for needs a signed index

                #pragma omp for
                for (long long i = 0; i < np; ++i)
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
            }

            if (this->isVerbose) {
                const std::chrono::duration<double> diff = std::chrono::steady_clock::now() - start;
                std::cout << " ?> Done calculating point neighbors average distances in " << diff.count() << "s" << std::endl;
            }

            size_t n(0);
            double M1(0.0);
            double M2(0.0);

            start = std::chrono::steady_clock::now();

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

            double threshold = mean + multiplier * stdev;

            if (this->isVerbose) {
                const std::chrono::duration<double> diff = std::chrono::steady_clock::now() - start;
                std::cout << " ?> Done calculating cloud average distance " << diff.count() << "s" << std::endl;
            }

            if (file.hasNormals()) {

                start = std::chrono::steady_clock::now();

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

                if (this->isVerbose) {
                    const std::chrono::duration<double> diff = std::chrono::steady_clock::now() - start;
                    std::cout << " ?> Done filtering points in " << diff.count() << "s" << std::endl;
                }

            }
            else {

                start = std::chrono::steady_clock::now();

                for (size_t i = 0; i < np; ++i)
                {
                    if (distances[i] < threshold) {
                        const auto pt = file.points[i];
                        newPoints.emplace_back(pt.x, pt.y, pt.z, pt.red, pt.green, pt.blue, pt.views);
                    }

                }

                newPoints.shrink_to_fit();

                file.points = newPoints;

                if (this->isVerbose) {
                    const std::chrono::duration<double> diff = std::chrono::steady_clock::now() - start;
                    std::cout << " ?> Done filtering points in " << diff.count() << "s" << std::endl;
                }

            }
        }


    };

}