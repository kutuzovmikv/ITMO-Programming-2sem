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

double sidex(vector<Point> &dots, unsigned a, unsigned b) {
    return dots[a].x - dots[b].x;
}

double sidey(vector<Point> &dots, unsigned a, unsigned b) {
    return dots[a].y - dots[b].y;
}

double calculate_side(vector<Point> &dots, unsigned a, unsigned b) {
    return sqrt(pow(sidex(dots, a, b), 2) + pow(sidey(dots, a, b), 2));
}

class Polyline {
public:
    bool isLength = false;
    double length = 0;
    vector<Point> dots;
public:
    explicit Polyline(vector<Point> &points) : dots(points) {}

    Polyline(const Polyline &other) = default;

    Polyline &operator=(const Polyline &other) = default;

    double calculate_length() {
        if (isLength) return length;
        isLength = true;
        for (int i = 1; i < dots.size(); i++) {
            length += calculate_side(dots, i - 1, i);
        }
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
    bool isLength = false, isSquare = false;
    Closed_Polyline contour;
    double length = 0, square = 0;
public:
    explicit Figure(Closed_Polyline &closedPolyline) : contour(closedPolyline) {}

    virtual double calculate_length() {
        if (isLength) return length;
        isLength = true;
        for (int i = 1; i < contour.dots.size(); i++) {
            length += calculate_side(contour.dots, i - 1, i);
        }
        return isLength;
    }

    virtual double calculate_square() {
        if (isSquare) return square;
        isSquare = true;
        for (int i = 0; i < contour.dots.size() - 1; i++) {
            square += contour.dots[i].x * contour.dots[i + 1].y;
        }
        square += contour.dots[contour.dots.size() - 1].x * contour.dots[0].y;
        for (int i = 0; i < contour.dots.size() - 1; i++) {
            square -= contour.dots[i + 1].x * contour.dots[i].y;
        }
        square -= contour.dots[0].x * contour.dots[contour.dots.size() - 1].y;
        return square;
    };
};

class Polygon : public Figure {
public:
    explicit Polygon(Closed_Polyline &closedPolyline) : Figure(closedPolyline) {
        for (int i = 0; i < contour.dots.size(); i++) {
            double abx, aby, bcx, bcy;
            if (i == 0) {
                abx = sidex(contour.dots, i, contour.dots.size() - 1), aby = sidey(contour.dots, i,
                                                                                   contour.dots.size() - 1);
                bcx = sidex(contour.dots, i + 1, i), bcy = sidey(contour.dots, i + 1, i);
            } else if (i == contour.dots.size() - 1) {
                abx = sidex(contour.dots, i, i - 1), aby = sidey(contour.dots, i, i - 1);
                bcx = sidex(contour.dots, 0, i), bcy = sidey(contour.dots, 0, i);
            } else {
                abx = sidex(contour.dots, i, i - 1), aby = sidey(contour.dots, i, i - 1);
                bcx = sidex(contour.dots, i + 1, i), bcy = sidey(contour.dots, i + 1, i);
            }
            try {
                if (abx * bcy - bcx * aby < 0) {
                    throw "It is not Polygon";
                }
            } catch (const char *exception) {
                cerr << "Error: " << exception << '\n';
            }
        }
    }

    Polygon(const Polygon &other) = default;

    Polygon &operator=(const Polygon &other) = default;
};

class Triangle : public Figure {
public:
    explicit Triangle(Closed_Polyline &closedPolyline) : Figure(closedPolyline) {
        try {
            if (contour.dots.size() != 4) throw "It is not Triangle";
        } catch (const char *exception) {
            cerr << "Error: " << exception << '\n';
        }
    }

    Triangle(const Triangle &other) = default;

    Triangle &operator=(const Triangle &other) = default;

    double calculate_square() override {
        if (isSquare) return square;
        isSquare = true;
        double a = calculate_side(contour.dots, 0, 1);
        double b = calculate_side(contour.dots, 1, 2);
        double c = calculate_side(contour.dots, 2, 3);
        double p = 0.5 * (a + b + c);
        square = sqrt(p * (p - a) * (p - b) * (p - c));
        return square;
    };
};

class Trapeze : public Figure {
private:
    pair<double, double> a, b, c, d;
public:
    explicit Trapeze(Closed_Polyline &closedPolyline) : Figure(closedPolyline) {
        a = make_pair(-sidey(contour.dots, 1, 0), sidex(contour.dots, 1, 0));
        b = make_pair(-sidey(contour.dots, 3, 2), sidex(contour.dots, 3, 2));
        c = make_pair(-sidey(contour.dots, 2, 1), sidex(contour.dots, 2, 1));
        d = make_pair(-sidey(contour.dots, 3, 0), sidex(contour.dots, 3, 0));
        try {
            if (a.first * b.first - a.second * b.second != 0 and c.first * d.first - c.second * d.second != 0 or contour.dots.size() != 5) {
                throw "It is not Trapeze";
            }
        } catch (const char *exception) {
            cerr << "Error: " << exception << '\n';
        }
    }

    Trapeze(const Trapeze &other) = default;

    Trapeze &operator=(const Trapeze &other) = default;

    double calculate_square() override {
        if (isSquare) return square;
        isSquare = true;
        if (a.first * b.first - a.second * b.second == 0) {
            double C1 = -a.first * contour.dots[0].x + a.second * contour.dots[0].y;
            double C2 = -a.first * contour.dots[2].x + a.second * contour.dots[2].y;
            square = (calculate_side(contour.dots, 0, 1) + calculate_side(contour.dots, 2, 3)) / 2
                     * (abs(C2 - C1)) / sqrt(pow(a.first, 2) + pow(a.second, 2));
        } else {
            double C1 = -c.first * contour.dots[1].x + c.second * contour.dots[1].y;
            double C2 = -c.first * contour.dots[3].x + c.second * contour.dots[3].y;
            square = (calculate_side(contour.dots, 1, 2) + calculate_side(contour.dots, 3, 0)) / 2
                     * (abs(C2 - C1)) / sqrt(pow(c.first, 2) + pow(c.second, 2));
        }
        return square;
    };
};

class Correct_Polygon : public Figure {
public:
    explicit Correct_Polygon(Closed_Polyline &closedPolyline) : Figure(closedPolyline) {
        double side = calculate_side(contour.dots, 0, 1), ang = -1;
        for (int i = 2; i < contour.dots.size(); i++) {
            double l = calculate_side(contour.dots, i - 1, i);
            double angle =
                    (contour.dots[i].x - contour.dots[i - 1].x) * (contour.dots[i - 1].x - contour.dots[i - 2].x) +
                    (contour.dots[i].y - contour.dots[i - 1].y) * (contour.dots[i - 1].y - contour.dots[i - 2].y) /
                    (side * l);
            if (ang == -1) ang = angle;
            try {
                if (l != side or ang != angle) throw "It is not correct polygon";

                side = l;
                ang = angle;
            } catch (const char *exception) {
                cerr << "Error: " << exception << '\n';
            }
        }
    }

    Correct_Polygon(const Correct_Polygon &other) = default;

    Correct_Polygon &operator=(const Correct_Polygon &other) = default;

    double calculate_length() override {
        if (isLength) return length;
        isLength = true;
        length = (contour.dots.size() - 1) * calculate_side(contour.dots, 0, 1);
        return length;
    }

    double calculate_square() override {
        if (isSquare) return square;
        isSquare = true;
        square = ((contour.dots.size() - 1) * pow(calculate_side(contour.dots, 0, 1), 2)) /
                 (4 * tan(180 / (contour.dots.size() - 1)));
        return square;
    };
};