#pragma once

#include <SSS/Commons.hpp>

#include <glm/gtc/quaternion.hpp>
#include <SSS/Math.hpp>


using namespace std::chrono_literals;

class Track : public SSS::Subject, public SSS::_EventRegistry<Track>
{

public:
    friend SSS::_EventRegistry<Track>;


    enum class LectureMode
    {
        Once,
        Reverse,
        Loop,
        PingPong
    };

    using EaseType  = SSS::Math::EaseType;
    using TimePoint = std::chrono::steady_clock::time_point;
    using Duration  = std::chrono::duration<float>;

    Track() = default;
    //Track(const Track&) = default;
    virtual ~Track() = default;


    void setLectureMode(LectureMode mode) { _mode = mode; }
    void setEase(EaseType ef) { _ease = ef; }

    // --- Control ---
    void start();
    void play();
    void stop();
    void pause();
    void resume();
    void restart();

    bool isRunning() const { return _playing; }
    bool isPaused()  const { return _paused; }

    void setDuration(float seconds) { _duration = std::chrono::duration<float>(seconds); if (_duration == 0s) stop(); }
    void setSpeed(float sp) { _speed = sp; if (_speed == 0) pause();  }
    float duration() const { return _duration.count(); }

    // Call every frame
    void update();

    void print();

protected:
    
    float normalizedTime()
    {
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - _startTime) - _pauseAccum;

        float raw = (elapsed / _duration);

        
        return std::clamp(raw, 0.f, 1.f);
    }


private:
    float _speed        = 1.0f;

    std::chrono::duration<double> _duration     = 0s;
    std::chrono::duration<double> _currentTime  = 0s;
    std::chrono::duration<double> _pauseAccum   = 0s;

    bool _playing   = false;
    bool _paused    = false;

    LectureMode  _mode = LectureMode::Once;
    EaseType     _ease = EaseType::Linear;

    //bool m_forward = true; // reserved for future manual stepping


    std::chrono::steady_clock::time_point _startTime{};
    std::chrono::steady_clock::time_point _pauseStart{};

    static void _register();
};


