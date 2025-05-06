#include "common.h"

constexpr size_t n_city = 4;
constexpr size_t n_ant = 3;

using CostMat = uint8_t[n_city][n_city];
// Define maximum value (255 for uint8_t) to be infinity.
constexpr auto CostInf = std::numeric_limits<uint8_t>::max();

// Assume it is undirected.
CostMat cost_mat = {
    {CostInf, 3, 1, 2},
    {3, CostInf, 5, 4},
    {1, 5, CostInf, 2},
    {2, 4, 2, CostInf},
};

using Path = std::array<size_t, n_city>;

// Assume that from every city every other city is reachable.
inline Path tsp_greedy(const size_t start = 0) {
    std::array<bool, n_city> visited {};
    visited[start] = true;
    size_t i_first_avail = 0;
    if (start == 0) ++i_first_avail;
    Path path {};
    path[0] = start;
    auto cost_local = cost_mat[start];
    for (size_t i = 1; i < n_city; ++i) {
        size_t i_min = 0;
        for (size_t j = i_first_avail; j < n_city; ++j) {
            if (cost_local[j] < cost_local[i_min]) {
                i_min = j;
            }
        }
        path[i] = i_min;
        visited[i_min] = true;
        cost_local = cost_mat[i_min];
        if (i_min == i_first_avail) {
            while (visited[i_first_avail]) ++i_first_avail;
        }
    }
    return path;
}

inline double cost_of(const Path & path) {
    auto cost = static_cast<double>(cost_mat[path[n_city - 1]][path[0]]);
    for (int i = 1; i < n_city - 1; ++i) {
        cost += static_cast<double>(cost_mat[path[i - 1]][path[i]]);
    }
    return cost;
}

using PhMat = std::array<std::array<double, n_city>, n_city>;

inline PhMat init_ph_mat() {
    const auto path = tsp_greedy();
    const auto cost = cost_of(path);
    const auto ph_init = n_ant / cost;
    PhMat phm {};
    for (int i = 0; i < n_city; ++i) {
        std::ranges::fill(phm[i], ph_init);
    }
    return phm;
}

struct AcoItem {
    size_t index;
    double weight;
};

constexpr auto weight_inf = std::numeric_limits<double>::max();

// Assume that from every city every other city is reachable.
template<double alpha, double beta>
Path tsp_aco(std::mt19937 & re, const PhMat & phm) {
    std::array<bool, n_city> visited {};
    Path path {};
    std::uniform_int_distribution<size_t> d_begin(1, n_city - 1);
    const size_t start = d_begin(re);
    visited[start] = true;
    path[0] = start;
    auto cur = start;
    std::array<AcoItem, n_city> items {};
    for (size_t i_path = 1; i_path < n_city; ++i_path) {
        double sum = 0.0;
        for (int i = 0; i < n_city; ++i) {
            items[i].index = i;
            if (cur == i || visited[i]) {
                items[i].weight = weight_inf;
                continue;
            }
            const auto fact_ph = std::pow(phm[cur][i], alpha);
            const auto fact_heu = 1.0 / std::pow(cost_mat[cur][i], beta);
            items[i].weight = fact_ph * fact_heu;
            sum += items[i].weight;
        }
        std::uniform_real_distribution d_roulette(0.0, sum);
        double value = d_roulette(re);
        auto choice = std::numeric_limits<size_t>::max();
        for (int i = 0; i < n_city; ++i) {
            if (items[i].weight == weight_inf) continue;
            if (value < items[i].weight) {
                choice = i;
                break;
            }
            value -= items[i].weight;
        }
        if (choice == std::numeric_limits<size_t>::max()) {
            std::cerr << "Error performing roulette selection." << std::endl;
            exit(1);
        }
        path[i_path] = choice;
        visited[choice] = true;
        cur = choice;
    }
    return path;
}

template<double rho>
void vapourise(PhMat & phm) {
    const auto factor = 1.0 - rho;
    for (int i = 0; i < n_city; ++i) {
        for (int j = 0; j < n_city; ++j) {
            phm[i][j] *= factor;
        }
    }
}

// Assume the pheromone map is undirected.
template<double rho>
void accumulate(PhMat & phm, const Path & path) {
    const auto comp = 1.0 / cost_of(path);
    for (int i = 1; i < n_city; ++i) {
        phm[path[i - 1]][path[i]] += comp;
        phm[path[i]][path[i - 1]] += comp;
    }
    phm[path[n_city - 1]][path[0]] += comp;
    phm[path[0]][path[n_city - 1]] += comp;
}

template<size_t n_epoch, size_t m, double alpha, double beta, double rho>
void demo_aco(const std::string & log_name) {
    std::ofstream log_file;
    log_file.open(log_name);
    // NOLINTNEXTLINE(*-msc51-cpp) I deliberately want predictable results.
    std::mt19937 engine;
    auto phm = init_ph_mat();
    std::array<Path, m> colony {};
    for (int epoch = 0; epoch < n_epoch; ++epoch) {
        for (auto & path : colony) {
            path = tsp_aco<alpha, beta>(engine, phm);
        }
        vapourise<rho>(phm);
        for (const auto & path : colony) {
            accumulate<rho>(phm, path);
        }
        for (int i = 0; i < n_city; ++i) {
            for (int j = 0; j < n_city; ++j) {
                log_file << phm[i][j] << " ";
            }
        }
        log_file << std::endl;
    }
    log_file.close();
}

void main_aco() {
    std::cout << "--> Ant Colony Optimization" << std::endl;
    demo_aco<100, 3, 1.0, 2.0, 0.5>("aco.log");
}
