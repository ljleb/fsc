#ifndef FSC2MIDI_SCORE_SAMPLE_HPP
#define FSC2MIDI_SCORE_SAMPLE_HPP

#include <MagicalIdentifier.hpp>

#include <cstdint>

namespace s2m {
    template <MagicalIdentifier::Enum _magical_identifier>
    struct ScoreSample;

    namespace _internal {
        struct ScoreSampleBase {
            ScoreSampleBase(std::ifstream& ifstream) {
                ifstream.read(reinterpret_cast<char*>(&_position), sizeof(_position));
            }

            constexpr uint32_t const& get_position() const noexcept {
                return _position;
            }

        private:
            uint32_t _position;
        };
    }

    template <>
    struct ScoreSample<MagicalIdentifier::PITCH> : public _internal::ScoreSampleBase {
        static constexpr uint8_t SIZE { 12 };

        ScoreSample(std::ifstream& ifstream):
            _internal::ScoreSampleBase { ifstream }
        {
            ifstream.read(reinterpret_cast<char*>(&_type), sizeof(_type));
            ifstream.read(reinterpret_cast<char*>(&_pitch), sizeof(_pitch));
        }

        constexpr float const& get_pitch() const noexcept {
            return _pitch;
        }

    private:
        uint32_t _type;
        float _pitch;
    };

    template <>
    struct ScoreSample<MagicalIdentifier::NOTE> : _internal::ScoreSampleBase {
        static constexpr uint8_t SIZE { 24 };

        ScoreSample(std::ifstream& ifstream):
            _internal::ScoreSampleBase { ifstream }
        {
            ifstream.seekg(12, std::ios::cur);
            ifstream.read(reinterpret_cast<char*>(&_pitch), sizeof(_pitch));
            ifstream.seekg(7, std::ios::cur);
        }

        constexpr void set_pitch(float const& pitch) noexcept {
            _pitch = static_cast<uint8_t>(pitch * 255);
        }

        constexpr void update_pitch(char* const& input_data) const noexcept {
            input_data[16] = _pitch;
        }

    private:
        uint8_t _pitch;
    };
}

#endif
