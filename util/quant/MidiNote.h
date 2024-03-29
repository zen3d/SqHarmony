#pragma once

#include <assert.h>
#include <algorithm>
#include <stdio.h>

class MidiNote {
public:
    MidiNote(int p) : pitch(p) {}
    MidiNote() : pitch(64) {}

    int get() const { return pitch; }

    bool operator==(const MidiNote& other) const {
        return this->pitch == other.pitch;
    }

    int getLedgerLine(bool bassStaff) const;

    static const int C3 = 60;  // C3 is 60 in midi spec.
    static const int C = 0;
    static const int MiddleC = 60 + 12;

private:
    int pitch = C3;
};

inline int MidiNote::getLedgerLine(bool bassStaff) const {
    const int normalizedPitch = pitch - MiddleC;
    int octave = (normalizedPitch / 12);
    int semi = normalizedPitch % 12;
    if (semi < 0) {
        semi += 12;
        octave -= 1;
    }
    int line = 0;
    switch (semi) {
        case 0:  // C, C#
        case 1:
            line = -2;
            break;
        case 2:  // D, D#
        case 3:
            line = -1;
            break;
        case 4:  // E
            line = 0;
            break;
        case 5:  // F, F#
        case 6:
            line = 1;
            break;
        case 7:  // G, G#
        case 8:
            line = 2;
            break;
        case 9:  // A, A#
        case 10:
            line = 3;
            break;
        case 11:  // B
            line = 4;
            break;
        default:
            assert(false);
    }
    line += (octave * 7);
    if (bassStaff) {
        line += 12;
    }
    // printf("MidiNote::Ll ret %d for pitch %d\n", line, pitch);
    return line;
}
