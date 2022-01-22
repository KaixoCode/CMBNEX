#pragma once
#include "pch.hpp"
#include "myplugincids.hpp"
#include "Modules.hpp"
#include "Knob.hpp"
#include "Label.hpp"
#include "WaveformView.hpp"

namespace Kaixo
{
    class MidiView : public CViewContainer
    {
    public:
        Knob* tnsp = nullptr;
        Knob* bend = nullptr;
        Knob* glde = nullptr;
        Knob* retr = nullptr;
        Knob* ovsm = nullptr;
        Knob* pann = nullptr;
        Knob* time = nullptr;
        Knob* clip = nullptr;

        Knob* mac1 = nullptr;
        Knob* mac2 = nullptr;
        Knob* mac3 = nullptr;
        Knob* mac4 = nullptr;
        Knob* mac5 = nullptr;

        DragThing* mcd1;
        DragThing* mcd2;
        DragThing* mcd3;
        DragThing* mcd4;
        DragThing* mcd5;

        DragThing* keym;
        DragThing* velm;
    
        Label* ovsl;
        Label* rtrl;
        Label* clpl;

        Label* keyl;
        Label* vell;

        MidiView(const CRect& size, IControlListener* listener, MyEditor* editor)
            : CViewContainer(size)
        {
            setBackgroundColor({ 0, 0, 0, 0 });
            addView(new BackgroundEffect{ { 0, 0, getWidth(), getHeight() } });

            tnsp = new Knob{ { 135,  5, 135 + 65,  5 + 50 }, editor };
            bend = new Knob{ { 200,  5, 200 + 65,  5 + 50 }, editor };
            pann = new Knob{ { 265,  5, 265 + 65,  5 + 50 }, editor };
            glde = new Knob{ {  70,  5,  70 + 65,  5 + 50 }, editor };
            time = new Knob{ {   5,  5,   5 + 70,  5 + 50 }, editor };
            clip = new Knob{ {  70, 84 + 85 + 17,  70 + 128, 84 + 20 + 85 + 17 }, editor };
            retr = new Knob{ { 200, 84 + 85 + 17, 200 + 20, 84 + 20 + 85 + 17 }, editor };
            ovsm = new Knob{ { 225, 84 + 85 + 17, 225 + 108, 84 + 20 + 85 + 17 }, editor };

            mac1 = new Knob{ {   5, 63,   5 + 65, 63 + 88 + 10 }, editor };
            mac2 = new Knob{ {  70, 63,  70 + 65, 63 + 88 + 10 }, editor };
            mac3 = new Knob{ { 135, 63, 135 + 65, 63 + 88 + 10 }, editor };
            mac4 = new Knob{ { 200, 63, 200 + 65, 63 + 88 + 10 }, editor };
            mac5 = new Knob{ { 265, 63, 265 + 65, 63 + 88 + 10 }, editor };

            mcd1 = new DragThing{ {   5, 64,   5 + 60, 64 + 15 } };
            mcd2 = new DragThing{ {  70, 64,  70 + 60, 64 + 15 } };
            mcd3 = new DragThing{ { 135, 64, 135 + 60, 64 + 15 } };
            mcd4 = new DragThing{ { 200, 64, 200 + 60, 64 + 15 } };
            mcd5 = new DragThing{ { 265, 64, 265 + 60, 64 + 15 } };

            keym = new DragThing{ {  5, 67 + 85 + 17,   5 + 60, 67 + 15 + 85 + 17 } };
            velm = new DragThing{ {  5, 88 + 85 + 17,   5 + 60, 88 + 15 + 85 + 17 } };

            keym->source = ModSources::Key;
            velm->source = ModSources::Vel;

            mcd1->source = ModSources::Mac1;
            mcd2->source = ModSources::Mac2;
            mcd3->source = ModSources::Mac3;
            mcd4->source = ModSources::Mac4;
            mcd5->source = ModSources::Mac5;

            keyl = new Label{ { 11, 66 + 85 + 17,  11 + 60, 66 + 20 + 85 + 17 } };
            keyl->fontsize = 14;
            keyl->center = false;
            keyl->value = "Key";

            vell = new Label{ { 11, 87 + 85 + 17,  11 + 60, 87 + 20 + 85 + 17 } };
            vell->fontsize = 14;
            vell->center = false;
            vell->value = "Velocity";

            ovsl = new Label{ { 226, 64 + 85 + 17, 226 + 90, 64 + 20 + 85 + 17 } };
            ovsl->fontsize = 14;
            ovsl->center = false;
            ovsl->value = "Oversample";

            clpl = new Label{ {  71, 64 + 85 + 17,  71 + 65, 64 + 20 + 85 + 17 } };
            clpl->fontsize = 14;
            clpl->center = false;
            clpl->value = "Clipping";

            rtrl = new Label{ { 201, 64 + 85 + 17, 201 + 20, 64 + 20 + 85 + 17 } };
            rtrl->fontsize = 14;
            rtrl->center = false;
            rtrl->value = "Rtr";

            tnsp->setListener(listener);
            bend->setListener(listener);
            glde->setListener(listener);
            retr->setListener(listener);
            ovsm->setListener(listener);
            pann->setListener(listener);
            time->setListener(listener);
            clip->setListener(listener);
            mac1->setListener(listener);
            mac2->setListener(listener);
            mac3->setListener(listener);
            mac4->setListener(listener);
            mac5->setListener(listener);

            tnsp->setTag(Params::Transpose);
            bend->setTag(Params::Bend);
            glde->setTag(Params::Glide);
            retr->setTag(Params::Retrigger);
            ovsm->setTag(Params::Oversample);
            pann->setTag(Params::Panning);
            time->setTag(Params::Time);
            clip->setTag(Params::Clipping);
            mac1->setTag(Params::Macro1);
            mac2->setTag(Params::Macro2);
            mac3->setTag(Params::Macro3);
            mac4->setTag(Params::Macro4);
            mac5->setTag(Params::Macro5);

            tnsp->name = "Transp";    bend->name = "Bend";
            tnsp->min = -48;          bend->min = 0;
            tnsp->max = 48;           bend->max = 24;
            tnsp->reset = 0;          bend->reset = 2;
            tnsp->decimals = 1;       bend->decimals = 1;
            tnsp->unit = " st";       bend->unit = " st";
            tnsp->type = 2;           bend->type = 2;

            glde->name = "Glide";
            glde->min = 0;
            glde->max = 10000;
            glde->reset = 0;
            glde->decimals = 1;
            glde->unit = " s";
            glde->type = 2;

            retr->name = "R";        ovsm->name = "Off,2x,4x,8x";
            retr->min = 0;           ovsm->min = 4;
            retr->max = 100;         ovsm->max = 4;
            retr->reset = 0;         ovsm->reset = 1;
            retr->decimals = 1;      ovsm->decimals = 1;
            retr->unit = "";         ovsm->unit = "";
            retr->type = 3;          ovsm->type = 4;

            pann->name = "Pan";      time->name = "Time";
            pann->min = -50;         time->min = -100;
            pann->max = 50;          time->max = 100;
            pann->reset = 0;         time->reset = 0;
            pann->decimals = 1;      time->decimals = 1;
            pann->unit = "pan";      time->unit = " %";
            pann->type = 2;          time->type = 2;

            clip->name = "Warm,Hard,Clip";
            clip->min = 4;
            clip->max = 3;
            clip->reset = 1;
            clip->decimals = 1;
            clip->unit = "";
            clip->type = 4;

            mac1->name = "Macro P"; mac2->name = "Macro Q"; mac3->name = "Macro R";
            mac1->min = 0;          mac2->min = 0;          mac3->min = 0;
            mac1->max = 100;        mac2->max = 100;        mac3->max = 100;
            mac1->reset = 0;        mac2->reset = 0;        mac3->reset = 0;
            mac1->decimals = 1;     mac2->decimals = 1;     mac3->decimals = 1;
            mac1->unit = "%";       mac2->unit = "%";       mac3->unit = "%";
            mac1->type = 0;         mac2->type = 0;         mac3->type = 0;

            mac4->name = "Macro S"; mac5->name = "Macro T";
            mac4->min = 0;          mac5->min = 0;
            mac4->max = 100;        mac5->max = 100;
            mac4->reset = 0;        mac5->reset = 0;
            mac4->decimals = 1;     mac5->decimals = 1;
            mac4->unit = "%";       mac5->unit = "%";
            mac4->type = 0;         mac5->type = 0;

            addView(tnsp);
            addView(bend);
            addView(glde);
            addView(retr);
            addView(ovsm);
            addView(pann);
            addView(time);
            addView(clip);
            addView(mac1);
            addView(mac2);
            addView(mac3);
            addView(mac4);
            addView(mac5);

            addView(mcd1);
            addView(mcd2);
            addView(mcd3);
            addView(mcd4);
            addView(mcd5);

            addView(ovsl);
            addView(rtrl);
            addView(clpl);

            addView(keyl);
            addView(vell);

            addView(keym);
            addView(velm);
        }

        ~MidiView()
        {
            tnsp->forget();
            bend->forget();
            glde->forget();
            retr->forget();
            ovsm->forget();
            pann->forget();
            time->forget();
            clip->forget();
            mac1->forget();
            mac2->forget();
            mac3->forget();
            mac4->forget();
            mac5->forget();
        }

    };
}