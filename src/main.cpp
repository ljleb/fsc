#include <ScoreFile.hpp>
#include <Options.hpp>
#include <iostream>
#include <vector>

template <typename SetEventFunction>
void apply_events(
    std::vector<s2m::ScoreBlock<s2m::BlockIdentifier::NOTE>>& notes,
    std::vector<s2m::ScoreBlock<s2m::BlockIdentifier::EVENT>> const& samples,
    SetEventFunction const& set_event);

template <typename SetEventFunction>
void read_and_apply_events(
    s2m::ScoreFile<s2m::BlockIdentifier::NOTE>& note_file,
    std::optional<std::reference_wrapper<std::string const> const> const& file_name,
    SetEventFunction const& set_event);

int main(int argc, char** argv)
{
    std::vector<std::string> args { argv + 1, argv + argc };
    s2m::Options options { s2m::Options::from_args(args) };

    s2m::ScoreFile<s2m::BlockIdentifier::NOTE> note_file;
    auto const&& notes_read_error { note_file.read(options.notes_file_name()) };
    if (notes_read_error.has_value())
    {
        std::cerr << notes_read_error.value() << std::endl;
        exit(1);
    }

    read_and_apply_events(note_file, options.pitches_file_name(), [](auto& note, auto const& pitch) {
        note.set_pitch(pitch);
    });

    read_and_apply_events(note_file, options.velocities_file_name(), [](auto& note, auto const& velocity) {
        note.set_velocity(velocity);
    });

    auto const&& write_error { note_file.write(options.output_file_name()) };
    if (write_error.has_value())
    {
        std::cerr << write_error.value() << std::endl;
        exit(1);
    }
    return 0;
}

template <typename SetEventFunction>
void read_and_apply_events(
    s2m::ScoreFile<s2m::BlockIdentifier::NOTE>& note_file,
    std::optional<std::reference_wrapper<std::string const> const> const& file_name,
    SetEventFunction const& set_event
) {
    if (file_name.has_value()) {
        s2m::ScoreFile<s2m::BlockIdentifier::EVENT> file;
        auto const&& read_error { file.read(file_name.value()) };
        if (read_error.has_value()) {
            std::cerr << read_error.value() << std::endl;
            exit(1);
        }

        apply_events(note_file.samples(), file.samples(), set_event);
    }
}

template <typename SetEventFunction>
void apply_events(
    std::vector<s2m::ScoreBlock<s2m::BlockIdentifier::NOTE>>& notes,
    std::vector<s2m::ScoreBlock<s2m::BlockIdentifier::EVENT>> const& samples,
    SetEventFunction const& set_event
) {
    for (s2m::ScoreBlock<s2m::BlockIdentifier::NOTE>& note: notes) {
        s2m::ScoreBlock<s2m::BlockIdentifier::EVENT> const* closest_sample { &samples[0] };
        for (s2m::ScoreBlock<s2m::BlockIdentifier::EVENT> const& sample: samples) {
            if (sample.get_position() <= note.get_position() && sample.get_position() > closest_sample->get_position()) {
                closest_sample = &sample;
            }
        }

        set_event(note, closest_sample->get_value());
    }
}
