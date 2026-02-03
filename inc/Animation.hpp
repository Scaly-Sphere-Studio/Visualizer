#include <SSS/Commons.hpp>


class Animation
{

public:
    enum class Mode
    {
        Once,
        Loop,
        PingPong
    };

    using Clock = std::chrono::steady_clock;
    using Seconds = std::chrono::duration<float>;

    explicit Animation(float durationSeconds = 1.0f, Mode mode = Mode::Once)
        : m_duration(std::max(0.0001f, durationSeconds))
        , m_mode(mode)
    {}

    virtual ~Animation() = default;


    void setMode(Mode mode) { m_mode = mode; }

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
    virtual void apply(float t) = 0;

    // Optional easing override
    virtual float ease(float t) { return t; }

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
            return ease(t);
        }

        case Mode::Loop:
        {
            float t = raw - std::floor(raw); // fract
            return ease(t);
        }

        case Mode::PingPong:
        {
            float cycle = raw - std::floor(raw); // 0..1
            int phase = static_cast<int>(std::floor(raw)) % 2;

            float t = (phase == 0) ? cycle : (1.0f - cycle);
            return ease(t);
        }
        }

        return 0.0f;
    }

private:
    float m_duration = 1.0f;
    Mode  m_mode = Mode::Once;

    bool m_running = false;
    bool m_paused = false;
    bool m_forward = true; // reserved for future manual stepping

    Clock::time_point m_startTime{};
    Clock::time_point m_pauseStart{};
    Seconds m_pauseAccum{ 0 };
};
