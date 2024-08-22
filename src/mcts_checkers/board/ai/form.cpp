#include <mcts_checkers/board/ai/form.hpp>
#include <mcts_checkers/index_converters.hpp>
#include <mcts_checkers/action_application_funcs.hpp>
#include <random>

namespace mcts_checkers::board::ai {

    namespace random {
        namespace {
            template<typename T>
            const T& get_random_element(const std::vector<T>& v, std::default_random_engine& generator) {
                assert(not v.empty() && "Distance can not be <= 0");
                auto distribution = std::uniform_int_distribution<size_t>(0, v.size() - 1);
                return v[distribution(generator)];
            }

            void select_attack_chain(
                const AttackTree& tree,
                std::default_random_engine& generator,
                std::vector<AttackAction>& chain
            ) {
                chain.emplace_back(tree.m_board_index);
                if(tree.m_child_trees.empty()) {
                    return;
                }
                const auto& child_tree = get_random_element(tree.m_child_trees, generator);
                select_attack_chain(child_tree, generator, chain);
            }

            struct SelectedMove {
                MoveAction m_data;
                CheckerIndex m_index;
            };

            struct SelectedAttack {
                std::vector<AttackAction> m_data;
            };

            SelectedMove select_random_move(
                turn_actions::MakeMove&& action,
                std::default_random_engine& generator
            ) {
                const auto& checker_actions = get_random_element(action, generator);
                const auto& specific_move = get_random_element(checker_actions.second, generator);
                return {specific_move, checker_actions.first};
            }

            SelectedAttack select_random_attack(
                turn_actions::MakeAttack&& action,
                std::default_random_engine& generator
            ) {
                const auto& checker_actions = get_random_element(action, generator);

                auto chain = std::vector<AttackAction>{};
                chain.reserve(checker_actions.second.depth + 1);
                chain.emplace_back(convert_checker_index_to_board_index(checker_actions.first));

                const auto& start_tree = get_random_element(checker_actions.second.actions, generator);
                select_attack_chain(start_tree, generator, chain);

                return {utils::checked_move(chain)};
            }

            PlayerMessage::Type calculate_move(const GameData& game_data) {
                auto generator = std::default_random_engine(std::chrono::system_clock::now().time_since_epoch().count());
                return std::visit(utils::overloaded{
                    [](const turn_actions::DeclareLoss message) -> PlayerMessage::Type {
                        return PlayerMessage::DeclareLoss{message.m_player_index};
                    },
                    [](const turn_actions::DeclareDraw) -> PlayerMessage::Type {
                        return PlayerMessage::DeclareDraw{};
                    },
                    [&generator, &game_data](turn_actions::MakeMove&& action) -> PlayerMessage::Type {
                        const auto selected_move = select_random_move(utils::checked_move(action), generator);
                        auto new_data = game_data;
                        apply_move(new_data, selected_move.m_index, selected_move.m_data);
                        return PlayerMessage::PlayerMadeSelection{new_data};
                    },
                    [&generator, &game_data](turn_actions::MakeAttack&& action) -> PlayerMessage::Type {
                        const auto selected_attack = select_random_attack(utils::checked_move(action), generator);
                        auto new_data = game_data;
                        apply_attack(new_data, selected_attack.m_data);
                        return PlayerMessage::PlayerMadeSelection{new_data};
                    }
                }, turn_actions::determine(game_data));
            }
        }
    }

    // namespace mcts {
    //     namespace {
    //         struct Node {
    //             Node(Node* const parent, const GameData& game_data)
    //                 : m_parent(parent), m_game_data(game_data) {}
    //             uint64_t m_visits = 0;
    //             int64_t m_value = 0;
    //             Node* m_parent = nullptr;
    //             GameData m_game_data{};
    //             std::vector<Node> m_children{};
    //         };

    //         void back_propagate(Node& node) {
    //             ++node.m_visits;
    //             auto parent = node.m_parent;
    //             while(node.m_parent != nullptr) {
    //                 parent->m_value += node.m_value;
    //                 ++parent->m_visits;
    //                 parent = parent->m_parent;
    //             }
    //         }

    //         void rollout(
    //             Node& node,
    //             std::default_random_engine& generator
    //         ) {
    //             auto game_data = node.m_game_data;
    //             while(
    //                 std::visit(utils::overloaded{
    //                     [&const_game_data = std::as_const(game_data), &node](const turn_actions::DeclareLoss action) {
    //                         node.m_value += const_game_data.m_current_player_index == action.m_player_index ? -1 : 1;
    //                         return false;
    //                     },
    //                     [](const turn_actions::DeclareDraw) {
    //                         return false;
    //                     },
    //                     [&game_data, &generator](turn_actions::MakeMove&& action) {
    //                         const auto confirmed_action = random::select_random_move(utils::checked_move(action), generator);
    //                         apply_move(game_data, confirmed_action.checker_index, confirmed_action.data);
    //                         return true;
    //                     },
    //                     [&game_data, &generator](turn_actions::MakeAttack&& action) {
    //                         const auto confirmed_action = random::select_random_attack(utils::checked_move(action), generator);
    //                         apply_attack(game_data, confirmed_action.data);
    //                         return true;
    //                     }
    //                 }, turn_actions::determine(game_data))
    //             ) {}
    //         }

    //         void rollout_back_propagate(Node& node, std::default_random_engine& generator) {
    //             rollout(node, generator);
    //             back_propagate(node);
    //         }

    //         void back_propagate_terminal(Node& node) {
    //             ++node.m_visits;
    //             auto parent = node.m_parent;
    //             while(node.m_parent != nullptr) {
    //                 ++parent->m_visits;
    //                 parent = parent->m_parent;
    //             }
    //         }

    //         namespace expand {

    //             void visitor(Node& node, turn_actions::MakeMove&& checkers_actions) {
    //                 for(const auto& [checker_index, local_actions] : checkers_actions) {
    //                     for(const auto checker_action : local_actions) {
    //                         auto child_data = node.m_game_data;
    //                         apply_move(child_data, checker_index, checker_action);
    //                         node.m_children.emplace_back(&node, child_data);
    //                     }
    //                 }
    //             }

    //             void attack_add_children(
    //                 const AttackTree& tree,
    //                 Node& node,
    //                 std::vector<AttackAction>& chain
    //             ) {
    //                 chain.emplace_back(tree.m_board_index);
    //                 if(tree.m_child_trees.empty()) {
    //                     auto child_data = node.m_game_data;
    //                     apply_attack(child_data, chain);
    //                     node.m_children.emplace_back(&node, child_data);
    //                     return;
    //                 }
    //                 for(const auto& child_tree : tree.m_child_trees) {
    //                     attack_add_children(child_tree, node, chain);
    //                 }
    //             }

    //             void visitor(Node& node, turn_actions::MakeAttack&& checkers_actions) {
    //                 auto chain = std::vector<AttackAction>{};
    //                 chain.reserve(checkers_actions.front().second.depth + 1);

    //                 for(const auto& [checker_index, local_actions] : checkers_actions) {
    //                     chain.emplace_back(convert_checker_index_to_board_index(checker_index));
    //                     for(const auto& tree : local_actions.actions) {
    //                         attack_add_children(tree, node, chain);
    //                     }
    //                     chain.pop_back();
    //                 }
    //             }
    //             void visitor(Node&, turn_actions::DeclareDraw) {}
    //             void visitor(Node&, turn_actions::DeclareLoss) {}

    //             void execute(Node& node) {
    //                 std::visit([&node](auto&& checkers_actions) {
    //                     visitor(node, utils::checked_move(checkers_actions));
    //                 }, turn_actions::determine(node.m_game_data));
    //             }

    //         }

    //         constexpr double calc_ucb1(
    //             const uint64_t parent_visits,
    //             const uint64_t node_visits,
    //             const int64_t node_value
    //         ) {
    //             const auto frac = static_cast<double>(parent_visits) / static_cast<double>(node_visits);
    //             const auto res = static_cast<double>(node_value) + 2 * std::sqrt(std::log(frac));
    //             return res;
    //         }


    //         std::vector<Node>::iterator get_max_score_child(Node& node) {
    //             return std::max_element(std::begin(node.m_children), std::end(node.m_children),
    //                 [parent_visits=node.m_visits](const Node& lhs, const Node& rhs) {
    //                     if(lhs.m_visits == 0) return true;
    //                     if(rhs.m_visits == 0) return false;
    //                     const auto lhs_usb = calc_ucb1(parent_visits, lhs.m_visits, lhs.m_value);
    //                     const auto rhs_usb = calc_ucb1(parent_visits, rhs.m_visits, rhs.m_value);
    //                     return lhs_usb > rhs_usb;
    //                 }
    //             );
    //         }

    //         void select(
    //             Node& node,
    //             std::default_random_engine& generator
    //         ) {

    //             if(node.m_children.empty()) {
    //                 if(node.m_visits == 0) {
    //                     rollout_back_propagate(node, generator);
    //                 } else {
    //                     expand::execute(node);
    //                     if(node.m_children.empty()) {
    //                         back_propagate_terminal(node);
    //                     } else {
    //                         rollout_back_propagate(node, generator);
    //                     }
    //                 }
    //             } else {
    //                 select(*get_max_score_child(node), generator);
    //             }
    //         }

    //         StrategyResult calculate_move(const GameData& game_data) {
    //             auto root = Node{nullptr, game_data};
    //             {
    //                 auto generator = std::default_random_engine(std::chrono::system_clock::now().time_since_epoch().count());
    //                 using namespace std::chrono_literals;
    //                 const auto start = std::chrono::high_resolution_clock::now();
    //                 while(std::chrono::high_resolution_clock::now() - start < 2s) {
    //                     select(root, generator);
    //                 }
    //             }
    //             if(root.m_children.empty()) {
    //                 return turn_actions::DeclareLoss{game_data.m_current_player_index};
    //             }
    //             std::distance(std::begin(root.m_children), get_max_score_child(root));


    //         }
    //     }
    // }

    Active::Active(const GameData& game_data) : m_task{
        std::async(
            std::launch::async,
            [game_data_c = game_data]() -> PlayerMessage::Type {
                return random::calculate_move(game_data_c);
            }
        )
    } {}

    PlayerMessage::Type iter(Form& form, const GameData& game_data) {
        return std::visit(utils::overloaded{
            [&form, &game_data](const Initial) -> PlayerMessage::Type {
                form.m_state = Active{game_data};
                return PlayerMessage::PlayerMadeNoSelection{};
            },
            [](Active& form) -> PlayerMessage::Type {
                if(
                    form.m_task.valid()
                    and form.m_task.wait_for(std::chrono::system_clock::duration::min()) == std::future_status::ready
                ) {
                    return utils::variant_move<PlayerMessage::Type>(form.m_task.get());
                }
                return PlayerMessage::PlayerMadeNoSelection{};
            }
        }, form.m_state);
    }

}