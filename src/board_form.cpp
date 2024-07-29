#include <mcts_checkers/board_form.hpp>
#include <mcts_checkers/utils.hpp>
#include <mcts_checkers/checkers_data.hpp>
#include <tl/optional.hpp>

namespace mcts_checkers::board_form {

    Form::Form()=default;

    static consteval ImVec4 normalize_rgba_color(const ImVec4 vec) {
        return {vec.x / 255, vec.y / 255, vec.z / 255, vec.w / 255};
    }

    static const auto BLACK_COLOR = ImGui::ColorConvertFloat4ToU32(ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
    static const auto WHITE_COLOR = ImGui::ColorConvertFloat4ToU32(ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
    static const auto GREY_COLOR = ImGui::ColorConvertFloat4ToU32(ImVec4(0.5f, 0.5f, 0.5f, 1.0f));

    static const auto BLUE_COLOR = ImGui::ColorConvertFloat4ToU32(ImVec4(0.0f, 0.0f, 1.0f, 1.0f));
    // static const auto RED_COLOR = ImGui::ColorConvertFloat4ToU32(ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
    static const auto YELLOW_COLOR = ImGui::ColorConvertFloat4ToU32(ImVec4(1.0f, 1.0f, 0.0f, 1.0f));
    static const auto GREEN_COLOR = ImGui::ColorConvertFloat4ToU32(ImVec4(0.0f, 153.f / 255, 0.0f, 1.0f));

    static const auto BOARD_CELL_ONE_COLOR = ImGui::ColorConvertFloat4ToU32(normalize_rgba_color({217.0f, 182.f, 140.0f, 255.0f}));
    static const auto BOARD_CELL_TWO_COLOR = ImGui::ColorConvertFloat4ToU32(normalize_rgba_color({188.0f, 117.f, 65.0f, 255.0f}));
    static const auto PLAYER_ONE_PAWN_COLOR = WHITE_COLOR;
    static const auto PLAYER_TWO_PAWN_COLOR = BLACK_COLOR;
    static const auto KING_HAT_COLOR = GREY_COLOR;

    static ImVec2 calc_cell_size() {
        return ImGui::GetWindowSize() / BOARD_SIDE_CELL_COUNT;
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

    constexpr bool is_even(const int16_t value) {
        return value % 2 == 0;
    }

    constexpr uint8_t convert_board_index(const ImVec2ih board_index) {
        return board_index.y * BOARD_SIDE_CELL_COUNT + board_index.x;
    }

    constexpr bool is_white_cell(const ImVec2ih cell_index) {
        return is_even(cell_index.y) == is_even(cell_index.x);
    }

    tl::optional<uint8_t> convert_board_index_to_checker_index(const ImVec2ih cell_index) {
        if(is_white_cell(cell_index)) {
            return tl::nullopt;
        }
        return cell_index.y * (BOARD_SIDE_CELL_COUNT / 2) + cell_index.x / 2;
    }

    void StateUnselected::iter(const ProtocolStateChanger<Form> state_changer, const CheckersData& checkers_data) {
        if(not ImGui::IsWindowHovered(ImGuiHoveredFlags_None)) return;

        const auto mouse_pos = calc_mouse_local_window_pos();
        const auto cell_size = calc_cell_size();
        const auto float_cell_index = mouse_pos / cell_size;
        const auto cell_index = ImVec2ih(
            static_cast<short>(float_cell_index.x),
            static_cast<short>(float_cell_index.y)
        );

        const auto p_min = cell_size * ImVec2(cell_index.x, cell_index.y);
        const auto p_max = p_min + cell_size;

        const auto form_pos = ImGui::GetCursorScreenPos();

        const auto real_p_min = p_min + form_pos;
        const auto real_p_max = p_max + form_pos;

        if(const auto checker_index_opt = convert_board_index_to_checker_index(cell_index)) {
            const auto checker_index = *checker_index_opt;
            if(checkers_data.m_is_in_place[checker_index] and checkers_data.m_player_index[checker_index] == checkers_data.m_current_player_index) {
                ImGui::GetWindowDrawList()->AddRect(real_p_min, real_p_max, GREEN_COLOR, 2, 0, 8);
                return;
            }
        }
        ImGui::GetWindowDrawList()->AddRect(real_p_min, real_p_max, YELLOW_COLOR, 2, 0, 8);
    }

    void StateSelected::iter(const ProtocolStateChanger<Form> state_changer, const CheckersData& checkers_data) {
    }

    void StateSelectionConfirmed::iter(const ProtocolStateChanger<Form> state_changer, const CheckersData& checkers_data) {

    }

    static void draw_rects() {
        const auto draw_list = ImGui::GetWindowDrawList();
        const auto form_pos = ImGui::GetCursorScreenPos();
        const auto cell_size = calc_cell_size();
        auto is_white = true;
        auto dev_y = form_pos.y;
        for(uint16_t y = 0; y < BOARD_SIDE_CELL_COUNT; ++y) {
            auto dev_x = form_pos.x;
            for(uint16_t x = 0; x < BOARD_SIDE_CELL_COUNT; ++x) {
                const auto deviation = ImVec2{dev_x, dev_y};
                draw_list->AddRectFilled(
                    deviation, deviation + cell_size, is_white ? BOARD_CELL_ONE_COLOR : BOARD_CELL_TWO_COLOR
                );
                is_white = not is_white;
                dev_x += cell_size.x;
            }
            dev_y += cell_size.y;
            is_white = not is_white;
        }
    }

    static void draw_checkers(const CheckersData& data) {
        const auto draw_list = ImGui::GetWindowDrawList();
        const auto cell_size = calc_cell_size();
        const auto half_cell_size = cell_size / 2;
        const auto pawn_radius = half_cell_size * 0.8;
        const auto king_hat_radius = half_cell_size / 2;
        const auto form_pos = ImGui::GetCursorScreenPos();
        for(uint8_t y = 0, checker_index = 0; y < BOARD_SIDE_CELL_COUNT; ++y) {
            for(uint8_t x = y % 2 == 0 ? 1 : 0; x < BOARD_SIDE_CELL_COUNT; x += 2, ++checker_index) {
                if(not data.m_is_in_place[checker_index]) continue;
                const auto center = ImVec2(x, y) * cell_size + half_cell_size + form_pos;
                const auto player_index = data.m_player_index[checker_index];
                draw_list->AddEllipseFilled(center, pawn_radius, player_index ? PLAYER_ONE_PAWN_COLOR : PLAYER_TWO_PAWN_COLOR);
                if(data.m_is_king[checker_index]) {
                    draw_list->AddEllipseFilled(center, king_hat_radius, KING_HAT_COLOR);
                }
            }
        }
    }

    void Form::iter(const CheckersData& checkers_data) {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        ImGui::BeginChild("BoardForm", ImVec2(0, -1), true, ImGuiWindowFlags_NoScrollWithMouse);
        draw_rects();
        draw_checkers(checkers_data);

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