#ifndef FSC2MIDI_SCORE_FILE_HPP
#define FSC2MIDI_SCORE_FILE_HPP

#include <ScoreSample.hpp>
#include <fstream>
#include <vector>

#include <iostream>

namespace s2m {
    struct ScoreFile {
        ScoreFile():
            _status { true }
        {}

        void read(std::string const& file_name) {
            std::ifstream raw_file { file_name, std::ios::binary };
            _status = raw_file.bad();
            if (!_status) {
                return;
            }

            raw_file.seekg(0x31, std::ios::beg);

            uint8_t data_segment_size;
            raw_file.read(reinterpret_cast<char*>(&data_segment_size), sizeof(data_segment_size));
            // std::cout << "size of data: " << (uint32_t) data_segment_size << std::endl;

            for (uint32_t i { 0 }; i < data_segment_size / 12; ++i) {
                uint32_t score_position;
                uint32_t score_type;
                float score_value;

                raw_file.read(reinterpret_cast<char*>(&score_position), sizeof(score_position));
                raw_file.read(reinterpret_cast<char*>(&score_type), sizeof(score_type));
                raw_file.read(reinterpret_cast<char*>(&score_value), sizeof(score_value));

                // std::cout << "\nscore #" << i << ":" << std::endl;
                // std::cout << "score position: " << score_position << std::endl;
                // std::cout << "score type: " << score_type << std::endl;
                // std::cout << "score value: " << score_value << std::endl;

                _samples.emplace_back(score_position, score_type, score_value);
            }
        }

        bool status() {
            return _status;
        }

        std::vector<ScoreSample> const& scores() {
            return _scores;
        }

    private:
        std::vector<ScoreSample> _samples;
        bool _status;
    };
}

#endif
