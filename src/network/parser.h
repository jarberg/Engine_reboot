#pragma once

#include <json.hpp>
#include <core/vectors.h>

using json = nlohmann::json;


static Vec3 parse_vec(std::string msg) {

    json data = json::parse(msg);
    Vec3 pos = Vec3();
    if (data.contains("pos") && data["pos"].is_array()) {
        const auto& arr = data["pos"];
        if (data["pos"].size() > 0) pos.X = arr.at(0).get<float>();
        if (data["pos"].size() > 1) pos.Y = arr.at(1).get<float>();
        if (data["pos"].size() > 2) pos.Z = arr.at(2).get<float>();
        
    }
    return pos;
}
