#include "HarmonyChords.h"
#include "ProgressionAnalyzer.h"

#include "Chord4Manager.h"

const Chord4* HarmonyChords::findChord(
    bool show,
    const Options& options,
    const Chord4Manager& manager,
    int root) {
    // TODO assert that root is in scale (scale needs size())
    assert(root >= 1);

    // but now we find issues, so let's get the best root pos
    const int size = manager.size(root);
    int rankToTry = 0;
    for (bool done = false; !done; ++rankToTry) {
        if (rankToTry >= size) {
            return nullptr;
        }
        const Chord4* chord = manager.get2(root, rankToTry);

        // only accept a chord in root position with nice doubling
        if ((chord->inversion(options) == ROOT_POS_INVERSION) &&
            (chord->isCorrectDoubling(options))) {
            return chord;
        }
    }
    assert(false);
    return nullptr;
}

const Chord4* HarmonyChords::findChord(
    bool show,
    const Options& options,
    const Chord4Manager& manager,
    const Chord4& prev,
    int root) {
    return find(show, options, manager, nullptr, &prev, root);
}

const Chord4* HarmonyChords::findChord(
    bool show,
    const Options& options,
    const Chord4Manager& manager,
    const Chord4& prevPrev,
    const Chord4& prev,
    int root) {
    const Chord4* ret = find(show, options, manager, &prevPrev, &prev, root);
    assert(ret);        // we should always find something;
    return ret;

#if 0
    a bc // we can never get here now.

    ret = find(options, manager, nullptr, &prev, root);
    printf(ret ? "found in delaxed\n" : "not found at all]n");
    return ret;
#endif
}

const Chord4* HarmonyChords::find(
    bool show,
    const Options& options,
    const Chord4Manager& manager,
    const Chord4* prevPrev,
    const Chord4* prev,
    int root) {
    if (prev && prevPrev) {
        printf("find called with prevOrev %s (root %d)\n", prevPrev->toString().c_str(), prevPrev->fetchRoot());
        printf("  .. prev %s (root %d)\n",
               prev->toString().c_str(),
               prev->fetchRoot());
        printf(" .. and cur root % d\n", root);
    } else if (prev) {
        printf("find called with prev %s (root %d) and cur root %d\n",
               prev->toString().c_str(),
               prev->fetchRoot(),
               root);
    }

    if (prev) {
        printf("chord 76 called with root = %d, prev = %d\n", root, prev->fetchRoot());
        fflush(stdout);
    }
    assert(!prev || (prev->fetchRoot() != root));  // should not have two rows in succession
    assert(!prevPrev || (prevPrev->fetchRoot() != prev->fetchRoot()));

  //  const int ppr = prevPrev ? prevPrev->fetchRoot() : -1;
  //  const int pr = prev ? prev->fetchRoot() : -1;

    const int size = manager.size(root);
    int rankToTry = 0;
    printf("in find, rank start = %d, size=%d\n", rankToTry, size);

    int lowestPenalty = ProgressionAnalyzer::MAX_PENALTY;
    const Chord4* bestChord = nullptr;

    for (bool done = false; !done; ++rankToTry) {
        if (rankToTry >= size) {
            done = true;
        }
        else {
            const Chord4* currentChord = manager.get2(root, rankToTry);
            const int currentPenalty = progressionPenalty(options, prevPrev, prev, currentChord, show);
            if (currentPenalty == 0) {
                printf("found penalty 0\n");
                return currentChord;
            }
            // printf("hit a penalty in search %d\n", currentPenalty);
            if (currentPenalty < lowestPenalty) {
                lowestPenalty = currentPenalty;
                bestChord = currentChord;
            }
        }
    }
    printf("didn't find perfect, returning penalty = %d\n", lowestPenalty);
    return bestChord;
}

int HarmonyChords::progressionPenalty(
    const Options& options,
    const Chord4* prevPrev,
    const Chord4* prev,
    const Chord4* current,
    bool show) {
    assert(current);

    if (!prevPrev && !prev) {
        return 0;  // chord on its own is always ok
    }

    // printf("isProgressionOK about to call can follow\n");
  //  const bool canFollow = current->canFollowThisGuy(options, *prev);
    int currentPenalty = current->penaltForFollowingThisGuy(options, *prev, show);
    if (!prevPrev) {
        return currentPenalty;
    }

    const bool firstEqualsThird = (*current == *prevPrev);
    
    if (firstEqualsThird) {
        printf("first equals third\n");
        printf("penalty %s\n", current->toString().c_str());
        printf("first = %s\n", prevPrev->toString().c_str());
        printf("second = %s\n", prev->toString().c_str());
        currentPenalty += ProgressionAnalyzer::PENALTY_FOR_REPEATED_CHORDS;
    }
    return currentPenalty;
}


#if 0  // orig, before penalty
const Chord4* HarmonyChords::find(
    const Options& options,
    const Chord4Manager& manager,
    const Chord4* prevPrev,
    const Chord4* prev,
    int root) {
    if (prev && prevPrev) {
        printf("find called with prevOrev %s (root %d)\n", prevPrev->toString().c_str(), prevPrev->fetchRoot());
        printf("  .. prev %s (root %d)\n",
               prev->toString().c_str(),
               prev->fetchRoot());
        printf(" .. and cur root % d\n", root);
    } else if (prev) {
        printf("find called with prev %s (root %d) and cur root %d\n",
               prev->toString().c_str(),
               prev->fetchRoot(),
               root);
    }

    assert(!prev || (prev->fetchRoot() != root));  // should not have two rows in succession
    assert(!prevPrev || (prevPrev->fetchRoot() != prev->fetchRoot()));

    const int ppr = prevPrev ? prevPrev->fetchRoot() : -1;
    const int pr = prev ? prev->fetchRoot() : -1;

    const int size = manager.size(root);
    int rankToTry = 0;
    printf("in find, rank start = %d, size=%d\n", rankToTry, size);
    for (bool done = false; !done; ++rankToTry) {
        if (rankToTry >= size) {
            //assert(false);  // give up
            printf("find is giving up\n");
            return nullptr;
        }
        const Chord4* currentChord = manager.get2(root, rankToTry);
        if (progressionOK(options, prevPrev, prev, currentChord)) {
            printf("find returning rand %d\n", rankToTry);
            return currentChord;
        }
    }
    return nullptr;
}

bool HarmonyChords::progressionOK(const Options& options,
                                  const Chord4* prevPrev,
                                  const Chord4* prev,
                                  const Chord4* current) {
    assert(current);
    if (!prevPrev && !prev) {
        return true;  // chord on its own is always ok
    }
    // printf("isProgressionOK about to call can follow\n");
    const bool canFollow = current->canFollowThisGuy(options, *prev);
    if (!prevPrev || !canFollow) {
        //  printf("can2 %s to %s ret %d\n", prev->toString().c_str(), current->toString().c_str(), canFollow);
        return canFollow;
    }

    const bool firstEqualsThird = (*current == *prevPrev);
    if (firstEqualsThird) {
        printf("rejecting %s\n", current->toString().c_str());
        printf("first = %s\n", prevPrev->toString().c_str());
        printf("second = %s\n", prev->toString().c_str());
    }
    return !firstEqualsThird;

    //return canFollow && !(*current == *prevPrev);
}
#endif
