#ifndef FSC2FSC_SCORE_FILE_HPP
#define FSC2FSC_SCORE_FILE_HPP

#include <ScoreBlock.hpp>
#include <BlockIdentifier.hpp>

#include <fstream>
#include <vector>
#include <memory>

namespace s2m {
    template <BlockIdentifier::Enum _block_identifier>
    struct ScoreFile {
        constexpr ScoreFile() noexcept:
            _status { true }
        {}

        void read(std::string const& file_name) {
            std::ifstream file { file_name, std::ios::binary };
            _status = file.good();
            if (!_status) {
                return;
            }

            read_header(file);
            if (!_status) {
                return;
            }

            read_fl_version(file);
            if (!_status) {
                return;
            }

            read_gibberish(file);
            if (!_status) {
                return;
            }

            read_samples(file);
            if (!_status) {
                return;
            }
        }

        void write(std::string const& file_name) {
            std::ofstream file { file_name, std::ios::binary };
            _status = file.good();
            if (!_status) {
                return;
            }

            file.write(_input_data.get(), 0x32);

            for (ScoreBlock<_block_identifier> const& sample: _samples) {
                char* const& current_data { _input_data.get() + file.tellp() };
                sample.update_pitch(current_data);
                file.write(current_data, ScoreBlock<_block_identifier>::SIZE);
            }
        }

        bool status() {
            return _status;
        }

        std::vector<ScoreBlock<_block_identifier>>& samples() {
            return _samples;
        }

    private:
        bool _status;
        std::vector<ScoreBlock<_block_identifier>> _samples;
        std::unique_ptr<char[]> _input_data;

        void read_header(std::ifstream& file) {
            file.seekg(0x16, std::ios::beg);
        }

        void read_fl_version(std::ifstream& file) {
            uint8_t fl_version_identifier;
            file.read(reinterpret_cast<char*>(&fl_version_identifier), sizeof(fl_version_identifier));
            _status = fl_version_identifier == BlockIdentifier::FL_VERSION;
            if (!_status) {
                return;
            }

            uint8_t fl_version_size;
            file.read(reinterpret_cast<char*>(&fl_version_size), sizeof(fl_version_size));
            file.seekg(fl_version_size, std::ios::cur);
        }

        void read_gibberish(std::ifstream& file) {
            file.seekg(0x0c, std::ios::cur);
        }

        void read_samples(std::ifstream& file) {
            uint8_t actual_block_identifier;
            file.read(reinterpret_cast<char*>(&actual_block_identifier), sizeof(actual_block_identifier));
            _status = actual_block_identifier == _block_identifier;
            if (!_status) {
                return;
            }

            uint8_t blocks_size;
            file.read(reinterpret_cast<char*>(&blocks_size), sizeof(blocks_size));

            for (uint32_t i { 0 }; i < blocks_size; i += ScoreBlock<_block_identifier>::SIZE) {
                _samples.emplace_back(file);
            }

            uint32_t const& file_size { file.tellg() };
            _input_data = std::make_unique<char[]>(file_size);

            file.seekg(0, std::ios::beg);
            file.read(_input_data.get(), file_size);
        }
    };
}

#endif
