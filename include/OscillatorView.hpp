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
        Knob* enbl = nullptr; // Enable
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

        CMouseEventResult onMouseDown(CPoint& where, const CButtonState& buttons) override
        {
            auto _r = CViewContainer::onMouseDown(where, buttons);
            if (_r != kMouseEventHandled)
            {
                int* _data = new int[1];
                _data[0] = (int)ModSources::Osc1 + index;
                doDrag(DragDescription{ CDropSource::create((void*)_data, sizeof(int) * 1, IDataPackage::Type::kBinary) });
                return kMouseEventHandled;
            }

            return _r;
        }

        void valueChanged(CControl* pControl)
        {
            bool _f = false;
            if (pControl == phse) wfrm->phase = phse->getValue(), _f =true;
            else if (pControl == wtps) wfrm->pos = wtps->getValue(), _f =true;
            else if (pControl == sync) wfrm->sync = sync->getValue(), _f =true;
            else if (pControl == shpr) wfrm->shaper = shpr->getValue(), _f =true;
            else if (pControl == shp2) wfrm->shaper2 = shp2->getValue(), _f =true;
            else if (pControl == plsw) wfrm->pw = plsw->getValue(), _f =true;

            else if (pControl == enbl)
            {
                titl->color = enbl->getValue() > 0.5 ? CColor{ 200, 200, 200, 255 } : CColor{ 128, 128, 128, 255 };

                CColor _clr = enbl->getValue() > 0.5 ? MainOsc : CColor{ 128, 128, 128, 255 };
                wfrm->color = _clr;
                tune->color = _clr;
                volm->color = _clr;
                detn->color = _clr;
                freq->color = _clr;
                rphs->color = _clr;
                sync->color = _clr;
                plsw->color = _clr;
                phse->color = _clr;
                pann->color = _clr;
                reso->color = _clr;
                shpr->color = _clr;
                shp2->color = _clr;
                wtps->color = _clr;
                nois->color = _clr;
                fltr->color = _clr;

                tune->setDirty(true);
                volm->setDirty(true);
                detn->setDirty(true);
                freq->setDirty(true);
                rphs->setDirty(true);
                sync->setDirty(true);
                plsw->setDirty(true);
                phse->setDirty(true);
                pann->setDirty(true);
                reso->setDirty(true);
                shpr->setDirty(true);
                shp2->setDirty(true);
                wtps->setDirty(true);
                nois->setDirty(true);
                fltr->setDirty(true);
                wfrm->setDirty(true);
            }

            if (_f)
                wfrm->setDirty(true);
        }

        void createControls(IControlListener* listener, MyEditor* editor)
        {
            enbl = new Knob{ { 153,   3     , 153 + 28,   3 + 28  }, editor };
            tune = new Knob{ {  65,   5     ,  65 + 70,   5 + 96 + 10      }, editor };
            volm = new Knob{ { 200,   5     , 200 + 70,   5 + 96 + 10      }, editor };
            detn = new Knob{ {   5,  16     ,   5 + 60,  21 + 80 + 10      }, editor };
            freq = new Knob{ { 270,  16     , 270 + 60,  21 + 80 + 10      }, editor };
            rphs = new Knob{ { 134,  80 + 10, 134 + 66,  80 + 10 + 21 }, editor };
            sync = new Knob{ {   5, 105 + 10,   5 + 65, 105 + 40 + 10 + 10 + 3 }, editor };
            plsw = new Knob{ {  70, 105 + 10,  70 + 65, 105 + 40 + 10 + 10 + 3 }, editor };
            phse = new Knob{ { 135, 105 + 10, 135 + 65, 105 + 40 + 10 + 10 + 3 }, editor };
            pann = new Knob{ { 200, 105 + 10, 200 + 65, 105 + 40 + 10 + 10 + 3 }, editor };
            reso = new Knob{ { 265, 105 + 10, 265 + 65, 105 + 40 + 10 + 10 + 3 }, editor };
            shpr = new Knob{ {   5, 152 + 20,   5 + 65, 152 + 40 + 10 + 20 + 3 }, editor };
            shp2 = new Knob{ {  70, 152 + 20,  70 + 65, 152 + 40 + 10 + 20 + 3 }, editor };
            wtps = new Knob{ { 135, 152 + 20, 135 + 65, 152 + 40 + 10 + 20 + 3 }, editor };
            nois = new Knob{ { 200, 152 + 20, 200 + 65, 152 + 40 + 10 + 20 + 3 }, editor };
            fltr = new Knob{ { 264, 172 + 20, 264 + 70, 172 + 20 + 00 + 20 }, editor };

            enbl->color = MainOsc;
            tune->color = MainOsc;
            volm->color = MainOsc;
            detn->color = MainOsc;
            freq->color = MainOsc;
            rphs->color = MainOsc;
            sync->color = MainOsc;
            plsw->color = MainOsc;
            phse->color = MainOsc;
            pann->color = MainOsc;
            reso->color = MainOsc;
            shpr->color = MainOsc;
            shp2->color = MainOsc;
            wtps->color = MainOsc;
            nois->color = MainOsc;
            fltr->color = MainOsc;

            wfrm = new WaveformView{ { 135, 35, 135 + 65, 35 + 50 } };
            wfrm->index = index;

            titl = new Label{ { 135,   3, 135 + 65,   3 + 30 } };
            titl->setMouseEnabled(false);
            titl->fontsize = 24;
            titl->center = true;
            titl->color = { 200, 200, 200, 255 };
            std::string a;
            a += (char)('A' + index);
            titl->value = a.c_str();

            ftrl = new Label{ { 265, 152 + 20, 265 + 65, 152 + 20 + 20 } };
            ftrl->fontsize = 14;
            ftrl->center = false;
            ftrl->color = { 200, 200, 200, 255 };            
            ftrl->value = "Filter";

            enbl->setListener(listener);
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

            enbl->registerControlListener(this);
            phse->registerControlListener(this);
            wtps->registerControlListener(this);
            plsw->registerControlListener(this);
            shpr->registerControlListener(this);
            shp2->registerControlListener(this);
            sync->registerControlListener(this);

            enbl->setTag(Params::Enable1 + index);
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

            enbl->name = "";
            enbl->min = 0;
            enbl->max = 1;
            enbl->reset = 0;
            enbl->decimals = 0;
            enbl->unit = "";
            enbl->type = 3;

            addView(enbl);
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

        OscillatorView(const CRect& size, int index, IControlListener* listener, MyEditor* editor)
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
            MyEditor* _editor = dynamic_cast<MyEditor*>(description->getController());
            auto* _value = new OscillatorView(_size, _index, description->getControlListener(""), _editor);
            apply(_value, attributes, description);
            return _value;
        }
    };

    static inline OscillatorViewFactory oscillatorViewFactory;
}