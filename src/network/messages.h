#pragma once
#include <string>
#include <cstdint>
#include <vector>

namespace proto {

    enum class MsgId : uint16_t {
        C2S_Join = 1,
        C2S_Input = 2,
        S2C_Welcome = 100,
        S2C_StateSnapshot = 101,
        S2C_Event = 102,
    };

    struct Join {
        std::string playerName;
    };

    struct Input {
        uint32_t tick;
        float    moveX, moveY;
    };

    struct Welcome {
        uint32_t playerId;
    };

    struct StateSnapshot {
        uint32_t tick;
        // Minimal ECS snapshot (positions, velocities, health, etc.)
        struct Entity {
            uint32_t id; float x, y;
        };
        std::vector<Entity> entities;
    };

} // namespace proto
