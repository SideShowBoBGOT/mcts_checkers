#include <mcts_checkers/board/utils.hpp>
#include <mcts_checkers/utils.hpp>
#include <mcts_checkers/checkers_data.hpp>
#include <mcts_checkers/action_collection_funcs.hpp>

namespace mcts_checkers {


    ImVec2 calc_cell_size() {
        return ImGui::GetWindowSize() / CELLS_PER_SIDE;
    }

    ImVec2 calc_cell_top_left(const BoardVector board_index) {
        return calc_cell_size() * convert_board_vector_to_imvec(board_index) + ImGui::GetCursorScreenPos();
    }

    namespace turn_actions {

        namespace {
            
            bool is_current_player_checker(const GameData& game_data, const CheckerIndex checker_index) {
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

        }

        Type determine(const GameData& game_data) {
            if(game_data.m_moves_count >= MAX_MOVES_COUNT) {
                return DeclareDraw{};
            }

            auto attacks = MakeAttack{};
            iterate_over_current_player_checkers(game_data,
                [&attacks](const GameData& game_data, const CheckerIndex checker_index) {
                    auto collected = collect_attacks(game_data.checkers, checker_index);
                    if(not collected.actions.empty()) {
                        attacks.emplace_back(checker_index, collected);
                    }
                }
            );
            if(not attacks.empty()) {
                const auto max_depth = std::max_element(std::begin(attacks), std::end(attacks),
                    [](const auto& first, const auto& second) {
                        return first.second.depth < second.second.depth;
                    }
                )->second.depth;
                attacks.erase(std::remove_if(std::begin(attacks), std::end(attacks),
                    [max_depth](const auto& el) { return el.second.depth < max_depth; }
                ), std::end(attacks));
                return attacks;
            }
            auto moves = MakeMove{};
            iterate_over_current_player_checkers(game_data,
                [&moves](const GameData& game_data, const CheckerIndex checker_index) {
                    auto collected_moves = collect_moves(game_data.checkers, checker_index);
                    if(not collected_moves.empty()) {
                        moves.emplace_back(checker_index, utils::checked_move(collected_moves));
                    }
                }
            );
            if(not moves.empty()) {
                return moves;
            }
            return DeclareLoss{game_data.m_current_player_index};
        }

    }
}