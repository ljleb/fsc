#ifndef FSC2MIDI_SCORE_SAMPLE_HPP
#define FSC2MIDI_SCORE_SAMPLE_HPP

#include <cstdint>

namespace s2m {
    struct ScoreSample {
        constexpr ScoreSample(uint32_t const& position, uint32_t const& type, float const& value) noexcept:
            _position { position },
            _type { type },
            _value { value }
        {}

        constexpr uint32_t const& get_position() const noexcept {
            return _position;
        }

        constexpr uint32_t const& get_type() const noexcept {
            return _type;
        }

        constexpr float const& get_value() const noexcept {
            return _value;
        }

    private:
        uint32_t _position;
        uint32_t _type;
        float _value;
    };
}

#endif
