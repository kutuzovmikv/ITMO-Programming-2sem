#include <iostream>
#include <vector>
#include <algorithm>
#include <cassert>

using namespace std;

class Polynom {
private:
    vector<double> koefs;
public:
    Polynom() = default;

    explicit Polynom(vector<double> &k) : koefs(k) {}

    Polynom(const Polynom &other) = default;

    ~Polynom() = default;

    Polynom &operator=(const Polynom &other) = default;

    bool operator==(const Polynom &other) {
        for (int i = 0; i < koefs.size(); i++) {
            if (koefs[i] != other.koefs[i]) {
                return false;
            }
        }
        return true;
    }

    bool operator!=(const Polynom &other) {
        return !(*this == other);
    }

    Polynom operator+(Polynom &other) {
        Polynom temp;

        int slice = min(koefs.size(), other.koefs.size());

        for (int i = 0; i < slice; i++) {
            temp.koefs[i] = koefs[i] + other[i];
        }

        if (koefs.size() > other.koefs.size()) {
            for (int i = slice; i < koefs.size(); i++) {
                temp.koefs[i] = koefs[i];
            }
        } else {
            for (int i = slice; i < other.koefs.size(); i++) {
                temp.koefs[i] = other.koefs[i];
            }
        }

        return temp;
    }

    Polynom operator-() const {
        Polynom temp;
        for (int i = 0; i < koefs.size(); i++) {
            temp.koefs[i] = -koefs[i];
        }

        return temp;
    }

    Polynom operator-(Polynom &other) {
        Polynom temp;

        int slice = min(koefs.size(), other.koefs.size());

        for (int i = 0; i < slice; i++) {
            temp.koefs[i] = koefs[i] - other[i];
        }

        if (koefs.size() > other.koefs.size()) {
            for (int i = slice; i < koefs.size(); i++) {
                temp.koefs[i] = koefs[i];
            }
        } else {
            for (int i = slice; i < other.koefs.size(); i++) {
                temp.koefs[i] = -other.koefs[i];
            }
        }

        return temp;
    }

    Polynom &operator+=(Polynom &other) {

        int slice = min(koefs.size(), other.koefs.size());

        for (int i = 0; i < slice; i++) {
            koefs[i] += other[i];
        }

        if (koefs.size() < other.koefs.size()) {
            for (int i = slice; i < other.koefs.size(); i++) {
                koefs[i] = other.koefs[i];
            }
        }

        return *this;
    }

    Polynom &operator-=(Polynom &other) {
        int slice = min(koefs.size(), other.koefs.size());

        for (int i = 0; i < slice; i++) {
            koefs[i] -= other[i];
        }

        if (koefs.size() < other.koefs.size()) {
            for (int i = slice; i < other.koefs.size(); i++) {
                koefs[i] = -other.koefs[i];
            }
        }

        return *this;
    }

    Polynom &operator/(double divider) {
        assert(divider != 0);
        for (double &koef : koefs) {
            koef /= divider;
        }
        return *this;
    }

    Polynom operator*(Polynom &other) {
        Polynom temp;
        temp.koefs.resize(max(koefs.size(), other.koefs.size()) + min(koefs.size(), other.koefs.size()) - 1);

        for (int i = 0; i < koefs.size(); i++) {
            for (int j = 0; j < other.koefs.size(); j++) {
                temp.koefs[i + j] = koefs[i] * other.koefs[j];
            }
        }

        return temp;
    }

    Polynom &operator*=(Polynom &other) {
        koefs.resize(max(koefs.size(), other.koefs.size()) + min(koefs.size(), other.koefs.size()) - 1);

        for (int i = 0; i < koefs.size(); i++) {
            for (int j = 0; j < other.koefs.size(); j++) {
                koefs[i + j] = koefs[i] * other.koefs[j];
            }
        }

        return *this;
    }

    Polynom &operator/=(Polynom &other) {
        koefs.resize(max(koefs.size(), other.koefs.size()) + min(koefs.size(), other.koefs.size()) - 1);

        for (int i = 0; i < koefs.size(); i++) {
            for (int j = 0; j < other.koefs.size(); j++) {
                koefs[i + j] = koefs[i] / other.koefs[j];
            }
        }

        return *this;
    }

    friend std::ostream &operator<<(std::ostream &out, const Polynom &obj) {
        out << obj.koefs.size() << endl;
        for (double koef : obj.koefs) {
            out << koef << " ";
        }
        return out;
    }

    friend std::istream &operator>>(std::istream &in, Polynom &obj) {
        int degree;
        in >> degree;
        obj.koefs.resize(degree);
        for (int i = 0; i < degree; i++) {
            in >> obj.koefs[i];
        }
        return in;
    }

    double operator[](int number) {
        return koefs[number];
    }
};