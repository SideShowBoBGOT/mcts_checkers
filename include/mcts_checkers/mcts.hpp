#pragma once
#include <vector>
#include <cstdint>

namespace mcts_checkers {

    class MCTS {
        public:
            void calc_move();

            std::vector<bool> m_is_expanded;
            std::vector<int64_t> m_values;
            std::vector<uint64_t> m_times_visited;
            std::vector<std::vector<size_t>> m_children_indexes;
    };

}