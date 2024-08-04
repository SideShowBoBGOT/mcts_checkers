#pragma once

#include <mcts_checkers/checkers_data.hpp>
#include <nlohmann/json.hpp>

namespace nlohmann {
    template<>
    struct adl_serializer<mcts_checkers::Vector<uint8_t>> {
        static void from_json(const json& j, mcts_checkers::Vector<uint8_t>& data);
    };
}