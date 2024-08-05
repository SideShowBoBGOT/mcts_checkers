#pragma once

#include <mcts_checkers/checkers_data.hpp>
#include <nlohmann/json.hpp>

namespace nlohmann {
    template<>
    struct adl_serializer<mcts_checkers::BoardVector> {
        static void from_json(const json& j, mcts_checkers::BoardVector& data);
    };

    template<>
    struct adl_serializer<mcts_checkers::CheckersData> {
        static void from_json(const json& j, mcts_checkers::CheckersData& data);
    };
}