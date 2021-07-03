#ifndef FSC2MIDI_SCORE_FILE_HPP
#define FSC2MIDI_SCORE_FILE_HPP

#include <ScoreSample.hpp>
#include <MagicalIdentifier.hpp>

#include <fstream>
#include <vector>
#include <memory>

namespace s2m {
    template <MagicalIdentifier::Enum _magical_identifier>
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

            file.seekg(0x30, std::ios::beg);

            uint8_t actual_magical_identifier;
            file.read(reinterpret_cast<char*>(&actual_magical_identifier), sizeof(actual_magical_identifier));
            _status = actual_magical_identifier == _magical_identifier;
            if (!_status) {
                return;
            }

            uint8_t data_segment_size;
            file.read(reinterpret_cast<char*>(&data_segment_size), sizeof(data_segment_size));

            for (uint32_t i { 0 }; i < data_segment_size; i += ScoreSample<_magical_identifier>::SIZE) {
                _samples.emplace_back(file);
            }

            uint32_t const& file_size { file.tellg() };
            _input_data = std::make_unique<char[]>(file_size + 1);
            _input_data[file_size] = '\0';

            file.seekg(0, std::ios::beg);
            file.read(_input_data.get(), file_size);
        }

        void write(std::string const& file_name) {
            std::ofstream file { file_name, std::ios::binary };
            _status = file.good();
            if (!_status) {
                return;
            }

            file.write(_input_data.get(), 0x32);

            for (ScoreSample<_magical_identifier> const& sample: _samples) {
                char* const& current_data { _input_data.get() + file.tellp() };
                sample.update_pitch(current_data);
                file.write(current_data, ScoreSample<_magical_identifier>::SIZE);
            }
        }

        bool status() {
            return _status;
        }

        std::vector<ScoreSample<_magical_identifier>>& samples() {
            return _samples;
        }

    private:
        bool _status;
        std::vector<ScoreSample<_magical_identifier>> _samples;
        std::unique_ptr<char[]> _input_data;
    };
}

#endif
