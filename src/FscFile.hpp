#ifndef FSC2FSC_FSC_FILE_HPP
#define FSC2FSC_FSC_FILE_HPP

#include <FscBlock.hpp>
#include <FscBlockIdentifier.hpp>

#include <optional>
#include <fstream>
#include <vector>
#include <memory>

namespace fsc {
    template <FscBlockIdentifier::Enum _block_identifier>
    struct FscFile {
        std::optional<std::string> read(std::string const& file_name) {
            std::ifstream file;
            file.open(file_name, std::ios::binary);
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
            file.write(_file_data.get(), _header->get_samples_position());

            for (FscBlock<_block_identifier> const& sample: _samples) {
                char* const& current_data { _file_data.get() + file.tellp() };
                sample.update_block(current_data);
                file.write(current_data, FscBlock<_block_identifier>::SIZE);
            }

            return {};
        }

        std::vector<FscBlock<_block_identifier>>& samples() {
            return _samples;
        }

    private:
        std::vector<FscBlock<_block_identifier>> _samples;
        std::unique_ptr<FscBlock<FscBlockIdentifier::HEADER>> _header;
        std::unique_ptr<char[]> _file_data;

        std::optional<std::string> read_header(std::ifstream& file) {
            _header = std::make_unique<FscBlock<FscBlockIdentifier::HEADER>>(file);
            return {};
        }

        std::optional<std::string> read_fl_version(std::ifstream& file) {
            uint8_t fl_version_identifier;
            file.read(reinterpret_cast<char*>(&fl_version_identifier), sizeof(fl_version_identifier));
            if (fl_version_identifier != FscBlockIdentifier::FL_VERSION) {
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

            file.seekg(_header->get_samples_size_size(), std::ios::cur);

            uint32_t const&& samples_size { _header->get_samples_size() };
            for (uint32_t i { 0 }; i < samples_size; i += FscBlock<_block_identifier>::SIZE) {
                _samples.emplace_back(file);
            }

            return {};
        }

        std::optional<std::string> read_file_data(std::ifstream& file) {
            _file_data = std::make_unique<char[]>(_header->get_data_size());
            file.read(_file_data.get(), _header->get_data_size());
            return {};
        }
    };
}

#endif
