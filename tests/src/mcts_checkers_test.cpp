#include <mcts_checkers_test/mcts_checkers_test.hpp>

namespace nlohmann {

    void adl_serializer<mcts_checkers::Vector<uint8_t>>::from_json(const json& j, mcts_checkers::Vector<uint8_t>& data) {
        const auto arr = j.get<std::array<uint8_t, 2>>();
        data.x = arr[0];
        data.y = arr[1];
    }
}