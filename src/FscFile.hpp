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

            auto const&& fl_version_error { read_block(file, _fl_version) };
            if (fl_version_error.has_value()) {
                return fl_version_error;
            }

            auto const&& gibberish_error { read_gibberish(file) };
            if (gibberish_error.has_value()) {
                return gibberish_error;
            }

            auto const&& block_error { read_block(file, _block) };
            if (block_error.has_value()) {
                return block_error;
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
            file.write(_file_data.get(), _block->get_samples_position());

            for (FscSample<_block_identifier> const& sample: _block->get_samples()) {
                char* const& current_data { _file_data.get() + file.tellp() };
                sample.update_block(current_data);
                file.write(current_data, FscSample<_block_identifier>::SIZE);
            }

            return {};
        }

        FscBlock<_block_identifier>& block() {
            return *_block;
        }

    private:
        std::unique_ptr<FscBlock<FscBlockIdentifier::HEADER>> _header;
        std::unique_ptr<FscBlock<FscBlockIdentifier::FL_VERSION>> _fl_version;
        std::unique_ptr<FscBlock<_block_identifier>> _block;
        std::unique_ptr<char[]> _file_data;

        std::optional<std::string> read_header(std::ifstream& file) {
            _header = std::make_unique<FscBlock<FscBlockIdentifier::HEADER>>(file);
            return {};
        }

        std::optional<std::string> read_gibberish(std::ifstream& file) {
            file.seekg(0x0c, std::ios::cur);
            return {};
        }

        template <FscBlockIdentifier::Enum _specific_block_identifier = _block_identifier>
        std::optional<std::string> read_block(std::ifstream& file, std::unique_ptr<FscBlock<_specific_block_identifier>>& block) {
            uint8_t actual_block_identifier;
            file.read(reinterpret_cast<char*>(&actual_block_identifier), sizeof(actual_block_identifier));
            if (actual_block_identifier != _specific_block_identifier) {
                return "expected block identifier " + std::to_string(_specific_block_identifier);
            }

            if constexpr (std::is_base_of_v<_internal::FscSamplesBlock<_specific_block_identifier>, FscBlock<_specific_block_identifier>>) {
                block = std::make_unique<FscBlock<_specific_block_identifier>>(file, *_header);
            }
            else {
                block = std::make_unique<FscBlock<_specific_block_identifier>>(file);
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
