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
        for (const auto &i : vec) {
            if (stops_amount < i.second.size()) {
                stops_amount = i.second.size();
                route = i.first;
            }
        }
        return route;
    }

    std::string countLength() {
        std::string route;

        double biggestpath = -1;

        for (auto &i : vec) {
            std::vector<bool> used(i.second.size(), false);
            int counter = 1;

            int cur = 0;
            used[cur] = true;
            double p = 0;

            while (counter != i.second.size()) {
                double mpath = 1000;
                int mcur = 0;
                for (int j = 0; j < i.second.size(); j++) {
                    double path = sqrt(pow(i.second[cur].first - i.second[j].first, 2)
                                       + pow(i.second[cur].second - i.second[j].second, 2));
                    if (path < mpath && path != 0 && !used[j]) {
                        mpath = path;
                        mcur = j;
                    }
                }
                p += mpath;
                counter++;
                used[mcur] = true;
            }

            if (p > biggestpath) {
                biggestpath = p;
                route = i.first;
            }
        }

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

    std::map<std::string, int> streetsFrequency;

    std::string delimiter = ",", token;
    for (pugi::xml_node tool = dataset.child("transport_station"); tool; tool = tool.next_sibling()) {
        std::string routes_list = tool.child("routes").first_child().value();
        std::string coords_string = tool.child("coordinates").first_child().value();
        std::string vehicle = tool.child("type_of_vehicle").first_child().value();
        std::string streets = tool.child("location").first_child().value();
        std::pair<float, float> coords;

        int pos = 0;
        while ((pos = streets.find(delimiter)) != std::string::npos) {
            token = streets.substr(0, pos);
            streets.erase(0, pos + delimiter.length());
            auto itT = streetsFrequency.find(token), itS = streetsFrequency.find(streets);
            if(itT == streetsFrequency.end()){
                streetsFrequency[token] = 1;
            } else {
                streetsFrequency[token]++;
            }

            if(itS == streetsFrequency.end()){
                streetsFrequency[streets] = 1;
            } else {
                streetsFrequency[streets]++;
            }
        }

        for (auto &i : routes_list) if (i == '.') i = ',';

        pos = 0;
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
    std::cout << bus.countLength() << " " << tram.countLength() << " " << trolleybus.countLength() << "\n";

    std::string famousStreet;
    int bestStop = 0;

    for(const auto& i : streetsFrequency){
        if(i.second > bestStop){
            famousStreet = i.first;
            bestStop = i.second;
        }
    }

    std::cout << famousStreet << " " << bestStop << " stops";

    return 0;
}
