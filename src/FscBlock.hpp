#ifndef FSC2FSC_FSC_BLOCK_HPP
#define FSC2FSC_FSC_BLOCK_HPP

#include <FscSample.hpp>
#include <FscBlockIdentifier.hpp>

#include <fstream>
#include <vector>
#include <memory>
#include <cstdint>

namespace fsc {
    template <FscBlockIdentifier::Enum _block_identifier>
    struct FscBlock;

    template <>
    struct FscBlock<FscBlockIdentifier::FL_VERSION> {
        FscBlock(std::ifstream& ifstream)
        {
            ifstream.read(reinterpret_cast<char*>(&_size), sizeof(_size));
            _version = std::make_unique<char[]>(_size);
            ifstream.read(_version.get(), _size);
        }

        char const* const get_fl_version() const noexcept {
            return _version.get();
        }

        uint8_t const& get_size() const {
            return _size;
        }

    private:
        std::unique_ptr<char[]> _version;
        uint8_t _size;
    };

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

        uint8_t const& get_samples_size_size() const {
            return _samples_size_size;
        }

    private:
        uint32_t _data_size;
        uint8_t _samples_size_size;
    };

    namespace _internal {
        template <FscBlockIdentifier::Enum _block_identifier>
        struct FscSamplesBlock {
            FscSamplesBlock(std::ifstream& ifstream, FscBlock<FscBlockIdentifier::HEADER> const& header) {
                ifstream.seekg(header.get_samples_size_size(), std::ios::cur);

                _samples_position = ifstream.tellg();

                uint32_t const&& samples_size { header.get_data_size() - _samples_position };
                for (uint32_t i { 0 }; i < samples_size; i += FscSample<_block_identifier>::SIZE) {
                    _samples.emplace_back(ifstream);
                }
            }

            std::vector<FscSample<_block_identifier>>& get_samples() {
                return _samples;
            }

            std::vector<FscSample<_block_identifier>> const& get_samples() const {
                return _samples;
            }

            uint32_t const& get_samples_position() const {
                return _samples_position;
            }

        private:
            std::vector<FscSample<_block_identifier>> _samples;
            uint32_t _samples_position;
        };
    }

    template <>
    struct FscBlock<FscBlockIdentifier::EVENT> : _internal::FscSamplesBlock<FscBlockIdentifier::EVENT> {
        FscBlock(std::ifstream& ifstream, FscBlock<FscBlockIdentifier::HEADER> const& header):
            _internal::FscSamplesBlock<FscBlockIdentifier::EVENT> { ifstream, header }
        {}
    };

    template <>
    struct FscBlock<FscBlockIdentifier::NOTE> : _internal::FscSamplesBlock<FscBlockIdentifier::NOTE> {
        FscBlock(std::ifstream& ifstream, FscBlock<FscBlockIdentifier::HEADER> const& header):
            _internal::FscSamplesBlock<FscBlockIdentifier::NOTE> { ifstream, header }
        {}
    };
}

#endif
