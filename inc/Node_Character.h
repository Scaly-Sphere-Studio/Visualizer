#pragma once

#include <SSS/Commons.hpp>
#include <SSS/SceneGraph/Node_UI.h>
enum class AnimationType {
	ROTATION,
	SCALE,
	TRANSLATION
};

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

    // --- Control ---
    void start()
    {
        m_running = true;
        m_paused = false;
        m_forward = true;
        m_startTime = Clock::now();
        m_pauseAccum = Seconds{ 0 };
    }

    void stop()
    {
        m_running = false;
        m_paused = false;
    }

    void pause()
    {
        if (!m_running || m_paused) return;
        m_paused = true;
        m_pauseStart = Clock::now();
    }

    void resume()
    {
        if (!m_running || !m_paused) return;
        m_paused = false;
        m_pauseAccum += std::chrono::duration_cast<Seconds>(Clock::now() - m_pauseStart);
    }

    bool isRunning() const { return m_running; }
    bool isPaused()  const { return m_paused; }

    void setMode(Mode mode) { m_mode = mode; }
    void setDuration(float seconds) { m_duration = std::max(0.0001f, seconds); }

    float duration() const { return m_duration; }

    // Call every frame
    void update()
    {
        if (!m_running) return;
        if (m_paused) return;

        float t = normalizedTime();
        apply(t);

        // handle completion for Once mode
        if (m_mode == Mode::Once && t >= 1.0f)
        {
            apply(1.0f);
            stop();
        }
    }

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




class Node_Character : public SSS::Node_UI, public SSS::_EventRegistry<Node_Character>
{
public:
	friend _EventRegistry<Node_Character>;
	Node_Character(SSS::SceneGraph* p_Sg, const std::string& path);
	virtual void _subjectUpdate(SSS::Subject const& subject, int event_id) override;

	void update();

	void translate(const glm::vec3 t) { charAtlas->translate(t); };
	void rotate(const glm::vec3 r) { charAtlas->rotate(r); };

	std::string dir_path;

	int frame = 0;
private :

	SSS::GL::Plane::Shared charAtlas;
	Node_Character() = default;
	static void _register();
};
