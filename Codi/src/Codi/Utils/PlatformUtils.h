#pragma once

#include "Codi/Core/Types.h"

namespace Codi {

    class Platform {
    public:
        static float64 GetAbsoluteTime();

        // Sleep on the thread for the provided ms. This blocks the main thread.
        // Should only be used for giving time back to the OS for unused update power.
        static void Sleep(uint32 ms);
    };

} // namespace Codi
