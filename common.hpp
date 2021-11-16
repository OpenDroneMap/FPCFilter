
#pragma once

namespace FPCFilter {
    
    // Point class (x, y)
    class Point2 {
    public:
        float x;
        float y;
        Point2(float x, float y) : x(x), y(y) {}
    };

    class Polygon {
        
        private:

        std::vector<Point2> points;

        public:

        Polygon() {}
        Polygon(std::vector<Point2> points) : points(points) {}
        Polygon(std::vector<float> x, std::vector<float> y) {
            for (int i = 0; i < x.size(); i++) {
                this->points.push_back(Point2(x[i], y[i]));
            }
        }

        std::vector<Point2> getPoints() {
            return this->points;
        }

        void addPoint(Point2 point) {
            this->points.push_back(point);
        }

        void addPoint(float x, float y) {
            this->points.push_back(Point2(x, y));
        }

        bool inside(float x, float y) const {
            // ray-casting algorithm based on
            // https://wrf.ecse.rpi.edu/Research/Short_Notes/pnpoly.html/pnpoly.html

            auto inside = false;

            size_t i;
            size_t j;

            for (i = 0, j = points.size() - 1; i < points.size(); j = i++) {
                const auto xi = points[i].x;
                const auto yi = points[i].y;
                const auto xj = points[j].x;
                const auto yj = points[j].y;

                const auto intersect = ((yi > y) != (yj > y))
                    && (x < (xj - xi)* (y - yi) / (yj - yi) + xi);

                if (intersect)
                    inside = !inside;
            }

            return inside;
        };

        bool inside(const Point2& point) const {            
            return inside(point.x, point.y);
        };


    };


}