#include "Chord4.h"
#include "Chord4Manager.h"
#include "HarmonyChords.h"
#include "Keysig.h"
#include "Options.h"
#include "ProgressionAnalyzer.h"
#include "Style.h"

#include "asserts.h"
#include <random>

static StylePtr makeStyle() {
    return std::make_shared<Style>();
}

static KeysigPtr makeKeysig() {
    return std::make_shared<Keysig>(Roots::C);
}

static Options makeOptions() {
    Options o(makeKeysig(), makeStyle());
    return o;
}

static void testFirstChord() {
    auto options = makeOptions();
    Chord4Manager mgr(options);
    const int root = 1;
    auto next = HarmonyChords::findChord(false, options, mgr, root);
    assert(next);
}

static void testBasic1() {
    auto options = makeOptions();
    Chord4Manager mgr(options);
    auto cp = HarmonyChords::findChord(false, options, mgr, 1);

    const int root = 3;  // 1 to 3 progression should be legal
    auto next = HarmonyChords::findChord(false, options, mgr, *cp, root);
    assert(next);
}

static void testAtoB(int a, int b) {
    auto options = makeOptions();
    Chord4Manager mgr(options);
    auto cp = HarmonyChords::findChord(false, options, mgr, a);

    auto next = HarmonyChords::findChord(false, options, mgr, *cp, b);
    assert(next);
    assertEQ(next->penaltForFollowingThisGuy(options, *cp, false), 0);
}

static void testAtoBtoA(int a, int b) {
    auto options = makeOptions();
    Chord4Manager mgr(options);
    auto chordA = HarmonyChords::findChord(false, options, mgr, a);

    auto chordB = HarmonyChords::findChord(false, options, mgr, *chordA, b);
    assert(chordB);
    assertEQ(chordB->penaltForFollowingThisGuy(options, *chordA, false), 0);

    auto chordC = HarmonyChords::findChord(false, options, mgr, *chordA, *chordB, a);
    assert(chordC);
    assertEQ(chordC->penaltForFollowingThisGuy(options, *chordB, false), 0);
}

static void test1to2to1() {
    printf("** test 1 to 2 to 1\n");
    testAtoBtoA(1, 2);
}

static void testBasic2() {
    testAtoB(1, 3);
}

static void test3to5() {
    testAtoB(3, 5);
}

static void test5to4() {
    testAtoB(5, 4);
}

static void test5to6() {
    testAtoB(5, 6);
}

static void test1to2() {
    testAtoB(1, 2);
}

static void test2to1() {
    // failing when first chord is D2A2F3A3
    // new doubling probably makes this possible, but we should be able to recover, even so.
    printf("\n-- test 2 to 1 --\n");
  //  ProgressionAnalyzer::showAnalysis();
    testAtoB(2, 1);
}


static void test2to1a() {
    printf("\n-- test 2 to 1 a --\n");
    // We are having problems with this simple 2-1 progression
    auto options = makeOptions();
    Chord4Manager mgr(options);
    auto chordA = Chord4::fromString(options, 2, "D2A2F3A3");
    assert(chordA);

    auto next = HarmonyChords::findChord(false, options, mgr, *chordA, 1);
    assert(next);
    assertEQ(next->penaltForFollowingThisGuy(options, *chordA, false), 0);
}

static void test2to1b() {
    printf("\n-- test 2 to 1 b --\n");
    // We are having problems with this simple 2-1 progression
    auto options = makeOptions();
    Chord4Manager mgr(options);
    auto chordA = Chord4::fromString(options, 2, "D2A2D3F3");
    assert(chordA);

    auto next = HarmonyChords::findChord(false, options, mgr, *chordA, 1);
    assert(next);
    std::string x = next->toString();
    printf("125 found %s\n", x.c_str());
    // I think new algorithm gives 90, but should check.
    assertEQ(next->penaltForFollowingThisGuy(options, *chordA, true), 0);
}

static void testFeb21Case() {
    printf("\n--- testFeb21Case -- \n");

    auto options = makeOptions();
    Chord4Manager mgr(options);
    // make special 5 chord, second in version from test case
    auto chordA = Chord4::fromString(options, 5, "D2B3D4G4");
    assert(chordA);

    auto next = HarmonyChords::findChord(false, options, mgr, *chordA, 6);
    assert(next);
    std::string x = next->toString();
    printf("found %s\n", x.c_str());

    auto penalty = next->penaltForFollowingThisGuy(options, *chordA, true);

    assert(false);

}


static void testThreeSequence() {
    auto options = makeOptions();
    Chord4Manager mgr(options);

    auto firstChord = HarmonyChords::findChord(false, options, mgr, 1);

    const int secondRoot = 3;
    auto secondChord = HarmonyChords::findChord(false, options, mgr, *firstChord, secondRoot);
    assert(secondChord);
    assertEQ(secondChord->penaltForFollowingThisGuy(options, *firstChord, false), 0);

    const int thirdRoot = 5;
    auto thirdChord = HarmonyChords::findChord(false, options, mgr, *firstChord, *secondChord, thirdRoot);
    assert(thirdChord);
    assertEQ(thirdChord->penaltForFollowingThisGuy(options, *secondChord, false), 0);
}

static void testXtoYtoX() {
    auto options = makeOptions();
    Chord4Manager mgr(options);

    for (int rootX = 1; rootX <= 7; ++rootX) {
        auto chordX = HarmonyChords::findChord(false, options, mgr, rootX);
        assert(chordX);
        for (int rootY = 1; rootY <= 7; ++rootY) {
            if (rootX != rootY) {
                printf("** in test xyx rootx = % d rooty = % d\n", rootX, rootY);
                auto chordY = HarmonyChords::findChord(false, options, mgr, *chordX, rootY);
                assert(chordY);
                assertEQ(chordY->penaltForFollowingThisGuy(options, *chordX, false), 0);
                auto thirdChord = HarmonyChords::findChord(false, options, mgr, *chordX, *chordY, rootX);
                assert(thirdChord);
                assertEQ(thirdChord->penaltForFollowingThisGuy(options, *chordX, false), 0);

                assert(!(*chordX == *thirdChord));
            }
        }
    }
}

static void testXtoY() {
    auto options = makeOptions();
    Chord4Manager mgr(options);

    for (int rootX = 1; rootX <= 7; ++rootX) {
        auto chordX = HarmonyChords::findChord(false, options, mgr, rootX);
        assert(chordX);
        for (int rootY = 1; rootY <= 7; ++rootY) {
            if (rootX != rootY) {
                printf("** in test xyx rootx = % d rooty = % d\n", rootX, rootY);
                auto chordY = HarmonyChords::findChord(false, options, mgr, *chordX, rootY);
                assert(chordY);
                assertEQ(chordY->penaltForFollowingThisGuy(options, *chordX, false), 0);
            }
        }
    }
}

static int genRandomRoot() {
    static std::mt19937 generator;
    static std::uniform_real_distribution<float> distribution{0, 1.0};

    const float f = distribution(generator);
    int i = int(7 * f);
    printf("rand = %d\n", i);
    return i; 
    //return  distribution(generator)
}

void testRand() {
    auto options = makeOptions();
    Chord4Manager mgr(options);

    int rootA = genRandomRoot();
    auto chordA =  HarmonyChords::findChord(false, options, mgr, rootA);
    int rootB = rootA;
    while (rootB == rootA) {
        rootB = genRandomRoot();
    }
    auto chordB = HarmonyChords::findChord(false, options, mgr, *chordA, genRandomRoot());
    assert(chordB);

    for (int i=0; i<1000; ++i) {
        int rootC = rootB;
        while(rootC == rootB) {
            rootC = genRandomRoot();
        }
        auto thirdChord = HarmonyChords::findChord(false, options, mgr, *chordA, *chordB, rootC);
        assert(thirdChord);

        chordA = chordB;
        chordB = thirdChord;
        rootB = rootC;
    }
}

void testHarmonyChords() {
    testFirstChord();
    testBasic1();
    testBasic2();
    test3to5();
    printf("\n--- test 5 to 4 ----\n");
    test5to4();
    printf("\n--- test 5 to 6 ----\n");
    test5to6();
    test1to2();

    testFeb21Case();

    test2to1();
    printf("put back 2 1 test\n");
   // test2to1a();
    test2to1b();
  
    test1to2to1();
    printf("--- test 3 ----\n");
    printf("put back 3 seq\n");
    // testThreeSequence();
    // testXtoY();
    printf("--- test xyx ----\n");
    // testXtoYtoX();
    printf("--- test rand ---\n");
    // testRand();
}