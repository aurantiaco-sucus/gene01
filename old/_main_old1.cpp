#include <iostream>
#include <array>
#include <random>
#include <cassert>
#include <numbers>

template<size_t M>
using Instance = std::array<double, M>;

template<size_t M>
std::ostream &operator<<(std::ostream &os, const Instance<M> inst) {
    os << "(";
    for (int i = 0; i < inst.size(); ++i) {
        for (const auto &comp: inst) {
            os << inst[i];
            if (i != inst.size() - 1) {
                os << ", ";
            }
        }
    }
    os << ")";
    return os;
}

template<size_t M, size_t N>
using Swarm = std::array<Instance<M>, N>;

template<typename T, size_t M>
concept InstGenerator = requires(T a)
{
    { a.next() } -> std::convertible_to<Instance<M> >;
};

template<size_t M, size_t N, InstGenerator<M> G, double (*Crit)(Instance<M>)>
Instance<M> diff_evo(G inst_gen, const int n_epoch) {
    for (Swarm<M, N> swarm; auto &inst: swarm) {
        inst = inst_gen.next();
    }
    for (int i = 0; i < n_epoch; ++i) {
    }
}

template<std::uniform_random_bit_generator T, size_t M>
class BoundedGenerator final {
    T engine;
    std::uniform_real_distribution<> dist;

public:
    BoundedGenerator() = delete;

    explicit BoundedGenerator(T engine, const double bound):
    engine(std::move(engine)), dist(-bound, bound) {}

    Instance<M> next() {
        return Instance{dist(engine), dist(engine)};
    }
};

double ex1_pred(const Instance<2> inst) {
    const double x = inst[0];
    const double y = inst[1];
    const double exp1 = -0.2 * std::sqrt((std::pow(x, 2.0) + std::pow(y, 2.0)) / 2.0);
    const double exp2 = (std::cos(2 * std::numbers::pi * x) + std::cos(2 * std::numbers::pi * y)) / 2.0;
    return -20.0 * std::pow(std::numbers::e, exp1) - std::pow(std::numbers::e,exp2) + 20 + std::numbers::e;
}

int main_old1() {
    // NOLINTNEXTLINE(*-msc51-cpp) I deliberately want predictable results.
    const BoundedGenerator<std::mt19937, 2> inst_gen{std::mt19937(), 4.0};
    const auto ex1_result = diff_evo<2, 128, BoundedGenerator<std::mt19937, 2>, ex1_pred>(inst_gen);
    std::cout << "Result once: " << ex1_result;
    std::vector results{ex1_result};
    return 0;
}
