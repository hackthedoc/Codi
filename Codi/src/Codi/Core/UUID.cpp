#include "codipch.h"
#include "UUID.h"

#include <random>

namespace Codi {

    namespace Utils {

        static std::random_device RandomDevice;
        static std::mt19937_64 Engine(RandomDevice());
        static std::uniform_int_distribution<uint64_t> UniformDistribution;

    } // namespace Utils

    UUID::UUID()
        : _UUID(Utils::UniformDistribution(Utils::Engine))
        {}

    UUID::UUID(uint64 uuid)
        : _UUID(uuid)
        {}

} // namespace Codi
