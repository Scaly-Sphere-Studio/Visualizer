#include "Animation.hpp"
#include "EaseFunctions.hpp"

void Animation::start() 
{
    m_running = true;
    m_paused = false;
    m_forward = true;
    m_startTime = Clock::now();
    m_pauseAccum = Seconds{ 0 };
}

void Animation::stop()
{
    m_running = false;
    m_paused = false;
}

void Animation::pause()
{
    if (!m_running || m_paused) return;
    m_paused = true;
    m_pauseStart = Clock::now();
}

void Animation::resume()
{
    if (!m_running || !m_paused) return;
    m_paused = false;
    m_pauseAccum += std::chrono::duration_cast<Seconds>(Clock::now() - m_pauseStart);
}

void Animation::resume()
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