#pragma once

#include "Codi/Core/Types.h"
#include "Codi/Utils/PlatformUtils.h"

namespace Codi {

    class Clock {
    public:
        Clock() { Reset(); }
        ~Clock() {}

        void Reset() { _StartTime = Platform::GetAbsoluteTime(); }

        float64 Elapsed() const { return Platform::GetAbsoluteTime() - _StartTime; }

        float64 ElapsedMillis() const { return Elapsed() * 1000.0; }

    private:
        float64 _StartTime;
    };

}; // namespace Codi
