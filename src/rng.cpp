#include "rng.h"

std::unique_ptr<Weighted_RNG> const& Weighted_RNG::get()
{
    static std::unique_ptr<Weighted_RNG> singleton(new Weighted_RNG);
    return singleton;
}

std::vector<float> Weighted_RNG::probabilities(const std::vector<int>& weights)
{
    float sum = 0;
    for (size_t i = 0; i < weights.size(); ++i) {
        sum += weights[i];
    }

    std::vector<float> probs;
    probs.reserve(weights.size());

    for (int w : weights) {
        probs.emplace_back(static_cast<float>(w) / sum);
    }

    return probs;
}

size_t Weighted_RNG::pick_element_weighted(const std::vector<int>& weights)
{
    std::discrete_distribution<> distr(weights.begin(), weights.end());
    return distr(gen);
}

Weighted_RNG::Weighted_RNG() {

}

Weighted_RNG::~Weighted_RNG()
{
}
