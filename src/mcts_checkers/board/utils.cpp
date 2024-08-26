#include <mcts_checkers/board/utils.hpp>
#include <mcts_checkers/checkers_data.hpp>
#include <mcts_checkers/action_collection_funcs.hpp>
#include <mcts_checkers/board/utils.hpp>

namespace mcts_checkers {

    namespace {
        ImVec2 convert_board_vector_to_imvec(const BoardVector board_vector) {
            return {
                static_cast<float>(board_vector.x),
                static_cast<float>(board_vector.y)
            };
        }
    }

    ImVec2 calc_cell_size() {
        return ImGui::GetWindowSize() / CELLS_PER_SIDE;
    }

    ImVec2 calc_cell_top_left(const BoardVector board_index) {
        return calc_cell_size() * convert_board_vector_to_imvec(board_index) + ImGui::GetCursorScreenPos();
    }

    namespace turn_actions {



    }
}