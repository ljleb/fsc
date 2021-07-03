#include <ScoreFile.hpp>
#include <Options.hpp>
#include <iostream>

template <typename SetEvent>
void applyPitches(
    std::vector<s2m::ScoreBlock<s2m::BlockIdentifier::NOTE>>& notes,
    std::vector<s2m::ScoreBlock<s2m::BlockIdentifier::EVENT>> const& samples,
    SetEvent const& set_event);

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

    auto const& pitches_file_name { options.pitches_file_name() };
    if (pitches_file_name.has_value()) {
        s2m::ScoreFile<s2m::BlockIdentifier::EVENT> pitch_file;
        auto const&& pitches_read_error { pitch_file.read(pitches_file_name.value()) };
        if (pitches_read_error.has_value()) {
            std::cerr << pitches_read_error.value() << std::endl;
            exit(1);
        }

        applyPitches(
            note_file.samples(),
            pitch_file.samples(),
            [](s2m::ScoreBlock<s2m::BlockIdentifier::NOTE>& note, float const& pitch) {
                note.set_pitch(pitch);
            });
    }

    auto const&& write_error { note_file.write(options.output_file_name()) };
    if (write_error.has_value())
    {
        std::cerr << write_error.value() << std::endl;
        exit(1);
    }
    return 0;
}

template <typename SetEvent>
void applyPitches(
    std::vector<s2m::ScoreBlock<s2m::BlockIdentifier::NOTE>>& notes,
    std::vector<s2m::ScoreBlock<s2m::BlockIdentifier::EVENT>> const& samples,
    SetEvent const& set_event) {
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
