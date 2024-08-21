#pragma once

#include <mcts_checkers/checkers_data.hpp>
#include <mcts_checkers/utils.hpp>

namespace mcts_checkers {

    namespace turn_actions {
        using MakeAttack = std::vector<std::pair<CheckerIndex, CollectAttacksResult>>;
        using MakeMove = std::vector<std::pair<CheckerIndex, std::vector<MoveAction>>>;
        struct DeclareLoss { PlayerIndex m_player_index; };
        struct DeclareDraw {};
        using Type = std::variant<DeclareLoss, DeclareDraw, MakeAttack, MakeMove>;

        Type determine(const GameData& game_data);
    }

    ImVec2 calc_cell_size();
    ImVec2 calc_cell_top_left(const BoardVector board_index);

    constexpr ImVec4 normalize_rgba_color(const ImVec4 vec) {
        return {vec.x / UINT8_MAX, vec.y / UINT8_MAX, vec.z / UINT8_MAX, vec.w / UINT8_MAX};
    }

    const auto BLACK_COLOR = ImGui::ColorConvertFloat4ToU32(ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
    const auto WHITE_COLOR = ImGui::ColorConvertFloat4ToU32(ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
    const auto GREY_COLOR = ImGui::ColorConvertFloat4ToU32(ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
    const auto BLUE_COLOR = ImGui::ColorConvertFloat4ToU32(ImVec4(0.0f, 0.0f, 1.0f, 1.0f));
    const auto PURPLE_BLUE_COLOR = ImGui::ColorConvertFloat4ToU32(ImVec4(normalize_rgba_color({102.0f, 178.f, 255.0f, 255.0f})));
    const auto RED_COLOR = ImGui::ColorConvertFloat4ToU32(ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
    const auto PINK_COLOR = ImGui::ColorConvertFloat4ToU32(ImVec4(normalize_rgba_color({255.0f, 0.f, 127.0f, 255.0f})));
    const auto PURPLE_COLOR = ImGui::ColorConvertFloat4ToU32(normalize_rgba_color({153.0f, 51.f, 255.0f, 255.0f}));
    const auto YELLOW_COLOR = ImGui::ColorConvertFloat4ToU32(ImVec4(1.0f, 1.0f, 0.0f, 1.0f));
    const auto GREEN_COLOR = ImGui::ColorConvertFloat4ToU32(ImVec4(0.0f, 153.f / 255, 0.0f, 1.0f));
    const auto BOARD_CELL_ONE_COLOR = ImGui::ColorConvertFloat4ToU32(normalize_rgba_color({217.0f, 182.f, 140.0f, 255.0f}));
    const auto BOARD_CELL_TWO_COLOR = ImGui::ColorConvertFloat4ToU32(normalize_rgba_color({188.0f, 117.f, 65.0f, 255.0f}));
    const auto PLAYER_ONE_PAWN_COLOR = WHITE_COLOR;
    const auto PLAYER_TWO_PAWN_COLOR = BLACK_COLOR;
    const auto KING_HAT_COLOR = GREY_COLOR;


    namespace PlayerMessage {
        struct PlayerMadeNoSelection {};
        struct PlayerMadeSelection { GameData m_game_data; };
        struct DeclareLoss { PlayerIndex m_index; };
        struct DeclareDraw {};
        using Type = std::variant<
            PlayerMadeNoSelection,
            PlayerMadeSelection,
            DeclareLoss,
            DeclareDraw
        >;
    }
}