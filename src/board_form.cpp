#include <mcts_checkers/board_form.hpp>
#include <mcts_checkers/utils.hpp>
#include <mcts_checkers/checkers_data.hpp>
#include <mcts_checkers/checkers_funcs.hpp>
#include <tl/optional.hpp>

namespace mcts_checkers::board {

    Form::Form()=default;

    static consteval ImVec4 normalize_rgba_color(const ImVec4 vec) {
        return {vec.x / 255, vec.y / 255, vec.z / 255, vec.w / 255};
    }

    static const auto BLACK_COLOR = ImGui::ColorConvertFloat4ToU32(ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
    static const auto WHITE_COLOR = ImGui::ColorConvertFloat4ToU32(ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
    static const auto GREY_COLOR = ImGui::ColorConvertFloat4ToU32(ImVec4(0.5f, 0.5f, 0.5f, 1.0f));

    static const auto BLUE_COLOR = ImGui::ColorConvertFloat4ToU32(ImVec4(0.0f, 0.0f, 1.0f, 1.0f));
    static const auto PURPLE_BLUE_COLOR = ImGui::ColorConvertFloat4ToU32(ImVec4(normalize_rgba_color({102.0f, 178.f, 255.0f, 255.0f})));
    static const auto RED_COLOR = ImGui::ColorConvertFloat4ToU32(ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
    static const auto PINK_COLOR = ImGui::ColorConvertFloat4ToU32(ImVec4(normalize_rgba_color({255.0f, 0.f, 127.0f, 255.0f})));

    static const auto PURPLE_COLOR = ImGui::ColorConvertFloat4ToU32(normalize_rgba_color({153.0f, 51.f, 255.0f, 255.0f}));

    static const auto YELLOW_COLOR = ImGui::ColorConvertFloat4ToU32(ImVec4(1.0f, 1.0f, 0.0f, 1.0f));
    static const auto GREEN_COLOR = ImGui::ColorConvertFloat4ToU32(ImVec4(0.0f, 153.f / 255, 0.0f, 1.0f));

    static const auto BOARD_CELL_ONE_COLOR = ImGui::ColorConvertFloat4ToU32(normalize_rgba_color({217.0f, 182.f, 140.0f, 255.0f}));
    static const auto BOARD_CELL_TWO_COLOR = ImGui::ColorConvertFloat4ToU32(normalize_rgba_color({188.0f, 117.f, 65.0f, 255.0f}));
    static const auto PLAYER_ONE_PAWN_COLOR = WHITE_COLOR;
    static const auto PLAYER_TWO_PAWN_COLOR = BLACK_COLOR;
    static const auto KING_HAT_COLOR = GREY_COLOR;

    static ImVec2 calc_cell_size() {
        return ImGui::GetWindowSize() / CELLS_PER_SIDE;
    }

    ImVec2 calc_mouse_local_window_pos() {
        return ImGui::GetMousePos() - ImGui::GetCursorScreenPos();
    }

    ImVec2 convert_board_vector_to_imvec(const BoardVector board_index) {
        return ImVec2{
            static_cast<float>(board_index.x),
            static_cast<float>(board_index.y)
        };
    }

    BoardVector convert_imvec_to_board_vector(const ImVec2 imvec) {
        return BoardVector{
            static_cast<uint8_t>(imvec.x),
            static_cast<uint8_t>(imvec.y)
        };
    }

    ImVec2 calc_cell_top_left(const BoardVector board_index) {
        return calc_cell_size() * convert_board_vector_to_imvec(board_index) + ImGui::GetCursorScreenPos();
    }

    constexpr uint8_t convert_board_index(const ImVec2ih board_index) {
        return board_index.y * CELLS_PER_SIDE + board_index.x;
    }

    constexpr bool is_white_cell(const BoardVector cell_index) {
        return is_even(cell_index.y) == is_even(cell_index.x);
    }

    tl::optional<CheckerIndex> try_convert_board_vector_to_checker_index(const BoardVector cell_index) {
        if(is_white_cell(cell_index)) {
            return tl::nullopt;
        }
        return convert_board_vector_to_checker_index(cell_index);
    }

    BoardVector calc_hovered_cell() {
        return convert_imvec_to_board_vector(calc_mouse_local_window_pos() / calc_cell_size());
    }

    void draw_hovered_cell(const BoardVector checker_board_vector, const ImU32 color) {
        const auto cell_size = calc_cell_size();
        const auto p_min = calc_cell_top_left(checker_board_vector);
        const auto p_max = p_min + cell_size;
        ImGui::GetWindowDrawList()->AddRect(p_min, p_max, color, 0, 0, 8);
    }

    struct StateNotChange{};

    using IterationResult = std::variant<
        StateNotChange,
        unselected::MoveForm,
        unselected::AttackForm,
        selected::MoveForm,
        selected::attack::Form,
        SelectionConfirmed
    >;

    bool is_current_player_checker(const GameData& game_data, const CheckerIndex checker_index) {
        return game_data.checkers.m_is_in_place[checker_index]
            and game_data.checkers.m_player_index[checker_index] == game_data.m_current_player_index;
    }

    template<typename Callable>
    void iterate_over_current_player_checkers(const GameData& game_data, Callable&& callable) {
        for(uint8_t i = 0; i < CHEKCERS_CELLS_COUNT; ++i) {
            const auto checker_index = CheckerIndex{i};
            if(is_current_player_checker(game_data, checker_index)) {
                callable(game_data, checker_index);
            }
        }
    }

    namespace unselected_selected_common {
        template<typename SelectedForm, typename Form>
        std::variant<StateNotChange, SelectedForm> iter(Form& form) {
            for(const auto& el : form.m_actions) {
                draw_hovered_cell(convert_checker_index_to_board_vector(el.first), PURPLE_BLUE_COLOR);
            }

            if(not ImGui::IsWindowHovered(ImGuiHoveredFlags_None)) return StateNotChange{};

            const auto checker_board_vector = calc_hovered_cell();

            if(const auto checker_index_opt = try_convert_board_vector_to_checker_index(checker_board_vector)) {
                const auto checker_index = *checker_index_opt;

                if constexpr (std::same_as<Form, selected::MoveForm> or std::same_as<Form, selected::attack::Form>) {
                    if(checker_index == form.m_index) {
                        draw_hovered_cell(checker_board_vector, YELLOW_COLOR);
                        return StateNotChange{};
                    }
                }

                const auto it = std::find_if(std::begin(form.m_actions), std::end(form.m_actions),
                    [checker_index](const auto& el) { return el.first == checker_index; });
                if(it != std::end(form.m_actions)) {
                    draw_hovered_cell(checker_board_vector, GREEN_COLOR);
                    if(ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
                        return SelectedForm(checker_index, utils::checked_move(form.m_actions));
                    }
                    return StateNotChange{};
                }
            }
            draw_hovered_cell(checker_board_vector, YELLOW_COLOR);
            return StateNotChange{};
        }
    }

    namespace unselected {

        std::variant<AttackForm, MoveForm> determine_form(const GameData& game_data) {
            auto attacks = std::vector<std::pair<CheckerIndex, CollectAttacksResult>>{};
            iterate_over_current_player_checkers(game_data,
                [&attacks](const GameData& game_data, const CheckerIndex checker_index) {
                    attacks.emplace_back(checker_index, collect_attacks(game_data.checkers, checker_index));
                }
            );
            const auto max_depth = std::max_element(std::begin(attacks), std::end(attacks),
                [](const auto& first, const auto& second) {
                    return first.second.depth < second.second.depth;
                }
            )->second.depth;
            if(max_depth > 0) {
                attacks.erase(std::remove_if(std::begin(attacks), std::end(attacks),
                    [max_depth](const auto& el) { return el.second.depth < max_depth; }
                ), std::end(attacks));
                return AttackForm{utils::checked_move(attacks)};
            }
            auto moves = std::vector<std::pair<CheckerIndex, std::vector<MoveAction>>>{};
            iterate_over_current_player_checkers(game_data,
                [&moves](const GameData& game_data, const CheckerIndex checker_index) {
                    auto collected_moves = collect_moves(game_data.checkers, checker_index);
                    if(not collected_moves.empty()) {
                        moves.emplace_back(checker_index, utils::checked_move(collected_moves));
                    }
                }
            );
            return MoveForm{utils::checked_move(moves)};
        }

    }

    std::variant<StateNotChange, selected::attack::Form> iter(
        unselected::AttackForm& form, const GameData&
    ) {
        return unselected_selected_common::iter<selected::attack::Form>(form);
    }

    std::variant<StateNotChange, selected::MoveForm> iter(
        unselected::MoveForm& form, const GameData&
    ) {
        return unselected_selected_common::iter<selected::MoveForm>(form);
    }

    namespace selected {

        template<typename ActionsType>
        static auto find_checker_actions(
            const CheckerIndex checker_index,
            const std::vector<std::pair<CheckerIndex, ActionsType>>& actions
        ) -> typename std::vector<std::pair<CheckerIndex, ActionsType>>::const_iterator {
            const auto it = std::find_if(std::begin(actions), std::end(actions),
                [checker_index](const auto& el) { return el.first == checker_index; }
            );
            assert(it != std::end(actions) && "No actions found for checker index");
            return it;
        }

        MoveForm::MoveForm(const CheckerIndex checker_index, std::vector<std::pair<CheckerIndex, std::vector<MoveAction>>>&& actions)
            : m_index{checker_index}, m_actions{utils::checked_move(actions)} {
            const auto it = find_checker_actions(checker_index, m_actions);
            m_index_actions = std::span{it->second};
        }

        attack::Form::Form(const CheckerIndex checker_index, std::vector<std::pair<CheckerIndex, CollectAttacksResult>>&& actions)
            : m_index{checker_index}, m_actions{utils::checked_move(actions)} {
            const auto it = find_checker_actions(checker_index, m_actions);
            m_index_nodes.emplace_back(std::nullopt, it->second.actions);
        }

        using OtherCheckerSelected = strong::type<CheckerIndex, struct OtherCheckerSelected_>;
        using IterationResult = std::variant<StateNotChange, SelectionConfirmed, MoveForm, attack::Form>;

        void draw_action_rect(const BoardVector board_vector, const ImU32 color) {
            const auto cell_size = calc_cell_size();
            constexpr auto padding_percentage = 0.3;
            const auto padding = cell_size * padding_percentage;
            const auto p_min = calc_cell_top_left(board_vector);
            const auto p_max = p_min + cell_size;
            const auto padded_p_min = p_min + padding;
            const auto padded_p_max = p_max - padding;
            ImGui::GetWindowDrawList()->AddRectFilled(padded_p_min, padded_p_max, color);
        }

        template<typename FormType>
        IterationResult common_iter(FormType& form) {
            auto iter_result = unselected_selected_common::iter<FormType>(form);
            draw_hovered_cell(convert_checker_index_to_board_vector(form.m_index), BLUE_COLOR);
            if(std::holds_alternative<FormType>(iter_result)) {
                return utils::variant_move<IterationResult>(utils::checked_move(iter_result));
            }
            return StateNotChange{};
        }
    }

    selected::IterationResult iter(selected::MoveForm& form, const GameData& game_data) {
        {
            auto iter_result = selected::common_iter(form);
            if(not std::holds_alternative<StateNotChange>(iter_result)) {
                return utils::variant_move<selected::IterationResult>(utils::checked_move(iter_result));
            }
        }

        for(const auto& action : form.m_index_actions) {
            selected::draw_action_rect(convert_board_index_to_board_vector(action._val), PURPLE_COLOR);
        }

        if(ImGui::IsWindowHovered(ImGuiHoveredFlags_None)) {
            const auto checker_board_vector = calc_hovered_cell();
            const auto it = std::find_if(std::begin(form.m_index_actions), std::end(form.m_index_actions),
            [index=convert_board_vector_to_board_index(checker_board_vector)](const MoveAction action) {
                return action._val == index;
            });
            if(it != std::end(form.m_index_actions)) {
                draw_hovered_cell(checker_board_vector, PURPLE_COLOR);
                if(ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
                    return SelectionConfirmed{};
                }
                return StateNotChange{};
            }
        }
        return StateNotChange{};
    }

    selected::IterationResult iter(selected::attack::Form& form, const GameData& game_data) {
        {
            auto iter_result = selected::common_iter(form);
            if(not std::holds_alternative<StateNotChange>(iter_result)) {
                return utils::variant_move<selected::IterationResult>(utils::checked_move(iter_result));
            }
        }

        {
            const auto& last_node = form.m_index_nodes.back();
            if(last_node.m_index) {
                selected::draw_action_rect(convert_board_index_to_board_vector(*last_node.m_index), RED_COLOR);
            }
            for(const auto& action : last_node.m_actions) {
                selected::draw_action_rect(convert_board_index_to_board_vector(action.m_board_index), PINK_COLOR);
            }
        }

        if(ImGui::IsWindowHovered(ImGuiHoveredFlags_None)) {
            if(ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
                if(form.m_index_nodes.size() > 1) {
                    form.m_index_nodes.pop_back();
                }
            }

            const auto checker_board_vector = calc_hovered_cell();
            const auto& last_node = form.m_index_nodes.back();
            const auto it = std::find_if(std::begin(last_node.m_actions), std::end(last_node.m_actions),
                [index=convert_board_vector_to_board_index(checker_board_vector)](const AttackAction& action) {
                    return action.m_board_index == index;
                });
            if(it != std::end(last_node.m_actions)) {
                draw_hovered_cell(checker_board_vector, PURPLE_COLOR);
                if(ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
                    if(it->m_child_actions.empty()) {
                        return SelectionConfirmed{};
                    }
                    form.m_index_nodes.emplace_back(it->m_board_index, it->m_child_actions);
                    return StateNotChange{};
                }
                return StateNotChange{};
            }
        }
        return StateNotChange{};
    }

    void draw_rects() {
        const auto draw_list = ImGui::GetWindowDrawList();
        const auto cell_size = calc_cell_size();
        auto is_white = true;
        for(uint8_t y = 0; y < CELLS_PER_SIDE; ++y) {
            for(uint8_t x = 0; x < CELLS_PER_SIDE; ++x) {
                const auto p_min = calc_cell_top_left(BoardVector{x, y});
                draw_list->AddRectFilled(
                    p_min, p_min + cell_size, is_white ? BOARD_CELL_ONE_COLOR : BOARD_CELL_TWO_COLOR
                );
                is_white = not is_white;
            }
            is_white = not is_white;
        }
    }

    void draw_checkers(const GameData& data) {
        const auto draw_list = ImGui::GetWindowDrawList();
        const auto cell_size = calc_cell_size();
        const auto half_cell_size = cell_size / 2;
        const auto pawn_radius = half_cell_size * 0.8;
        const auto king_hat_radius = half_cell_size / 2;
        auto checker_index = CheckerIndex{0};
        for(uint8_t y = 0; y < CELLS_PER_SIDE; ++y) {
            for(uint8_t x = y % 2 == 0 ? 1 : 0; x < CELLS_PER_SIDE; x += 2, ++checker_index) {
                if(not data.checkers.m_is_in_place[checker_index]) continue;
                const auto center = calc_cell_top_left(BoardVector{x, y}) + half_cell_size;
                const auto player_index = data.checkers.m_player_index[checker_index];
                draw_list->AddEllipseFilled(center, pawn_radius, player_index ? PLAYER_ONE_PAWN_COLOR : PLAYER_TWO_PAWN_COLOR);
                if(data.checkers.m_is_king[checker_index]) {
                    draw_list->AddEllipseFilled(center, king_hat_radius, KING_HAT_COLOR);
                }
            }
        }
    }

    std::variant<unselected::AttackForm, unselected::MoveForm> iter(const InitialState, const GameData& game_data) {
        return unselected::determine_form(game_data);
    }

    void Form::iter_sss(const GameData& game_data) {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        ImGui::BeginChild("BoardForm", ImVec2(0, -1), true, ImGuiWindowFlags_NoScrollWithMouse);
        draw_rects();
        draw_checkers(game_data);

        auto new_state = std::visit(
            [this, &game_data](auto& state) -> IterationResult {
            return utils::variant_move<IterationResult>(iter(state, game_data));
        }, m_state);

        std::visit(utils::overloaded{
            [](const StateNotChange) {},
            [this, &game_data](const SelectionConfirmed) {
                m_state = utils::variant_move<State>(unselected::determine_form(game_data));
            },
            [this](auto&& state) {
                m_state = utils::checked_move(state);
            }
        }, utils::checked_move(new_state));

        ImGui::EndChild();
        ImGui::PopStyleVar();
    }

    void Form::change_state(State&& state) {
        m_state = utils::checked_move(state);
    }

}