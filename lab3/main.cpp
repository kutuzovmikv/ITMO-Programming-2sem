#include <iostream>
#include "XML/pugixml.hpp"
#include <vector>
#include <map>
#include <algorithm>
#include <sstream>
#include <cmath>

int main() {
    system("chcp 65001");
    pugi::xml_document doc;

    pugi::xml_parse_result result = doc.load_file("data.xml");

    pugi::xml_node dataset = doc.child("dataset");

    std::map<std::string, std::vector<std::pair<float, float> >> bus;
    std::map<std::string, std::vector<std::pair<float, float> >> tram;
    std::map<std::string, std::vector<std::pair<float, float> >> trolleybus;

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
                bus[token].emplace_back(coords.first, coords.second);
            } else if (vehicle == "Трамвай") {
                tram[token].emplace_back(coords.first, coords.second);
            } else {
                trolleybus[token].emplace_back(coords.first, coords.second);
            }
            routes_list.erase(0, pos + delimiter.length());
        }
    }

    unsigned busmax = 0, trammax = 0, trolleybusmax = 0;
    for(auto &i : bus){
        busmax = std::max(busmax, i.second.size());
    }
    for(auto &i : tram){
        trammax = std::max(trammax, i.second.size());
    }
    for(auto &i : trolleybus){
        trolleybusmax = std::max(trolleybusmax, i.second.size());
    }
    std::cout << busmax << " " << trammax << " " << trolleybusmax << "\n";

    double buspath = 0, trampath = 0, trolleybuspath = 0;
    for(auto &i : bus){
        double path = 0;
        for(int j = 1; j < i.second.size(); j++){
            path += std::sqrt(pow(i.second[j].first - i.second[j-1].first,2) +
                    pow(i.second[j].second - i.second[j-1].second,2));
        }
        buspath = std::max(path, buspath);
    }
    for(auto &i : tram){
        double path = 0;
        for(int j = 1; j < i.second.size(); j++){
            path += std::sqrt(pow(i.second[j].first - i.second[j-1].first,2) +
                    pow(i.second[j].second - i.second[j-1].second,2));
        }
        trampath = std::max(path, trampath);
    }
    for(auto &i : trolleybus){
        double path = 0;
        for(int j = 1; j < i.second.size(); j++){
            path += std::sqrt(pow(i.second[j].first - i.second[j-1].first,2) +
                    pow(i.second[j].second - i.second[j-1].second,2));
        }
        trolleybuspath = std::max(path, trolleybuspath);
    }
    std::cout << buspath << " " << trampath << " " << trolleybuspath << "\n";



    return 0;
}
