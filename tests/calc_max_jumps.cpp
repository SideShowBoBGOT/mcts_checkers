#include <bitset>
#include <concepts>
#include <array>

constexpr uint8_t BOARD_SIDE_CELL_COUNT = 10;
constexpr uint8_t BOARD_CELLS_COUNT = BOARD_SIDE_CELL_COUNT * BOARD_SIDE_CELL_COUNT;
constexpr uint8_t CHEKCERS_CELLS_COUNT = BOARD_CELLS_COUNT / 2;
constexpr uint8_t EDGES_PER_ROW = BOARD_SIDE_CELL_COUNT - 1;
constexpr uint8_t EDGES_COUNT = EDGES_PER_ROW * EDGES_PER_ROW;

template<typename T>
struct Vector {
    T x{};
    T y{};
};

template<std::integral T>
static constexpr bool is_even(const T value) {
    return value % 2 == 0;
}

static constexpr bool is_coord_not_valid(const int8_t coord) {
    return coord < 0 or coord >= static_cast<int8_t>(BOARD_CELLS_COUNT);
}

static uint16_t calc_jumps_count(const std::bitset<EDGES_COUNT> edges, const Vector<uint8_t> pos) {
    constexpr std::array<int8_t, 2> even_deviations_x = {0, 1};
    constexpr std::array<int8_t, 2> uneven_deviations_x = {0, -1};
    constexpr auto deviation_cases = std::array{
        std::array{even_deviations_x, uneven_deviations_x},
        std::array{uneven_deviations_x, even_deviations_x}
    };
    const auto deviations_x = deviation_cases[is_even(pos.y)];
    for(const int8_t dev_x : deviations_x[0]) {
        const auto next_dev_x = deviations_x[1][dev_x != deviations_x[0][0]];
        const auto jump_x = static_cast<int8_t>(next_dev_x + dev_x + pos.x);
        if(is_coord_not_valid(jump_x)) {
            continue;
        }
        for(const int8_t dev_y : {-1, 1}) {
            const auto jump_y = pos.y + 2 * dev_y;
            if(is_coord_not_valid(jump_y)) {
                continue;
            }
            const
            const auto jump_pos = Vector<int8_t>{
                pos.x + dev_x + next_dev_x,
                static_cast<int8_t>()
            };
        }
    }
}

int main() {



}