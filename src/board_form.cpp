#include <mcts_checkers/board_form.hpp>
#include <mcts_checkers/utils.hpp>
#include <mcts_checkers/checkers_data.hpp>

namespace mcts_checkers::board_form {

    Form::Form()=default;

    static constexpr uint8_t BOARD_SIDE_CELLS_COUNT = 10;
    static constexpr uint8_t BOARD_CELLS_COUNT = BOARD_SIDE_CELLS_COUNT * BOARD_SIDE_CELLS_COUNT;
    static const auto BLACK_COLOR = ImGui::ColorConvertFloat4ToU32(ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
    static const auto WHITE_COLOR = ImGui::ColorConvertFloat4ToU32(ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
    static const auto BLUE_COLOR = ImGui::ColorConvertFloat4ToU32(ImVec4(0.0f, 0.0f, 1.0f, 1.0f));

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
        if(not ImGui::IsWindowHovered(ImGuiHoveredFlags_None)) return;

        const auto mouse_pos = calc_mouse_local_window_pos();
        // std::printf("Mouse pos: (%f, %f)\n", mouse_pos.x, mouse_pos.y);

        const auto cell_size = calc_cell_size();
        const auto float_cell_index = mouse_pos / cell_size;
        const auto cell_index = ImVec2(
            static_cast<short>(float_cell_index.x),
            static_cast<short>(float_cell_index.y)
        );

        const auto p_min = cell_size * cell_index;
        const auto p_max = p_min + cell_size;

        const auto form_pos = ImGui::GetCursorScreenPos();

        const auto real_p_min = p_min + form_pos;
        const auto real_p_max = p_max + form_pos;
        ImGui::GetWindowDrawList()->AddRect(real_p_min, real_p_max, BLUE_COLOR, 2, 0, 8);
    }

    void StateUnselected::iter(const ProtocolStateChanger<Form> state_changer, const CheckersData& checkers_data) {
        draw_hovered_cell();
    }

    void StateSelected::iter(const ProtocolStateChanger<Form> state_changer, const CheckersData& checkers_data) {

    }

    void StateSelectionConfirmed::iter(const ProtocolStateChanger<Form> state_changer, const CheckersData& checkers_data) {

    }

    // struct StateIterVisitor {
    //     void operator()(State&& state) {
    //     }
    //
    //     Form& m_form;
    //     const CheckersData& m_checkers_data;
    // };

    void Form::iter(const CheckersData& checkers_data) {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        ImGui::BeginChild("BoardForm", ImVec2(0, -1), true, ImGuiWindowFlags_NoScrollWithMouse);
        draw_rects();

        std::visit([this, &checkers_data](auto& state) {
            state.iter(*this, checkers_data);
        }, m_state);

        ImGui::EndChild();
        ImGui::PopStyleVar();
    }

    void Form::change_state(State&& state) {
        m_state = utils::checked_move(state);
    }

}