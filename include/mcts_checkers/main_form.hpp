#pragma once
#include <mcts_checkers/board_form.hpp>
#include <mcts_checkers/statistic_form.hpp>
#include <mcts_checkers/checkers_data.hpp>

namespace mcts_checkers {

class MainForm {
    public:
        MainForm();
        void iter();

    private:
        board::Form m_board_form;
        StatisticForm m_statistic_form;
        GameData m_data;
};

}