#pragma once
#include <mcts_checkers/board/form.hpp>
#include <mcts_checkers/statistic_form.hpp>
#include <mcts_checkers/checkers_data.hpp>

namespace mcts_checkers {

class MainForm {
    public:
        MainForm();
        void iter();

    private:
        board::Form m_board_form;
        statistic::Form m_statistic_form;
        GameData m_data;

        statistic::InputMessage m_statistic_input_message = statistic::ShowNothing{};
};

}