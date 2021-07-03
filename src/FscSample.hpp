#ifndef FSC2FSC_FSC_SAMPLE_HPP
#define FSC2FSC_FSC_SAMPLE_HPP

#include <FscBlockIdentifier.hpp>

#include <fstream>
#include <cstdint>

namespace fsc {
    template <FscBlockIdentifier::Enum _magical_identifier>
    struct FscSample;

    namespace _internal {
        struct FscSampleBase {
            FscSampleBase(std::ifstream& ifstream) {
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
    struct FscSample<FscBlockIdentifier::EVENT> : public _internal::FscSampleBase {
        static constexpr uint8_t SIZE { 0xc };

        FscSample(std::ifstream& ifstream):
            _internal::FscSampleBase { ifstream }
        {
            ifstream.seekg(sizeof(uint32_t), std::ios::cur);
            ifstream.read(reinterpret_cast<char*>(&_value), sizeof(_value));
        }

        constexpr float const& get_value() const noexcept {
            return _value;
        }

    private:
        float _value;
    };

    template <>
    struct FscSample<FscBlockIdentifier::NOTE> : _internal::FscSampleBase {
        static constexpr uint8_t SIZE { 0x18 };

        FscSample(std::ifstream& ifstream):
            _internal::FscSampleBase { ifstream }
        {
            ifstream.seekg(12, std::ios::cur);
            ifstream.read(reinterpret_cast<char*>(&_pitch), sizeof(_pitch));
            ifstream.seekg(1, std::ios::cur);
            ifstream.read(reinterpret_cast<char*>(&_release), sizeof(_release));
            ifstream.seekg(1, std::ios::cur);
            ifstream.read(reinterpret_cast<char*>(&_pan), sizeof(_pan));
            ifstream.read(reinterpret_cast<char*>(&_velocity), sizeof(_velocity));
            ifstream.seekg(2, std::ios::cur);
        }

        constexpr void set_pan(float const& pan) noexcept {
            _pan = static_cast<uint8_t>(pan * 128);
        }

        constexpr void set_velocity(float const& velocity) noexcept {
            _velocity = static_cast<uint8_t>(velocity * 128);
        }

        constexpr void set_release(float const& release) noexcept {
            _release = static_cast<uint8_t>(release * 128);
        }

        constexpr void set_mod_x(float const& mod_x) noexcept {
            _mod_x = static_cast<uint8_t>(mod_x * 255);
        }

        constexpr void set_mod_y(float const& mod_x) noexcept {
            _mod_x = static_cast<uint8_t>(mod_x * 255);
        }

        constexpr void set_pitch(float const& pitch) noexcept {
            _pitch = static_cast<uint8_t>(pitch * 255);
        }

        constexpr void update_block(char* const& input_data) const noexcept {
            input_data[0x14] = _pan;
            input_data[0x15] = _velocity;
            input_data[0x12] = _release;
            input_data[0x16] = _mod_x;
            input_data[0x17] = _mod_y;
            input_data[0x0f] = _pitch;
        }

    private:
        uint8_t _pan;
        uint8_t _velocity;
        uint8_t _release;
        uint8_t _mod_x;
        uint8_t _mod_y;
        uint8_t _pitch;
    };
}

#endif
