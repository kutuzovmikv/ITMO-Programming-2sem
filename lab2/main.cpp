#include <iostream>
#include <unordered_map>
#include <algorithm>

class Polynomial {
private:
    std::unordered_map<int, double> ratios;
public:
    Polynomial() = default;

    explicit Polynomial(std::unordered_map<int, double> &indices) : ratios(indices) {}

    Polynomial(const Polynomial &other) = default;

    ~Polynomial() = default;

    Polynomial &operator=(const Polynomial &other) = default;

    bool operator==(const Polynomial &other) const {
        return ratios == other.ratios;
    }

    bool operator!=(const Polynomial &other) const {
        return !(*this == other);
    }

    Polynomial &operator+=(const Polynomial &other) {
        for (const auto &i : other.ratios) {
            ratios[i.first] += i.second;
            if (ratios[i.first] == 0) ratios.erase(i.first);
        }

        return *this;
    }

    Polynomial &operator-=(const Polynomial &other) {
        for (const auto &i : other.ratios) {
            ratios[i.first] -= i.second;
            if (ratios[i.first] == 0) ratios.erase(i.first);
        }

        return *this;
    }

    Polynomial operator+(const Polynomial &other) const {
        Polynomial temp = *this;

        temp += other;

        return temp;
    }

    Polynomial operator-() const {
        Polynomial temp;

        temp -= *this;

        return temp;
    }

    Polynomial operator-(const Polynomial &other) const {
        Polynomial temp = *this;

        temp -= other;

        return temp;
    }

    Polynomial &operator/(const double divider) {
        if (divider == 0) throw std::invalid_argument("You can't divide by zero");

        for (auto &i : ratios) {
            i.second /= divider;
        }

        return *this;
    }

    Polynomial &operator*=(const Polynomial &other) {
        Polynomial temp;

        for (const auto &i : ratios) {
            for (const auto &j : other.ratios) {
                temp.ratios[i.first + j.first] += i.second * j.second;
            }
        }

        return *this = temp;
    }

    Polynomial &operator/=(Polynomial &other) {
        Polynomial temp = *this;
        unsigned deg = temp.ratios.size() - other.ratios.size() + 1;
        Polynomial res;
        for (int i = 0; i < deg; i++) {
            res.ratios[deg - i - 1] = temp.ratios[temp.ratios.size() - i - 1] / other.ratios[other.ratios.size() - 1];
            for (int j = 0; j < other.ratios.size(); j++) {
                temp.ratios[temp.ratios.size() - j - i - 1] -=
                        other.ratios[other.ratios.size() - j - 1] * res.ratios[deg - i - 1];
            }
        }
        return *this = res;
    }

    Polynomial operator*(Polynomial &other) const {
        Polynomial temp = *this;
        temp *= other;

        return temp;
    }

    friend std::ostream &operator<<(std::ostream &out, const Polynomial &obj) {

        for (const auto &i : obj.ratios) {
            out << i.first << " : " << i.second << "\n";
        }

        return out;
    }

    friend std::istream &operator>>(std::istream &in, Polynomial &obj) {
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
    std::unordered_map<int, double> a = {
            {2, 6},
            {1, 9},
            {0, -12}
    };
    std::unordered_map<int, double> b = {
            {2, 2},
            {1, 3},
            {0, -4}
    };
    Polynomial A(a);
    Polynomial B(b);
    Polynomial C = A /= B;
    std::cout << C;
}