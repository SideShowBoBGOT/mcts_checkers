#pragma once
#include <variant>

namespace mcts_checkers::statistic {

    struct Nothing {};
    struct ReplayGame {};

    using Message = std::variant<Nothing, ReplayGame>;

    struct Form {};
    Message iter_out(const Form& stat);

}