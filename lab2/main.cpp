#include <iostream>
#include <unordered_map>
#include <algorithm>
#include <vector>

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

    /*Polynom &operator /= (const Polynom &other) {

    }*/

    Polynom operator*(Polynom &other) const {
        Polynom temp;

        temp = *this;
        temp *= other;

        return temp;
    }

    friend std::ostream &operator<<(std::ostream &out, const Polynom &obj) {

        for(const auto &i : obj.ratios){
            out << i.first << " : " << i.second << "\n";
        }

        return out;
    }

    friend std::istream &operator>>(std::istream &in, Polynom &obj) {
        int degree;
        in >> degree;
        for (int i = 0; i < degree; i++) {
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
            {5, 6},
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
    Polynom C = A * B;
    cout << C;
}