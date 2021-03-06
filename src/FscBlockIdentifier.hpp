#ifndef FSC2FSC_FSC_BLOCK_IDENTIFIER_HPP
#define FSC2FSC_FSC_BLOCK_IDENTIFIER_HPP

namespace fsc {
    namespace FscBlockIdentifier {
        enum Enum {
            HEADER = 0x64684c46,
            FL_VERSION = 0xc7,
            EVENT = 0xdf,
            NOTE = 0xe0,
        };
    }
}

#endif
