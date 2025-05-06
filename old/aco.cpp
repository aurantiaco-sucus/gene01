#include "common.h"

template<typename T>
using Atlas = std::array<T, 65536>;
using uvec2 = std::array<size_t, 2>;

template<typename T>
T & locate(Atlas<T> & atlas, const size_t x, const size_t y) {
    return atlas[x + y * 256];
}

template<typename T>
T & locate(Atlas<T> & atlas, const uvec2 pos) {
    return atlas[pos[0] + pos[1] * 256];
}

std::optional<Atlas<char>> load_heightmap() {
    std::ifstream file("heightmap.txt");
    if (!file.is_open()) return std::nullopt;
    Atlas<char> atlas;
    for (int i = 0; i < 65536; i++) {
        char c;
        file >> c;
        atlas[i] = c;
    }
    file.close();
    return atlas;
}

struct Choice {
    uvec2 dest;
    double cost;

};

std::array<uvec2, 8>

void demo_aco() {
    auto hm_opt = load_heightmap();
    if (!hm_opt.has_value()) {
        std::cerr << "Failed to load heightmap." << std::endl;
        return;
    }
    auto& hm = hm_opt.value();
    Atlas<double> pm;
    for (auto & val : pm) val = 0;
    for (int i = 1; i < 255; ++i) {

    }
}

void main_aco() {
    std::cout << "--> Ant Colony Optimization" << std::endl;
    demo_aco();
}