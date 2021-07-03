#include <FscFile.hpp>
#include <Options.hpp>
#include <iostream>
#include <vector>

template <typename SetEventFunction>
void apply_events(
    fsc::FscBlock<fsc::FscBlockIdentifier::NOTE>& notes,
    fsc::FscBlock<fsc::FscBlockIdentifier::EVENT> const& events,
    SetEventFunction const& set_event);

template <typename SetEventFunction>
void read_and_apply_events(
    fsc::FscFile<fsc::FscBlockIdentifier::NOTE>& note_file,
    std::optional<std::reference_wrapper<std::string const> const> const& file_name,
    SetEventFunction const& set_event);

int main(int argc, char** argv)
{
    std::vector<std::string> args { argv + 1, argv + argc };
    fsc::Options options { fsc::Options::from_args(args) };

    fsc::FscFile<fsc::FscBlockIdentifier::NOTE> note_file;
    auto const&& notes_read_error { note_file.read(options.notes_file_name()) };
    if (notes_read_error.has_value())
    {
        std::cerr << notes_read_error.value() << std::endl;
        exit(1);
    }

    read_and_apply_events(note_file, options.pans_file_name(), [](auto& note, auto const& pan) {
        note.set_pan(pan);
    });

    read_and_apply_events(note_file, options.velocities_file_name(), [](auto& note, auto const& velocity) {
        note.set_velocity(velocity);
    });

    read_and_apply_events(note_file, options.releases_file_name(), [](auto& note, auto const& release) {
        note.set_release(release);
    });

    read_and_apply_events(note_file, options.mod_xs_file_name(), [](auto& note, auto const& release) {
        note.set_mod_x(release);
    });

    read_and_apply_events(note_file, options.mod_ys_file_name(), [](auto& note, auto const& release) {
        note.set_mod_y(release);
    });

    read_and_apply_events(note_file, options.pitches_file_name(), [](auto& note, auto const& pitch) {
        note.set_pitch(pitch);
    });

    auto const&& output_write_error { note_file.write(options.output_file_name()) };
    if (output_write_error.has_value())
    {
        std::cerr << output_write_error.value() << std::endl;
        exit(1);
    }
    return 0;
}

template <typename SetEventFunction>
void read_and_apply_events(
    fsc::FscFile<fsc::FscBlockIdentifier::NOTE>& note_file,
    std::optional<std::reference_wrapper<std::string const> const> const& file_name,
    SetEventFunction const& set_event
) {
    if (file_name.has_value()) {
        fsc::FscFile<fsc::FscBlockIdentifier::EVENT> file;
        auto const&& read_error { file.read(file_name.value()) };
        if (read_error.has_value()) {
            std::cerr << read_error.value() << std::endl;
            exit(1);
        }

        apply_events(note_file.block(), file.block(), set_event);
    }
}

template <typename SetEventFunction>
void apply_events(
    fsc::FscBlock<fsc::FscBlockIdentifier::NOTE>& notes,
    fsc::FscBlock<fsc::FscBlockIdentifier::EVENT> const& events,
    SetEventFunction const& set_event
) {
    for (fsc::FscSample<fsc::FscBlockIdentifier::NOTE>& note: notes.get_samples()) {
        fsc::FscSample<fsc::FscBlockIdentifier::EVENT> const*&& closest_event { &events.get_samples()[0] };
        for (fsc::FscSample<fsc::FscBlockIdentifier::EVENT> const& event: events.get_samples()) {
            if (event.get_position() <= note.get_position() && event.get_position() > closest_event->get_position()) {
                closest_event = &event;
            }
        }

        set_event(note, closest_event->get_value());
    }
}
