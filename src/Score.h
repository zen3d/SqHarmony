#pragma once

#include <assert.h>
#include <stdio.h>

#include <codecvt>
#include <locale>
#include <string>

#include "Harmony1Module.h"
#include "plugin.hpp"

using FontPtr = std::shared_ptr<Font>;
class Harmony1Module;

// #define _TESTCHORD

class Score : public app::LightWidget {
public:
    Score(Harmony1Module *);
    void draw(const DrawArgs &args) override;
    void step() override;

    //  using Comp = Harmony1Module;
    void onNewChord(Comp::Chord newChord);

private:
    void filledRect(NVGcontext *vg, NVGcolor color, float x, float y, float w, float h) const;
    void drawHLine(NVGcontext *vg, NVGcolor color, float x, float y, float length, float width) const;
    void drawVLine(NVGcontext *vg, NVGcolor color, float x, float y, float length, float width) const;

    // FontPtr loadFont() const;
    void prepareFontMusic(const DrawArgs &args) const;
    void prepareFontText(const DrawArgs &args) const;
    float noteXPos(int noteNumber) const;

    class YInfo {
    public:
        float position = 200;
        float ledgerPos[3] = {};
    };
    /**
     * @brief get info about vertical placement
     * 
     * @param note 
     * @param bassStaff 
     * @return std::pair<float, bool> first is the y position, second it flag if need ledger line
     */
    YInfo noteYInfo(const MidiNote &note, bool bassStaff) const;

    Harmony1Module *const module;
    std::list<Comp::Chord> chords;

    const std::string noteQuarterUp = u8"\ue1d5";
    const std::string noteQuarterDown = u8"\ue1d6";
    const std::string staffFiveLines = u8"\ue014";
    const std::string gClef = u8"\ue050";
    const std::string fClef = u8"\ue062";
    const std::string ledgerLine = u8"\ue022";

    void drawStaff(const DrawArgs &args, float y);
    void drawBarLine(const DrawArgs &args, float x, float y);
    void drawChordInfo(const DrawArgs &args, float x, const Comp::Chord &chord);
    void drawMusic(const DrawArgs &args);
    void drawText(const DrawArgs &args);

    // Y axis pos
    const float topMargin = 36.5f;
    const float yTrebleStaff = topMargin + 0;
    const float yBassStaff = yTrebleStaff + 42;  // 28 way too close
    const float yTrebleClef = yTrebleStaff - 3.3;  // 3 a little low, 4 way high
    const float yBassClef = yBassStaff - 10;     // 11 too much
    const float yNoteInfo = yBassStaff + 12;     // 0 too high

    // X axis pos
    const float leftMargin = 5.5f;
    const float xStaff = leftMargin;
    const float xClef = xStaff + 2;
    const float xNote0 = xClef + 18;

    const float deltaXNote = 10;            // 8 seemed close
    const float spaceBetweenLines = 1.67f;  // 1.7 slightly high
                                            // 1.65 low
    const float barlineHeight = 55.5;       // 55 low
                                            // 57 went  high

    //  const float barlineX0 = leftMargin;
    const float barlineX1 = xClef + 61;  //  small 63 big
    //const float barlineX2 = barlineX1 + 43;     // 40 too small 46 too big
};

inline Score::Score(Harmony1Module *m) : module(m) {
#ifdef _TESTCHORD
    Comp::Chord ch;

    ch.pitch[0] = 0;
    ch.pitch[1] = 0;
    ch.pitch[2] = 0;
    ch.pitch[3] = 0;


    // bass
    ch.pitch[0] = MidiNote::MiddleC - (12);
    ch.pitch[1] = MidiNote::MiddleC;
    ch.pitch[2] = MidiNote::MiddleC + 4;
    ch.pitch[3] = MidiNote::MiddleC + 8;
    chords.push_back(ch);
   
#endif
}

inline void Score::step() {
#ifndef _TESTCHORD
    if (module) {
        if (module->isChordAvailable()) {
            auto ch = module->getChord();
            chords.push_back(ch);
            if (chords.size() > 8) {
                chords.pop_front();
            }
        }
    }
#endif
    LightWidget::step();
}

inline float Score::noteXPos(int noteNumber) const {
    float x = xNote0 + noteNumber * deltaXNote;
    if (noteNumber > 3) {
        x += deltaXNote;
    }
    return x;
}

inline Score::YInfo Score::noteYInfo(const MidiNote &note, bool bassStaff) const {
    YInfo ret;
    if (note.get() < 10) {
        return ret;
    }

    float y = 0;

    const int ledgerLine = note.getLedgerLine(bassStaff);
    const float staffBasePos = bassStaff ? yBassStaff : yTrebleStaff;

    if (ledgerLine < -1) {
        ret.ledgerPos[0] = staffBasePos + (2.f * spaceBetweenLines);
    }
    if (ledgerLine < -3) {
        ret.ledgerPos[1] = staffBasePos + (4.f * spaceBetweenLines);
    }
    if (ledgerLine < -5) {
        ret.ledgerPos[2] = staffBasePos + (6.f * spaceBetweenLines);
    }
    if (ledgerLine > 9) {
        ret.ledgerPos[0] = staffBasePos + (-10.f * spaceBetweenLines);
    }
    if (ledgerLine > 11) {
        ret.ledgerPos[1] = staffBasePos + (-12.f * spaceBetweenLines);
    }
    if (ledgerLine > 13) {
        ret.ledgerPos[2] = staffBasePos + (-14.f * spaceBetweenLines);
    }

    y -= ledgerLine * spaceBetweenLines;
    y += staffBasePos;

#if 0
    printf("notYInfo p=%d, bass=%d ll=%d lp0=%f 1=%f 2=%f\n",
        note.get(),
        bassStaff,
        ledgerLine,
        ret.ledgerPos[0],
        ret.ledgerPos[1],
        ret.ledgerPos[2]
        );
#endif

    ret.position = y;
    return ret;
}

inline void Score::draw(const DrawArgs &args) {
    nvgScissor(args.vg, RECT_ARGS(args.clipBox));
    drawMusic(args);
    drawText(args);
}

inline void Score::drawText(const DrawArgs &args) {
    if (chords.empty()) {
        return;
    }
    prepareFontText(args);
    int i = 0;
    for (auto chord : chords) {
        const float x = noteXPos(i);
        drawChordInfo(args, x, chord);
        ++i;
    }
}

inline void Score::drawMusic(const DrawArgs &args) {
    NVGcolor color = nvgRGB(0, 0, 0);

    // TODO : USE SIZE
    filledRect(args.vg, color, 0, 0, 200, 200);

    prepareFontMusic(args);

    color = nvgRGB(0xff, 0xff, 0xff);
    // Background text
    nvgFillColor(args.vg, color);

    drawStaff(args, yTrebleStaff);
    nvgText(args.vg, xClef, yTrebleClef, gClef.c_str(), NULL);

    drawStaff(args, yBassStaff);
    nvgText(args.vg, xClef, yBassClef, fClef.c_str(), NULL);

    if (!chords.empty()) {
        // INFO("chords to score %d", int(chords.size()));
        int i = 0;
        for (auto chord : chords) {
            const float x = noteXPos(i);

            for (int i = 0; i < 4; ++i) {
                const bool stemUp = i % 2;
                auto yInfo = noteYInfo(chord.pitch[i], i < 2);

                for (int i = 0; i < 3; ++i) {
                    if (yInfo.ledgerPos[i] != 0) {
                        // printf("drawing ledger at %f\n", yInfo.ledgerPos[i]);
                        nvgText(args.vg, x, yInfo.ledgerPos[i], ledgerLine.c_str(), NULL);
                    }
                }
                const char *note = stemUp ? noteQuarterUp.c_str() : noteQuarterDown.c_str();
                // printf("drawing note at %f\n", yInfo.position);
                nvgText(args.vg, x, yInfo.position, note, NULL);
            }
            ++i;
        }
    }

    drawBarLine(args, xStaff, yBassStaff);
    drawBarLine(args, barlineX1, yBassStaff);

    const float barlineX2 = args.clipBox.size.x - leftMargin;
    drawBarLine(args, barlineX2, yBassStaff);

    TransparentWidget::draw(args);
}

inline void Score::drawChordInfo(const DrawArgs &args, float x, const Comp::Chord &chord) {
    {
        std::stringstream s;
        s << chord.root;
        nvgText(args.vg, x, yNoteInfo, s.str().c_str(), NULL);
    }
    {
        std::stringstream s;
        s << chord.inversion;
        nvgText(args.vg, x, yNoteInfo+8, s.str().c_str(), NULL);
    }
#if 0
    std::stringstream s;
    s << chord.root;
    s << ":";
    s << chord.inversion;
    INFO("draw into at %f, %f, %s", x, yNoteInfo, s.str().c_str());
    nvgText(args.vg, x, yNoteInfo, s.str().c_str(), NULL);
#endif
}

inline void Score::drawStaff(const DrawArgs &args, float yBase) {
    const float x = xStaff;
    const float length = args.clipBox.size.x - 2 * leftMargin;
    auto color = nvgRGB(0xff, 0xff, 0xff);
    for (int i = 0; i < 5; ++i) {
        float y = yBase - 2.f * float(i) * spaceBetweenLines;
        drawHLine(args.vg, color, x, y, length, .5f);
    }
}

inline void Score::drawBarLine(const DrawArgs &args, float x, float y) {
    auto color = nvgRGB(0xff, 0xff, 0xff);
    drawVLine(args.vg, color, x, y, barlineHeight, .5f);
}

void Score::drawVLine(NVGcontext *vg, NVGcolor color, float x, float y, float length, float width) const {
    nvgBeginPath(vg);
    nvgMoveTo(vg, x, y);
    nvgLineTo(vg, x, y - length);
    nvgStrokeColor(vg, color);
    nvgStrokeWidth(vg, width);
    nvgStroke(vg);
    nvgClosePath(vg);
}

void Score::drawHLine(NVGcontext *vg, NVGcolor color, float x, float y, float length, float width) const {
    nvgBeginPath(vg);
    nvgMoveTo(vg, x, y);
    nvgLineTo(vg, x + length, y);
    nvgStrokeColor(vg, color);
    nvgStrokeWidth(vg, width);
    nvgStroke(vg);
    nvgClosePath(vg);
}

void Score::filledRect(NVGcontext *vg, NVGcolor color, float x, float y, float w, float h) const {
    nvgFillColor(vg, color);
    nvgBeginPath(vg);
    nvgRect(vg, x, y, w, h);
    nvgFill(vg);
}

void Score::prepareFontMusic(const DrawArgs &args) const {
    std::string fontPath("res/");
    fontPath += "Bravura.otf";
    // Get font
    std::shared_ptr<Font> font = APP->window->loadFont(asset::plugin(pluginInstance, fontPath.c_str()));
    if (!font) {
        printf("font didn't load\n");
        return;
    }
    nvgFontFaceId(args.vg, font->handle);
    nvgFontSize(args.vg, 54);
}

void Score::prepareFontText(const DrawArgs &args) const {
    // int f = APP->window->uiFont->handle;
    nvgFontFaceId(args.vg, APP->window->uiFont->handle);
    nvgFontSize(args.vg, 9);
}
