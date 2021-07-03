#ifndef FSC2FSC_OPTIONS_HPP
#define FSC2FSC_OPTIONS_HPP

#include <vector>
#include <string>
#include <optional>
#include <cstdint>
#include <iostream>

namespace s2m {
    struct Options {
        static Options from_args(std::vector<std::string> const& args) {
            std::string const* notes_file_name { nullptr };
            std::string const* output_file_name { nullptr };
            std::string const* pitches_file_name { nullptr };
            std::string const* velocities_file_name { nullptr };

            for (uint8_t i { 0 }; i < args.size(); i += 2) {
                if (i + 1 >= args.size())
                {
                    show_usage_and_exit();
                }

                auto const& arg { args[i] };

                if (arg == "-P" || arg == "--pitch-from") {
                    pitches_file_name = &args[i+1];
                } else if (arg == "-V" || arg == "--velocity-from") {
                    velocities_file_name = &args[i+1];
                } else {
                    notes_file_name = &arg;
                    output_file_name = &args[i+1];

                    if (i + 2 < args.size()) {
                        show_usage_and_exit();
                    }
                }
            }

            return Options {
                *notes_file_name,
                *output_file_name,
                pitches_file_name,
                velocities_file_name
            };
        }

        Options(
            std::string const& notes_file_name,
            std::string const& output_file_name,
            std::string const* const& pitches_file_name,
            std::string const* const& velocities_file_name
        ):
            _notes_file_name { notes_file_name },
            _output_file_name { output_file_name },
            _pitches_file_name { pitches_file_name },
            _velocities_file_name { velocities_file_name }
        {}

        std::optional<std::reference_wrapper<std::string const> const> pitches_file_name() const {
            if (_pitches_file_name != nullptr) {
                return *_pitches_file_name;
            }
            else {
                return {};
            }
        }

        std::optional<std::reference_wrapper<std::string const> const> velocities_file_name() const {
            if (_velocities_file_name != nullptr) {
                return *_velocities_file_name;
            }
            else {
                return {};
            }
        }

        constexpr std::string const& notes_file_name() const {
            return _notes_file_name;
        }

        constexpr std::string const& output_file_name() const {
            return _output_file_name;
        }

    private:
        std::string const& _notes_file_name;
        std::string const& _output_file_name;
        std::string const* const _pitches_file_name;
        std::string const* const _velocities_file_name;

        static void show_usage_and_exit() {
            std::cerr << "usage: fsc [options] <fsc-pattern> <fsc-output>" << std::endl;
            exit(1);
        }
    };
}

#endif
