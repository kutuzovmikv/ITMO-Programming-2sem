#include <iostream>
#include <unordered_map>
#include <algorithm>

using namespace std;

class Polynom {
public:
    unordered_map<unsigned, double> koefs;
public:
    Polynom() = default;

    explicit Polynom(unordered_map<unsigned, double> &k) : koefs(k) {}

    Polynom(const Polynom &other) = default;

    ~Polynom() = default;

    Polynom &operator=(const Polynom &other) = default;

    bool operator==(const Polynom &other) const {
        return koefs == other.koefs;
    }

    bool operator!=(const Polynom &other) const {
        return !(*this == other);
    }

    Polynom &operator+=(const Polynom &other) {
        for (auto i : other.koefs) {
            koefs[i.first] += i.second;
        }

        return *this;
    }

    Polynom &operator-=(const Polynom &other) {
        for (auto i : other.koefs) {
            koefs[i.first] -= i.second;
        }

        return *this;
    }

    Polynom operator+(const Polynom &other) const {
        Polynom temp;

        temp += *this;
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

        temp += *this;
        temp -= other;

        return temp;
    }

    Polynom &operator/(double divider) {
        try {
            if (divider == 0) throw "It is Coca Cola";
        } catch (const char *exception) {
            cerr << "Error: " << exception << '\n';
        }
        for (auto &koef : koefs) {
            koef.second /= divider;
        }
        return *this;
    }

    Polynom &operator*=(Polynom &other) {

        for (auto i : koefs) {
            for (auto j : other.koefs) {
                koefs[i.first + j.first] = i.second * j.second;
            }
        }

        return *this;
    }

    Polynom &operator/=(Polynom &other) {

        for (auto i : koefs) {
            for (auto j : other.koefs) {
                try {
                    if (j.second == 0) throw "It is Coca Cola";
                    koefs[i.first - j.first] = i.second / j.second;
                } catch (const char *exception) {
                    cerr << "Error: " << exception << '\n';
                }
            }
        }

        return *this;
    }

    Polynom operator*(Polynom &other) const {
        Polynom temp;

        temp = *this;
        temp *= other;

        return temp;
    }

    friend std::ostream &operator<<(std::ostream &out, const Polynom &obj) {
        for (auto koef : obj.koefs) {
            out << koef.first << " : " << koef.second << "\n";
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
            obj.koefs[key] = value;
        }
        return in;
    }

    double operator[](int number) {
        return koefs[number];
    }
};

int main(){
    unordered_map <unsigned, double> a = {
            {1, 2},
            {2,3},
            {3, 4}
    };
    unordered_map <unsigned, double> b = {
            {1, 16}
    };
    Polynom A(a);
    Polynom B(b);
    Polynom C = A * B;
    cout << C;
}