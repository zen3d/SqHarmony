
#pragma once

#include "RankedChord.h"

#include <memory>
#include <vector>

/**
 * @brief This guy is designed to write whole songs.
 * 
 * It knows how to find a legal sequence of chords, given the roots.
 * It knows how to search for an optimal song, using branch and bound.
 */
class HarmonySong {
    friend class HarmonySong2SequencerTrack;

public:
    HarmonySong(const Options&, const int *);  // pass the progression this way

    void print() const;
    std::shared_ptr<RankedChord> get(int n) {
        assert(n < int(chords.size()));
        return chords[n];
    }
    int size() const {
        return chords.size();
    }
    bool Generate(const Options& options, int Nlevel, bool show);  // ret true if ok!
private:
    // the final chords we make
    std::vector<std::shared_ptr<RankedChord>> chords;

    Chord4Manager chordManager;

    bool firstTime=true;
    bool isValid() const;
    void analyze(const Options& options) const;
};
