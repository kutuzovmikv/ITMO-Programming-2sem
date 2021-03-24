#include <iostream>
#include <unordered_map>
#include <algorithm>

using namespace std;

class Polynom {
private:
    unordered_map<int, double> ratios;
public:
    Polynom() = default;

    explicit Polynom(unordered_map<int, double> &indices) : ratios(indices) {}

    Polynom(const Polynom &other) = default;

    ~Polynom() = default;

    Polynom &operator=(const Polynom &other) = default;

    bool operator==(const Polynom &other) const {
        return ratios == other.ratios;
    }

    bool operator!=(const Polynom &other) const {
        return !(*this == other);
    }

    Polynom &operator+=(const Polynom &other) {
        for (const auto &i : other.ratios) {
            ratios[i.first] += i.second;
            if (ratios[i.first] == 0) ratios.erase(i.first);
        }

        return *this;
    }

    Polynom &operator-=(const Polynom &other) {
        for (const auto &i : other.ratios) {
            ratios[i.first] -= i.second;
            if (ratios[i.first] == 0) ratios.erase(i.first);
        }

        return *this;
    }

    Polynom operator+(const Polynom &other) const {
        Polynom temp;

        temp = *this;
        temp += other;

        return temp;
    }

    Polynom operator-() const {
        Polynom temp;

        temp -= *this;

        return temp;
    }

    Polynom operator-(const Polynom &other) const {
        Polynom temp;

        temp = *this;
        temp -= other;

        return temp;
    }

    Polynom &operator/(const double divider) {
        if (divider == 0) throw std::invalid_argument("You can't divide by zero");

        for (auto &i : ratios) {
            i.second /= divider;
        }

        return *this;
    }

    Polynom &operator*=(const Polynom &other) {
        Polynom temp;

        for (const auto &i : ratios) {
            for (const auto &j : other.ratios) {
                temp.ratios[i.first + j.first] += i.second * j.second;
            }
        }

        return *this = temp;
    }

    Polynom &operator/=(Polynom &other) {
        Polynom temp = *this;
        unsigned rdeg = temp.ratios.size() - other.ratios.size() + 1;
        Polynom res;
        for (int i = 0; i < rdeg; i++) {
            res.ratios[rdeg - i - 1] = temp.ratios[temp.ratios.size() - i - 1] / other.ratios[other.ratios.size() - 1];
            for (int j = 0; j < other.ratios.size(); j++) {
                temp.ratios[temp.ratios.size() - j - i - 1] -=
                        other.ratios[other.ratios.size() - j - 1] * res.ratios[rdeg - i - 1];
            }
        }
        return *this = res;
    }

    Polynom operator*(Polynom &other) const {
        Polynom temp;

        temp = *this;
        temp *= other;

        return temp;
    }

    friend std::ostream &operator<<(std::ostream &out, const Polynom &obj) {

        for (const auto &i : obj.ratios) {
            out << i.first << " : " << i.second << "\n";
        }

        return out;
    }

    friend std::istream &operator>>(std::istream &in, Polynom &obj) {
        unsigned degree;
        in >> degree;
        for (unsigned i = 0; i < degree; i++) {
            int key;
            double value;
            in >> key >> value;
            obj.ratios[key] = value;
        }
        return in;
    }

    double operator[](int number) {
        return ratios[number];
    }
};

int main() {
    unordered_map<int, double> a = {
            {2, 6},
            {1, 9},
            {0, -12}
    };
    unordered_map<int, double> b = {
            {2, 2},
            {1, 3},
            {0, -4}
    };
    Polynom A(a);
    Polynom B(b);
    Polynom C = A /= B;
    cout << C;
}