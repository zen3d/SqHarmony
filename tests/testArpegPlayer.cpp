
#include "ArpegPlayer.h"
#include "asserts.h"

static void testArpegPlayerOneNote() {
    NoteBuffer nb(1);
    nb.push_back(5, 23);
    ArpegPlayer ap(&nb);
    ap.setMode(ArpegPlayer::Mode::UP);
    const float x = ap.clock();
    assertEQ(x, 5);
}

static void testArpegPlayerTwoNotes() {
    NoteBuffer nb(4);
    nb.push_back(5, 55);
    nb.push_back(100, 56);
    ArpegPlayer ap(&nb);
    ap.setMode(ArpegPlayer::Mode::UP);

    float x = ap.clock();
    assertEQ(x, 5);

    x = ap.clock();
    assertEQ(x, 100);

    // back to start
    x = ap.clock();
    assertEQ(x, 5);
}

static void testArpegPlayerZeroNotes() {
    NoteBuffer nb(1);
    nb.push_back(5, 55);
    nb.removeForChannel(55);

    ArpegPlayer ap(&nb);
    const float x = ap.clock();
    assertEQ(x, 0);
}

// no pause, simpler version
static void testArpegSubx(ArpegPlayer::Mode mode, const float* input, int numInput, const float* expectedOutput, int numOutput) {
    const int requiredSize = numInput + 2;  // 2 is "just because
    NoteBuffer nb(requiredSize);
    ArpegPlayer ap(&nb);
    ap.setMode(mode);

    for (int i = 0; i < numInput; ++i) {
        nb.push_back(input[i], i);
    }

    for (int i = 0; i < numOutput; ++i) {
        const float expected = expectedOutput[i];
        const float actual = ap.clock();
        // printf("i=%d actual=%f expected=%f\n", i, actual, expected);
        assertEQ(actual, expected);
    }
}

static void testArpegPauseSub2(ArpegPlayer::Mode mode,
                               const float* input,
                               int numInput,
                               const float* expectedOutputBefore,
                               int numOutputBefore,
                               bool withClockInBetween

) {
    // printf("----- enter testArpegPauseSub2\n");
    NoteBuffer nb(numInput + 4);
    ArpegPlayer ap(&nb);
    ap.setMode(mode);

    // put in the input chord
    for (int i = 0; i < numInput; ++i) {
        nb.push_back(input[i], i);
    }

    // printf("testArpegPauseSub about to play first\n");

    // verify initial output
    for (int i = 0; i < numOutputBefore; ++i) {
        const float expected = expectedOutputBefore[i];
        const float actual = ap.clock();
        // printf("i=%d actual=%f expected=%f\n", i, actual, expected);
        assertEQ(actual, expected);
    }

    // printf("testArpegPauseSub about to remove input\n");
    // remove the input
    for (int i = 0; i < numInput; ++i) {
        //  nb.push_back(input[i], i);
        nb.removeForChannel(i);
    }
    assert(nb.empty());

    if (withClockInBetween) {
        // printf("testArpegPauseSub about to send dummy clock\n");
        assert(withClockInBetween);
        // one clock to make it work
        float x = ap.clock();
        assertEQ(x, 0);
    }

    // printf("testArpegPauseSub about to add input back\n");
    // add the input back
    for (int i = 0; i < numInput; ++i) {
        nb.push_back(input[i], i);
    }

    // printf("testArpegPauseSub about to play again\n");
    // verify the post o
    for (int i = 0; i < numOutputBefore; ++i) {
        const float expected = expectedOutputBefore[i];
        const float actual = ap.clock();
        // printf("i=%d actual=%f expected=%f\n", i, actual, expected);
        assertEQ(actual, expected);
    }
}

static void testArpegPauseSub(ArpegPlayer::Mode mode,
                              const float* input,
                              int numInput,
                              const float* expectedOutput,
                              int numOutput) {
    testArpegSubx(mode, input, numInput, expectedOutput, numOutput);

    printf("\nnow doing - mode\n");
    // everyone should output zero when fed with it
    float zeros[3] = {0};
    testArpegSubx(mode, input, 0, zeros, 2);

    printf("skipping pause for now\n");
    // testArpegPauseSub2(mode, input, numInput, expectedOutput, numOutput, true);
    //  testArpegPauseSub2(mode, input, numInput, expectedOutput, numOutput, false);
}

static void testArpegPlayerUp() {
    printf("\n----- testArpegPlayerUp ---\n");
    auto mode = ArpegPlayer::Mode::UP;
    float input[] = {10, 9, 8};
    float expectedOutput[] = {8, 9, 10, 8, 9, 10};
    testArpegPauseSub(mode, input, 3, expectedOutput, 6);

    printf("\n----- testArpegPlayerUp B ---\n");
    float input2[] = {10};
    float expectedOutput2[] = {10, 10, 10, 10};
    testArpegSubx(mode, input2, 1, expectedOutput2, 4);

    printf("\n----- testArpegPlayerUp  C ---\n");
    float input3[] = {10, 11};
    float expectedOutput3[] = {10, 11, 10, 11};
    testArpegSubx(mode, input3, 2, expectedOutput3, 4);
}

static void testArpegPlayerDown() {
    printf("\n----- testArpegPlayerDOWN ---\n");
    const auto mode = ArpegPlayer::Mode::DOWN;
    float input[] = {10, 9, 8};
    float expectedOutput[] = {10, 9, 8, 10, 9, 8, 10};
    testArpegPauseSub(mode, input, 3, expectedOutput, 7);

    printf("\n----- testArpegPlayerDown B ---\n");
    float input2[] = {10};
    float expectedOutput2[] = {10, 10, 10, 10};
    testArpegSubx(mode, input2, 1, expectedOutput2, 4);

    printf("\n----- testArpegPlayerDown  C ---\n");
    float input3[] = {10, 11};
    float expectedOutput3[] = {11, 10, 11, 10};
    testArpegSubx(mode, input3, 2, expectedOutput3, 4);
}

static void testArpegPlayerUp_Down() {
    printf("\n----- testArpegPlayerUp_Down 6---\n");

    const auto mode = ArpegPlayer::Mode::UP_DOWN_DBL;
    const float input[] = {10, 9, 8, 7, 6, 5};
    float expectedOutput[] = {5, 6, 7, 8, 9, 10, 10, 9, 8, 7, 6, 5, 5, 6, 7};
    testArpegPauseSub(mode, input, 6, expectedOutput, 15);

    printf("\n----- testArpegPlayerUp_Down 5---\n");
    float expectedOutput5[] = {6, 7, 8, 9, 10, 10, 9, 8, 7, 6, 6, 7, 8, 9, 10};
    testArpegPauseSub(mode, input, 5, expectedOutput5, 15);

    printf("\n----- testArpegPlayerUp_Down 4---\n");
    float expectedOutput4[] = {7, 8, 9, 10, 10, 9, 8, 7, 7, 8, 9, 10, 10, 9, 8};
    testArpegPauseSub(mode, input, 4, expectedOutput4, 15);

    printf("\n----- testArpegPlayerUp_Down 3---\n");
    float expectedOutput3[] = {8, 9, 10, 10, 9, 8, 8, 9, 10, 10, 9};
    testArpegPauseSub(mode, input, 3, expectedOutput3, 11);

    printf("\n----- testArpegPlayerUp_Down 2---\n");
    float expectedOutput2[] = { 9, 10, 10, 9, 9, 10, 10, 9, 9};
    testArpegPauseSub(mode, input, 2, expectedOutput2, 9);

    printf("\n----- testArpegPlayerUp_Down 1---\n");
    float expectedOutput1[] = {
        10,
        10,
        10,
        10,
    };
    testArpegPauseSub(mode, input, 1, expectedOutput1, 4);
}

static void testArpegPlayerUpDown() {
    printf("\n----- testArpegPlayerUPDOWN ---\n");

    const auto mode = ArpegPlayer::Mode::UPDOWN;
    float input[] = {10, 9, 8, 7};
    float expectedOutput[] = {7, 8, 9, 10, 9, 8, 7, 8, 9, 10};
    testArpegPauseSub(mode, input, 4, expectedOutput, 10);

    printf("\n----- testArpegPlayerUPDown B ---\n");
    float input2[] = {10};
    float expectedOutput2[] = {10, 10, 10, 10};
    testArpegSubx(mode, input2, 1, expectedOutput2, 4);

    printf("\n----- testArpegPlayerUPDown  C ---\n");
    float input3[] = {10, 11};
    float expectedOutput3[] = {10, 11, 10, 11, 10};
    testArpegSubx(mode, input3, 2, expectedOutput3, 5);

    printf("\n----- testArpegPlayerUPDown  D ---\n");
    float input4[] = {10, 9, 8};
    float expectedOutput4[] = {8, 9, 10, 9, 8, 9, 10, 9};
    testArpegSubx(mode, input4, 3, expectedOutput4, 8);
}

static void testArpegPlayerDownUp() {
    printf("\n----- testArpegPlayerDownUp ---\n");
    const auto mode = ArpegPlayer::Mode::DOWNUP;
    float input[] = {10, 9, 8, 7};

    float expectedOutput[] = {10, 9, 8, 7, 8, 9, 10, 9};
    testArpegPauseSub(mode, input, 4, expectedOutput, 8);

    printf("\n----- testArpegPlayerDownUp B ---\n");
    float input2[] = {10};
    float expectedOutput2[] = {10, 10, 10, 10};
    testArpegSubx(mode, input2, 1, expectedOutput2, 4);

    printf("\n----- testArpegPlayerDownUp  C ---\n");
    float input3[] = {10, 11};
    float expectedOutput3[] = {11, 10, 11, 10, 11};
    testArpegSubx(mode, input3, 2, expectedOutput3, 5);

    printf("\n----- testArpegPlayerDownUp  D ---\n");
    float input4[] = {10, 9, 8};
    float expectedOutput4[] = {10, 9, 8, 9, 10, 9, 8, 9};
    testArpegSubx(mode, input4, 3, expectedOutput4, 8);

    printf("\n----- testArpegPlayerDownUp  E ---\n");
    float input5[] = {10, 9, 8, 7, 6, 5};
    float expectedOutput5[] = {10, 9, 8, 7, 6, 5, 6, 7, 8, 9, 10, 9, 8, 7, 6, 5};
    testArpegSubx(mode, input5, 6, expectedOutput5, 16);
}

static void testArpegPlayerDown_Up() {
    printf("\n----- testArpegPlayerDOWN_UP_DBL 6---\n");

    const auto mode = ArpegPlayer::Mode::DOWN_UP_DBL;
    const float input[] = {10, 9, 8, 7, 6, 5};

    const float expectedOutput[] = {10, 9, 8, 7, 6, 5, 5, 6, 7, 8, 9, 10, 10, 9, 8};
    testArpegPauseSub(mode, input, 6, expectedOutput, 15);

    printf("\n----- testArpegPlayerDOWN_UP_DBL 5---\n");
    float expectedOutput5[] = {10, 9, 8, 7, 6, 6, 7, 8, 9, 10, 10, 9, 8, 7};
    testArpegPauseSub(mode, input, 5, expectedOutput5, 14);

    printf("\n----- testArpegPlayerDOWN_UP_DBL 4---\n");
    float expectedOutput4[] = {10, 9, 8, 7, 7, 8, 9, 10, 10, 9, 8, 7, 7};
    testArpegPauseSub(mode, input, 4, expectedOutput4, 13);

    printf("\n----- testArpegPlayerDOWN_UP_DBL 3---\n");
    float expectedOutput3[] = {10, 9, 8, 8, 9, 10, 10, 9, 8, 8, 9};
    testArpegPauseSub(mode, input, 3, expectedOutput3, 11);

    printf("\n----- testArpegPlayerDOWN_UP_DBL 2---\n");
    float expectedOutput2[] = {10, 9, 9, 10, 10, 9, 9};
    testArpegPauseSub(mode, input, 2, expectedOutput2, 7);

    printf("\n----- testArpegPlayerDOWN_UP_DBL 1---\n");
    float expectedOutput1[] = {
        10,
        10,
        10,
        10,
    };
    testArpegPauseSub(mode, input, 1, expectedOutput1, 4);
}

static void testArpegPlayerUpPause() {
    float input[] = {10, 9, 8};
    float expectedOutput[] = {8, 9};
    testArpegPauseSub(ArpegPlayer::Mode::UP,
                      input, 3, expectedOutput, 2);
}

static void testArpegPlayerINSIDE_OUT() {
    const auto mode = ArpegPlayer::Mode::INSIDE_OUT;
    const float input[] = {5, 4, 3, 2, 1};

    printf("\n----- testArpegPlayerINSIDE_OUT 5---\n");
    const float expectedOutput5[] = {3, 4, 2, 5, 1};
    testArpegPauseSub(mode, input, 5, expectedOutput5, 5);

    printf("\n----- testArpegPlayerINSIDE_OUT 4---\n");
    const float expectedOutput4[] = {4, 3, 5, 2};
    testArpegPauseSub(mode, input, 4, expectedOutput4, 4);

    printf("\n----- testArpegPlayerINSIDE_OUT 3---\n");
    const float expectedOutput3[] = {4, 5, 3};
    testArpegPauseSub(mode, input, 3, expectedOutput3, 3);

    printf("\n----- testArpegPlayerINSIDE_OUT 2---\n");
    // this doesn't seem quite right.??
    const float expectedOutput2[] = {5, 4};
    testArpegPauseSub(mode, input, 2, expectedOutput2, 2);

    printf("\n----- testArpegPlayerINSIDE_OUT 1---\n");
    const float expectedOutput1[] = {5, 5, 5};
    testArpegPauseSub(mode, input, 1, expectedOutput1, 3);
}

static void testArpegPlayerOUTSIDE_IN() {
    const auto mode = ArpegPlayer::Mode::OUTSIDE_IN;
    const float input[] = {5, 4, 3, 2, 1};

    printf("\n----- testArpegPlayerOUTSIDE_IN 5---\n");
    const float expectedOutput5[] = {5, 1, 4, 2, 3};
    testArpegPauseSub(mode, input, 5, expectedOutput5, 5);

    printf("\n----- testArpegPlayerOUTSIDE_IN 4---\n");
    const float expectedOutput4[] = {5, 2, 4, 3};
    testArpegPauseSub(mode, input, 4, expectedOutput4, 4);

    printf("\n----- testArpegPlayerOUTSIDE_IN 3---\n");
    const float expectedOutput3[] = {5, 3, 4};
    testArpegPauseSub(mode, input, 3, expectedOutput3, 3);

    printf("\n----- testArpegPlayerOUTSIDE_IN 2---\n");
    const float expectedOutput2[] = {5, 4};
    testArpegPauseSub(mode, input, 2, expectedOutput2, 2);

    printf("\n----- testArpegPlayerOUTSIDE_IN 1---\n");
    const float expectedOutput1[] = {5, 5, 5};
    testArpegPauseSub(mode, input, 1, expectedOutput1, 3);
}

static void testArpegPlayerOrderPlayed() {
    const auto mode = ArpegPlayer::Mode::ORDER_PLAYED;
    const float input[] = {5, 4, 30, 2, 1};

    printf("\n----- testArpegPlayerORDER_PLAYED 5---\n");
    const float expectedOutput5[] = {5, 4, 30, 2, 1, 5, 4, 30, 2, 1};
    testArpegPauseSub(mode, input, 5, expectedOutput5, 10);

    printf("\n----- testArpegPlayerORDER_PLAYED 4---\n");
    const float expectedOutput4[] = {
        5,
        4,
        30,
        2,
        5,
        4,
        30,
        2,
    };
    testArpegPauseSub(mode, input, 4, expectedOutput4, 8);

    printf("\n----- testArpegPlayerORDER_PLAYEDN 3---\n");
    const float expectedOutput3[] = {5, 4, 30, 5, 4, 30};
    testArpegPauseSub(mode, input, 3, expectedOutput3, 6);

    printf("\n----- testArpegPlayerORDER_PLAYED 2---\n");
    const float expectedOutput2[] = {5, 4};
    testArpegPauseSub(mode, input, 2, expectedOutput2, 2);

    printf("\n----- testArpegPlayerORDER_PLAYED 1---\n");
    const float expectedOutput1[] = {5, 5, 5};
    testArpegPauseSub(mode, input, 1, expectedOutput1, 3);
}

static void testArpegPlayerREPEAT_BOTTOM() {
      const auto mode = ArpegPlayer::Mode::REPEAT_BOTTOM;
    const float input[] = {1, 2, 3, 4, 5};

    printf("\n----- testArpegPlayerREPEAT_BOTTOM 5---\n");
    const float expectedOutput5[] = {1, 2, 1, 3, 1, 4, 1, 5, 1, 2};
    testArpegPauseSub(mode, input, 5, expectedOutput5, 10);

    printf("\n----- testArpegPlayerREPEAT_BOTTOM 4---\n");
    const float expectedOutput4[] = {1, 2, 1, 3, 1, 4, 1, 2};
    testArpegPauseSub(mode, input, 4, expectedOutput4, 8);

    printf("\n----- testArpegPlayerREPEAT_BOTTOM 3---\n");
    const float expectedOutput3[] = {1, 2, 1, 3, 1, 2};
    testArpegPauseSub(mode, input, 3, expectedOutput3, 6);

    printf("\n----- testArpegPlayerREPEAT_BOTTOM 2---\n");
    const float expectedOutput2[] = {1, 2, 1, 2};
    testArpegPauseSub(mode, input, 2, expectedOutput2, 4);

    printf("\n----- testArpegPlayerREPEAT_BOTTOM 1---\n");
    const float expectedOutput1[] = {1, 1, 1};
    testArpegPauseSub(mode, input, 1, expectedOutput1, 3);
}

static void testArpegPlayerREPEAT_TOP() {
    const auto mode = ArpegPlayer::Mode::REPEAT_TOP;
    const float input[] = { 1, 2, 3, 4, 5 };

    printf("\n----- testArpegPlayerREPEAT_TOP 5---\n");
    const float expectedOutput5[] = { 5, 1, 5, 2, 5, 3, 5, 4, 5, 1, 5, 2 };
    testArpegPauseSub(mode, input, 5, expectedOutput5, 12);

    printf("\n----- testArpegPlayerREPEAT_TOP 4---\n");
    const float expectedOutput4[] = { 4, 1, 4, 2, 4, 3, 4, 1};
    testArpegPauseSub(mode, input, 4, expectedOutput4, 8);

    printf("\n----- testArpegPlayerREPEAT_TOP 3---\n");
    const float expectedOutput3[] = { 3, 1, 3, 2, 3, 1 };
    testArpegPauseSub(mode, input, 3, expectedOutput3, 6);

    printf("\n----- testArpegPlayerREPEAT_TOP 2---\n");
    const float expectedOutput2[] = { 2, 1, 2, 1 };
    testArpegPauseSub(mode, input, 2, expectedOutput2, 4);

    printf("\n----- testArpegPlayerREPEAT_TOP 1---\n");
    const float expectedOutput1[] = { 1, 1, 1 };
    testArpegPauseSub(mode, input, 1, expectedOutput1, 3);
}

static void testArpegPlayerSHUFFLE() {

    printf("\n----- testArpegPlayerSHUFFLE ---\n");
    const auto mode = ArpegPlayer::Mode::SHUFFLE;

    const int numInput = 12;
    const float input[numInput] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 };
    float outputFirst[numInput] = {0 };
    float outputSecond[numInput] = {0 };
    
    NoteBuffer nb(numInput + 4);
    ArpegPlayer ap(&nb);
    ap.setMode(mode);

    // put in the input chord
    for (int i = 0; i < numInput; ++i) {
        nb.push_back(input[i], i);
    }

    bool didSee[numInput] = {false};

    for (int i=0; i<numInput; ++i) {
        float x = ap.clock();
        assert(x > 0);
        assert(x < 13);
        outputFirst[i] = x;

        // now search the iput of this one
        for (int j=0; j<numInput; ++j) {
           // printf("in test loop, x = %f input=%f\n", x, input[j]);
            if (input[j] == x) {
                assert(!didSee[j]);
                didSee[j] = true;
            }
        }
    }
    for (int i = 0; i < numInput; ++i) {
        assert(didSee[i]);
        assert(outputFirst[i] > 0);
    }

    for (int i=0; i<numInput; ++i) {
        const float x = ap.clock();
        assert(x > 0);
        assert(x < 13);
        outputSecond[i] = x;
    }

    bool anyDifferent = false;
    for (int i = 0; i < numInput; ++i) {
        if (outputFirst[i] != outputSecond[i]) {
            anyDifferent = true;
        }
    }
    assert(anyDifferent);
}

void testArpegPlayer() {
    testArpegPlayerOneNote();
    testArpegPlayerTwoNotes();
    testArpegPlayerZeroNotes();

    testArpegPlayerUp();
    testArpegPlayerDown();
    testArpegPlayerUpDown();
    testArpegPlayerDownUp();
    testArpegPlayerUp_Down();
    testArpegPlayerDown_Up();
    testArpegPlayerINSIDE_OUT();
    testArpegPlayerOUTSIDE_IN();
    testArpegPlayerOrderPlayed();
    testArpegPlayerREPEAT_BOTTOM();
    testArpegPlayerREPEAT_TOP();
    testArpegPlayerSHUFFLE();
}