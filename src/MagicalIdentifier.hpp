#ifndef FSC2MIDI_MAGICAL_IDENTIFIER_HPP
#define FSC2MIDI_MAGICAL_IDENTIFIER_HPP

#include <ScoreSample.hpp>
#include <fstream>
#include <vector>

#include <iostream>

namespace s2m {
    namespace MagicalIdentifier {
        enum Enum {
            PITCH = 0xdf,
            NOTE = 0xe0,
        };
    }
}

#endif
