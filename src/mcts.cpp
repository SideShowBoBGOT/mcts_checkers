#include <mcts_checkers/mcts.hpp>
#include <mcts_checkers/checkers_data.hpp>
#include <cmath>


namespace mcts_checkers {

    // static constexpr auto EXPLORATION_CONSTANT = 1.41421356237309504880168872420969807856967187537694807317667973799;
    //
    // double calc_ucb1(const int64_t value, const uint64_t parent_times_visited, const uint64_t times_visited) {
    //     return static_cast<double>(value) + EXPLORATION_CONSTANT * std::sqrt(std::log(parent_times_visited) / static_cast<double>(times_visited));
    // }
    //
    //  size_t select_best_child(
    //     const std::vector<int64_t>& values,
    //     const std::vector<uint64_t>& times_visited,
    //     const std::vector<size_t>& child_indexes,
    //     const uint64_t parent_times_visited
    //     ) {
    //     auto it = std::begin(child_indexes);
    //     auto max_ucb1 = calc_ucb1(values[*it], parent_times_visited, times_visited[*it]);
    //     size_t max_ucb1_index = *it;
    //     for(; it != std::end(child_indexes); ++it) {
    //         const auto current_ucb1 = calc_ucb1(values[*it], parent_times_visited, times_visited[*it]);
    //         if(current_ucb1 >= max_ucb1) {
    //             max_ucb1 = current_ucb1;
    //             max_ucb1_index = *it;
    //         }
    //     }
    //     return max_ucb1_index;
    // }
    //
    // static void expand(
    //     const GameData& checkers_data,
    //     std::vector<bool>& is_expanded,
    //     std::vector<int64_t>& values,
    //     std::vector<uint64_t>& times_visited,
    //     std::vector<std::vector<size_t>>& children_indexes,
    //     std::vector<uint8_t>& actions
    //
    // ) {
    // }
    //
    // static void rollout() {
    //
    // }
    //
    // static void selection() {
    //     if(is_leaf(node)) {
    //         if(ni == 0) {
    //             rodllout(node);
    //         } else {
    //             for(action in node.actions) {
    //                 add_new_state(node, action);
    //             }
    //             rollout(childnode);
    //         }
    //     } else {
    //         node = max_ucb1(node.children);
    //     }
    //
    // }
    //
    // size_t MCTS::calc_move(const size_t index) {
    //     while(true) {
    //         if(m_is_expanded[index]) {
    //             if(m_children_indexes[index].empty()) {
    //                 return index;
    //             }
    //             return select_best_child(
    //                 m_values, m_times_visited,
    //                 m_children_indexes[index], m_times_visited[index]
    //             );
    //         }
    //         expand(node);
    //         m_is_expanded[index] = true;
    //         if(m_children_indexes[index].empty()) {
    //             return index;
    //         } else {
    //             node.children[0];
    //         }
    //     }
    // }
}