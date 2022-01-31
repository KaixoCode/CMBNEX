#include "Views/SubOscView.hpp"

namespace Kaixo
{
    SubOscView::SubOscView(const CRect& size, IControlListener* listener, MyEditor* editor)
        : CViewContainer(size)
    {
        setBackgroundColor({ 0, 0, 0, 0 });
        addView(new BackgroundEffect{ {.size = { 0, 0, getWidth(), getHeight() } } });

        gain = new Parameter{ {
            .tag = Params::SubGain,
            .editor = editor,
            .size = {  10,   5,  10 + 65,   5 + 100 },
            .type = Parameter::KNOB, .name = "Sub",
            .min = 0, .max = 1, .reset = 0, .decimals = 1,
            .unit = " dB",
        } };

        octv = new Parameter{ {
            .tag = Params::SubOct,
            .editor = editor,
            .size = {   5, 112,   5 + 60, 112 + 50 },
            .type = Parameter::NUMBER, .modable = false, .name = "Octave",
            .min = -2, .max = 2, .reset = 0, .decimals = 0,
            .unit = "",
        } };

        ovrt = new Parameter{ {
            .tag = Params::SubOvertone,
            .editor = editor,
            .size = {   5, 155,   5 + 60, 155 + 50 },
            .type = Parameter::NUMBER, .name = "Overtone",
            .min = 0, .max = 100, .reset = 0, .decimals = 1,
            .unit = " %",
        } };

        addView(gain);
        addView(octv);
        addView(ovrt);
    }

    SubOscView::~SubOscView()
    {
        gain->forget();
        octv->forget();
        ovrt->forget();
    }
}