#include <iostream>
#include "XML/pugixml.hpp"
#include <vector>
#include <map>
#include <algorithm>
#include <sstream>
#include <cmath>

struct Vehicle {
    std::map<std::string, std::vector<std::pair<float, float> >> vec;
    double longestpath = 0;
    unsigned stops_amount = 0;

    std::string countStops() {
        std::string route;
        for (auto &i : vec) {
            if (stops_amount < i.second.size()) {
                stops_amount = i.second.size();
                route = i.first;
            }
        }
        return route;
    }

    std::string countLength() {
        std::string route;



        return route;
    }
};

int main() {
    system("chcp 65001");
    pugi::xml_document doc;

    pugi::xml_parse_result result = doc.load_file("data.xml");

    pugi::xml_node dataset = doc.child("dataset");

    Vehicle bus;
    Vehicle tram;
    Vehicle trolleybus;

    std::string delimiter = ",", token;
    for (pugi::xml_node tool = dataset.child("transport_station"); tool; tool = tool.next_sibling()) {
        std::string routes_list = tool.child("routes").first_child().value();
        std::string coords_string = tool.child("coordinates").first_child().value();
        std::string vehicle = tool.child("type_of_vehicle").first_child().value();
        std::pair<float, float> coords;

        for (auto &i : routes_list) if (i == '.') i = ',';

        int pos = 0;
        while ((pos = coords_string.find(delimiter)) != std::string::npos) {
            token = coords_string.substr(0, pos);
            coords_string.erase(0, pos + delimiter.length());
            coords.first = std::stof(token);
            coords.second = std::stof(coords_string);
        }

        pos = 0;
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

    std::cout << bus.countStops() << " " << tram.countStops() << " " << trolleybus.countStops() << "\n";

    //std::cout << bus.countLength() << " " << tram.countLength() << " " << trolleybus.countLength() << "\n";

    return 0;
}
