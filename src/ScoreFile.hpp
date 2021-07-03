#ifndef FSC2FSC_SCORE_FILE_HPP
#define FSC2FSC_SCORE_FILE_HPP

#include <ScoreBlock.hpp>
#include <BlockIdentifier.hpp>

#include <optional>
#include <fstream>
#include <vector>
#include <memory>

namespace s2m {
    template <BlockIdentifier::Enum _block_identifier>
    struct ScoreFile {
        std::optional<std::string> read(std::string const& file_name) {
            std::ifstream file { file_name, std::ios::binary };
            if (!file.good()) {
                return "error reading file " + file_name;
            }

            auto const&& header_error { read_header(file) };
            if (header_error.has_value()) {
                return header_error;
            }

            auto const&& fl_version_error { read_fl_version(file) };
            if (fl_version_error.has_value()) {
                return fl_version_error;
            }

            auto const&& gibberish_error { read_gibberish(file) };
            if (gibberish_error.has_value()) {
                return gibberish_error;
            }

            auto const&& samples_error { read_samples(file, file_name) };
            if (samples_error.has_value()) {
                return samples_error;
            }

            file.close();
            file.clear();
            file.open(file_name, std::ios::binary);
            if (!file.good()) {
                return "error reading file " + file_name;
            }
            auto const&& file_data_error { read_file_data(file) };
            if (file_data_error.has_value()) {
                return file_data_error;
            }

            return {};
        }

        std::optional<std::string> write(std::string const& file_name) {
            std::ofstream file { file_name, std::ios::binary };
            if (!file.good()) {
                return "error writing to file " + file_name;
            }

            file.seekp(0, std::ios::beg);
            file.write(_file_data.get(), _samples_position);

            for (ScoreBlock<_block_identifier> const& sample: _samples) {
                char* const& current_data { _file_data.get() + file.tellp() };
                sample.update_block(current_data);
                file.write(current_data, ScoreBlock<_block_identifier>::SIZE);
            }

            return {};
        }

        std::vector<ScoreBlock<_block_identifier>>& samples() {
            return _samples;
        }

    private:
        std::vector<ScoreBlock<_block_identifier>> _samples;
        std::unique_ptr<char[]> _file_data;
        uint32_t _data_size;
        uint32_t _samples_position;
        uint8_t _samples_size_size;

        std::optional<std::string> read_header(std::ifstream& file) {
            file.seekg(0x12, std::ios::beg);

            file.read(reinterpret_cast<char*>(&_data_size), sizeof(_data_size));
            _samples_size_size = 1 + (_data_size > 0xa0) + (_data_size > 0x4022);
            _data_size += ScoreBlock<BlockIdentifier::HEADER>::SIZE;

            return {};
        }

        std::optional<std::string> read_fl_version(std::ifstream& file) {
            uint8_t fl_version_identifier;
            file.read(reinterpret_cast<char*>(&fl_version_identifier), sizeof(fl_version_identifier));
            if (fl_version_identifier != BlockIdentifier::FL_VERSION) {
                return "expected fl version identifier";
            }

            uint8_t fl_version_size;
            file.read(reinterpret_cast<char*>(&fl_version_size), sizeof(fl_version_size));
            file.seekg(fl_version_size, std::ios::cur);

            return {};
        }

        std::optional<std::string> read_gibberish(std::ifstream& file) {
            file.seekg(0x0c, std::ios::cur);

            return {};
        }

        std::optional<std::string> read_samples(std::ifstream& file, std::string const& file_name) {
            uint8_t actual_block_identifier;
            file.read(reinterpret_cast<char*>(&actual_block_identifier), sizeof(actual_block_identifier));
            if (actual_block_identifier != _block_identifier) {
                return "expected block identifier " + std::to_string(_block_identifier);
            }

            uint32_t samples_size { 0 };
            file.read(reinterpret_cast<char*>(&samples_size), _samples_size_size);
            if (_samples_size_size >= 2) {
                samples_size -= 0x100;
            }
            if (_samples_size_size >= 3) {
                samples_size -= 0x10000;
            }
            if (samples_size > 0x100) {
                samples_size -= 0x80;
            }

            _samples_position = file.tellg();

            for (uint32_t i { 0 }; i < samples_size; i += ScoreBlock<_block_identifier>::SIZE) {
                _samples.emplace_back(file);
            }

            return {};
        }

        std::optional<std::string> read_file_data(std::ifstream& file) {
            _file_data = std::make_unique<char[]>(_data_size);
            file.read(_file_data.get(), _data_size);

            return {};
        }
    };
}

#endif
