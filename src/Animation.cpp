#include "Animation.hpp"



void Track::_register()
{
    REGISTER_EVENT("SSS_Track_STOP");
    REGISTER_EVENT("SSS_Track_PLAY");
    REGISTER_EVENT("SSS_Track_PAUSE");
    REGISTER_EVENT("SSS_Track_RESUME");
    REGISTER_EVENT("SSS_Track_LOOPED");
}


void Track::start() 
{
    _playing       = true;
    _paused        = false;
    _startTime     = std::chrono::steady_clock::now();
    _pauseAccum    = 0s;
}

void Track::play()
{
    start();
}

void Track::stop()
{
    _playing = false;
    _paused = false;
}

void Track::pause()
{
    if (!_playing || _paused) return;
    _paused = true;
    _pauseStart = std::chrono::steady_clock::now();
}

void Track::restart()
{
    stop();
    play();
}

void Track::resume()
{
    if (!_playing || !_paused) return;
    _paused = false;
    _pauseAccum += std::chrono::steady_clock::now() - _pauseStart;
}

void Track::update()
{
    if (!_playing) return;

    auto now = std::chrono::steady_clock::now();
    Duration elapsed = now - _startTime;
    _currentTime = elapsed * _speed;

    if (_mode == LectureMode::Loop && _currentTime > _duration) 
    {
        _currentTime = std::chrono::duration<double>(std::fmod(_currentTime.count(), _duration.count()));
        _startTime = now - std::chrono::duration_cast<std::chrono::nanoseconds>(_currentTime/_speed);
    }
    else if (_currentTime >= _duration) 
    {
        _currentTime = _duration;
        _playing = false;
    }

    print();
}

void Track::print()
{
    // integer scale conversion with no precision loss: no cast
    std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(_currentTime) << " milliseconds" << std::endl;
    std::cout << normalizedTime() << " normalized t\n"; ;
}


