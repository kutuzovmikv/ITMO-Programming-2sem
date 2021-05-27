//
// Created by XyeCoC on 27.05.2021.
//

#ifndef LAB6_CUBE_H
#define LAB6_CUBE_H

#include <vector>
#include <unordered_map>
#include <functional>
#include <tuple>

#define GREEN "G"
#define ORANGE "O"
#define RED "R"
#define WHITE "W"
#define YELLOW "Y"
#define BLUE "B"

#define FRONT "Front"
#define LEFT "Left"
#define BACK "Back"
#define RIGHT "Right"
#define TOP "Top"
#define BOTTOM "Bottom"

void rot90(std::vector<std::vector<std::string>> &m, int k) {
    for (int r = 0; r < 3; r++) {
        for (int c = r; c < 3; c++) {
            swap(m[r][c], m[c][r]);
        }
    }

    if (k == 1) { // clockwise
        for (int r = 0; r < 3; r++) {
            for (int c = 0; c < 3 / 2; c++) {
                swap(m[r][c], m[r][3 - c - 1]);
            }
        }
    } else if (k == -1) { //counterclockwise
        for (int r = 0; r < 3 / 2; r++) {
            for (int c = 0; c < 3; c++) {
                swap(m[r][c], m[3 - r - 1][c]);
            }
        }
    }
}

std::vector<std::reference_wrapper<std::string>> flip(std::vector<std::reference_wrapper<std::string>> m) {
    std::reverse(m.begin(), m.end());
    return m;
}

std::vector<std::string> &flip(std::vector<std::string> &m) {
    std::reverse(m.begin(), m.end());
    return m;
}

std::vector<std::reference_wrapper<std::string>> column(std::vector<std::vector<std::string >> &m, int k) {
    std::vector<std::reference_wrapper<std::string>> ans{m[0][k], m[1][k], m[2][k]};
    return ans;
}

class Cube {
private:
    int fitness_ = 0;
    std::vector<std::vector<std::string>> move_history_;
    std::unordered_map<std::string, void (Cube::*)()> moves_lookup_{
            // horizontal
            {"D",  &Cube::D},
            {"D'", &Cube::D_prime},
            {"D2", &Cube::D2},
            {"E",  &Cube::E},
            {"E'", &Cube::E_prime},
            {"E2", &Cube::E2},
            {"U",  &Cube::U},
            {"U'", &Cube::U_prime},
            {"U2", &Cube::U2},
            // vertical
            {"L",  &Cube::L},
            {"L'", &Cube::L_prime},
            {"L2", &Cube::L2},
            {"R",  &Cube::R},
            {"R'", &Cube::R_prime},
            {"R2", &Cube::R2},
            {"M",  &Cube::M},
            {"M'", &Cube::M_prime},
            {"M2", &Cube::M2},
            // z
            {"B",  &Cube::B},
            {"B'", &Cube::B_prime},
            {"B2", &Cube::B2},
            {"F",  &Cube::F},
            {"F'", &Cube::F_prime},
            {"F2", &Cube::F2},
            {"S",  &Cube::S},
            {"S'", &Cube::S_prime},
            {"S2", &Cube::S2},
            // full rotations
            {"x",  &Cube::x_full},
            {"x'", &Cube::x_prime_full},
            {"x2", &Cube::x2_full},
            {"y",  &Cube::y_full},
            {"y'", &Cube::y_prime_full},
            {"y2", &Cube::y2_full},
            {"z",  &Cube::z_full},
            {"z'", &Cube::z_prime_full},
            {"z2", &Cube::z2_full},
    };
    std::unordered_map<std::string, std::vector<std::vector<std::string >>> faces_{
            {FRONT,  std::vector<std::vector<std::string>>(3, std::vector<std::string>(3, GREEN))},
            {LEFT,   std::vector<std::vector<std::string>>(3, std::vector<std::string>(3, ORANGE))},
            {RIGHT,  std::vector<std::vector<std::string>>(3, std::vector<std::string>(3, RED))},
            {TOP,    std::vector<std::vector<std::string>>(3, std::vector<std::string>(3, WHITE))},
            {BOTTOM, std::vector<std::vector<std::string>>(3, std::vector<std::string>(3, YELLOW))},
            {BACK,   std::vector<std::vector<std::string>>(3, std::vector<std::string>(3, BLUE))}
    };
public:
    void execute(const std::vector<std::string> &moves) {
        for (const std::string &m : moves) {
            moves_lookup_[m];
        }
        move_history_.push_back(moves);
        calculate_fitness();
    }

    void calculate_fitness() {
        int misplaced_stickers = 0;

        for (const auto&[k, face] : faces_) {
            std::string center = face[1][1];

            for (int i = 0; i < 3; i++)
                for (int j = 0; j < 3; j++)
                    if (face[i][j] != center)
                        misplaced_stickers++;
        }

        fitness_ = misplaced_stickers;
    }

    bool is_solved() const {
        if (fitness_ == 0)
            return true;
        return false;
    }

    void copy_stickers(std::vector<std::string> &destination, std::vector<std::string> &origin) {
        destination[0] = origin[0];
        destination[1] = origin[1];
        destination[2] = origin[2];
    }

    void
    copy_stickers(std::vector<std::string> &destination, std::vector<std::reference_wrapper<std::string>> origin) {
        destination[0] = origin[0];
        destination[1] = origin[1];
        destination[2] = origin[2];
    }

    void
    copy_stickers(std::vector<std::reference_wrapper<std::string>> destination, std::vector<std::string> &origin) {
        destination[0] = origin[0];
        destination[1] = origin[1];
        destination[2] = origin[2];
    }

    void
    copy_stickers(std::vector<std::reference_wrapper<std::string>> destination,
                  std::vector<std::reference_wrapper<std::string>> origin) {
        destination[0] = origin[0];
        destination[1] = origin[1];
        destination[2] = origin[2];
    }

    //----------------------------------------------------------------
    // X Axis movements - D, E and U
    //----------------------------------------------------------------

    void D() {
        rot90(faces_[BOTTOM], 1);
        swap_x(std::make_tuple(FRONT, 2), std::make_tuple(RIGHT, 2), std::make_tuple(BACK, 2),
               std::make_tuple(LEFT, 2));
    }

    void D_prime() {
        rot90(faces_[BOTTOM], -1);
        swap_x(std::make_tuple(FRONT, 2), std::make_tuple(LEFT, 2), std::make_tuple(BACK, 2),
               std::make_tuple(RIGHT, 2));
    }

    void D2() {
        D();
        D();
    }

    void E() {
        swap_x(std::make_tuple(FRONT, 1), std::make_tuple(RIGHT, 1), std::make_tuple(BACK, 1),
               std::make_tuple(LEFT, 1));
    }

    void E_prime() {
        swap_x(std::make_tuple(FRONT, 1), std::make_tuple(LEFT, 1), std::make_tuple(BACK, 1),
               std::make_tuple(RIGHT, 1));
    }

    void E2() {
        E();
        E();
    }

    void U() {
        rot90(faces_[TOP], 1);
        swap_x(std::make_tuple(FRONT, 0), std::make_tuple(LEFT, 0), std::make_tuple(BACK, 0),
               std::make_tuple(RIGHT, 0));
    }

    void U_prime() {
        rot90(faces_[TOP], -1);
        swap_x(std::make_tuple(FRONT, 0), std::make_tuple(RIGHT, 0), std::make_tuple(BACK, 0),
               std::make_tuple(LEFT, 0));
    }

    void U2() {
        U();
        U();
    }

    void swap_x(std::tuple<std::string, int> t1, std::tuple<std::string, int> t2, std::tuple<std::string, int> t3,
                std::tuple<std::string, int> t4) {
        std::vector<std::string> backup{"", "", ""};
        copy_stickers(backup, faces_[get<0>(t4)][get<1>(t4)]);
        copy_stickers(faces_[get<0>(t4)][get<1>(t4)], faces_[get<0>(t3)][get<1>(t3)]);
        copy_stickers(faces_[get<0>(t3)][get<1>(t3)], faces_[get<0>(t2)][get<1>(t2)]);
        copy_stickers(faces_[get<0>(t2)][get<1>(t2)], faces_[get<0>(t1)][get<1>(t1)]);
        copy_stickers(faces_[get<0>(t1)][get<1>(t1)], backup);
    }

    //----------------------------------------------------------------
    // Y Axis movements - L, R and M
    //----------------------------------------------------------------

    void L() {
        rot90(faces_[LEFT], 1);
        swap_y(std::make_tuple(BOTTOM, 0, true), std::make_tuple(BACK, 2, true), std::make_tuple(TOP, 0, false),
               std::make_tuple(FRONT, 0, false));
    }

    void L_prime() {
        rot90(faces_[LEFT], -1);
        swap_y(std::make_tuple(BOTTOM, 0, false), std::make_tuple(FRONT, 0, false), std::make_tuple(TOP, 0, true),
               std::make_tuple(BACK, 2, true));
    }

    void L2() {
        L();
        L();
    }

    void R() {
        rot90(faces_[RIGHT], 1);
        swap_y(std::make_tuple(BOTTOM, 2, false), std::make_tuple(FRONT, 2, false), std::make_tuple(TOP, 2, true),
               std::make_tuple(BACK, 0, true));
    }

    void R_prime() {
        rot90(faces_[RIGHT], -1);
        swap_y(std::make_tuple(BOTTOM, 2, true), std::make_tuple(BACK, 0, true), std::make_tuple(TOP, 2, false),
               std::make_tuple(FRONT, 2, false));
    }

    void R2() {
        R();
        R();
    }

    void M() {
        swap_y(std::make_tuple(BOTTOM, 1, true), std::make_tuple(BACK, 1, true), std::make_tuple(TOP, 1, false),
               std::make_tuple(FRONT, 1, false));
    }

    void M_prime() {
        swap_y(std::make_tuple(BOTTOM, 1, false), std::make_tuple(FRONT, 1, false), std::make_tuple(TOP, 1, true),
               std::make_tuple(BACK, 1, true));
    }

    void M2() {
        M();
        M();
    }

    void swap_y(std::tuple<std::string, int, bool> t1, std::tuple<std::string, int, bool> t2,
                std::tuple<std::string, int, bool> t3,
                std::tuple<std::string, int, bool> t4) {
        std::vector<std::string> backup{"", "", ""};

        if (get<2>(t4))
            copy_stickers(backup, flip(column(faces_[get<0>(t4)], get<1>(t4))));
        else
            copy_stickers(backup, column(faces_[get<0>(t4)], get<1>(t4)));

        if (get<2>(t3))
            copy_stickers(column(faces_[get<0>(t4)], get<1>(t4)), flip(column(faces_[get<0>(t3)], get<1>(t3))));
        else
            copy_stickers(column(faces_[get<0>(t4)], get<1>(t4)), column(faces_[get<0>(t3)], get<1>(t3)));

        if (get<2>(t2))
            copy_stickers(column(faces_[get<0>(t3)], get<1>(t3)), flip(column(faces_[get<0>(t2)], get<1>(t2))));
        else
            copy_stickers(column(faces_[get<0>(t3)], get<1>(t3)), column(faces_[get<0>(t2)], get<1>(t2)));

        if (get<2>(t1))
            copy_stickers(column(faces_[get<0>(t2)], get<1>(t2)), flip(column(faces_[get<0>(t1)], get<1>(t1))));
        else
            copy_stickers(column(faces_[get<0>(t2)], get<1>(t2)), column(faces_[get<0>(t1)], get<1>(t1)));

        copy_stickers(column(faces_[get<0>(t1)], get<1>(t1)), backup);
    }

    //----------------------------------------------------------------
    // Z Axis movements - B, F and S
    //----------------------------------------------------------------

    void B() {}

    void B_prime() {}

    void B2() {
        B();
        B();
    }

    void F() {}

    void F_prime() {}

    void F2() {
        F();
        F();
    }

    void S() {}

    void S_prime() {}

    void S2() {
        S();
        S();
    }

    void swap_z(std::tuple<std::string, int, bool> t1, std::tuple<std::string, int, bool> t2,
                std::tuple<std::string, int, bool> t3,
                std::tuple<std::string, int, bool> t4) {
        std::vector<std::string> backup{"", "", ""};

        if (get<2>(t4))
            copy_stickers(backup, flip(column(faces_[get<0>(t4)], get<1>(t4))));
        else
            copy_stickers(backup, column(faces_[get<0>(t4)], get<1>(t4)));

        if (get<2>(t3))
            copy_stickers(column(faces_[get<0>(t4)], get<1>(t4)), flip(faces_[get<0>(t3)][get<1>(t3)]));
        else
            copy_stickers(column(faces_[get<0>(t4)], get<1>(t4)), faces_[get<0>(t3)][get<1>(t3)]);

        if (get<2>(t2))
            copy_stickers(faces_[get<0>(t3)][get<1>(t3)], flip(column(faces_[get<0>(t2)], get<1>(t2))));
        else
            copy_stickers(faces_[get<0>(t3)][get<1>(t3)], column(faces_[get<0>(t2)], get<1>(t2)));

        if (get<2>(t1))
            copy_stickers(column(faces_[get<0>(t2)], get<1>(t2)), flip(faces_[get<0>(t1)][get<1>(t1)]));
        else
            copy_stickers(column(faces_[get<0>(t2)], get<1>(t2)), faces_[get<0>(t1)][get<1>(t1)]);

        copy_stickers(faces_[get<0>(t1)][get<1>(t1)], backup);
    }

    //----------------------------------------------------------------
    // Full rotations
    //----------------------------------------------------------------

    void x_full() {
        L_prime();
        M_prime();
        R();
    }

    void x_prime_full() {
        L();
        M();
        R_prime();
    }

    void x2_full() {
        x_full();
        x_full();
    }

    void y_full() {
        U();
        E_prime();
        D_prime();
    }

    void y_prime_full() {
        U_prime();
        E();
        D();
    }

    void y2_full() {
        y_full();
        y_full();
    }

    void z_full() {
        F();
        S();
        B_prime();
    }

    void z_prime_full() {
        F_prime();
        S_prime();
        B();
    }

    void z2_full() {
        z_full();
        z_full();
    }

    //----------------------------------------------------------------
    // Util
    //----------------------------------------------------------------

    void get_face_as_str(const std::string &face) {
        std::vector<std::vector<std::string>> m = faces_[face];
        std::cout << m[0][0] << " " << m[0][1] << " " << m[0][2] << " - ";
        std::cout << m[1][0] << " " << m[1][1] << " " << m[1][2] << " - ";
        std::cout << m[2][0] << " " << m[2][1] << " " << m[2][2];
    }

    std::vector<std::string> get_scramble() {
        return move_history_[0];
    }

    void get_scramble_str() {
        std::vector<std::string> m = get_scramble();
        for (const std::string &i : m)
            std::cout << i << " ";
    }

    std::vector<std::vector<std::string>> get_algorithm() {
        std::vector<std::vector<std::string>> ans;
        for (int i = 1; i < move_history_.size(); i++)
            ans.push_back(move_history_[i]);
        return ans;
    }

    void get_algorithm_str() {
        std::vector<std::vector<std::string>> ans = get_algorithm();
        for (int i = 0; i < ans.size(); i++)
            for (const std::string &j : ans[i])
                std::cout << i << " ";
    }

    int& fitness() {
        return fitness_;
    }

    std::unordered_map<std::string, std::vector<std::vector<std::string >>>& faces()  {
        return faces_;
    }

    std::vector<std::vector<std::string>> move_history() {
        return move_history_;
    }
};

struct CubeCmp {
    bool operator () (const Cube & a, const Cube & b) {
        return a.fitness() < b.fitness();
    }
};

#endif //LAB6_CUBE_H
