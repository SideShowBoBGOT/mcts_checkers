#include <mcts_checkers/board/human/form.hpp>
#include <mcts_checkers/utils.hpp>
#include <mcts_checkers/index_converters.hpp>

namespace mcts_checkers::board::human {

    namespace initial {
        namespace {
            namespace OutMessage {
                struct TransitionToUnselectedMove { turn_actions::MakeMove m_actions; };
                struct TransitionToUnselectedAttack { turn_actions::MakeAttack m_actions; };
                struct DeclareLoss { PlayerIndex m_player_index; };
                struct DeclareDraw {};

                using Type = std::variant<
                    TransitionToUnselectedMove,
                    TransitionToUnselectedAttack,
                    DeclareLoss,
                    DeclareDraw
                >;
            }

            OutMessage::Type iter(Form, const GameData& game_data);

            OutMessage::Type iter(Form, const GameData& game_data) {
                return std::visit(utils::overloaded{
                    [](turn_actions::MakeMove&& actions) -> OutMessage::Type {
                        return OutMessage::TransitionToUnselectedMove{utils::checked_move(actions)};
                    },
                    [](turn_actions::MakeAttack&& actions) -> OutMessage::Type {
                        return OutMessage::TransitionToUnselectedAttack{utils::checked_move(actions)};
                    },
                    [](const turn_actions::DeclareLoss actions) -> OutMessage::Type {
                        return OutMessage::DeclareLoss{actions.m_player_index};
                    },
                    [](const turn_actions::DeclareDraw actions) -> OutMessage::Type {
                        return OutMessage::DeclareDraw{};
                    }
                }, turn_actions::determine(game_data));
            }

        }
    }

    namespace {
        bool is_window_hovered() {
            return ImGui::IsWindowHovered(ImGuiHoveredFlags_None);
        }

    }

    namespace unselected_selected_common {
        namespace {
            template<typename ActionsType>
            void draw_action_cells(const std::vector<std::pair<CheckerIndex, ActionsType>>& actions) {
                for(const auto& el : actions) {
                    draw_hovered_cell(convert_checker_index_to_board_vector(el.first), PURPLE_BLUE_COLOR);
                }
            }

            template<typename Selected, typename Unselected, typename Form>
            std::variant<Selected, Unselected> select_checker(Form& form) {
                assert(is_window_hovered() && "Window is not hovered");

                const auto checker_board_vector = calc_hovered_cell();
                if(const auto checker_index_opt = try_convert_board_vector_to_checker_index(checker_board_vector)) {
                    const auto checker_index = *checker_index_opt;
                    const auto it = std::find_if(std::begin(form.m_actions), std::end(form.m_actions),
                        [checker_index](const auto& el) { return el.first == checker_index; });
                    if(it != std::end(form.m_actions)) {
                        draw_hovered_cell(checker_board_vector, GREEN_COLOR);
                        if(ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
                            return Selected(checker_index, utils::checked_move(form.m_actions));
                        }
                        return Unselected{};
                    }
                }
                draw_hovered_cell(checker_board_vector, YELLOW_COLOR);
                return Unselected{};
            }
        }
    }

    namespace unselected {

        namespace move {
            namespace {
                namespace OutMessage {

                    struct NotSelected {};
                    struct Selected {
                        CheckerIndex m_index;
                        turn_actions::MakeMove m_actions;
                    };

                    using Type = std::variant<NotSelected, Selected>;
                }

                OutMessage::Type iter(Form& form, const GameData& game_data) {
                    unselected_selected_common::draw_action_cells(form.m_actions);
                    if(is_window_hovered()) {
                        return unselected_selected_common::select_checker<OutMessage::Selected, OutMessage::NotSelected, Form>(form);
                    }
                    return OutMessage::NotSelected{};
                }
            }
        }

        namespace attack {
            namespace {
                namespace OutMessage {

                    struct NotSelected {};
                    struct Selected {
                        CheckerIndex m_index;
                        turn_actions::MakeAttack m_actions;
                    };

                    using Type = std::variant<NotSelected, Selected>;
                }

                OutMessage::Type iter(Form& form, const GameData& game_data) {
                    unselected_selected_common::draw_action_cells(form.m_actions);
                    if(is_window_hovered()) {
                        return unselected_selected_common::select_checker<OutMessage::Selected, OutMessage::NotSelected, Form>(form);
                    }
                    return OutMessage::NotSelected{};
                }
            }
        }
    }

    namespace selected {

        namespace {
            template<typename ActionsType>
            auto find_checker_actions(
                const CheckerIndex checker_index,
                const std::vector<std::pair<CheckerIndex, ActionsType>>& actions
            ) -> typename std::vector<std::pair<CheckerIndex, ActionsType>>::const_iterator {
                const auto it = std::find_if(std::begin(actions), std::end(actions),
                    [checker_index](const auto& el) { return el.first == checker_index; }
                );
                assert(it != std::end(actions) && "No actions found for checker index");
                return it;
            }

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

        namespace move {
            Form::Form(const CheckerIndex checker_index, std::vector<std::pair<CheckerIndex, std::vector<MoveAction>>>&& actions)
                : m_index{checker_index}, m_actions{utils::checked_move(actions)} {
                const auto it = find_checker_actions(checker_index, m_actions);
                m_index_actions = std::span{it->second};
            }
        }
        namespace attack {
            Form::Form(const CheckerIndex checker_index, std::vector<std::pair<CheckerIndex, CollectAttacksResult>>&& actions)
                : m_index{checker_index}, m_actions{utils::checked_move(actions)} {
                const auto it = find_checker_actions(checker_index, m_actions);
                m_index_nodes.emplace_back(convert_checker_index_to_board_index(checker_index), it->second.actions);
            }
        }
    }


    namespace {
        initial::OutMessage::Type iter(const initial::Form state, const GameData& game_data) {
            return initial::iter(state, game_data);
        }
    }

    OutMessage::Type iter(Form& form, const GameData& game_data) {
        


    }

}

