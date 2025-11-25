#pragma once

#include "Codi/Core/Types.h"

#include <xhash>

namespace Codi {

    class UUID {
    public:
        UUID();
        UUID(uint64 uuid);
        UUID(const UUID&) = default;

        operator uint64() const { return _UUID; };

    private:
        uint64 _UUID;
    };

} // namespace Codi

namespace std {

    template<>
    struct hash<Codi::UUID> {
        std::size_t operator()(const Codi::UUID& uuid) const {
            return (uint64)uuid;
        }
    };

} // namespace std
