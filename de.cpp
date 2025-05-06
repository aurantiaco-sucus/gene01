#include "common.h"

using gene = std::array<double, 2>;

double criterion(gene inst) {
    double x = inst[0];
    double y = inst[1];
    double exp1 = -0.2 * std::sqrt((std::pow(x, 2.0) + std::pow(y, 2.0)) / 2.0);
    double exp2 = (std::cos(2 * std::numbers::pi * x) + std::cos(2 * std::numbers::pi * y)) / 2.0;
    return -20.0 * std::pow(std::numbers::e, exp1) - std::pow(std::numbers::e, exp2) + 20 + std::numbers::e;
}

using mutate_func = double (*)(double, double, std::array<double, 5> &, double);

double de_rand_1(double cur, double best, std::array<double, 5> &other, double f) {
    return other[0] + f * (other[1] - other[2]);
}

double de_best_1(double cur, double best, std::array<double, 5> &other, double f) {
    return best + f * (other[0] - other[1]);
}

double de_cur2best_1(double cur, double best, std::array<double, 5> &other, double f) {
    return cur + f * (best - cur) + f * (other[0] - other[1]);
}

double de_best_2(double cur, double best, std::array<double, 5> &other, double f) {
    return best + f * (other[0] - other[1]) + f * (other[2] - other[3]);
}

double de_rand_2(double cur, double best, std::array<double, 5> &other, double f) {
    return other[0] + f * (other[1] - other[2]) + f * (other[3] - other[4]);
}

template<typename T, size_t N>
    requires std::equality_comparable<T>
bool array_contains(std::array<T, N> &arr, T &value) {
    return std::ranges::any_of(arr.begin(), arr.end(), [&](auto elem) { return elem == value; });
}

template<size_t np, double cr, double f, double bound, size_t n_epoch, mutate_func mutate>
void demo_de(const std::string &log_name) {
    static_assert(bound >= 0.0);
    std::vector<gene> swarm;
    std::vector<gene> swarm_old;
    // NOLINTNEXTLINE(*-msc51-cpp) I deliberately want predictable results.
    std::mt19937 engine;
    std::uniform_real_distribution d_init(-bound, bound);
    for (int i = 0; i < np; ++i) {
        swarm.push_back(gene{d_init(engine), d_init(engine)});
    }
    gene best = swarm[0];
    double best_val = criterion(best);
    for (auto inst: swarm) {
        if (criterion(inst) < best_val) {
            best = inst;
            best_val = criterion(inst);
        }
    }
    std::ofstream log_file;
    log_file.open(log_name);
    std::uniform_int_distribution<size_t> d_other(0, np - 1);
    std::uniform_int_distribution<size_t> d_mandated(0, 1);
    std::uniform_real_distribution<> d_prob;
    for (int epoch = 0; epoch < n_epoch; ++epoch) {
        swarm_old = swarm;
        for (int i = 0; i < np; ++i) {
            std::array<size_t, 5> other{};
            for (auto &val: other) {
                val = std::numeric_limits<size_t>::max();
            }
            for (int j = 0; j < 5; ++j) {
                size_t idx = d_other(engine);
                while (array_contains(other, idx) || idx == i) idx = d_other(engine);
                other[j] = idx;
            }
            auto mandated = d_mandated(engine);
            for (int j = 0; j < 2; ++j) {
                if (d_prob(engine) > cr && j != mandated) continue;
                std::array<double, 5> other_mapped{};
                for (int k = 0; k < 5; ++k) {
                    other_mapped[k] = swarm_old[other[k]][j];
                }
                gene updated = swarm[i];
                updated[j] = mutate(swarm[i][j], best[j], other_mapped, f);
                auto updated_val = criterion(updated);
                if (updated_val < criterion(swarm[i])) {
                    swarm[i] = updated;
                }
                if (updated_val < best_val) {
                    best = updated;
                    best_val = updated_val;
                }
            }
        }
        log_file << best[0] << " " << best[1] << " " << best_val;
        for (auto inst: swarm) {
            log_file << " " << inst[0] << " " << inst[1] << " " << criterion(inst);
        }
        log_file << std::endl;
    }
    log_file.close();
}

void main_de() {
    std::cout << "--> Differential Evolution Algorithm" << std::endl;
    demo_de<20, 0.5, 0.5, 4.0, 300, de_rand_1>("de_rand_1.log");
    demo_de<20, 0.5, 0.5, 4.0, 300, de_best_1>("de_best_1.log");
    demo_de<20, 0.5, 0.5, 4.0, 300, de_cur2best_1>("de_cur2best_1.log");
    demo_de<20, 0.5, 0.5, 4.0, 300, de_best_2>("de_best_2.log");
    demo_de<20, 0.5, 0.5, 4.0, 300, de_rand_2>("de_rand_2.log");
}
