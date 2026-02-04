#include <SSS/Commons.hpp>
#include <glm/gtc/quaternion.hpp>

#include "EaseFunctions.hpp"

template<typename T>
T interpolate(const T& a, const T& b, float t) {
    return glm::mix(a, b, t);
}

// Specialization for quaternions (use slerp)
template<>
inline glm::quat interpolate(const glm::quat& a, const glm::quat& b, float t) {
    return glm::slerp(a, b, t);
}


class Animation : public SSS::Subject, public SSS::_EventRegistry<Animation>
{

public:
    friend SSS::_EventRegistry<Animation>;
    Animation() = default;

    enum class Mode
    {
        Once,
        Loop,
        PingPong
    };


    using Clock = std::chrono::steady_clock;
    using Seconds = std::chrono::duration<float>;

    Animation(
        glm::vec3* _val,
        glm::vec3 _Begin,
        glm::vec3 _End,
        float durationSeconds = 1.0f, 
        Mode mode = Mode::Once
    )
        : m_duration(std::max(0.0001f, durationSeconds))
        , m_mode(mode), val(_val), Begin(_Begin), End(_End)
    {}

    Animation(const Animation&) = default;
    virtual ~Animation() = default;


    void setMode(Mode mode) { m_mode = mode; }
    void setEase(EaseFunction ef) { m_ease = ef; }

    // --- Control ---
    void start();
    void stop();
    void pause();
    void resume();

    bool isRunning() const { return m_running; }
    bool isPaused()  const { return m_paused; }

    void setDuration(float seconds) { m_duration = std::max(0.0001f, seconds); }
    float duration() const { return m_duration; }

    // Call every frame
    void update();

protected:
    // Override this: t is normalized [0..1]
    void apply(float t);

    // Optional easing override
    float ease(const float t, const EaseFunction _ease = EaseFunction::Linear);

    float normalizedTime()
    {
        auto now = Clock::now();
        Seconds elapsed = std::chrono::duration_cast<Seconds>(now - m_startTime) - m_pauseAccum;

        float raw = elapsed.count() / m_duration;
        raw = std::max(0.0f, raw);

        switch (m_mode)
        {
        case Mode::Once:
        {
            float t = std::min(raw, 1.0f);
            return ease(t, m_ease);
        }

        case Mode::Loop:
        {
            float t = raw - std::floor(raw); // fract
            return ease(t, m_ease);
        }

        case Mode::PingPong:
        {
            float cycle = raw - std::floor(raw); // 0..1
            int phase = static_cast<int>(std::floor(raw)) % 2;

            float t = (phase == 0) ? cycle : (1.0f - cycle);
            return ease(t, m_ease);
        }
        }

        return 0.0f;
    }

    glm::vec3* val;
    glm::vec3 Begin;
    glm::vec3 End;

private:
    float m_duration = 1.0f;
    Mode  m_mode = Mode::Once;
    EaseFunction  m_ease = EaseFunction::Linear;

    bool m_running = false;
    bool m_paused = false;
    bool m_forward = true; // reserved for future manual stepping

    Clock::time_point m_startTime{};
    Clock::time_point m_pauseStart{};
    Seconds m_pauseAccum{ 0 };

    static void _register();
};
