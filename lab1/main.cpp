#include <iostream>
#include <vector>
#include <cmath>
#include <cassert>

using namespace std;

#define PI 3.14

class Point {
public:
    double x, y;

    Point(double xcoord, double ycoord) : x(xcoord), y(ycoord) {}

    Point(const Point &other) = default;

    Point &operator=(const Point &other) = default;
};

double sidex(vector<Point> &dots, int a, int b) {
    return dots[a].x - dots[b].x;
}

double sidey(vector<Point> &dots, int a, int b) {
    return dots[a].y - dots[b].y;
}

double calculate_side(vector<Point> &dots, int a, int b) {
    return sqrt(pow(sidex(dots, a, b), 2) + pow(sidey(dots, a, b), 2));
}

class Polyline {
protected:
    double length = 0;
    vector<Point> dots;
public:
    explicit Polyline(vector<Point> &points) : dots(points) {}

    Polyline(const Polyline &other) = default;

    Polyline &operator=(const Polyline &other) = default;

    void calculate_length() {
        for (int i = 1; i < dots.size(); i++) {
            length += calculate_side(dots, i - 1, i);
        }
    }

    double get_length() const {
        return length;
    }
};

class Closed_Polyline : public Polyline {
public:
    explicit Closed_Polyline(vector<Point> &points) : Polyline(points) {
        dots.push_back(dots[0]);
    };

    Closed_Polyline(const Closed_Polyline &other) = default;

    Closed_Polyline &operator=(const Closed_Polyline &other) = default;
};

class Figure {
public:
    vector<Point> dots;
    double length = 0, square = 0;
public:
    explicit Figure(vector<Point> &points) : dots(points) {
        dots.push_back(dots[0]);
    }

    virtual void calculate_length() {
        for (int i = 1; i < dots.size(); i++) {
            length += calculate_side(dots, i - 1, i);
        }
    }

    virtual void calculate_square() {
        for (int i = 0; i < dots.size() - 1; i++) {
            square += dots[i].x * dots[i + 1].y;
        }
        square += dots[dots.size() - 1].x * dots[0].y;
        for (int i = 0; i < dots.size() - 1; i++) {
            square -= dots[i + 1].x * dots[i].y;
        }
        square -= dots[0].x * dots[dots.size() - 1].y;
    };

    double get_square() const {
        return square;
    }

    double get_length() const {
        return length;
    }
};

class Polygon : public Figure {
public:
    explicit Polygon(vector<Point> &points) : Figure(points) {
        for (int i = 0; i < dots.size(); i++) {
            double abx, aby, bcx, bcy;
            if (i == 0) {
                abx = sidex(dots, i, dots.size() - 1), aby = sidey(dots, i, dots.size() - 1);
                bcx = sidex(dots, i + 1, i), bcy = sidey(dots, i + 1, i);
            } else if (i == dots.size() - 1) {
                abx = sidex(dots, i, i - 1), aby = sidey(dots, i, i - 1);
                bcx = sidex(dots, 0, i), bcy = sidey(dots, 0, i);
            } else {
                abx = sidex(dots, i, i - 1), aby = sidey(dots, i, i - 1);
                bcx = sidex(dots, i + 1, i), bcy = sidey(dots, i + 1, i);
            }
            assert(abx * bcy - bcx * aby >= 0);
        }
    }

    Polygon(const Polygon &other) = default;

    Polygon &operator=(const Polygon &other) = default;
};

class Triangle : public Figure {
public:
    explicit Triangle(vector<Point> &points) : Figure(points) {}

    Triangle(const Triangle &other) = default;

    Triangle &operator=(const Triangle &other) = default;

    void calculate_square() override {
        double a = calculate_side(dots, 0, 1);
        double b = calculate_side(dots, 1, 2);
        double c = calculate_side(dots, 2, 3);
        double p = 0.5 * (a + b + c);
        square = sqrt(p * (p - a) * (p - b) * (p - c));
    };
};

class Trapeze : public Figure {
private:
    pair<double, double> a, b, c, d;
public:
    explicit Trapeze(vector<Point> &points) : Figure(points) {
        a = make_pair(-sidey(dots, 1, 0), sidex(dots, 1, 0));
        b = make_pair(-sidey(dots, 3, 2), sidex(dots, 3, 2));
        c = make_pair(-sidey(dots, 2, 1), sidex(dots, 2, 1));
        d = make_pair(-sidey(dots, 3, 0), sidex(dots, 3, 0));
        assert (a.first * b.first - a.second * b.second == 0 or c.first * d.first - c.second * d.second == 0);
    }

    Trapeze(const Trapeze &other) = default;

    Trapeze &operator=(const Trapeze &other) = default;

    void calculate_square() override {
        if(a.first * b.first - a.second * b.second == 0){
            double C1 = - a.first * dots[0].x + a.second * dots[0].y;
            double C2 = - a.first * dots[2].x + a.second * dots[2].y;
            square = (calculate_side(dots, 0, 1) + calculate_side(dots, 2, 3))/2
                    * (abs(C2-C1))/sqrt(pow(a.first,2) + pow(a.second,2));
        } else {
            double C1 = - c.first * dots[1].x + c.second * dots[1].y;
            double C2 = - c.first * dots[3].x + c.second * dots[3].y;
            square = (calculate_side(dots, 1, 2) + calculate_side(dots, 3, 0))/2
                     * (abs(C2-C1))/sqrt(pow(c.first,2) + pow(c.second,2));
        }
    };
};

class Correct_Polygon : public Figure {
public:
    explicit Correct_Polygon(vector<Point> &points) : Figure(points) {}

    Correct_Polygon(const Correct_Polygon &other) = default;

    Correct_Polygon &operator=(const Correct_Polygon &other) = default;

    void calculate_length() override {
        length = (dots.size() - 1) * calculate_side(dots, 0, 1);
    }

    void calculate_square() override {
        square = ((dots.size() - 1) * pow(calculate_side(dots, 0, 1), 2)) / (4 * tan(180 / (dots.size() - 1)));
    };
};