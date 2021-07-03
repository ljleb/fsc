#ifndef FSC2FSC_FSC_BLOCK_HPP
#define FSC2FSC_FSC_BLOCK_HPP

#include <FscBlockIdentifier.hpp>

#include <fstream>
#include <vector>
#include <memory>
#include <cstdint>

namespace fsc {
    template <FscBlockIdentifier::Enum _magical_identifier>
    struct FscBlock;

    namespace _internal {
        struct FscBlockBase {
            FscBlockBase(std::ifstream& ifstream) {
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
    struct FscBlock<FscBlockIdentifier::HEADER> {
        static constexpr uint8_t SIZE { 0x16 };

        FscBlock(std::ifstream& ifstream)
        {
            ifstream.seekg(0x12, std::ios::beg);
            ifstream.read(reinterpret_cast<char*>(&_data_size), sizeof(_data_size));
            _samples_size_size = 1 + (_data_size > 0xa0) + (_data_size > 0x4022);
            _data_size += SIZE;
        }

        uint32_t const& get_data_size() const {
            return _data_size;
        }

        uint32_t const get_samples_size() const {
            return _data_size - get_samples_position();
        }

        uint32_t const get_samples_position() const {
            return SIZE + _samples_size_size;
        }

        uint8_t const& get_samples_size_size() const {
            return _samples_size_size;
        }

    private:
        uint32_t _data_size;
        uint8_t _samples_size_size;
    };

    template <>
    struct FscBlock<FscBlockIdentifier::FL_VERSION> : public _internal::FscBlockBase {
        FscBlock(std::ifstream& ifstream):
            _internal::FscBlockBase { ifstream }
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
    struct FscBlock<FscBlockIdentifier::EVENT> : public _internal::FscBlockBase {
        static constexpr uint8_t SIZE { 0xc };

        FscBlock(std::ifstream& ifstream):
            _internal::FscBlockBase { ifstream }
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
    struct FscBlock<FscBlockIdentifier::NOTE> : _internal::FscBlockBase {
        static constexpr uint8_t SIZE { 0x18 };

        FscBlock(std::ifstream& ifstream):
            _internal::FscBlockBase { ifstream }
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

        constexpr void set_pitch(float const& pitch) noexcept {
            _pitch = static_cast<uint8_t>(pitch * 255);
        }

        constexpr void update_block(char* const& input_data) const noexcept {
            input_data[20] = _pan;
            input_data[21] = _velocity;
            input_data[18] = _release;
            input_data[16] = _pitch;
        }

    private:
        uint8_t _pan;
        uint8_t _velocity;
        uint8_t _release;
        uint8_t _pitch;
    };
}

#endif
