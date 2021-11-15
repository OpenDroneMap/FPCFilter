
#pragma once

#include <iostream>
#include "vendor/octree.hpp"
#include "vendor/happly.hpp"
#include "common.hpp"
#include <unordered_set>

namespace FPCFilter
{

    #define VRT "vertex"

    class Pipeline {
        
        //std::unique_ptr<happly::PLYData> ply;
        std::vector<float> vrtX;
        std::vector<float> vrtY;
        std::vector<float> vrtZ;

        std::vector<size_t> indexes;

        std::string source;
                 
        public:

            Pipeline(const std::string& source);

            void crop(const Polygon& p);
            void sample(double radius);
            void filter(double std, int meank);

            void write(const std::string& output);

    };

}