#pragma once
#include <mcts_checkers/board_form.hpp>
#include <mcts_checkers/statistic_form.hpp>

namespace mcts_checkers {

class MainForm {
    public:
        MainForm();
        void iter();

    private:
        BoardForm m_board_form;
        StatisticForm m_statistic_form;
};

}