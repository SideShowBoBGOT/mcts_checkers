#include <mcts_checkers/board_form.hpp>
#include <mcts_checkers/utils.hpp>
#include <mcts_checkers/checkers_data.hpp>
#include <mcts_checkers/checkers_funcs.hpp>
#include <tl/optional.hpp>
#include <ranges>
#include <range/v3/view/drop.hpp>
#include <range/v3/view/transform.hpp>
#include <range/v3/range/conversion.hpp>

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

    void draw_hovered_cell(const CheckerIndex checker_index, const ImU32 color) {
        draw_hovered_cell(convert_checker_index_to_board_vector(checker_index), color);
    }

    struct StateNotChange{};

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

    bool is_window_hovered() {
        return ImGui::IsWindowHovered(ImGuiHoveredFlags_None);
    }

    namespace unselected_selected_common {

        template<typename Form>
        void draw_action_cells(const Form& form) {
            for(const auto& el : form.m_actions) {
                draw_hovered_cell(convert_checker_index_to_board_vector(el.first), PURPLE_BLUE_COLOR);
            }
        }

        template<typename SelectedForm, typename Form>
        std::variant<StateNotChange, SelectedForm> select_checker(Form& form) {
            assert(is_window_hovered() && "Window is not hovered");

            const auto checker_board_vector = calc_hovered_cell();
            if(const auto checker_index_opt = try_convert_board_vector_to_checker_index(checker_board_vector)) {
                const auto checker_index = *checker_index_opt;
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

        template<typename SelectedForm, typename Form>
        std::variant<StateNotChange, SelectedForm> iter(Form& form) {
            unselected_selected_common::draw_action_cells(form);
            if(is_window_hovered()) {
                return unselected_selected_common::select_checker<SelectedForm>(form);
            }
            return StateNotChange{};
        }

    }

    std::variant<StateNotChange, selected::attack::Form> iter(
        unselected::AttackForm& form, const GameData&
    ) {
        return unselected::iter<selected::attack::Form>(form);
    }

    std::variant<StateNotChange, selected::MoveForm> iter(
        unselected::MoveForm& form, const GameData&
    ) {
        return unselected::iter<selected::MoveForm>(form);
    }

    namespace selection_confirmed {
        struct Move {
            MoveAction action;
            CheckerIndex checker_index;
        };
        struct Attack {
            std::vector<AttackAction> actions;
            CheckerIndex checker_index;
        };
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
        using IterationResult = std::variant<
            StateNotChange,
            MoveForm,
            attack::Form,
            selection_confirmed::Move,
            selection_confirmed::Attack
        >;

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

        void draw_action_rect(const BoardIndex board_vector, const ImU32 color) {
            draw_action_rect(convert_board_index_to_board_vector(board_vector), color);
        }
    }

    selected::IterationResult iter(selected::MoveForm& form, const GameData& game_data) {
        unselected_selected_common::draw_action_cells(form);
        for(const auto& action : form.m_index_actions) {
            selected::draw_action_rect(action._val, PURPLE_COLOR);
        }
        draw_hovered_cell(form.m_index, BLUE_COLOR);

        if(ImGui::IsWindowHovered(ImGuiHoveredFlags_None)) {
            const auto checker_board_vector = calc_hovered_cell();
            const auto checker_board_index = convert_board_vector_to_board_index(checker_board_vector);
            const auto it = std::find_if(std::begin(form.m_index_actions), std::end(form.m_index_actions),
            [checker_board_index](const MoveAction action) { return action._val == checker_board_index; });
            if(it != std::end(form.m_index_actions)) {
                draw_hovered_cell(checker_board_vector, PURPLE_COLOR);
                if(ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
                    return selection_confirmed::Move{
                        MoveAction{checker_board_index},
                        convert_board_vector_to_checker_index(checker_board_vector)
                    };
                }
            } else if(
                auto selection_result = unselected_selected_common::select_checker<selected::MoveForm>(form);
                not std::holds_alternative<StateNotChange>(selection_result)
            ) {
                return utils::variant_move<selected::IterationResult>(utils::checked_move(selection_result));
            }
        }
        return StateNotChange{};
    }

    selected::IterationResult iter(selected::attack::Form& form, const GameData& game_data) {
        unselected_selected_common::draw_action_cells(form);
        for(const auto& node : form.m_index_nodes | std::views::drop(1)) {
            selected::draw_action_rect(*node.m_index, RED_COLOR);
        }
        for(const auto& action : form.m_index_nodes.back().m_actions) {
            selected::draw_action_rect(action.m_board_index, PINK_COLOR);
        }
        draw_hovered_cell(form.m_index, BLUE_COLOR);


        if(is_window_hovered()) {
            if(ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
                if(form.m_index_nodes.size() > 1) {
                    form.m_index_nodes.pop_back();
                }
            }

            const auto checker_board_vector = calc_hovered_cell();
            const auto& last_node = form.m_index_nodes.back();
            const auto it = std::find_if(std::begin(last_node.m_actions), std::end(last_node.m_actions),
                [index=convert_board_vector_to_board_index(checker_board_vector)](const AttackTree& action) {
                    return action.m_board_index == index;
                });
            if(it != std::end(last_node.m_actions)) {
                draw_hovered_cell(checker_board_vector, PINK_COLOR);
                if(ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
                    if(it->m_child_trees.empty()) {

                        return selection_confirmed::Attack{
                            form.m_index_nodes
                                | ranges::views::drop(1)
                                | ranges::views::transform(
                                    [](const selected::attack::Node& node) {
                                        return AttackAction{*node.m_index};
                                    })
                                | ranges::to_vector,
                            convert_board_vector_to_checker_index(checker_board_vector)
                        };

                    }
                    form.m_index_nodes.emplace_back(it->m_board_index, it->m_child_trees);
                    return StateNotChange{};
                }
            } else if(
                auto selection_result = unselected_selected_common::select_checker<selected::attack::Form>(form);
                not std::holds_alternative<StateNotChange>(selection_result)
            ) {
                return utils::variant_move<selected::IterationResult>(utils::checked_move(selection_result));
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

        using IterationResult = std::variant<
            StateNotChange,
            unselected::MoveForm,
            unselected::AttackForm,
            selected::MoveForm,
            selected::attack::Form,
            selection_confirmed::Move,
            selection_confirmed::Attack
        >;

        auto new_state = std::visit(
            [this, &game_data](auto& state) -> IterationResult {
            return utils::variant_move<IterationResult>(iter(state, game_data));
        }, m_state);

        std::visit(utils::overloaded{
            [](const StateNotChange) {},
            [this, &game_data](selection_confirmed::Move) {
                m_state = utils::variant_move<State>(unselected::determine_form(game_data));
            },
            [this, &game_data](selection_confirmed::Attack&&) {
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