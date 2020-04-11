#ifndef InputDef_hpp
#define InputDef_hpp

#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>

namespace InputDef {
    struct InputKeys {
        sf::Keyboard::Key up, down, right, left, joinable, ungroup, stop, intent;
    };

    struct UnreliableInput {
        bool toggle_up;
        bool toggle_down;
        bool toggle_right;
        bool toggle_left;
        bool toggle_stop;
        void setAll(bool value) {
            toggle_up = value;
            toggle_down = value;
            toggle_right = value;
            toggle_left = value;
            toggle_stop = value;
        };
        bool allFalse() {
            return !toggle_up && !toggle_down && !toggle_right && !toggle_left && !toggle_stop;
        };
    };

    struct ReliableInput {
        bool toggle_ungroup;
        bool toggle_joinable;
        bool toggle_intent;
        void setAll(bool value) {
            toggle_joinable = value;
            toggle_ungroup = value;
            toggle_intent = value;
        };
        bool allFalse() {
            return !toggle_joinable && !toggle_ungroup && !toggle_intent;
        };
    };

    struct PlayerUnreliableInput {
        uint32_t player_id;
        UnreliableInput unreliable_input;
    };

    struct PlayerReliableInput {
        uint32_t player_id;
        ReliableInput reliable_input;
    };

    struct PlayerInputs {
        std::vector<PlayerUnreliableInput> player_unreliable_inputs;
        std::vector<PlayerReliableInput> player_reliable_inputs;
    };

    struct ClientInputAndTick {
        UnreliableInput ui;
        ReliableInput ri;
        unsigned int tick;
    };
} // namespace InputDef

sf::Packet& operator<<(sf::Packet& packet, const InputDef::UnreliableInput& ui);
sf::Packet& operator>>(sf::Packet& packet, InputDef::UnreliableInput& ui);

sf::Packet& operator<<(sf::Packet& packet, const InputDef::ReliableInput& ri);
sf::Packet& operator>>(sf::Packet& packet, InputDef::ReliableInput& ri);

#endif /* InputDef_hpp */
