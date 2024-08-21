#include <mcts_checkers/board/human/form.hpp>
#include <mcts_checkers/utils.hpp>
#include <mcts_checkers/index_converters.hpp>
#include <mcts_checkers/action_application_funcs.hpp>

#include <ranges>
#include <range/v3/view/drop.hpp>
#include <range/v3/view/transform.hpp>
#include <range/v3/range/conversion.hpp>

#include <tl/optional.hpp>

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

            OutMessage::Type iter(const GameData& game_data) {
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
                    [](const turn_actions::DeclareDraw) -> OutMessage::Type {
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

        ImVec2 calc_mouse_local_window_pos() {
            return ImGui::GetMousePos() - ImGui::GetCursorScreenPos();
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

        constexpr BoardVector convert_imvec_to_board_vector(const ImVec2 imvec) {
            return BoardVector{
                static_cast<uint8_t>(imvec.x),
                static_cast<uint8_t>(imvec.y)
            };
        }

        BoardVector calc_hovered_cell() {
            return convert_imvec_to_board_vector(calc_mouse_local_window_pos() / calc_cell_size());
        }

        constexpr bool is_white_cell(const BoardVector cell_index) {
            return is_even(cell_index.y) == is_even(cell_index.x);
        }

        constexpr tl::optional<CheckerIndex> try_convert_board_vector_to_checker_index(const BoardVector cell_index) {
            if(is_white_cell(cell_index)) {
                return tl::nullopt;
            }
            return convert_board_vector_to_checker_index(cell_index);
        }

        namespace unselected_selected_common {

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

                OutMessage::Type iter(Form& form) {
                    unselected_selected_common::draw_action_cells(form.m_actions);
                    if(is_window_hovered()) {
                        return utils::variant_move<OutMessage::Type>(
                            unselected_selected_common::select_checker<OutMessage::Selected, OutMessage::NotSelected, Form>(form)
                        );
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

                OutMessage::Type iter(Form& form) {
                    unselected_selected_common::draw_action_cells(form.m_actions);
                    if(is_window_hovered()) {
                        return utils::variant_move<OutMessage::Type>(
                            unselected_selected_common::select_checker<OutMessage::Selected, OutMessage::NotSelected, Form>(form)
                        );
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
            Form::Form(const CheckerIndex checker_index, turn_actions::MakeMove&& actions)
                : m_index{checker_index}, m_actions{utils::checked_move(actions)} {
                const auto it = find_checker_actions(checker_index, m_actions);
                m_index_actions = std::span{it->second};
            }

            namespace {
                
                namespace OutMessage {
                    struct StateNotChange {};
                    struct Selected {
                        MoveAction m_data;
                        CheckerIndex m_index;
                    };
                    struct SelectedOtherChecker {
                        CheckerIndex m_index;
                        turn_actions::MakeMove m_actions;
                    };
                    using Type = std::variant<
                        StateNotChange,
                        Selected,
                        SelectedOtherChecker
                    >;
                }

                OutMessage::Type iter(Form& form) {
                    unselected_selected_common::draw_action_cells(form.m_actions);
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
                                return OutMessage::Selected{
                                    MoveAction{checker_board_index},
                                    form.m_index
                                };
                            }
                        } else {
                            return utils::variant_move<OutMessage::Type>(
                                unselected_selected_common::select_checker<OutMessage::SelectedOtherChecker, OutMessage::StateNotChange>(form)
                            );
                        }
                    }
                    return OutMessage::StateNotChange{};
                }
            }
        }
        namespace attack {
            Form::Form(const CheckerIndex checker_index, std::vector<std::pair<CheckerIndex, CollectAttacksResult>>&& actions)
                : m_index{checker_index}, m_actions{utils::checked_move(actions)} {
                const auto it = find_checker_actions(checker_index, m_actions);
                m_index_nodes.emplace_back(convert_checker_index_to_board_index(checker_index), it->second.actions);
            }

            namespace {
                namespace OutMessage {
                    struct StateNotChange {};
                    struct Selected {
                        std::vector<AttackAction> m_data;
                    };
                    struct SelectedOtherChecker {
                        CheckerIndex m_index;
                        turn_actions::MakeAttack m_actions;
                    };
                    using Type = std::variant<
                        StateNotChange,
                        Selected,
                        SelectedOtherChecker
                    >;
                }

                OutMessage::Type iter(Form& form) {
                    unselected_selected_common::draw_action_cells(form.m_actions);
                    for(const auto& node : form.m_index_nodes | std::views::drop(1)) {
                        selected::draw_action_rect(node.m_index, RED_COLOR);
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
                                form.m_index_nodes.emplace_back(it->m_board_index, it->m_child_trees);
                                if(it->m_child_trees.empty()) {
                                    auto actions = form.m_index_nodes
                                        | ranges::views::transform(
                                            [index=convert_checker_index_to_board_index(form.m_index)]
                                            (const selected::attack::Node& node) {
                                                return AttackAction{node.m_index};
                                            })
                                        | ranges::to_vector;

                                    assert(not actions.empty() && "Actions can not be empty");
                                    return OutMessage::Selected{utils::checked_move(actions)};
                                }
                                return OutMessage::StateNotChange{};
                            }
                        } else {
                            return utils::variant_move<OutMessage::Type>(
                                unselected_selected_common::select_checker<OutMessage::SelectedOtherChecker, OutMessage::StateNotChange>(form)
                            );
                        }
                    }
                    return OutMessage::StateNotChange{};
                }
            }
        }
    }

    PlayerMessage::Type iter(Form& form, const GameData& game_data) {
        return std::visit(utils::overloaded{
            [&game_data, &form](const initial::Form) {
                return std::visit(utils::overloaded{
                    [&form](initial::OutMessage::TransitionToUnselectedMove&& message) -> PlayerMessage::Type {
                        form.m_state = unselected::move::Form{utils::checked_move(message.m_actions)};
                        return PlayerMessage::PlayerMadeNoSelection{};
                    },
                    [&form](initial::OutMessage::TransitionToUnselectedAttack&& message) -> PlayerMessage::Type {
                        form.m_state = unselected::attack::Form{utils::checked_move(message.m_actions)};
                        return PlayerMessage::PlayerMadeNoSelection{};
                    },
                    [](const initial::OutMessage::DeclareLoss message) -> PlayerMessage::Type {
                        return PlayerMessage::DeclareLoss{message.m_player_index};
                    },
                    [](const initial::OutMessage::DeclareDraw) -> PlayerMessage::Type {
                        return PlayerMessage::DeclareDraw{};
                    }
                }, initial::iter(game_data));
            },
            [&form](unselected::move::Form& state) {
                return std::visit(utils::overloaded{
                    [](unselected::move::OutMessage::NotSelected) -> PlayerMessage::Type {
                        return PlayerMessage::PlayerMadeNoSelection{};
                    },
                    [&form](unselected::move::OutMessage::Selected&& message) -> PlayerMessage::Type {
                        form.m_state = selected::move::Form(message.m_index, utils::checked_move(message.m_actions));
                        return PlayerMessage::PlayerMadeNoSelection{};
                    }
                }, unselected::move::iter(state));
            },
            [&form](unselected::attack::Form& state) {
                return std::visit(utils::overloaded{
                    [](unselected::attack::OutMessage::NotSelected) -> PlayerMessage::Type {
                        return PlayerMessage::PlayerMadeNoSelection{};
                    },
                    [&form](unselected::attack::OutMessage::Selected&& message) -> PlayerMessage::Type {
                        form.m_state = selected::attack::Form(message.m_index, utils::checked_move(message.m_actions));
                        return PlayerMessage::PlayerMadeNoSelection{};
                    }
                }, unselected::attack::iter(state));
            },
            [&form, &game_data](selected::move::Form& state) {
                return std::visit(utils::overloaded{
                    [&game_data](selected::move::OutMessage::Selected&& message) -> PlayerMessage::Type {
                        auto new_game_data = game_data;
                        apply_move(new_game_data, message.m_index, message.m_data);
                        return PlayerMessage::PlayerMadeSelection{new_game_data};
                    },
                    [&form](selected::move::OutMessage::SelectedOtherChecker&& message) -> PlayerMessage::Type {
                        form.m_state = selected::move::Form(message.m_index, utils::checked_move(message.m_actions));
                        return PlayerMessage::PlayerMadeNoSelection{};
                    },
                    [](const selected::move::OutMessage::StateNotChange) -> PlayerMessage::Type {
                        return PlayerMessage::PlayerMadeNoSelection{};
                    },
                }, selected::move::iter(state));
            },
            [&form, &game_data](selected::attack::Form& state) {
                return std::visit(utils::overloaded{
                    [&game_data](selected::attack::OutMessage::Selected&& message) -> PlayerMessage::Type {
                        auto new_game_data = game_data;
                        apply_attack(new_game_data, message.m_data);
                        return PlayerMessage::PlayerMadeSelection{new_game_data};
                    },
                    [&form](selected::attack::OutMessage::SelectedOtherChecker&& message) -> PlayerMessage::Type {
                        form.m_state = selected::attack::Form(message.m_index, utils::checked_move(message.m_actions));
                        return PlayerMessage::PlayerMadeNoSelection{};
                    },
                    [](const selected::attack::OutMessage::StateNotChange) -> PlayerMessage::Type {
                        return PlayerMessage::PlayerMadeNoSelection{};
                    },
                }, selected::attack::iter(state));
            },
        }, form.m_state);
    }

}

