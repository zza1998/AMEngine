#include "ECS/AdUUID.h"
#include <random>

namespace ade{
    static std::random_device s_RandomDevice;
    static std::mt19937_64 s_Engine(s_RandomDevice());
    static std::uniform_int_distribution<uint32_t> s_UniformDistribution(1, UINT32_MAX);

    AdUUID::AdUUID() : mUUID(s_UniformDistribution(s_Engine)) {

    }

    AdUUID::AdUUID(uint32_t uuid) : mUUID(uuid) {

    }
}