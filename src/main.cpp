#include <ScoreFile.hpp>
#include <iostream>

void applyPitches(
    std::vector<s2m::ScoreBlock<s2m::BlockIdentifier::NOTE>>& notes,
    std::vector<s2m::ScoreBlock<s2m::BlockIdentifier::PITCH>> const& pitches);

int main(int argc, char** argv)
{
    std::vector<std::string> args { argv, argv + argc };
    if (argc != 4)
    {
        std::cerr << "usage: fsc2fsc <pitches-fsc> <notes-fsc> <output>\n";
        exit(1);
    }

    s2m::ScoreFile<s2m::BlockIdentifier::PITCH> pitch_file;
    auto const&& pitches_read_error { pitch_file.read(args[1]) };
    if (pitches_read_error.has_value()) {
        std::cerr << pitches_read_error.value() << std::endl;
        exit(1);
    }

    s2m::ScoreFile<s2m::BlockIdentifier::NOTE> note_file;
    auto const&& notes_read_error { note_file.read(args[2]) };
    if (notes_read_error.has_value())
    {
        std::cerr << notes_read_error.value() << std::endl;
        exit(1);
    }

    applyPitches(note_file.samples(), pitch_file.samples());

    auto const&& write_error { note_file.write(args[3]) };
    if (write_error.has_value())
    {
        std::cerr << write_error.value() << std::endl;
        exit(1);
    }
    return 0;
}

void applyPitches(
    std::vector<s2m::ScoreBlock<s2m::BlockIdentifier::NOTE>>& notes,
    std::vector<s2m::ScoreBlock<s2m::BlockIdentifier::PITCH>> const& pitches) {
    for (s2m::ScoreBlock<s2m::BlockIdentifier::NOTE>& note: notes) {

        s2m::ScoreBlock<s2m::BlockIdentifier::PITCH> const* closest_pitch { &pitches[0] };
        for (s2m::ScoreBlock<s2m::BlockIdentifier::PITCH> const& pitch: pitches) {
            if (pitch.get_position() < note.get_position() && pitch.get_position() > closest_pitch->get_position()) {
                closest_pitch = &pitch;
            }
        }

        note.set_pitch(closest_pitch->get_pitch());
    }
}
