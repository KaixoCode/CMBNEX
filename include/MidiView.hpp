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
        /*
         * Midi:
         *  - TPose
         *  - Bend
         *  - Glide
         *  - Rtg
         *  - Oversampling
         *  - Osc<Vel
         *  - Freq<Key
         *  - Freq<Vel
         */

        Knob* tnsp = nullptr;
        Knob* bend = nullptr;
        Knob* glde = nullptr;
        Knob* retr = nullptr;
        Knob* ovsm = nullptr;
        Knob* oscv = nullptr;
        Knob* frqk = nullptr;
        Knob* frqv = nullptr;
        Knob* clip = nullptr;

        Label* ovsl;
        Label* rtrl;
        Label* clpl;

        int index = 0;

        void createControls(IControlListener* listener, MyEditor* editor)
        {
            clip = new Knob{ {   4, 25,   4 + 125, 25 + 20 }, editor };
            tnsp = new Knob{ { 135,  5, 135 +  65,  5 + 50 }, editor };
            bend = new Knob{ { 200,  5, 200 +  65,  5 + 50 }, editor };
            frqk = new Knob{ { 265,  5, 265 +  65,  5 + 50 }, editor };
            ovsm = new Knob{ {   4, 75,   4 + 100, 75 + 20 }, editor };
            retr = new Knob{ { 105, 75, 105 +  20, 75 + 20 }, editor };
            oscv = new Knob{ { 135, 59, 135 +  65, 59 + 50 }, editor };
            glde = new Knob{ { 200, 59, 200 +  65, 59 + 50 }, editor };
            frqv = new Knob{ { 265, 59, 264 +  70, 59 + 50 }, editor };

            ovsl = new Label{ {   5, 55,   5 + 90, 55 + 20 } };
            ovsl->fontsize = 14;
            ovsl->center = false;
            ovsl->value = "Oversample";

            clpl = new Label{ {   5,  5,   5 + 65,  5 + 20 } };
            clpl->fontsize = 14;
            clpl->center = false;
            clpl->value = "Clipping";

            rtrl = new Label{ { 105, 55, 105 + 20, 55 + 20 } };
            rtrl->fontsize = 14;
            rtrl->center = false;
            rtrl->value = "Rtr";

            tnsp->setListener(listener);
            bend->setListener(listener);
            glde->setListener(listener);
            retr->setListener(listener);
            ovsm->setListener(listener);
            oscv->setListener(listener);
            frqk->setListener(listener);
            frqv->setListener(listener);
            clip->setListener(listener);

            tnsp->setTag(Params::Transpose);
            bend->setTag(Params::Bend);
            glde->setTag(Params::Glide);
            retr->setTag(Params::Retrigger);
            ovsm->setTag(Params::Oversample);
            oscv->setTag(Params::OscVel);
            frqk->setTag(Params::Panning);
            frqv->setTag(Params::Time);
            clip->setTag(Params::Clipping);

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

            retr->name = "R";        ovsm->name = "Off,2x,4x,8x"; oscv->name = "Osc<Vel";
            retr->min = 0;           ovsm->min = 4;           oscv->min = 0;
            retr->max = 100;         ovsm->max = 4;           oscv->max = 100;
            retr->reset = 0;         ovsm->reset = 1;         oscv->reset = 0;
            retr->decimals = 1;      ovsm->decimals = 1;      oscv->decimals = 1;
            retr->unit = "";         ovsm->unit = "";         oscv->unit = " %";
            retr->type = 3;          ovsm->type = 4;          oscv->type = 2;

            frqk->name = "Pan";      frqv->name = "Time";
            frqk->min = -50;         frqv->min = -100;
            frqk->max = 50;          frqv->max = 100;
            frqk->reset = 0;         frqv->reset = 0;
            frqk->decimals = 1;      frqv->decimals = 1;
            frqk->unit = "pan";      frqv->unit = " %";
            frqk->type = 2;          frqv->type = 2;

            clip->name = "Warm,Hard,Clip";
            clip->min = 4;
            clip->max = 3;
            clip->reset = 1;
            clip->decimals = 1;
            clip->unit = "";
            clip->type = 4;

            addView(tnsp);
            addView(bend);
            addView(glde);
            addView(retr);
            addView(ovsm);
            addView(oscv);
            addView(frqk);
            addView(frqv);
            addView(clip);

            addView(ovsl);
            addView(rtrl);
            addView(clpl);
        }

        MidiView(const CRect& size, int index, IControlListener* listener, MyEditor* editor)
            : CViewContainer(size), index(index)
        {
            createControls(listener, editor);
            setBackgroundColor({ 23, 23, 23, 255 });
        }

        void draw(CDrawContext* pContext) override
        {
            CViewContainer::draw(pContext);
        }
    };

    struct MidiAttributes
    {
        static inline CPoint Size = { 335, 100 };

        static inline auto Name = "Midi";
        static inline auto BaseView = UIViewCreator::kCViewContainer;

        static inline std::tuple Attributes
        {
            Attr{ "index", &MidiView::index },
        };
    };

    class MidiViewFactory : public ViewFactoryBase<MidiView, MidiAttributes>
    {
    public:
        CView* create(const UIAttributes& attributes, const IUIDescription* description) const override
        {
            CRect _size{ CPoint{ 45, 45 }, MidiAttributes::Size }; 
            int _index = 0;
            attributes.getIntegerAttribute("index", _index);
            MyEditor* _editor = dynamic_cast<MyEditor*>(description->getController());
            auto* _value = new MidiView(_size, _index, description->getControlListener(""), _editor);
            apply(_value, attributes, description);
            return _value;
        }
    };

    static inline MidiViewFactory midiViewFactory;
}