#include <iostream>
#include <ctime>
#include "Cube.h"

std::vector<std::string> split(const std::string &s, const std::string &delimiter) {
    size_t pos_start = 0, pos_end, delim_len = delimiter.length();
    std::string token;
    std::vector<std::string> res;

    while ((pos_end = s.find(delimiter, pos_start)) != std::string::npos) {
        token = s.substr(pos_start, pos_end - pos_start);
        pos_start = pos_end + delim_len;
        res.push_back(token);
    }

    res.push_back(s.substr(pos_start));
    return res;
}

std::vector<std::string> SINGLE_MOVES = {"U", "U'", "U2", "D", "D'", "D2",
                                         "R", "R'", "R2", "L", "L'", "L2",
                                         "F", "F'", "F2", "B", "B'", "B2"};

std::vector<std::string> FULL_ROTATIONS = {"x", "x'", "x2", "y", "y'", "y2"};

std::vector<std::string> ORIENTATIONS = {"z", "z'", "z2"};

std::vector<std::vector<std::string>> PERMUTATIONS = {
        // permutes two edges: U face, bottom edge and right edge
        split("F' L' B' R' U' R U' B L F R U R' U", " "),
        // permutes two edges: U face, bottom edge and left edge
        split("F R B L U L' U B' R' F' L' U' L U'", " "),
        // permutes two corners: U face, bottom left and bottom right
        split("U2 B U2 B' R2 F R' F' U2 F' U2 F R'", " "),
        // permutes three corners: U face, bottom left and top left
        split("U2 R U2 R' F2 L F' L' U2 L' U2 L F'", " "),
        // permutes three centers: F face, top, right, bottom
        split("U' B2 D2 L' F2 D2 B2 R' U'", " "),
        // permutes three centers: F face, top, right, left
        split("U B2 D2 R F2 D2 B2 L U", " "),
        // U face: bottom edge <-> right edge, bottom right corner <-> top right corner
        split("D' R' D R2 U' R B2 L U' L' B2 U R2", " "),
        // U face: bottom edge <-> right edge, bottom right corner <-> left right corner
        split("D L D' L2 U L' B2 R' U R B2 U' L2", " "),
        // U face: top edge <-> bottom edge, bottom left corner <-> top right corner
        split("R' U L' U2 R U' L R' U L' U2 R U' L U'", " "),
        // U face: top edge <-> bottom edge, bottom right corner <-> top left corner
        split("L U' R U2 L' U R' L U' R U2 L' U R' U", " "),
        // permutes three corners: U face, bottom right, bottom left and top left
        split("F' U B U' F U B' U'", " "),
        // permutes three corners: U face, bottom left, bottom right and top right
        split("F U' B' U F' U' B U", " "),
        // permutes three edges: F face bottom, F face top, B face top
        split("L' U2 L R' F2 R", " "),
        // permutes three edges: F face top, B face top, B face bottom
        split("R' U2 R L' B2 L", " "),
        // H permutation: U Face, swaps the edges horizontally and vertically
        split("M2 U M2 U2 M2 U M2", " ")};

class Solver {
private:
    int population_size;
    int max_generations;
    int max_resets;
    int elitism_num;
public:
    Solver(const int &population_size, const int &max_generations, const int &max_resets, const int &elitism_num)
            : population_size(population_size), max_generations(max_generations), max_resets(max_resets),
              elitism_num(elitism_num) {}

    void solve(const std::vector<std::string> &scramble, bool verbose = false) const {
        time_t begin, end;
        time(&begin);

        if (verbose)
            std::cout << "Starting...\n";

        for (int r = 0; r < max_resets; r++) {
            std::vector<Cube> cubes;
            for (int i = 0; i < population_size; i++) {
                Cube cube = Cube();
                cube.execute(scramble);
                cube.execute(rnd_single_move());
                cube.execute(rnd_single_move());
                cubes.push_back(cube);
            }

            for (int g = 0; g < max_generations; g++) {
                std::sort(cubes.begin(), cubes.end(), CubeCmp());

                if (verbose and g % 20 == 0 and g != 0) {
                    std::cout << "World: " << r + 1 << " - Generation: " << g;
                    std::cout << "\nBest solution so far\n";
                    cubes[0].get_algorithm_str();
                    std::cout << "\n";
                }

                for (int i = 0; i < cubes.size(); i++) {
                    if (cubes[i].fitness() == 0) {
                        std::cout << "Solution found\n";
                        std::cout << "World: " << r + 1 << " - Generation: " << g << "\n";
                        cubes[i].get_scramble_str();
                        std::cout << "\nSolution\n";
                        cubes[i].get_algorithm_str();
                        std::cout << "\nMoves: " << cubes[i].get_algorithm().size();
                        time(&end);
                        std::cout << end - begin << " seconds\n";
                        return;
                    }

                    if (i > elitism_num) {
                        copy(cubes[i], cubes[rand() % elitism_num]);
                        int evolution_type = rand() % 6;

                        switch (evolution_type) {
                            case 0:
                                cubes[i].execute(rnd_permutation());
                                break;
                            case 1:
                                cubes[i].execute(rnd_permutation());
                                cubes[i].execute(rnd_permutation());
                                break;
                            case 2:
                                cubes[i].execute(rnd_full_rotation());
                                cubes[i].execute(rnd_permutation());
                                break;
                            case 3:
                                cubes[i].execute(rnd_orientation());
                                cubes[i].execute(rnd_permutation());
                                break;
                            case 4:
                                cubes[i].execute(rnd_full_rotation());
                                cubes[i].execute(rnd_orientation());
                                cubes[i].execute(rnd_permutation());
                                break;
                            case 5:
                                cubes[i].execute(rnd_orientation());
                                cubes[i].execute(rnd_full_rotation());
                                cubes[i].execute(rnd_permutation());
                                break;
                        }
                    }
                }
            }

            if(verbose)
                std::cout << "Resetting the world";

            std::cout << "Solution not found\n";
            time(&end);
            std::cout << end - begin << " seconds\n";
        }
    }

    void copy(Cube cube_to, Cube cube_from) const {
        for (const auto &f : cube_from.faces()) {
            for (int i = 0; i < 3; i++) {
                for (int j = 0; j < 3; j++) {
                    cube_to.faces()[f.first][i][j] = cube_from.faces()[f.first][i][j];
                }
            }
        }

        cube_to.move_history() = cube_from.move_history();
        cube_to.fitness() = cube_from.fitness();
    }

    std::vector<std::string> rnd_single_move() const {
        std::vector<std::string> ans{SINGLE_MOVES[rand() % SINGLE_MOVES.size()]};
        return ans;
    }

    std::vector<std::string> rnd_permutation() const {
        return PERMUTATIONS[rand() % PERMUTATIONS.size()];
    }

    std::vector<std::string> rnd_full_rotation() const {
        std::vector<std::string> ans{FULL_ROTATIONS[rand() % FULL_ROTATIONS.size()]};
        return ans;
    }

    std::vector<std::string> rnd_orientation() const {
        std::vector<std::string> ans{ORIENTATIONS[rand() % ORIENTATIONS.size()]};
        return ans;
    }
};

int main() {
    srand((unsigned) time(nullptr));

    std::string scramble_str = "U";
    std::vector<std::string> scramble = split(scramble_str, " ");

    int population_size = 500;
    int max_generations = 300;
    int max_resets = 10;
    int elitism_num = 50;

    Solver solver(population_size, max_generations, max_resets, elitism_num);
    solver.solve(scramble, true);

    return 0;
}
