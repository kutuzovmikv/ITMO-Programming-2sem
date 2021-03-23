#include <iostream>
#include <vector>
#include <cmath>
#include <cassert>

using namespace std;

#define PI 3.14

class Point {
private:
    double x_, y_;
public:
    Point(double xcoord, double ycoord) : x_(xcoord), y_(ycoord) {}

    Point(const Point &other) = default;

    Point &operator=(const Point &other) = default;

    static double calculate_side(const Point &a, const Point &b) {
        return sqrt(pow(a.x_ - b.x_, 2) + pow(a.y_ - b.y_, 2));
    }

    double x() const {
        return x_;
    }

    double y() const {
        return y_;
    }
};

class Polyline {
protected:
    bool isLength = false;
    double length_ = 0;
    vector<Point> dots_;
public:
    explicit Polyline(vector<Point> &points) : dots_(points) {}

    Polyline(const Polyline &other) = default;

    Polyline &operator=(const Polyline &other) = default;

    double length() {
        if (isLength) return length_;
        isLength = true;
        for (int i = 1; i < dots_.size(); i++) {
            length_ += Point::calculate_side(dots_[i - 1], dots_[i]);
        }
        return length_;
    }

    const vector<Point> &dots() const {
        return dots_;
    }
};

class Closed_Polyline : public Polyline {
public:
    explicit Closed_Polyline(vector<Point> &points) : Polyline(points) {
        dots_.push_back(dots_[0]);
    };

    Closed_Polyline(const Closed_Polyline &other) = default;

    Closed_Polyline &operator=(const Closed_Polyline &other) = default;
};

class Polygon {
protected:
    bool isLength = false, isSquare = false;
    Closed_Polyline contour;
    double length_ = 0, square_ = 0;
public:
    explicit Polygon(Closed_Polyline &closedPolyline) : contour(closedPolyline) {
        for (int i = 0; i < contour.dots().size(); ++i) {

            double abx =
                    contour.dots()[i].x() - contour.dots()[(contour.dots().size() + i - 1) % contour.dots().size()].x(),
                    aby =
                    contour.dots()[i].y() - contour.dots()[(contour.dots().size() + i - 1) % contour.dots().size()].y();

            double bcx = contour.dots()[(i + 1) % contour.dots().size()].x() - contour.dots()[i].x(),
                    bcy = contour.dots()[(i + 1) % contour.dots().size()].y() - contour.dots()[i].y();

            double product = abx * bcy - aby * bcx;

            if (product < 0) throw std::invalid_argument("It is not Polygon");
        }
    }

    Polygon(const Polygon &other) = default;

    Polygon &operator=(const Polygon &other) = default;

    virtual double length() {
        if (isLength) return length_;
        isLength = true;
        for (int i = 1; i < contour.dots().size(); i++) {
            length_ += Point::calculate_side(contour.dots()[i - 1], contour.dots()[i]);
        }
        return isLength;
    }

    virtual double square() {
        if (isSquare) return square_;
        isSquare = true;
        for (int i = 0; i < contour.dots().size() - 1; i++) {
            square_ += contour.dots()[i].x() * contour.dots()[i + 1].y();
        }
        square_ += contour.dots()[contour.dots().size() - 1].x() * contour.dots()[0].y();
        for (int i = 0; i < contour.dots().size() - 1; i++) {
            square_ -= contour.dots()[i + 1].x() * contour.dots()[i].y();
        }
        square_ -= contour.dots()[0].x() * contour.dots()[contour.dots().size() - 1].y();
        return square_;
    };

};

class Triangle : public Polygon {
public:
    explicit Triangle(Closed_Polyline &closedPolyline) : Polygon(closedPolyline) {
        if (contour.dots().size() != 4) throw std::invalid_argument("It is not Triangle");
    }

    Triangle(const Triangle &other) = default;

    Triangle &operator=(const Triangle &other) = default;

    double square() override {
        if (isSquare) return square_;
        isSquare = true;
        double a = Point::calculate_side(contour.dots()[0], contour.dots()[1]);
        double b = Point::calculate_side(contour.dots()[1], contour.dots()[2]);
        double c = Point::calculate_side(contour.dots()[2], contour.dots()[3]);
        double p = 0.5 * (a + b + c);
        square_ = sqrt(p * (p - a) * (p - b) * (p - c));
        return square_;
    };
};

class Trapeze : public Polygon {
private:
    pair<double, double> a, b, c, d;
public:
    explicit Trapeze(Closed_Polyline &closedPolyline) : Polygon(closedPolyline) {
        a = make_pair(-(contour.dots()[1].y() - contour.dots()[0].y()),
                      (contour.dots()[1].x() - contour.dots()[0].x()));
        b = make_pair(-(contour.dots()[3].y() - contour.dots()[3].y()),
                      (contour.dots()[2].x() - contour.dots()[2].x()));
        c = make_pair(-(contour.dots()[2].y() - contour.dots()[2].y()),
                      (contour.dots()[1].x() - contour.dots()[1].x()));
        d = make_pair(-(contour.dots()[3].y() - contour.dots()[3].y()),
                      (contour.dots()[0].x() - contour.dots()[0].x()));
        if (a.first * b.first - a.second * b.second != 0 and
            c.first * d.first - c.second * d.second != 0 or contour.dots().size() != 5) {
            throw std::invalid_argument("It is not Trapeze");
        }
    }

    Trapeze(const Trapeze &other) = default;

    Trapeze &operator=(const Trapeze &other) = default;

    double square() override {
        if (isSquare) return square_;
        isSquare = true;
        if (a.first * b.first - a.second * b.second == 0) {
            double C1 = -a.first * contour.dots()[0].x() + a.second * contour.dots()[0].y();
            double C2 = -a.first * contour.dots()[2].x() + a.second * contour.dots()[2].y();
            square_ = (Point::calculate_side(contour.dots()[0], contour.dots()[1])
                       + Point::calculate_side(contour.dots()[2], contour.dots()[3])) / 2
                      * (abs(C2 - C1)) / sqrt(pow(a.first, 2) + pow(a.second, 2));
        } else {
            double C1 = -c.first * contour.dots()[1].x() + c.second * contour.dots()[1].y();
            double C2 = -c.first * contour.dots()[3].x() + c.second * contour.dots()[3].y();
            square_ = (Point::calculate_side(contour.dots()[1], contour.dots()[2])
                       + Point::calculate_side(contour.dots()[3], contour.dots()[0])) / 2
                      * (abs(C2 - C1)) / sqrt(pow(c.first, 2) + pow(c.second, 2));
        }
        return square_;
    };
};

class Correct_Polygon : public Polygon {
public:
    explicit Correct_Polygon(Closed_Polyline &closedPolyline) : Polygon(closedPolyline) {
        double side = Point::calculate_side(contour.dots()[0], contour.dots()[1]), ang = -1;
        for (int i = 2; i < contour.dots().size(); i++) {
            double l = Point::calculate_side(contour.dots()[i - 1], contour.dots()[i]);
            double angle =
                    (contour.dots()[i].x() - contour.dots()[i - 1].x()) *
                    (contour.dots()[i - 1].x() - contour.dots()[i - 2].x()) +
                    (contour.dots()[i].y() - contour.dots()[i - 1].y()) *
                    (contour.dots()[i - 1].y() - contour.dots()[i - 2].y()) /
                    (side * l);

            if (ang == -1) ang = angle;

            if (l != side or ang != angle) throw std::invalid_argument("It is not correct polygon");

            side = l;
            ang = angle;
        }
    }

    Correct_Polygon(const Correct_Polygon &other) = default;

    Correct_Polygon &operator=(const Correct_Polygon &other) = default;

    double length() override {
        if (isLength) return length_;
        isLength = true;
        length_ = (contour.dots().size() - 1) * Point::calculate_side(contour.dots()[0], contour.dots()[1]);
        return length_;
    }

    double square() override {
        if (isSquare) return square_;
        isSquare = true;
        square_ = ((contour.dots().size() - 1) * pow(Point::calculate_side(contour.dots()[0], contour.dots()[1]), 2)) /
                  (4 * tan(180 / (contour.dots().size() - 1)));
        return square_;
    };
};

/*int main(){
    vector<Point> a = {Point(1,1), Point(2,2), Point(3,2)};

    Figure f((Closed_Polyline(a)));
}*/