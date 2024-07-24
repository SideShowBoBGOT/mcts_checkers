#pragma once
#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui_internal.h"

namespace mcts_checkers {

    namespace utils {
        template<class... Ts>
        struct overloaded : Ts... { using Ts::operator()...; };

        template<class... Ts>
        overloaded(Ts...) -> overloaded<Ts...>;


    }
}