#pragma once
#define IMGUI_DEFINE_MATH_OPERATORS
#include <variant>

#define ON_SCOPE_EXIT_CAT2(x, y) x##y
#define ON_SCOPE_EXIT_CAT(x, y) ON_SCOPE_EXIT_CAT2(x, y)
#define ON_SCOPE_EXIT                   auto const ON_SCOPE_EXIT_CAT(__on_scope_exit_, __LINE__) = on_scope_exit_init | [&]()
#define INLINE                          inline __attribute__((always_inline))

constexpr struct on_scope_exit_init_t {} on_scope_exit_init;

template<typename ToDo>
struct on_scope_exit_t : ToDo {
    on_scope_exit_t(ToDo&& todo) : ToDo{ todo } {}
    ~on_scope_exit_t() { (*this)(); }
};

template<typename ToDo>
auto operator |(on_scope_exit_init_t, ToDo&& todo) {
    return on_scope_exit_t<ToDo>{ std::forward<ToDo>(todo) };
}

namespace mcts_checkers {

    namespace utils {
        template <typename T>
        decltype(auto) checked_move(T&& arg) noexcept {
            using unreferenced_t = std::remove_reference_t<T>;
            static_assert(std::is_move_constructible_v<unreferenced_t>, "T must be move constructible");
            static_assert(std::is_move_assignable_v<unreferenced_t>, "T must be move assignable");
            static_assert(not std::is_const_v<unreferenced_t>, "T must not be const");
            return static_cast<unreferenced_t&&>(arg);
        }

        template<class... Ts>
        struct overloaded : Ts... { using Ts::operator()...; };

        template<class... Ts>
        overloaded(Ts...) -> overloaded<Ts...>;

        template<typename ToVariant, typename... Ts>
        ToVariant variant_move(std::variant<Ts...>&& from_variant) {
            return std::visit([](auto&& el) -> ToVariant {
                return utils::checked_move(el);
            }, utils::checked_move(from_variant));
        }



    }
}