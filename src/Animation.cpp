#include "Animation.hpp"



void Animation::_register()
{
    REGISTER_EVENT("SSS_ANIMATION_STOP");
    REGISTER_EVENT("SSS_ANIMATION_PLAY");
    REGISTER_EVENT("SSS_ANIMATION_PAUSE");
    REGISTER_EVENT("SSS_ANIMATION_RESUME");
    REGISTER_EVENT("SSS_ANIMATION_LOOPED");
}


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

void Animation::update()
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

void Animation::apply(float t)
{
    if (val == nullptr) return;
    *val = glm::mix(Begin, End, t);
}

float Animation::ease(const float t, const EaseFunction _ease)
{
    switch (_ease) 
    {
        case EaseFunction::Linear:          return t;
        case EaseFunction::SineEaseIn       :return easeInSine(t);
        case EaseFunction::SineEaseOut      :return easeOutSine(t);
        case EaseFunction::SineEaseInOut    :return easeInOutSine(t);
        case EaseFunction::QuadEaseIn       :return easeInQuad(t);
        case EaseFunction::QuadEaseOut      :return easeOutQuad(t);
        case EaseFunction::QuadEaseInOut    :return easeInOutQuad(t);
        case EaseFunction::CubicEaseIn      :return easeInCubic(t);
        case EaseFunction::CubicEaseOut     :return easeOutCubic(t);
        case EaseFunction::CubicEaseInOut   :return easeInOutCubic(t);
        case EaseFunction::QuartEaseIn      :return easeInQuart(t);
        case EaseFunction::QuartEaseOut     :return easeOutQuart(t);
        case EaseFunction::QuartEaseInOut   :return easeInOutQuart(t);
        case EaseFunction::QuintEaseIn      :return easeInQuint(t);
        case EaseFunction::QuintEaseOut     :return easeOutQuint(t);
        case EaseFunction::QuintEaseInOut   :return easeInOutQuint(t);
        case EaseFunction::ExpoEaseIn       :return easeInExpo(t);
        case EaseFunction::ExpoEaseOut      :return easeOutExpo(t);
        case EaseFunction::ExpoEaseInOut    :return easeInOutExpo(t);
        case EaseFunction::CircEaseIn       :return easeInCirc(t);
        case EaseFunction::CircEaseOut      :return easeOutCirc(t);
        case EaseFunction::CircEaseInOut    :return easeInOutCirc(t);
        case EaseFunction::BackEaseIn       :return easeInBack(t);
        case EaseFunction::BackEaseOut      :return easeOutBack(t);
        case EaseFunction::BackEaseInOut    :return easeInOutBack(t);
        case EaseFunction::ElasticEaseIn    :return easeInElastic(t);
        case EaseFunction::ElasticEaseOut   :return easeOutElastic(t);
        case EaseFunction::ElasticEaseInOut :return easeInOutElastic(t);
        case EaseFunction::BounceEaseIn     :return easeInBounce(t);
        case EaseFunction::BounceEaseOut    :return easeOutBounce(t);
        case EaseFunction::BounceEaseInOut  :return easeInOutBounce(t);
        default:
            return t;
    }
}
