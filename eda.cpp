#include "common.h"

using gene = std::array<double, 2>;
double criterion(gene inst);

template<size_t n_flock, size_t n_choice, size_t n_epoch, double mean1, double std1, double mean2, double std2>
void demo_eda(const std::string &log_name) {
    std::ofstream log_file;
    log_file.open(log_name);
    // NOLINTNEXTLINE(*-msc51-cpp) I deliberately want predictable results.
    std::mt19937 engine;
    std::array<gene, n_flock> flock {};
    std::array mean { mean1, mean2 }, std { std1, std2 };
    std::normal_distribution dist1(mean[0], std[0]), dist2(mean[1], std[1]);
    for (auto & it: flock) {
        it = gene { dist1(engine), dist2(engine) };
    }
    for (int epoch = 0; epoch < n_epoch; ++epoch) {
        std::sort(flock.begin(), flock.end(),
            [](const gene & a, const gene & b) { return criterion(a) < criterion(b); });
        double sum1 = 0.0, sum2 = 0.0;
        for (int i = 0; i < n_choice; ++i) {
            sum1 += flock[i][0];
            sum2 += flock[i][1];
        }
        mean = { sum1 / n_choice, sum2 / n_choice };
        std = { 0.0, 0.0 };
        for (auto & it: flock) {
            std[0] += (it[0] - mean[0]) * (it[0] - mean[0]);
            std[1] += (it[1] - mean[1]) * (it[1] - mean[1]);
        }
        std[0] = std::sqrt(std[0] / n_flock);
        std[1] = std::sqrt(std[1] / n_flock);
        for (auto & it: flock) {
            it[0] = std::normal_distribution(mean[0], std[0])(engine);
            it[1] = std::normal_distribution(mean[1], std[1])(engine);
        }
        log_file << criterion(flock[0]) << " " << mean[0] << " " << std[0] << " " << mean[1] << " " << std[1] << std::endl;
    }
    log_file.close();
}

void main_eda() {
    std::cout << "--> Estimation of Distribution Algorithm" << std::endl;
    demo_eda<500, 100, 100, 10.0, 2.0, 10.0, 2.0>("eda.log");
}