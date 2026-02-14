#include "Animation.hpp"
#include <SSS/SceneGraph/Node_UI.h>

#include "Animation.hpp"
// Add this helper function at the top of the ShakeGenerator class
static float hash(unsigned int seed) {
    seed = (seed ^ 61) ^ (seed >> 16);
    seed *= 9;
    seed = seed ^ (seed >> 4);
    seed *= 0x27d4eb2d;
    seed = seed ^ (seed >> 15);
    return (static_cast<float>(seed) / 4294967296.0f) * 2.0f - 1.0f;
}



class ShakeGenerator {
public:
    struct ShakeParams {
        float amplitude = 1.0f;
        float frequency = 10.0f;
        float decay = 5.0f;
        unsigned int seed = 0;

        ShakeParams() = default;
        ShakeParams(float amp, float freq, float dec, unsigned int s = 0)
            : amplitude(amp), frequency(freq), decay(dec), seed(s) {}
    };

    // Generate Perlin-like noise-based shake
    static SSS::Math::EasingFunction shake(const ShakeParams& params = ShakeParams()) {
        return [params](float t) -> float {
            // Use combination of sine waves with different frequencies for pseudo-random shake
            float decay = std::exp(-params.decay * t);

            // Multiple octaves of noise
            float value = 0.0f;
            value += std::sin(2.0f * glm::pi<float>() * params.frequency * t + params.seed);
            value += 0.5f * std::sin(2.0f * glm::pi<float>() * params.frequency * 2.37f * t + params.seed * 1.3f);
            value += 0.25f * std::sin(2.0f * glm::pi<float>() * params.frequency * 4.21f * t + params.seed * 2.7f);

            return params.amplitude * decay * value / 1.75f;
        };
    }

    // Generate smooth random shake using interpolated random values
// Fixed smoothShake function
    static SSS::Math::EasingFunction smoothShake(const ShakeParams& params = ShakeParams()) {
        return [params](float t) -> float {
            // Get the integer time step
            unsigned int timeStep = static_cast<unsigned int>(t * params.frequency);

            // Get two adjacent random values using hash function
            float r1 = hash(timeStep + params.seed);
            float r2 = hash(timeStep + 1 + params.seed);

            // Interpolate between them
            float frac = std::fmod(t * params.frequency, 1.0f);
            float smoothed = r1 + frac * (r2 - r1);

            // Apply decay
            float decay = std::exp(-params.decay * t);

            return params.amplitude * decay * smoothed;
        };
    }
};
