#include "common.h"

using Gene = std::array<double, 4>;

struct Entity {
    Gene gene;
    double fit;
};

std::ostream &operator<<(std::ostream &os, const Entity &entity) {
    os << entity.gene[0] << " " << entity.gene[1] << " " << entity.gene[2] << " " << entity.gene[3] << " " << entity.fit;
    return os;
}

double evaluate(const Gene & sol) {
    const auto den = std::pow(sol[0], 2.0) + std::pow(sol[1], 2.0) +
        std::pow(sol[2], 2.0) + std::pow(sol[3], 2.0) + 1.0;
    return 1.0 / den;
}

template<size_t n_gene, size_t n_choice, size_t n_epoch, double bound, double p_cross, double p_mutate>
void demo_ga(const std::string &log_name) {
    static_assert(bound >= 0.0);
    std::ofstream log_file;
    log_file.open(log_name);

    // initialize the flock
    std::array<Entity, n_gene> flock {};
    Entity best { Gene {}, std::numeric_limits<double>::min() };
    // NOLINTNEXTLINE(*-msc51-cpp) I deliberately want predictable results.
    std::mt19937 engine;
    std::uniform_real_distribution d_init(-bound, bound);
    for (auto & entity: flock) {
        for (auto & value: entity.gene) {
            value = d_init(engine);
        }
        entity.fit = evaluate(entity.gene);
        if (entity.fit > best.fit) {
            best = entity;
        }
    }

    // evolution loop
    std::bernoulli_distribution d_cross(p_cross);
    for (size_t epoch = 0; epoch < n_epoch; ++epoch) {
        // compute the sum of flock's fitness values
        double sum = 0.0;
        for (auto & entity: flock) {
            sum += entity.fit;
        }

        // selection
        std::array<Entity, n_choice> pool {};
        std::uniform_real_distribution d_selected(0.0, sum);
        for (size_t i = 0; i < n_choice; ++i) {
            double r = d_selected(engine);
            for (auto & entity: flock) {
                r -= entity.fit;
                if (r <= 0.0) {
                    pool[i] = entity;
                    break;
                }
            }
        }

        // crossover
        // decide availability
        std::bitset<n_choice> avail;
        for (size_t i = 0; i < n_choice; ++i) {
            avail[i] = d_cross(engine);
        }

        // actual operation
        std::uniform_int_distribution d_comp(0, 3);
        for (size_t i = 0; i < n_choice; ++i) {
            if (!avail[i]) {
                // not available and stay same
                flock[i] = pool[i];
                continue;
            }
            size_t target = i + 1;
            while (!avail[target] && target < n_choice) {
                // copy over those not available
                flock[i] = pool[i];
                ++target;
            }
            if (target >= n_choice) {
                // choices exhausted
                flock[i] = pool[i];
                break;
            }
            // copy, swap and re-evaluate
            flock[i] = pool[i];
            flock[target] = pool[target];
            size_t comp = d_comp(engine);
            std::swap(flock[i].gene[comp], flock[target].gene[comp]);
            flock[i].fit = evaluate(flock[i].gene);
            flock[target].fit = evaluate(flock[target].gene);
            if (flock[i].fit > best.fit) {
                best = flock[i];
            }
            i = target;
        }

        // mutate
        std::bernoulli_distribution d_mutate(p_mutate);
        for (auto & entity: flock) {
            for (auto & value: entity.gene) {
                if (!d_mutate(engine)) continue;
                value = d_init(engine);
            }
        }

        log_file << best;
        for (auto & entity: flock) log_file << " " << entity;
        log_file << std::endl;
    }
    log_file.close();
}

void main_ga() {
    std::cout << "--> Genetic Algorithm" << std::endl;
    demo_ga<5, 5, 100, 5.0, 0.88, 0.1>("ga.log");
}