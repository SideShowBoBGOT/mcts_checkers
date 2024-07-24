#include <mcts_checkers/board_form.hpp>
#include <mcts_checkers/utils.hpp>
#include <cstdio>

namespace mcts_checkers {

    BoardForm::BoardForm()=default;

    static constexpr uint8_t BOARD_SIDE_CELLS_COUNT = 10;
    static constexpr uint8_t BOARD_CELLS_COUNT = BOARD_SIDE_CELLS_COUNT * BOARD_SIDE_CELLS_COUNT;
    static const auto BLACK_COLOR = ImGui::ColorConvertFloat4ToU32(ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
    static const auto WHITE_COLOR = ImGui::ColorConvertFloat4ToU32(ImVec4(1.0f, 1.0f, 1.0f, 1.0f));

    static ImVec2 calc_cell_size() {
        return ImGui::GetWindowSize() / BOARD_SIDE_CELLS_COUNT;
    }

    void draw_rects() {
        const auto draw_list = ImGui::GetWindowDrawList();
        const auto form_pos = ImGui::GetCursorScreenPos();
        const auto cell_size = calc_cell_size();
        auto is_white = true;
        auto dev_y = form_pos.y;
        for(uint16_t y = 0; y < BOARD_SIDE_CELLS_COUNT; ++y) {
            auto dev_x = form_pos.x;
            for(uint16_t x = 0; x < BOARD_SIDE_CELLS_COUNT; ++x) {
                const auto deviation = ImVec2{dev_x, dev_y};
                draw_list->AddRectFilled(
                    deviation, deviation + cell_size, is_white ? WHITE_COLOR : BLACK_COLOR
                );
                is_white = not is_white;
                dev_x += cell_size.x;
            }
            dev_y += cell_size.y;
            is_white = not is_white;
        }
    }

    ImVec2 calc_mouse_local_window_pos() {
        return ImGui::GetMousePos() - ImGui::GetCursorScreenPos();
    }

    void draw_hovered_cell() {
        if(not ImGui::IsWindowHovered()) return;

        calc_mouse_local_window_pos()
    }

    void BoardForm::iter() {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        ImGui::BeginChild("BoardForm", ImVec2(0, -1), true, ImGuiWindowFlags_NoScrollWithMouse);

        ImGui::EndChild();
        ImGui::PopStyleVar();

    }

}