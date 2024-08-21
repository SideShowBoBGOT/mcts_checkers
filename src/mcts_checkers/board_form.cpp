#include <mcts_checkers/board_form.hpp>
#include <mcts_checkers/utils.hpp>
#include <mcts_checkers/checkers_data.hpp>
#include <mcts_checkers/action_collection_funcs.hpp>
#include <mcts_checkers/index_converters.hpp>
#include <tl/optional.hpp>
#include <ranges>
#include <range/v3/view/drop.hpp>
#include <range/v3/view/transform.hpp>
#include <range/v3/range/conversion.hpp>
#include <utility>
#include <mcts_checkers/action_application_funcs.hpp>
#include <future>
#include <random>
#include <imgui_internal.h>

namespace mcts_checkers::board {

    struct PlayerMadeNoSelection {};

    using IterationResult = std::variant<
        PlayerMadeNoSelection,
        selection_confirmed::Move,
        selection_confirmed::Attack,
        turn_actions::DeclareLoss,
        turn_actions::DeclareDraw
    >;

    
        
}

namespace mcts_checkers::board::human {

    static constexpr uint8_t convert_board_index(const ImVec2ih board_index) {
        return board_index.y * CELLS_PER_SIDE + board_index.x;
    }

    static constexpr bool is_white_cell(const BoardVector cell_index) {
        return is_even(cell_index.y) == is_even(cell_index.x);
    }

    static constexpr tl::optional<CheckerIndex> try_convert_board_vector_to_checker_index(const BoardVector cell_index) {
        if(is_white_cell(cell_index)) {
            return tl::nullopt;
        }
        return convert_board_vector_to_checker_index(cell_index);
    }

    

    

    

    struct StateNotChange{};

    namespace unselected {

        using Form = std::variant<MoveForm, AttackForm>;

        template<typename SelectedForm, typename Form>
        static std::variant<StateNotChange, SelectedForm> iter(Form& form) {
            unselected_selected_common::draw_action_cells(form.m_actions);
            if(is_window_hovered()) {
                return unselected_selected_common::select_checker<SelectedForm>(form);
            }
            return StateNotChange{};
        }

    }

    static std::variant<StateNotChange, selected::attack::Form> iter(
        unselected::AttackForm& form, const GameData&
    ) {
        return unselected::iter<selected::attack::Form>(form);
    }

    static std::variant<StateNotChange, selected::MoveForm> iter(
        unselected::MoveForm& form, const GameData&
    ) {
        return unselected::iter<selected::MoveForm>(form);
    }

    namespace selected {

        

        

        

        using OtherCheckerSelected = strong::type<CheckerIndex, struct OtherCheckerSelected_>;
        using IterationResult = std::variant<
            StateNotChange,
            MoveForm,
            attack::Form,
            selection_confirmed::Move,
            selection_confirmed::Attack
        >;

        
    }

    namespace initial {
        using IterationResult = std::variant<
            unselected::MoveForm,
            unselected::AttackForm,
            turn_actions::DeclareLoss,
            turn_actions::DeclareDraw
        >;
    }

    initial::IterationResult iter(const initial::Form, const GameData& game_data) {
        
    }

    IterationResult iter(Form& form, const GameData& game_data) {
        using TransitionResult = std::variant<
            StateNotChange,
            unselected::MoveForm,
            unselected::AttackForm,
            selected::MoveForm,
            selected::attack::Form,

            selection_confirmed::Move,
            selection_confirmed::Attack,
            turn_actions::DeclareLoss,
            turn_actions::DeclareDraw
        >;

        auto new_state = std::visit(
            [&game_data](auto& state) -> TransitionResult {
            return utils::variant_move<TransitionResult>(human::iter(state, game_data));
        }, form.value_of());

        return std::visit(utils::overloaded{
            [](const StateNotChange) -> IterationResult {
                return PlayerMadeNoSelection{};
            },
            [&form](const selection_confirmed::Move action) -> IterationResult {
                form.value_of() = initial::Form{};
                return action;
            },
            [&form](selection_confirmed::Attack&& action) -> IterationResult  {
                form.value_of() = initial::Form{};
                return utils::checked_move(action);
            },
            [&form](const turn_actions::DeclareLoss action) -> IterationResult {
                form.value_of() = initial::Form{};
                return action;
            },
            [&form](const turn_actions::DeclareDraw action) -> IterationResult {
                form.value_of() = initial::Form{};
                return action;
            },
            [&form](auto&& state) -> IterationResult {
                form.value_of() = utils::checked_move(state);
                return PlayerMadeNoSelection{};
            }
        }, utils::checked_move(new_state));

    }
}

namespace mcts_checkers::board::ai {

    namespace random {
        template<typename T>
        const T& get_random_element(const std::vector<T>& v, std::default_random_engine& generator) {
            assert(not v.empty() && "Distance can not be <= 0");
            auto distribution = std::uniform_int_distribution<size_t>(0, v.size() - 1);
            return v[distribution(generator)];
        }

        static void select_attack_chain(
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

        static selection_confirmed::Move select_random_move(
            turn_actions::MakeMove&& action,
            std::default_random_engine& generator
        ) {
            const auto& checker_actions = get_random_element(action, generator);
            const auto& specific_move = get_random_element(checker_actions.second, generator);
            return selection_confirmed::Move{specific_move, checker_actions.first};
        }

        static selection_confirmed::Attack select_random_attack(
            turn_actions::MakeAttack&& action,
            std::default_random_engine& generator
        ) {
            const auto& checker_actions = get_random_element(action, generator);

            auto chain = std::vector<AttackAction>{};
            chain.reserve(checker_actions.second.depth + 1);
            chain.emplace_back(convert_checker_index_to_board_index(checker_actions.first));

            const auto& start_tree = get_random_element(checker_actions.second.actions, generator);
            select_attack_chain(start_tree, generator, chain);

            return selection_confirmed::Attack{utils::checked_move(chain)};
        }

        static StrategyResult calculate_move(const GameData& game_data) {
            auto generator = std::default_random_engine(std::chrono::system_clock::now().time_since_epoch().count());
            return std::visit(utils::overloaded{
                [](const turn_actions::DeclareLoss action) -> StrategyResult {
                    return action;
                },
                [](const turn_actions::DeclareDraw action) -> StrategyResult {
                    return action;
                },
                [&generator](turn_actions::MakeMove&& action) -> StrategyResult {
                    return select_random_move(utils::checked_move(action), generator);
                },
                [&generator](turn_actions::MakeAttack&& action) -> StrategyResult {
                    return select_random_attack(utils::checked_move(action), generator);
                }
            }, turn_actions::determine(game_data));
        }
    }

    namespace mcts {
        struct Node {
            Node(Node* const parent, const GameData& game_data)
                : m_parent(parent), m_game_data(game_data) {}
            uint64_t m_visits = 0;
            int64_t m_value = 0;
            Node* m_parent = nullptr;
            GameData m_game_data{};
            std::vector<Node> m_children{};
        };

        static void back_propagate(Node& node) {
            ++node.m_visits;
            auto parent = node.m_parent;
            while(node.m_parent != nullptr) {
                parent->m_value += node.m_value;
                ++parent->m_visits;
                parent = parent->m_parent;
            }
        }

        void rollout(
            Node& node,
            std::default_random_engine& generator
        ) {
            auto game_data = node.m_game_data;
            while(
                std::visit(utils::overloaded{
                    [&const_game_data = std::as_const(game_data), &node](const turn_actions::DeclareLoss action) {
                        node.m_value += const_game_data.m_current_player_index == action.m_player_index ? -1 : 1;
                        return false;
                    },
                    [](const turn_actions::DeclareDraw) {
                        return false;
                    },
                    [&game_data, &generator](turn_actions::MakeMove&& action) {
                        const auto confirmed_action = random::select_random_move(utils::checked_move(action), generator);
                        apply_move(game_data, confirmed_action.checker_index, confirmed_action.data);
                        return true;
                    },
                    [&game_data, &generator](turn_actions::MakeAttack&& action) {
                        const auto confirmed_action = random::select_random_attack(utils::checked_move(action), generator);
                        apply_attack(game_data, confirmed_action.data);
                        return true;
                    }
                }, turn_actions::determine(game_data))
            ) {}
        }

        void rollout_back_propagate(Node& node, std::default_random_engine& generator) {
            rollout(node, generator);
            back_propagate(node);
        }

        void back_propagate_terminal(Node& node) {
            ++node.m_visits;
            auto parent = node.m_parent;
            while(node.m_parent != nullptr) {
                ++parent->m_visits;
                parent = parent->m_parent;
            }
        }

        namespace expand {

            static void visitor(Node& node, turn_actions::MakeMove&& checkers_actions) {
                for(const auto& [checker_index, local_actions] : checkers_actions) {
                    for(const auto checker_action : local_actions) {
                        auto child_data = node.m_game_data;
                        apply_move(child_data, checker_index, checker_action);
                        node.m_children.emplace_back(&node, child_data);
                    }
                }
            }

            static void attack_add_children(
                const AttackTree& tree,
                Node& node,
                std::vector<AttackAction>& chain
            ) {
                chain.emplace_back(tree.m_board_index);
                if(tree.m_child_trees.empty()) {
                    auto child_data = node.m_game_data;
                    apply_attack(child_data, chain);
                    node.m_children.emplace_back(&node, child_data);
                    return;
                }
                for(const auto& child_tree : tree.m_child_trees) {
                    attack_add_children(child_tree, node, chain);
                }
            }

            static void visitor(Node& node, turn_actions::MakeAttack&& checkers_actions) {
                auto chain = std::vector<AttackAction>{};
                chain.reserve(checkers_actions.front().second.depth + 1);

                for(const auto& [checker_index, local_actions] : checkers_actions) {
                    chain.emplace_back(convert_checker_index_to_board_index(checker_index));
                    for(const auto& tree : local_actions.actions) {
                        attack_add_children(tree, node, chain);
                    }
                    chain.pop_back();
                }
            }
            static void visitor(Node&, turn_actions::DeclareDraw) {}
            static void visitor(Node&, turn_actions::DeclareLoss) {}

            static void execute(Node& node) {
                std::visit([&node](auto&& checkers_actions) {
                    visitor(node, utils::checked_move(checkers_actions));
                }, turn_actions::determine(node.m_game_data));
            }

        }

        static constexpr double calc_ucb1(
            const uint64_t parent_visits,
            const uint64_t node_visits,
            const int64_t node_value
        ) {
            const auto frac = static_cast<double>(parent_visits) / static_cast<double>(node_visits);
            const auto res = static_cast<double>(node_value) + 2 * std::sqrt(std::log(frac));
            return res;
        }


        std::vector<Node>::iterator get_max_score_child(Node& node) {
            return std::max_element(std::begin(node.m_children), std::end(node.m_children),
                [parent_visits=node.m_visits](const Node& lhs, const Node& rhs) {
                    if(lhs.m_visits == 0) return true;
                    if(rhs.m_visits == 0) return false;
                    const auto lhs_usb = calc_ucb1(parent_visits, lhs.m_visits, lhs.m_value);
                    const auto rhs_usb = calc_ucb1(parent_visits, rhs.m_visits, rhs.m_value);
                    return lhs_usb > rhs_usb;
                }
            );
        }

        void select(
            Node& node,
            std::default_random_engine& generator
        ) {

            if(node.m_children.empty()) {
                if(node.m_visits == 0) {
                    rollout_back_propagate(node, generator);
                } else {
                    expand::execute(node);
                    if(node.m_children.empty()) {
                        back_propagate_terminal(node);
                    } else {
                        rollout_back_propagate(node, generator);
                    }
                }
            } else {
                select(*get_max_score_child(node), generator);
            }
        }

        StrategyResult calculate_move(const GameData& game_data) {
            auto root = Node{nullptr, game_data};
            {
                auto generator = std::default_random_engine(std::chrono::system_clock::now().time_since_epoch().count());
                using namespace std::chrono_literals;
                const auto start = std::chrono::high_resolution_clock::now();
                while(std::chrono::high_resolution_clock::now() - start < 2s) {
                    select(root, generator);
                }
            }
            if(root.m_children.empty()) {
                return turn_actions::DeclareLoss{game_data.m_current_player_index};
            }
            std::distance(std::begin(root.m_children), get_max_score_child(root));


        }
    }

    Form::Form(const GameData& game_data) : m_task{
        std::async(
            std::launch::async,
            [game_data_c = game_data]() -> StrategyResult {
                return random::calculate_move(game_data_c);
            }
        )
    } {}

    IterationResult iter(Form& form, const GameData&) {
        if(
            form.m_task.valid()
            and form.m_task.wait_for(std::chrono::system_clock::duration::min()) == std::future_status::ready
        ) {
            return utils::variant_move<IterationResult>(form.m_task.get());
        }
        return PlayerMadeNoSelection{};
    }

}

namespace mcts_checkers::board {

    IterationResult iter(human::Form& form, const GameData& game_data) {
        return human::iter(form, game_data);
    }

    IterationResult iter(ai::Form& form, const GameData& game_data) {
        return ai::iter(form, game_data);
    }

    

    Form::Form()
        : m_state{STATE_FACTORIES[static_cast<uint8_t>(m_game_data.m_current_player_index)](m_game_data)} {}

    OutMessage iter_out(Form& form) {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        ImGui::BeginChild("BoardForm", ImVec2(0, -1), true, ImGuiWindowFlags_NoScrollWithMouse);
        ON_SCOPE_EXIT {
            ImGui::EndChild();
            ImGui::PopStyleVar();
        };

        human::draw_rects();
        human::draw_checkers(form.m_game_data);

        const auto action = std::visit(
            [&game_data = std::as_const(form.m_game_data)]
            (auto& state) -> IterationResult {
                return board::iter(state, game_data);
            }, form.m_state
        );

        return std::visit(utils::overloaded{
            [&game_data=std::as_const(form.m_game_data)](const PlayerMadeNoSelection) -> OutMessage {
                return MakingDecision{game_data.m_current_player_index};
            },
            [](const turn_actions::DeclareLoss action) -> OutMessage {
                return DeclareWin{opposite_player(action.m_player_index)};
            },
            [&game_data=std::as_const(form.m_game_data)](const turn_actions::DeclareDraw) -> OutMessage {
                return DeclareDraw{};
            },
            [&form](const selection_confirmed::Move& action) -> OutMessage {
                apply_move(form.m_game_data, action.checker_index, action.data);
                form.m_state = STATE_FACTORIES[static_cast<uint8_t>(form.m_game_data.m_current_player_index)](form.m_game_data);
                return MakingDecision{form.m_game_data.m_current_player_index};
            },
            [&form](const selection_confirmed::Attack& action) -> OutMessage {
                apply_attack(form.m_game_data, action.data);
                form.m_state = STATE_FACTORIES[static_cast<uint8_t>(form.m_game_data.m_current_player_index)](form.m_game_data);
                return MakingDecision{form.m_game_data.m_current_player_index};
            }
        }, action);
    }

}