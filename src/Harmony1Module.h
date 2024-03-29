
#pragma once

#include "Harmony.h"
#include "WidgetComposite.h"
#include "plugin.hpp"

using Comp = Harmony<WidgetComposite>;

class Harmony1Module : public Module {
public:
    std::shared_ptr<Comp> comp = std::make_shared<Comp>(this);

    Harmony1Module() {
        config(Comp::NUM_PARAMS, Comp::NUM_INPUTS, Comp::NUM_OUTPUTS, Comp::NUM_LIGHTS);
        // configParam(PITCH_PARAM, 0, 10, 4, "Initial Pitch");
        /*
            BASS_OUTPUT,
        TENOR_OUTPUT,
        ALTO_OUTPUT,
        SOPRANO_OUTPUT,
        */
        this->configOutput(Comp::BASS_OUTPUT, "Bass voice pitch");
        this->configOutput(Comp::TENOR_OUTPUT, "Tenor voice pitch");
        this->configOutput(Comp::ALTO_OUTPUT, "Alto voice pitch");
        this->configOutput(Comp::SOPRANO_OUTPUT, "Soprano voice pitch");

        this->configInput(Comp::CV_INPUT, "Chord root scale degee");
    }

    using Chord = Comp::Chord;
    bool isChordAvailable() const { return comp->isChordAvailable(); }
    Chord getChord() { return comp->getChord(); }

    void process(const ProcessArgs& args) override {
        comp->process(args);
    }
};
