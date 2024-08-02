#include <gtest/gtest.h>

#include <mcts_checkers/checkers_data.hpp>
#include <filesystem>
#include <fstream>
#include <nlohmann/json.hpp>
#include <string_view>
#include <ranges>

struct TestData {

};

namespace nlohmann {
    template<>
        struct adl_serializer<::mcts_checkers::Vector<uint8_t>> {
        static void from_json(const json& j, ::mcts_checkers::Vector<uint8_t>& data) {
            const auto arr = j.get<std::array<uint8_t, 2>>();
            data.x = arr[0];
            data.y = arr[1];
        }
    };

    template<>
    struct adl_serializer<::mcts_checkers::CheckersData> {
        static void from_json(const json& j, ::mcts_checkers::CheckersData& data) {
            for(
                const auto& line : j
                | std::views::drop(1)
                | std::views::take(::mcts_checkers::CELLS_PER_SIDE)
            ) {
                auto line = line.get<std::string_view>();
            }
        }
    };
}

TEST(TestAttacks, Sanity) {
    auto file_stream = std::ifstream("../../tests/test_attacks_0.json");
    auto json_file = nlohmann::json();
    file_stream >> json_file;
    for(const auto& j : json_file["board"]) {
        std::cout << j.get<std::string>() << "\n";
    }

}
