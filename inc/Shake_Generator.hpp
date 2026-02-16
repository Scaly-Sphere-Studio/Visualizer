#include "Animation.hpp"
#include <SSS/SceneGraph/Node_UI.h>

#include "Animation.hpp"

// Helper class to create a consistent hash without srand and without using noise 
// TODO implement noise functions (perlin/simplex)
static float hash(unsigned int seed) {
    seed = (seed ^ 61) ^ (seed >> 16);
    seed *= 9;
    seed = seed ^ (seed >> 4);
    seed *= 0x27d4eb2d;
    seed = seed ^ (seed >> 15);
    return (static_cast<float>(seed) / 4294967296.0f) * 2.0f - 1.0f;
}


class ShakeGenerator : public Track {
public:
    struct ShakeParams {
        float amplitude     = 55.0f;
        float frequency     = 15.0f;
        float decay         = 15.0f;
        unsigned int seed   = 25;
        glm::vec3 direction = glm::vec3{ 1.0, 0.7, 0.0 }; // 2D Shake

        ShakeParams() {
        };
        ShakeParams(float amp, float freq, float dec, glm::vec3 dir)
            : amplitude(amp), frequency(freq), decay(dec), direction(dir) {}
    };

    ShakeGenerator(float amp, float freq, float dec, glm::vec3 dir = glm::vec3{ 1.0, 1.0, 0.0 })
    {
        _duration = 1s;
        params = ShakeParams(amp,  freq, dec, dir);
    }

    ShakeGenerator(const ShakeParams& param = ShakeParams()) :
        params(param) 
    {
        _duration = 1s;
    }


    ShakeParams params;
    glm::vec3 shakeOffset = glm::vec3{ 0 };


    glm::vec3 shake();
    glm::vec3 smoothShake();

private:
    float smoothShakeEasing(const float& t, const unsigned int seed) const ;
    float shakeEasing(const float& t, const unsigned int seed) const;
};


glm::vec3 ShakeGenerator::shake() 
{
    if (!_playing || _duration == 0s) { return glm::vec3{}; }
    update();

    float t = normalizedTime();
    // Apply shake to multiple axes with different seeds
    shakeOffset.x = shakeEasing(t, params.seed) * params.direction.x;
    shakeOffset.y = shakeEasing(t + 0.3f, params.seed+10) * params.direction.y; // Offset phase
    shakeOffset.z = shakeEasing(t + 0.6f, params.seed+20) * params.direction.z; // Offset phase

    return shakeOffset;
}

glm::vec3 ShakeGenerator::smoothShake() 
{
    if (!_playing || _duration == 0s) { return glm::vec3{}; }
    update();

    float t = normalizedTime();
    // Apply shake to multiple axes with different seeds
    shakeOffset.x = smoothShakeEasing(t, params.seed) * params.direction.x;
    shakeOffset.y = smoothShakeEasing(t + 0.3f, params.seed+10) * params.direction.y; // Offset phase
    shakeOffset.z = smoothShakeEasing(t + 0.6f, params.seed+20) * params.direction.z; // Offset phase

    return shakeOffset;
}


float ShakeGenerator::smoothShakeEasing(const float& t, const unsigned int seed) const
{

        // Scale to frequency range
        float scaledT = t * params.frequency;
        unsigned int timeStep = static_cast<unsigned int>(scaledT);

        // For seamless looping
        unsigned int maxStep = glm::max(1u, static_cast<unsigned int>(params.frequency));
        unsigned int nextStep = (timeStep + 1) % maxStep;

        float r1 = hash(timeStep + seed);
        float r2 = hash(nextStep + seed + 1);

        // Smoothstep interpolation
        float frac = scaledT - static_cast<float>(timeStep);
        frac = glm::clamp(frac, 0.0f, 1.0f);
        float smooth = frac * frac * (3.0f - 2.0f * frac);
        float smoothed = r1 + smooth * (r2 - r1);

        // Looping decay: smooth fade in/out using sine wave
        float loopingDecay = std::exp(-params.decay * t) * glm::sin(t * glm::pi<float>());

        return params.amplitude * smoothed * loopingDecay;
}

float ShakeGenerator::shakeEasing(const float& t, const unsigned int seed) const
{
    // Use combination of sine waves with different frequencies for pseudo-random shake
    float decay = std::exp(-params.decay * t);

    // Multiple octaves of noise
    float value = 0.0f;
    value += std::sin(2.0f * glm::pi<float>() * params.frequency * t + seed);
    value += 0.5f * std::sin(2.0f * glm::pi<float>() * params.frequency * 2.37f * t + seed * 1.3f);
    value += 0.25f * std::sin(2.0f * glm::pi<float>() * params.frequency * 4.21f * t + seed * 2.7f);

    return params.amplitude * decay * value / 1.75f;
}