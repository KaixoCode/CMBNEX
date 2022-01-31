#include "Views/MidiView.hpp"
namespace Kaixo
{
    MidiView::MidiView(const CRect& size, IControlListener* listener, MyEditor* editor)
        : CViewContainer(size)
    {
        setBackgroundColor({ 0, 0, 0, 0 });
        addView(new BackgroundEffect{ {.size = { 0, 0, getWidth(), getHeight() } } });

        tnsp = new Parameter{ {
            .tag = Params::Transpose,
            .editor = editor,
            .size = { 135,  5, 135 + 65,  5 + 50 },
            .type = Parameter::NUMBER, .name = "Transp",
            .min = -48, .max = 48, .reset = 0, .decimals = 1,
            .unit = " st",
        } };

        bend = new Parameter{ {
            .tag = Params::Bend,
            .editor = editor,
            .size = { 200,  5, 200 + 65,  5 + 50 },
            .type = Parameter::NUMBER, .name = "Bend",
            .min = 0, .max = 24, .reset = 2, .decimals = 1,
            .unit = " st",
        } };

        gain = new Parameter{ {
            .tag = Params::GlobalGain,
            .editor = editor,
            .size = { 265,  5, 265 + 65,  5 + 50 },
            .type = Parameter::NUMBER, .name = "Gain",
            .min = 0, .max = 2, .reset = 1, .decimals = 1,
            .unit = " dB"
        } };

        glde = new Parameter{ {
            .tag = Params::Glide,
            .editor = editor,
            .size = {  70,  5,  70 + 65,  5 + 50 },
            .type = Parameter::NUMBER, .name = "Glide",
            .min = 0, .max = 10000, .reset = 0, .decimals = 1,
            .unit = " s",
        } };

        time = new Parameter{ {
            .tag = Params::Time,
            .editor = editor,
            .size = {   5,  5,   5 + 70,  5 + 50 },
            .type = Parameter::NUMBER, .name = "Time",
            .min = -100, .max = 100, .reset = 0, .decimals = 1,
            .unit = " %",
        } };

        clip = new Parameter{ {
            .tag = Params::Oversample,
            .editor = editor,
            .size = {  205, 84 + 85 + 19,  205 + 128, 84 + 18 + 85 + 19 },
            .type = Parameter::GROUP, .parts = { "1x", "2x", "4x", "8x"},
            .padding = 4
        } };

        thrd = new Parameter{ {
            .tag = Params::Threading,
            .editor = editor,
            .size = { 269, 84 + 82,  269 + 60, 84 + 18 + 82 },
            .type = Parameter::BUTTON, .name = "Threads"
        } };

        retr = new Parameter{ {
            .tag = Params::Retrigger,
            .editor = editor,
            .size = { 140, 84 + 82,  140 + 60, 84 + 18 + 82 },
            .type = Parameter::BUTTON, .name = "Retrigger"
        } };

        poly = new Parameter{ {
            .tag = Params::Voices,
            .editor = editor,
            .size = { 140, 84 + 82 + 22,  140 + 60, 84 + 18 + 82 + 22 },
            .type = Parameter::BUTTON, .name = "Polphon"
        } };

        mac1 = new Parameter{ {
            .tag = Params::Macro1,
            .editor = editor,
            .size = {   5, 63,   5 + 65, 63 + 88 + 10 },
            .type = Parameter::KNOB, .name = "Macro P",
            .min = 0, .max = 100, .reset = 0, .decimals = 1,
            .unit = " %"
        } };

        mac2 = new Parameter{ {
            .tag = Params::Macro2,
            .editor = editor,
            .size = {  70, 63,  70 + 65, 63 + 88 + 10 },
            .type = Parameter::KNOB, .name = "Macro Q",
            .min = 0, .max = 100, .reset = 0, .decimals = 1,
            .unit = " %"
        } };

        mac3 = new Parameter{ {
            .tag = Params::Macro3,
            .editor = editor,
            .size = { 135, 63, 135 + 65, 63 + 88 + 10 },
            .type = Parameter::KNOB, .name = "Macro R",
            .min = 0, .max = 100, .reset = 0, .decimals = 1,
            .unit = " %"
        } };

        mac4 = new Parameter{ {
            .tag = Params::Macro4,
            .editor = editor,
            .size = { 200, 63, 200 + 65, 63 + 88 + 10 },
            .type = Parameter::KNOB, .name = "Macro S",
            .min = 0, .max = 100, .reset = 0, .decimals = 1,
            .unit = " %"
        } };

        mac5 = new Parameter{ {
            .tag = Params::Macro5,
            .editor = editor,
            .size = { 265, 63, 265 + 65, 63 + 88 + 10 },
            .type = Parameter::KNOB, .name = "Macro T",
            .min = 0, .max = 100, .reset = 0, .decimals = 1,
            .unit = " %"
        } };

        mcd1 = new DragThing{ {   5, 64,   5 + 60, 64 + 15 } };
        mcd2 = new DragThing{ {  70, 64,  70 + 60, 64 + 15 } };
        mcd3 = new DragThing{ { 135, 64, 135 + 60, 64 + 15 } };
        mcd4 = new DragThing{ { 200, 64, 200 + 60, 64 + 15 } };
        mcd5 = new DragThing{ { 265, 64, 265 + 60, 64 + 15 } };

        keym = new DragThing{ {  5, 67 + 85 + 17,   5 + 60, 67 + 15 + 85 + 17 } };
        velm = new DragThing{ {  5, 88 + 85 + 17,   5 + 60, 88 + 15 + 85 + 17 } };
        ranm = new DragThing{ {  5 + 65, 67 + 85 + 17,   5 + 65 + 60, 67 + 15 + 85 + 17 } };
        modm = new DragThing{ {  5 + 65, 88 + 85 + 17,   5 + 65 + 60, 88 + 15 + 85 + 17 } };

        keym->source = ModSources::Key;
        velm->source = ModSources::Vel;
        ranm->source = ModSources::Ran;
        modm->source = ModSources::Mod;

        mcd1->source = ModSources::Mac1;
        mcd2->source = ModSources::Mac2;
        mcd3->source = ModSources::Mac3;
        mcd4->source = ModSources::Mac4;
        mcd5->source = ModSources::Mac5;

        keyl = new Label{ {
            .size = { 11, 66 + 85 + 17,  11 + 60, 66 + 20 + 85 + 17 },
            .value = "Key", .center = false, .fontsize = 14,
        } };

        vell = new Label{ {
            .size = { 11, 87 + 85 + 17,  11 + 60, 87 + 20 + 85 + 17 },
            .value = "Velocity", .center = false, .fontsize = 14,
        } };

        ranl = new Label{ {
            .size = { 11 + 65, 66 + 85 + 17,  11 + 65 + 60, 66 + 20 + 85 + 17 },
            .value = "Random", .center = false, .fontsize = 14,
        } };

        modl = new Label{ {
            .size = { 11 + 65, 87 + 85 + 17,  11 + 65 + 60, 87 + 20 + 85 + 17 },
            .value = "Mod", .center = false, .fontsize = 14,
        } };

        clpl = new Label{ {
            .size = {  206, 64 + 85 + 17,  206 + 65, 64 + 20 + 85 + 17 },
            .value = "Quality", .center = false, .fontsize = 14,
        } };

        addView(tnsp); addView(bend); addView(glde); addView(gain); addView(retr); addView(poly);
        addView(time); addView(clip); addView(mac1); addView(mac2); addView(mac3); addView(mac4);
        addView(mac5); addView(mcd1); addView(mcd2); addView(mcd3); addView(mcd4); addView(mcd5);
        addView(clpl); addView(keyl); addView(vell); addView(keym); addView(thrd);
        addView(velm); addView(ranl); addView(modl); addView(ranm); addView(modm);
    }

    MidiView::~MidiView()
    {
        tnsp->forget(); bend->forget(); glde->forget(); gain->forget();
        time->forget(); clip->forget(); mac1->forget(); mac2->forget();
        mac3->forget(); mac4->forget(); mac5->forget(); thrd->forget();
        retr->forget(); poly->forget();
    }
}