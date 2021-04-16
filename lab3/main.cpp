#include <iostream>
#include "XML/pugixml.hpp"
#include <vector>
#include <unordered_map>
#include <sstream>
#include <cmath>
#include <algorithm>

struct Vehicle {
    std::unordered_map<std::string, std::vector<std::pair<float, float> >> vec;

    std::string countStops() {
        std::string route;

        auto x = std::max_element(vec.begin(), vec.end(),
                                  [](const std::pair<std::string, std::vector<std::pair<float, float> >> &p1,
                                     const std::pair<std::string, std::vector<std::pair<float, float> >> &p2) {
                                      return p1.second.size() < p2.second.size();
                                  });

        return x->first;
    }

    std::string countLength() {
        std::string route;

        double biggestPath = -1;

        for (const auto &i : vec) {
            std::vector<bool> used(i.second.size(), false);
            int counter = 1;

            int cur = 0;
            used[cur] = true;
            double p = 0;

            while (counter != i.second.size()) {
                double minPath = 1000000;
                int minCur = 0;
                for (int j = 0; j < i.second.size(); j++) {
                    double path = sqrt(pow(i.second[cur].first - i.second[j].first, 2)
                                       + pow(i.second[cur].second - i.second[j].second, 2));
                    if (path < minPath && path != 0 && !used[j]) {
                        minPath = path;
                        minCur = j;
                    }
                }
                p += minPath;
                counter++;
                used[minCur] = true;
            }

            if (p > biggestPath) {
                biggestPath = p;
                route = i.first;
            }
        }

        return route;
    }
};

int main() {
    system("chcp 65001");
    //setlocale(LC_ALL, 0);
    pugi::xml_document doc;

    pugi::xml_parse_result result = doc.load_file("data.xml");

    pugi::xml_node dataset = doc.child("dataset");

    Vehicle bus;
    Vehicle tram;
    Vehicle trolleybus;

    std::unordered_map<std::string, int> streetsFrequency;

    std::string delimiter = ",", token;
    for (pugi::xml_node tool = dataset.child("transport_station"); tool; tool = tool.next_sibling()) {
        std::string routes_list = tool.child("routes").first_child().value();
        std::string coords_string = tool.child("coordinates").first_child().value();
        std::string vehicle = tool.child("type_of_vehicle").first_child().value();
        std::string streets = tool.child("location").first_child().value();
        std::pair<float, float> coords;

        unsigned pos;
        while ((pos = streets.find(delimiter)) != std::string::npos) {
            token = streets.substr(0, pos);
            streets.erase(0, pos + delimiter.length());

            streetsFrequency[token]++;

            streetsFrequency[streets]++;

        }

        for (auto &i : routes_list) if (i == '.') i = ',';

        while ((pos = coords_string.find(delimiter)) != std::string::npos) {
            token = coords_string.substr(0, pos);
            coords_string.erase(0, pos + delimiter.length());
            coords.first = std::stof(token);
            coords.second = std::stof(coords_string);
        }

        while ((pos = routes_list.find(delimiter)) != std::string::npos) {
            token = routes_list.substr(0, pos);
            if (vehicle == "Автобус") {
                bus.vec[token].emplace_back(coords.first, coords.second);
            } else if (vehicle == "Трамвай") {
                tram.vec[token].emplace_back(coords.first, coords.second);
            } else {
                trolleybus.vec[token].emplace_back(coords.first, coords.second);
            }
            routes_list.erase(0, pos + delimiter.length());
        }
    }

    std::cout << "Max bus stops : " << bus.countStops() << "\nMax tram stops : " << tram.countStops()
              << "\nMax trolleybus stops : " << trolleybus.countStops() << "\n";
    std::cout << "Longest bus route" << bus.countLength() << "\nLongest tram route " << tram.countLength()
              << "\nLongest trolleybus route " << trolleybus.countLength() << "\n";

    auto famousStreet = std::max_element(streetsFrequency.begin(), streetsFrequency.end(),
                                         [](const std::pair<std::string, int> &p1,
                                            const std::pair<std::string, int> &p2) {
                                             return p1.second < p2.second;
                                         });

    std::cout << "Most famous street: " << famousStreet->first << " " << famousStreet->second << " stops";

    return 0;
}
