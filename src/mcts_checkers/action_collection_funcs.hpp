#pragma once
#include <mcts_checkers/checkers_data.hpp>
#include <boost/container/static_vector.hpp>
#include <mcts_checkers/index_converters.hpp>
#include <mcts_checkers/allocators.hpp>

#include <array>
#include <algorithm>
#include <memory_resource>
#include <mcts_checkers/utils.hpp>
#include <list>

#include <range/v3/algorithm/max_element.hpp>



namespace mcts_checkers::action_collection {
    constexpr auto DEVIATIONS = std::array{
        std::array<int8_t, 2>{-1, -1},
        std::array<int8_t, 2>{CELLS_PER_SIDE, +1},
    };

    namespace move {
        template<template<typename> typename AllocatorType>
        struct Output {
            Output(const CheckerIndex checker_index, const AllocatorType<MoveAction>& allocator)
                : m_actions{allocator}, m_checker_index{checker_index} {}
            std::list<MoveAction, AllocatorType<MoveAction>> m_actions;
            CheckerIndex m_checker_index;
        };

        template<template<typename> typename AllocatorType>
        Output<AllocatorType> collect(
            const CheckersData& data,
            const CheckerIndex checker_index,
            const AllocatorType<MoveAction>& allocator
        ) {
            assert(data.m_is_in_place[checker_index]);
            const auto checker_vector = convert_checker_index_to_board_vector(checker_index);
            auto actions = Output<AllocatorType>(checker_index, allocator);

            const auto is_king = data.m_is_king[checker_index];
            const auto max_iterations = is_king ? DEVIATIONS.size() : 1;
            const size_t start_deviation_index = is_king ? 0 : data.m_player_index[checker_index];
            for(size_t dev_index = start_deviation_index, i = 0; i < max_iterations; ++dev_index, ++i) {
                const auto [bound_y, dev_y] = DEVIATIONS[dev_index];
                for(const auto [bound_x, dev_x] : DEVIATIONS) {
                    auto y = static_cast<int8_t>(static_cast<int8_t>(checker_vector.y) + dev_y);
                    auto x = static_cast<int8_t>(static_cast<int8_t>(checker_vector.x) + dev_x);
                    while(y != bound_y and x != bound_x) {

                        const auto move_board_index = BoardVector{static_cast<uint8_t>(x), static_cast<uint8_t>(y)};
                        const auto move_checker_index = convert_board_vector_to_checker_index(move_board_index);
                        if(data.m_is_in_place[move_checker_index]) {
                            break;
                        }
                        actions.m_actions.emplace_back(convert_board_vector_to_board_index(move_board_index));

                        if(not is_king) break;
                        y = static_cast<int8_t>(y + dev_y);
                        x = static_cast<int8_t>(x + dev_x);
                    }
                }
            }
            return actions;
        }
    }

    namespace attack {
        template<template<typename> typename AllocatorType>
        struct Node;

        template<template<typename> typename AllocatorType>
        using NodeList = std::list<Node<AllocatorType>, AllocatorType<Node<AllocatorType>>>;

        template<template<typename> typename AllocatorType>
        struct Node {
            BoardIndex m_board_index;
            NodeList<AllocatorType> m_child_trees;
        };

        template<template<typename> typename AllocatorType>
        struct Output {
            NodeList<AllocatorType> actions;
            CheckerIndex m_checker_index;
            uint64_t depth;
        };

        template<template<typename> typename AllocatorType>
        Output<AllocatorType> collect(
            const CheckersData& data,
            const CheckerIndex checker_index,
            const AllocatorType<Node<AllocatorType>>& allocator
        ) {
            assert(data.m_is_in_place[checker_index]);

            auto actions = NodeList<AllocatorType>(allocator);
            auto action_sizes = boost::container::static_vector<uint64_t, BOARD_CELLS_COUNT>();

            const auto is_king = data.m_is_king[checker_index];
            const auto checker_player = data.m_player_index[checker_index];
            const auto checker_vector = convert_checker_index_to_board_vector(checker_index);
            for(const auto [bound_y, dev_y] : DEVIATIONS) {
                for(const auto [bound_x, dev_x] : DEVIATIONS) {
                    auto y = static_cast<int8_t>(static_cast<int8_t>(checker_vector.y) + dev_y);
                    auto x = static_cast<int8_t>(static_cast<int8_t>(checker_vector.x) + dev_x);
                    while(y != bound_y and x != bound_x) {
                        const auto enemy_board_index = BoardVector{static_cast<uint8_t>(x), static_cast<uint8_t>(y)};
                        const auto enemy_checker_index = convert_board_vector_to_checker_index(enemy_board_index);
                        if(data.m_is_in_place[enemy_checker_index]) {
                            if(data.m_player_index[enemy_checker_index] != checker_player) {
                                auto block_y = static_cast<int8_t>(y + dev_y);
                                auto block_x = static_cast<int8_t>(x + dev_x);

                                while(block_y != bound_y and block_x != bound_x) {

                                    const auto block_board_vector = BoardVector{static_cast<uint8_t>(block_x), static_cast<uint8_t>(block_y)};
                                    const auto block_checker_index = convert_board_vector_to_checker_index(block_board_vector);

                                    if(data.m_is_in_place[block_checker_index]) break;

                                    auto& attack_action = actions.emplace_back(convert_checker_index_to_board_index(block_checker_index));

                                    auto new_data = data;
                                    new_data.m_is_in_place[enemy_checker_index] = false;
                                    new_data.m_is_in_place[checker_index] = false;
                                    new_data.m_is_in_place[block_checker_index] = true;
                                    new_data.m_player_index[block_checker_index] = checker_player;
                                    new_data.m_is_king[block_checker_index] = is_king;

                                    auto [action, _, action_size] = collect(new_data, block_checker_index, allocator);
                                    action_sizes.emplace_back(action_size + 1);
                                    attack_action.m_child_trees = std::move(action);

                                    if(not is_king) break;

                                    block_y = static_cast<int8_t>(block_y + dev_y);
                                    block_x = static_cast<int8_t>(block_x + dev_x);
                                }
                            }
                            break;
                        }
                        if(not is_king) break;

                        y = static_cast<int8_t>(y + dev_y);
                        x = static_cast<int8_t>(x + dev_x);
                    }
                }
            }

            if(action_sizes.empty()) {
                return {NodeList<AllocatorType>(allocator), checker_index, 0};
            }

            const auto max_action_size = *ranges::max_element(action_sizes);
            auto filtered_vector = NodeList<AllocatorType>(allocator);
            for(size_t i = 0; i < action_sizes.size(); ++i) {
                if(action_sizes[i] == max_action_size) {
                    filtered_vector.emplace_back(utils::checked_move(*std::next(std::begin(actions), i)));
                }
            }
            return {utils::checked_move(filtered_vector), checker_index, max_action_size};
        }
    }


    namespace turn_actions {

        namespace allocators {
            using Move = LinearAllocator<move::Output<LinearAllocator>>;
            using Attack = LinearAllocator<attack::Output<LinearAllocator>>;
        }

        namespace Output {
            using MakeMove = std::list<move::Output<LinearAllocator>, LinearAllocator<move::Output<LinearAllocator>>>;
            using MakeAttack = std::list<attack::Output<LinearAllocator>, LinearAllocator<attack::Output<LinearAllocator>>>;
            struct DeclareLoss { PlayerIndex m_player_index; };
            struct DeclareDraw {};

            using Type = std::variant<DeclareLoss, DeclareDraw, MakeAttack, MakeMove>;
        }

        inline bool is_current_player_checker(const GameData& game_data, const CheckerIndex checker_index) {
            return game_data.checkers.m_is_in_place[checker_index]
                and game_data.checkers.m_player_index[checker_index] == static_cast<bool>(game_data.m_current_player_index);
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

        inline Output::Type determine(
            const GameData& game_data,
            LinearMemoryResource& memory_resource
        ) {
            if(game_data.m_moves_count >= MAX_MOVES_COUNT) {
                return Output::DeclareDraw{};
            }
            {
                auto attacks = Output::MakeAttack(allocators::Attack(memory_resource));
                iterate_over_current_player_checkers(game_data,
                    [&attacks, &memory_resource](const GameData& game_data, const CheckerIndex checker_index) {
                        auto collected = collect(
                            game_data.checkers, checker_index,
                            LinearAllocator<attack::Node<LinearAllocator>>(memory_resource)
                        );
                        if(not collected.actions.empty()) {
                            attacks.emplace_back(utils::checked_move(collected));
                        }
                    }
                );
                if(not attacks.empty()) {
                    const auto max_depth = ranges::max_element(attacks,
                        [](const auto& first, const auto& second) { return first.depth < second.depth; }
                    )->depth;
                    attacks.erase(std::remove_if(std::begin(attacks), std::end(attacks),
                        [max_depth](const auto& el) { return el.depth < max_depth; }
                    ), std::end(attacks));
                    return attacks;
                }
                memory_resource.deallocate_all();
            }
            {
                auto moves = Output::MakeMove{allocators::Move(memory_resource)};
                iterate_over_current_player_checkers(game_data,
                    [&moves, &memory_resource](const GameData& game_data, const CheckerIndex checker_index) {
                        auto collected_moves = move::collect(
                            game_data.checkers,
                            checker_index,
                            LinearAllocator<MoveAction>(memory_resource)
                        );
                        if(not collected_moves.m_actions.empty()) {
                            moves.emplace_back(utils::checked_move(collected_moves));
                        }
                    }
                );
                if(not moves.empty()) {
                    return moves;
                }
                memory_resource.deallocate_all();
            }
            return Output::DeclareLoss{game_data.m_current_player_index};
        }
    }
}
