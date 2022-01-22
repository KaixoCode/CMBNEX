#pragma once
#include "pch.hpp"
#include "myplugincids.hpp"
#include "Modules.hpp"
#include "Knob.hpp"
#include "Label.hpp"
#include "WaveformView.hpp"

namespace Kaixo
{
    class SubOscView : public CViewContainer
    {
    public:
        Knob* gain; // Tune
        Knob* octv; // Pos
        Knob* ovrt; // Pos
        Label* titl;

        SubOscView(const CRect& size, IControlListener* listener, MyEditor* editor)
            : CViewContainer(size)
        {
            setBackgroundColor({ 0, 0, 0, 0 });
            addView(new BackgroundEffect{ { 0, 0, getWidth(), getHeight() } });

            gain = new Knob{ {  10,   5,  10 + 65,   5 + 100 }, editor };
            octv = new Knob{ {   5, 112,   5 + 60, 112 + 50 }, editor };
            ovrt = new Knob{ {   5, 155,   5 + 60, 155 + 50 }, editor };

            octv->modable = false;

            //titl = new Label{ { 0,   3, 0 + 85,   3 + 30 } };
            //titl->fontsize = 24;
            //titl->center = true;
            //titl->value = "S U B";

            gain->setListener(listener);
            octv->setListener(listener);
            ovrt->setListener(listener);

            gain->setTag(Params::SubGain);
            octv->setTag(Params::SubOct);
            ovrt->setTag(Params::SubOvertone);

            gain->name = "Sub";  octv->name = "Octave"; ovrt->name = "Overtone";
            gain->min = 0;       octv->min = -2;        ovrt->min = 0;
            gain->max = 1;       octv->max = 2;         ovrt->max = 100;
            gain->reset = 0;     octv->reset = 0;       ovrt->reset = 0;
            gain->decimals = 1;  octv->decimals = 0;    ovrt->decimals = 1;
            gain->unit = " dB";  octv->unit = "";       ovrt->unit = " %";
            gain->type = 0;      octv->type = 2;        ovrt->type = 2;

            addView(gain);
            addView(octv);
            addView(ovrt);
            //addView(titl);
        }

        ~SubOscView()
        {
            gain->forget();
            octv->forget();
            ovrt->forget();
        }
    };
}