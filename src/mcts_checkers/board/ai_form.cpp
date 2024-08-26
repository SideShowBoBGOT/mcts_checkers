#include <mcts_checkers/board/ai_form.hpp>
#include <mcts_checkers/action_application_funcs.hpp>
#include <random>
#include <fmt/printf.h>
#include <limits>

namespace mcts_checkers::board::ai {

    struct CleanedUpMemoryResource {
        explicit CleanedUpMemoryResource(MonotonicMemoryResource& memory_resource)
            : m_val(memory_resource) {
            memory_resource.release();
        }
        MonotonicMemoryResource& m_val;
    };

    action_collection::turn_actions::Output::Type<MonotonicAllocator> ai_determine(
        const GameData& game_data,
        MonotonicMemoryResource& memory_resource
    ) {
        memory_resource.release();
        return action_collection::turn_actions::determine<MonotonicAllocator>(game_data, memory_resource);
    }

    namespace random {
        namespace {
            template<typename T>
            const T& get_random_element(const std::list<T, MonotonicAllocator<T>>& v, std::default_random_engine& generator) {
                assert(not v.empty() && "Distance can not be <= 0");
                auto distribution = std::uniform_int_distribution<size_t>(0, v.size() - 1);
                return *std::next(std::begin(v), distribution(generator));
            }

            void select_attack_chain(
                const action_collection::attack::Node<MonotonicAllocator>& tree,
                std::default_random_engine& generator,
                std::vector<BoardIndex, MonotonicAllocator<BoardIndex>>& chain
            ) {
                chain.emplace_back(tree.m_board_index);
                if(tree.m_child_trees.empty()) {
                    return;
                }
                const auto& child_tree = get_random_element(tree.m_child_trees, generator);
                select_attack_chain(child_tree, generator, chain);
            }

            apply_action::Move select_random_move(
                action_collection::turn_actions::Output::MakeMove<MonotonicAllocator>&& action,
                std::default_random_engine& generator
            ) {
                const auto& checker_actions = get_random_element(action, generator);
                const auto& specific_move = get_random_element(checker_actions.m_actions, generator);
                return {checker_actions.m_checker_index, specific_move};
            }

            apply_action::Attack<MonotonicAllocator> select_random_attack(
                action_collection::turn_actions::Output::MakeAttack<MonotonicAllocator>&& action,
                std::default_random_engine& generator,
                MonotonicMemoryResource& memory_resource
            ) {
                const auto& checker_actions = get_random_element(action, generator);

                auto chain = std::vector<BoardIndex, MonotonicAllocator<BoardIndex>>(MonotonicAllocator<BoardIndex>(memory_resource));
                chain.reserve(checker_actions.depth);

                const auto& start_tree = get_random_element(checker_actions.m_actions, generator);
                select_attack_chain(start_tree, generator, chain);

                return {checker_actions.m_checker_index, utils::checked_move(chain)};
            }

            [[maybe_unused]] PlayerMessage::Type calculate_move(const GameData& game_data, MonotonicMemoryResource& memory_resource) {
                auto generator = std::default_random_engine(std::chrono::system_clock::now().time_since_epoch().count());
                return std::visit(utils::overloaded{
                    [](const action_collection::turn_actions::Output::DeclareLoss message) -> PlayerMessage::Type {
                        return PlayerMessage::DeclareLoss{message.m_player_index};
                    },
                    [](const action_collection::turn_actions::Output::DeclareDraw) -> PlayerMessage::Type {
                        return PlayerMessage::DeclareDraw{};
                    },
                    [&generator, &game_data](action_collection::turn_actions::Output::MakeMove<MonotonicAllocator>&& action) -> PlayerMessage::Type {
                        const auto selected_move = select_random_move(utils::checked_move(action), generator);
                        auto new_data = game_data;
                        apply_action::move(new_data, selected_move);
                        return PlayerMessage::PlayerMadeSelection{new_data};
                    },
                    [&generator, &game_data, &memory_resource](action_collection::turn_actions::Output::MakeAttack<MonotonicAllocator>&& action) -> PlayerMessage::Type {
                        const auto selected_attack = select_random_attack(utils::checked_move(action), generator, memory_resource);
                        auto new_data = game_data;
                        apply_action::attack(new_data, selected_attack);
                        return PlayerMessage::PlayerMadeSelection{new_data};
                    }
                }, ai_determine(game_data, memory_resource));
            }
        }
    }

    namespace mcts {
        namespace {
            struct Node {
                Node(Node* const parent, const GameData& game_data, MonotonicMemoryResource& tree_memory_resource)
                    : m_parent(parent), m_game_data(game_data), m_children(MonotonicAllocator<Node>(tree_memory_resource)) {}
                uint64_t m_visits = 0;
                int64_t m_value = 0;
                Node* m_parent = nullptr;
                GameData m_game_data;
                std::list<Node, MonotonicAllocator<Node>> m_children;
            };

            void back_propagate(Node& node, const int8_t score) {
                node.m_value = static_cast<int64_t>(score);
                ++node.m_visits;
                auto parent = node.m_parent;
                while(parent != nullptr) {
                    parent->m_value += score;
                    ++parent->m_visits;
                    parent = parent->m_parent;
                }
            }

            int8_t rollout(
                const Node& node,
                std::default_random_engine& generator,
                const PlayerIndex ai_player_index,
                MonotonicMemoryResource& memory_resource
            ) {
                auto game_data = node.m_game_data;
                int8_t score = 0;
                while(
                    std::visit(utils::overloaded{
                        [&const_game_data = std::as_const(game_data), &score, ai_player_index]
                            (const action_collection::turn_actions::Output::DeclareLoss action) {
                            score = ai_player_index == action.m_player_index ? -1 : 1;
                            return false;
                        },
                        [](const action_collection::turn_actions::Output::DeclareDraw) {
                            return false;
                        },
                        [&game_data, &generator](action_collection::turn_actions::Output::MakeMove<MonotonicAllocator>&& action) {
                            const auto confirmed_action = random::select_random_move(utils::checked_move(action), generator);
                            apply_action::move(game_data, confirmed_action.m_checker_index, confirmed_action.m_destination);
                            return true;
                        },
                        [&game_data, &generator, &memory_resource](action_collection::turn_actions::Output::MakeAttack<MonotonicAllocator>&& action) {
                            const auto confirmed_action = random::select_random_attack(utils::checked_move(action), generator, memory_resource);
                            apply_action::attack(game_data, confirmed_action);
                            return true;
                        }
                    }, ai_determine(game_data, memory_resource))
                ) {}
                return score;
            }

            namespace expand {

                void visitor(
                    Node& node,
                    action_collection::turn_actions::Output::MakeMove<MonotonicAllocator>&& checkers_actions,
                    MonotonicMemoryResource&,
                    MonotonicMemoryResource& tree_memory_resource
                ) {
                    for(const auto& [local_actions, checker_index] : checkers_actions) {
                        for(const auto checker_action : local_actions) {
                            auto child_data = node.m_game_data;
                            apply_action::move(child_data, checker_index, checker_action);
                            node.m_children.emplace_back(&node, child_data, tree_memory_resource);
                        }
                    }
                }

                void attack_add_children(
                    const action_collection::attack::Node<MonotonicAllocator>& tree,
                    const CheckerIndex checker_index,
                    Node& node,
                    std::vector<BoardIndex, MonotonicAllocator<BoardIndex>>& chain,
                    MonotonicMemoryResource& tree_memory_resource
                ) {
                    chain.emplace_back(tree.m_board_index);
                    ON_SCOPE_EXIT { chain.pop_back(); };
                    if(tree.m_child_trees.empty()) {
                        auto child_data = node.m_game_data;
                        apply_action::attack(child_data, checker_index, chain);
                        node.m_children.emplace_back(&node, child_data, tree_memory_resource);
                        return;
                    }
                    for(const auto& child_tree : tree.m_child_trees) {
                        attack_add_children(child_tree, checker_index, node, chain, tree_memory_resource);
                    }
                }

                void visitor(
                    Node& node,
                    action_collection::turn_actions::Output::MakeAttack<MonotonicAllocator>&& checkers_actions,
                    MonotonicMemoryResource& action_memory_resource,
                    MonotonicMemoryResource& tree_memory_resource
                ) {
                    auto chain = std::vector<BoardIndex, MonotonicAllocator<BoardIndex>>{MonotonicAllocator<BoardIndex>(action_memory_resource)};
                    chain.reserve(checkers_actions.front().depth);

                    for(const auto& [local_actions, checker_index, _] : checkers_actions) {
                        for(const auto& tree : local_actions) {
                            attack_add_children(tree, checker_index, node, chain, tree_memory_resource);
                        }
                    }
                }
                void visitor(Node&, action_collection::turn_actions::Output::DeclareDraw, MonotonicMemoryResource&, MonotonicMemoryResource&) {}
                void visitor(Node&, action_collection::turn_actions::Output::DeclareLoss, MonotonicMemoryResource&, MonotonicMemoryResource&) {}

                void execute(Node& node, MonotonicMemoryResource& memory_resource, MonotonicMemoryResource& tree_memory_resource) {
                    std::visit([&node, &memory_resource, &tree_memory_resource](auto&& checkers_actions) {
                        visitor(node, utils::checked_move(checkers_actions), memory_resource, tree_memory_resource);
                    }, ai_determine(node.m_game_data, memory_resource));
                }

            }

            constexpr double calc_ucb1(
                const uint64_t parent_visits,
                const uint64_t node_visits,
                const int64_t node_value
            ) {
                const auto frac = std::log(static_cast<double>(parent_visits)) / static_cast<double>(node_visits);
                const auto res = static_cast<double>(node_value) / static_cast<double>(node_visits) + 10 * std::sqrt(frac);
                return std::isnan(res) ? std::numeric_limits<double>::infinity() : res;
            }

            std::list<Node, MonotonicAllocator<Node>>::iterator get_max_score_child(Node& node) {
                return std::ranges::max_element(node.m_children,
                    [parent_visits=node.m_visits](const Node& lhs, const Node& rhs) {
                        const auto lhs_usb = calc_ucb1(parent_visits, lhs.m_visits, lhs.m_value);
                        const auto rhs_usb = calc_ucb1(parent_visits, rhs.m_visits, rhs.m_value);
                        return lhs_usb < rhs_usb;
                    }
                );
            }

            PlayerMessage::Type calculate_move(
                const GameData& game_data,
                MonotonicMemoryResource& action_memory_resource,
                MonotonicMemoryResource& tree_memory_resource
            ) {
                tree_memory_resource.release();

                auto root = Node{nullptr, game_data, tree_memory_resource};
                {
                    const auto player_message = ai_determine(game_data, action_memory_resource);
                    if(std::holds_alternative<action_collection::turn_actions::Output::DeclareDraw>(player_message)) {
                        return PlayerMessage::DeclareDraw{};
                    }
                    if(const auto loss = std::get_if<action_collection::turn_actions::Output::DeclareLoss>(&player_message)) {
                        return PlayerMessage::DeclareLoss{loss->m_player_index};
                    }
                }
                {
                    auto generator = std::default_random_engine(std::chrono::system_clock::now().time_since_epoch().count());
                    for(size_t i = 0; i < 10000; ++i) {
                        auto current_node = &root;
                        while(true) {
                            if(current_node->m_visits == 0) {
                                expand::execute(*current_node, action_memory_resource, tree_memory_resource);
                                if(current_node->m_children.empty()) {
                                    back_propagate(*current_node, 0);
                                } else {
                                    const auto score = rollout(current_node->m_children.front(), generator, game_data.m_current_player_index, action_memory_resource);
                                    back_propagate(*current_node, score);
                                }
                                break;
                            }
                            if(current_node->m_children.empty()) {
                                back_propagate(*current_node, 0);
                                break;
                            }
                            current_node = &*get_max_score_child(*current_node);
                        }
                    }
                }
                assert(not root.m_children.empty());
                const auto max_score_child = get_max_score_child(root);
                fmt::print("tree used memory bytes: {}\n", tree_memory_resource.used());
                return PlayerMessage::PlayerMadeSelection{max_score_child->m_game_data};
            }
        }
    }

    Active::Active(const GameData& game_data) : m_task{
        std::async(
            std::launch::async,
            [game_data_c = game_data]() -> PlayerMessage::Type {
                static auto ACTION_MEMORY_RESOURCE_BUFFER = std::vector<std::byte>(1024 * 60);
                static auto ACTION_MEMORY_RESOURCE = MonotonicMemoryResource(ACTION_MEMORY_RESOURCE_BUFFER);
                static auto TREE_MEMORY_RESOURCE_BUFFER = std::vector<std::byte>(static_cast<size_t>(std::pow(1024, 3)));
                static auto TREE_MEMORY_RESOURCE = MonotonicMemoryResource(TREE_MEMORY_RESOURCE_BUFFER);
                return mcts::calculate_move(game_data_c, ACTION_MEMORY_RESOURCE, TREE_MEMORY_RESOURCE);
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