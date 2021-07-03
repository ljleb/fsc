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
        std::cerr << "usage: fsc2midi <pitches-fsc> <notes-fsc> <output>\n";
        exit(1);
    }

    s2m::ScoreFile<s2m::BlockIdentifier::PITCH> pitch_file;
    pitch_file.read(args[1]);
    if (!pitch_file.status())
    {
        std::cerr << "Error reading pitches file " << args[1] << std::endl;
        exit(1);
    }

    s2m::ScoreFile<s2m::BlockIdentifier::NOTE> note_file;
    note_file.read(args[2]);
    if (!note_file.status())
    {
        std::cerr << "Error reading notes file " << args[2] << std::endl;
        exit(1);
    }

    applyPitches(note_file.samples(), pitch_file.samples());
    note_file.write(args[3]);
    return 0;
}

void applyPitches(
    std::vector<s2m::ScoreBlock<s2m::BlockIdentifier::NOTE>>& notes,
    std::vector<s2m::ScoreBlock<s2m::BlockIdentifier::PITCH>> const& pitches) {
    for (s2m::ScoreBlock<s2m::BlockIdentifier::NOTE>& note: notes) {

        s2m::ScoreBlock<s2m::BlockIdentifier::PITCH> const* closest_pitch { &pitches[0] };
        for (s2m::ScoreBlock<s2m::BlockIdentifier::PITCH> const& pitch: pitches) {
            if (note.get_position() > pitch.get_position()) {
                break;
            }
            closest_pitch = &pitch;
        }

        note.set_pitch(closest_pitch->get_pitch());
    }
}
