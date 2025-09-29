#pragma once

namespace Codi {

class DeltaTime {
public:
    DeltaTime(float time = 0.0f) : _time(time) {}

    
    /// @brief Returns the delta time as a float in seconds.
    operator float() const { return _time; }

    float getSeconds() const { return _time; }
    float getMilliseconds() const { return _time * 1000.0f; }

private:
    float _time;
};

} // namespace Codi
