#pragma once
#include "pch.hpp"
#include "myplugincids.hpp"
#include "Modules.hpp"
#include "Knob.hpp"
#include "Label.hpp"
#include "WaveformView.hpp"

namespace Kaixo
{
    class OscillatorView : public CViewContainer, public IControlListener
    {
    public:
        Knob* tune = nullptr; // Tune
        Knob* wtps = nullptr; // Pos
        Knob* sync = nullptr; // Sync
        Knob* detn = nullptr; // Detune
        Knob* volm = nullptr; // Gain
        Knob* phse = nullptr; // Phase
        Knob* plsw = nullptr; // Pulse Width
        Knob* pann = nullptr; // Pan
        Knob* nois = nullptr; // Noise
        Knob* fltr = nullptr; // Filter type
        Knob* freq = nullptr; // Filter Frequency
        Knob* reso = nullptr; // Filter Resonance
        Knob* rphs = nullptr; // Random Phase
        Knob* shpr = nullptr; // Shaper
        Knob* shp2 = nullptr; // Shaper
        Label* titl = nullptr; // Oscillator title
        Label* ftrl = nullptr; // Filter title
        WaveformView* wfrm = nullptr; // Waveform view

        int index = 0;

        void valueChanged(CControl* pControl)
        {
            bool _f = false;
            if (pControl == phse) wfrm->phase = phse->getValue(), _f =true;
            else if (pControl == wtps) wfrm->pos = wtps->getValue(), _f =true;
            else if (pControl == sync) wfrm->sync = sync->getValue(), _f =true;
            else if (pControl == shpr) wfrm->shaper = shpr->getValue(), _f =true;
            else if (pControl == shp2) wfrm->shaper2 = shp2->getValue(), _f =true;
            else if (pControl == plsw) wfrm->pw = plsw->getValue(), _f =true;

            if (_f)
                wfrm->setDirty(true);
        }

        void createControls(IControlListener* listener)
        {
            tune = new Knob{ {  60,   5,  60 + 70,   5 + 96 } };
            wtps = new Knob{ { 135, 155, 135 + 65, 155 + 40 } };
            sync = new Knob{ {   5, 105,   5 + 65, 105 + 40 } };
            detn = new Knob{ {   5,  20,   5 + 55,  20 + 80 } };
            volm = new Knob{ { 205,   5, 205 + 70,   5 + 95 } };
            phse = new Knob{ { 135, 105, 135 + 65, 105 + 40 } };
            plsw = new Knob{ {  70, 105,  70 + 65, 105 + 40 } };
            pann = new Knob{ { 200, 105, 200 + 65, 105 + 40 } };
            nois = new Knob{ { 200, 155, 200 + 65, 155 + 40 } };
            fltr = new Knob{ { 264, 175, 264 + 70, 175 + 20 } };
            freq = new Knob{ { 275,  20, 275 + 55,  20 + 80 } };
            reso = new Knob{ { 265, 105, 265 + 65, 105 + 40 } };
            rphs = new Knob{ { 135,  80, 135 + 65,  80 + 20 } };
            shpr = new Knob{ {   5, 155,   5 + 65, 155 + 40 } };
            shp2 = new Knob{ {  70, 155,  70 + 65, 155 + 40 } };

            tune->index = index;
            wtps->index = index;
            sync->index = index;
            detn->index = index;
            volm->index = index;
            phse->index = index;
            plsw->index = index;
            pann->index = index;
            nois->index = index;
            fltr->index = index;
            freq->index = index;
            reso->index = index;
            rphs->index = index;
            shpr->index = index;
            shp2->index = index;

            wfrm = new WaveformView{ { 135, 35, 135 + 65, 35 + 40 } };
            wfrm->index = index;

            titl = new Label{ { 135,   3, 135 + 65,   3 + 30 } };
            titl->fontsize = 24;
            titl->center = true;
            titl->color = { 200, 200, 200, 255 };
            std::string a;
            a += (char)('A' + index);
            titl->value = a.c_str();

            ftrl = new Label{ { 265, 155, 265 + 65, 155 + 20 } };
            ftrl->fontsize = 14;
            ftrl->center = false;
            ftrl->color = { 200, 200, 200, 255 };            
            ftrl->value = "Filter";

            tune->setListener(listener);
            wtps->setListener(listener);
            sync->setListener(listener);
            detn->setListener(listener);
            volm->setListener(listener);
            phse->setListener(listener);
            plsw->setListener(listener);
            pann->setListener(listener);
            nois->setListener(listener);
            fltr->setListener(listener);
            freq->setListener(listener);
            reso->setListener(listener);
            rphs->setListener(listener);
            shpr->setListener(listener);
            shp2->setListener(listener);

            phse->registerControlListener(this);
            wtps->registerControlListener(this);
            plsw->registerControlListener(this);
            shpr->registerControlListener(this);
            shp2->registerControlListener(this);
            sync->registerControlListener(this);

            tune->setTag(Params::Pitch1 + index);
            wtps->setTag(Params::WTPos1 + index);
            sync->setTag(Params::Sync1 + index);
            detn->setTag(Params::Detune1 + index);
            volm->setTag(Params::Volume1 + index);
            phse->setTag(Params::Phase1 + index);
            plsw->setTag(Params::PulseW1 + index);
            pann->setTag(Params::Pan1 + index);
            nois->setTag(Params::Noise1 + index);
            fltr->setTag(Params::Filter1 + index);
            freq->setTag(Params::Freq1 + index);
            reso->setTag(Params::Reso1 + index);
            rphs->setTag(Params::RandomPhase1 + index);
            shpr->setTag(Params::Shaper1 + index);
            shp2->setTag(Params::Shaper21 + index);

            tune->name = "Pitch";    wtps->name = "Pos";      sync->name = "Sync";
            tune->min = -24;         wtps->min = 0;           sync->min = 100;
            tune->max = 24;          wtps->max = 100;         sync->max = 800;
            tune->reset = 0;         wtps->reset = 0;         sync->reset = 100;
            tune->decimals = 1;      wtps->decimals = 1;      sync->decimals = 1;
            tune->unit = " st";      wtps->unit = " %";       sync->unit = " %";
            tune->type = 0;          wtps->type = 2;          sync->type = 2;
                                     
            detn->name = "Detune";   volm->name = "Gain";     phse->name = "Phase";
            detn->min = -200;        volm->min = 0;           phse->min = 0;
            detn->max = 200;         volm->max = 1;           phse->max = 100;
            detn->reset = 0;         volm->reset = 1;         phse->reset = 0;
            detn->decimals = 0;      volm->decimals = 1;      phse->decimals = 1;
            detn->unit = " ct";      volm->unit = " dB";      phse->unit = " %";
            detn->type = 0;          volm->type = 0;          phse->type = 2;
                                     
            plsw->name = "PW";       pann->name = "Pan";      nois->name = "Noise";
            plsw->min = -100;        pann->min = -50;         nois->min = 0;
            plsw->max = 100;         pann->max = 50;          nois->max = 100;
            plsw->reset = 0;         pann->reset = 0;         nois->reset = 0;
            plsw->decimals = 1;      pann->decimals = 1;      nois->decimals = 1;
            plsw->unit = " %";       pann->unit = "pan";      nois->unit = " %";
            plsw->type = 2;          pann->type = 2;          nois->type = 2;

            fltr->name = "L,H,B";    freq->name = "Freq";     reso->name = "Reso";
            fltr->min = 4;           freq->min = 20;          reso->min = 0;
            fltr->max = 3;           freq->max = 22000;       reso->max = 125;
            fltr->reset = 1;         freq->reset = 22000;     reso->reset = 0;
            fltr->decimals = 0;      freq->decimals = 1;      reso->decimals = 1;
            fltr->unit = "";         freq->unit = " Hz";      reso->unit = " %";
            fltr->type = 4;          freq->type = 0;          reso->type = 2;

            rphs->name = "Random";   shpr->name = "SHP-X";    shp2->name = "SHP-Y";
            rphs->min = 0;           shpr->min = 0;           shp2->min = 0;
            rphs->max = 1;           shpr->max = 100;         shp2->max = 100;
            rphs->reset = 0;         shpr->reset = 0;         shp2->reset = 0;
            rphs->decimals = 0;      shpr->decimals = 1;      shp2->decimals = 1;
            rphs->unit = "";         shpr->unit = " %";       shp2->unit = " %";
            rphs->type = 3;          shpr->type = 2;          shp2->type = 2;

            addView(tune);
            addView(wtps);
            addView(sync);
            addView(detn);
            addView(volm);
            addView(phse);
            addView(plsw);
            addView(pann);
            addView(nois);
            addView(fltr);
            addView(freq);
            addView(reso);
            addView(rphs);
            addView(shpr);
            addView(shp2);

            addView(wfrm);

            addView(titl);
            addView(ftrl);
        }

        OscillatorView(const CRect& size, int index, IControlListener* listener)
            : CViewContainer(size), index(index)
        {
            createControls(listener);
            setBackgroundColor({ 23, 23, 23, 255 });
        }

        void draw(CDrawContext* pContext) override
        {
            CViewContainer::draw(pContext);
        }
    };

    struct OscillatorAttributes
    {
        static inline CPoint Size = { 335, 200 };

        static inline auto Name = "Oscillator";
        static inline auto BaseView = UIViewCreator::kCViewContainer;

        static inline std::tuple Attributes
        {
            Attr{ "index", &OscillatorView::index },
        };
    };

    class OscillatorViewFactory : public ViewFactoryBase<OscillatorView, OscillatorAttributes>
    {
    public:
        CView* create(const UIAttributes& attributes, const IUIDescription* description) const override
        {
            CRect _size{ CPoint{ 45, 45 }, OscillatorAttributes::Size };
            int _index = 0;
            attributes.getIntegerAttribute("index", _index);
            auto* _value = new OscillatorView(_size, _index, description->getControlListener(""));
            apply(_value, attributes, description);
            return _value;
        }
    };

    static inline OscillatorViewFactory oscillatorViewFactory;
}