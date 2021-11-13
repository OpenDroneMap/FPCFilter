
#pragma once

namespace FPCFilter {
    
    // Point class (x, y)
    class Point2 {
    public:
        double x;
        double y;
        Point2(double x, double y) : x(x), y(y) {}
    };

    class Polygon {
        
        private:

        std::vector<Point2> points;

        public:

        Polygon() {}
        Polygon(std::vector<Point2> points) : points(points) {}
        Polygon(std::vector<double> x, std::vector<double> y) {
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

        void addPoint(double x, double y) {
            this->points.push_back(Point2(x, y));
        }

        bool inside(const Point2& point) {
            // ray-casting algorithm based on
            // https://wrf.ecse.rpi.edu/Research/Short_Notes/pnpoly.html/pnpoly.html

            const auto y = point.y;

            auto inside = false;

            size_t i;
            size_t j;

            for (i = 0, j = points.size() - 1; i < points.size(); j = i++) {
                const auto  xi = points[i].x, yi = points[i].y;
                const auto  xj = points[j].x, yj = points[j].y;

                const auto intersect = ((yi > y) != (yj > y))
                    && (point.x < (xj - xi)* (y - yi) / (yj - yi) + xi);

                if (intersect)
                    inside = !inside;
            }

            return inside;
        };


    };


}