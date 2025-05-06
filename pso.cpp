#include "common.h"

using value = std::array<double, 2>;
using value_in = const value &;

double criterion(value_in x) {
    return std::pow(x[0], 2) + std::pow(x[1], 2);
}

value forward(value_in x, value_in v) {
    return {x[0] + v[0], x[1] + v[1]};
}

value update(
    value_in v, value_in x, value_in p_best, value_in g_best,
    const double w, const double c1, const double c2, const double r1, const double r2) {
    return {
        w * v[0] + c1 * r1 * (p_best[0] - x[0]) + c2 * r2 * (g_best[0] - x[0]),
        w * v[1] + c1 * r1 * (p_best[1] - x[1]) + c2 * r2 * (g_best[1] - x[1])
    };
}

struct particle {
    value x;
    value v;
    value p_best;
    double p_best_val;
};

template<size_t n_particle, size_t n_epoch, double bound, double w, double c1, double c2>
void demo_pso(const std::string &log_name) {
    static_assert(bound >= 0.0);
    std::ofstream log_file;
    log_file.open(log_name);
    // NOLINTNEXTLINE(*-msc51-cpp) I deliberately want predictable results.
    std::mt19937 engine;
    std::uniform_real_distribution dx(-bound, bound);
    std::uniform_real_distribution dv(-2 * bound, 2 * bound);
    std::uniform_real_distribution<> dr;
    std::vector<particle> swarm;
    value g_best;
    auto g_best_val = std::numeric_limits<double>::max();
    for (size_t i = 0; i < n_particle; ++i) {
        const auto x = value {dx(engine), dx(engine)};
        const auto x_val = criterion(x);
        if (x_val < g_best_val) {
            g_best = x;
            g_best_val = x_val;
        }
        const auto v = value {dv(engine), dv(engine)};
        swarm.push_back(particle {x , v, x, x_val});
    }
    for (size_t epoch = 0; epoch < n_epoch; ++epoch) {
        for (size_t i = 0; i < n_particle; ++i) {
            const auto r1 = dr(engine);
            const auto r2 = dr(engine);
            swarm[i].v = update(swarm[i].v, swarm[i].x, swarm[i].p_best, g_best, w, c1, c2, r1, r2);
            swarm[i].x = forward(swarm[i].x, swarm[i].v);
            if (const auto val = criterion(swarm[i].x); val < swarm[i].p_best_val) {
                swarm[i].p_best = swarm[i].x;
                swarm[i].p_best_val = val;
                if (val < g_best_val) {
                    g_best = swarm[i].x;
                    g_best_val = val;
                }
            }
        }
        log_file << g_best[0] << " " << g_best[1] << g_best_val;
        for (const auto &p : swarm) {
            log_file << " " << p.x[0] << " " << p.x[1] << " " << criterion(p.x);
        }
        log_file << std::endl;
    }
    log_file.close();
}

void main_pso() {
    std::cout << "--> Particle Swarm Optimization" << std::endl;
    demo_pso<50, 100, 10.0, 0.5, 0.25, 0.25>("pso.log");
}
