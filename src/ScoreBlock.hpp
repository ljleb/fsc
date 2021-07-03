#ifndef FSC2FSC_SCORE_BLOCK_HPP
#define FSC2FSC_SCORE_BLOCK_HPP

#include <BlockIdentifier.hpp>

#include <fstream>
#include <vector>
#include <memory>
#include <cstdint>

namespace s2m {
    template <BlockIdentifier::Enum _magical_identifier>
    struct ScoreBlock;

    namespace _internal {
        struct ScoreBlockBase {
            ScoreBlockBase(std::ifstream& ifstream) {
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
    struct ScoreBlock<BlockIdentifier::FL_VERSION> : public _internal::ScoreBlockBase {
        ScoreBlock(std::ifstream& ifstream):
            _internal::ScoreBlockBase { ifstream }
        {
            uint8_t content_size;
            ifstream.read(reinterpret_cast<char*>(&content_size), sizeof(content_size));
            ifstream.read(_version.get(), content_size);
        }

        char const* const get_fl_version() const noexcept {
            return _version.get();
        }

    private:
        std::unique_ptr<char[]> _version;
    };

    template <>
    struct ScoreBlock<BlockIdentifier::EVENT> : public _internal::ScoreBlockBase {
        static constexpr uint8_t SIZE { 0xc };

        ScoreBlock(std::ifstream& ifstream):
            _internal::ScoreBlockBase { ifstream }
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
    struct ScoreBlock<BlockIdentifier::NOTE> : _internal::ScoreBlockBase {
        static constexpr uint8_t SIZE { 0x18 };

        ScoreBlock(std::ifstream& ifstream):
            _internal::ScoreBlockBase { ifstream }
        {
            ifstream.seekg(12, std::ios::cur);
            ifstream.read(reinterpret_cast<char*>(&_pitch), sizeof(_pitch));
            ifstream.seekg(4, std::ios::cur);
            ifstream.read(reinterpret_cast<char*>(&_velocity), sizeof(_velocity));
            ifstream.seekg(2, std::ios::cur);
        }

        constexpr void set_pitch(float const& pitch) noexcept {
            _pitch = static_cast<uint8_t>(pitch * 255);
        }

        constexpr void set_velocity(float const& velocity) noexcept {
            _velocity = static_cast<uint8_t>(velocity * 128);
        }

        constexpr void update_block(char* const& input_data) const noexcept {
            input_data[16] = _pitch;
            input_data[21] = _velocity;
        }

    private:
        uint8_t _pitch;
        uint8_t _velocity;
    };
}

#endif
