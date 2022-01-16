#pragma once
#include "pch.hpp"
#include "myplugincids.hpp"
#include "Modules.hpp"
#include "Knob.hpp"
#include "Label.hpp"
#include "OptionMenu.hpp"
#include "WaveformView.hpp"

namespace Kaixo
{
    class LFOView : public CViewContainer, public IControlListener
    {
    public:
        int index = 0;

        WaveformView* curve;
        Knob* rate;
        Knob* amnt;
        Knob* posi;
        Knob* offs;
        Knob* shpr;
        Knob* sync;

        Label* lfo1;
        Label* lfo2;
        Label* lfo3;
        Label* lfo4;
        Label* lfo5;

        DragThing* nvd1;
        DragThing* nvd2;
        DragThing* nvd3;
        DragThing* nvd4;
        DragThing* nvd5;

        SwitchThing* swtc;

        bool pressed = false;

        CMouseEventResult onMouseDown(CPoint& where, const CButtonState& buttons) override
        {
            auto _r = CViewContainer::onMouseDown(where, buttons);
            pressed = true;
            return kMouseEventHandled;
        }

        CMouseEventResult onMouseMoved(CPoint& where, const CButtonState& buttons) override
        {
            auto _r = CViewContainer::onMouseMoved(where, buttons);

            CPoint where2(where);
            where2.offset(-getViewSize().left, -getViewSize().top);
            getTransform().inverse().transform(where2);

            if (pressed)
            {
                pressed = false;
                nvd1->onMouseMoved(where2, buttons);
                nvd2->onMouseMoved(where2, buttons);
                nvd3->onMouseMoved(where2, buttons);
                nvd4->onMouseMoved(where2, buttons);
                nvd5->onMouseMoved(where2, buttons);
            }

            return kMouseEventHandled;
        }

        void valueChanged(CControl* pControl)
        {
            bool _f = false;
            if (pControl == sync)
            {
                if (sync->getValue()) rate->unit = "time";
                else rate->unit = " Hz";
                rate->setDirty(true);
                _f = true;
            }
            
            if (pControl == rate || pControl == sync)
            {
                if (sync->getValue())
                {
                    size_t _index = std::floor(rate->getValue() * (TimesAmount - 1));
                    curve->repeat = 1 + _index;
                }
                else
                {
                    curve->repeat = 1 + rate->getValue() * rate->getValue() * 20;
                }
                _f = true;
            }
            else if (pControl == amnt) curve->level = amnt->getValue() * 2 - 1, _f = true;
            else if (pControl == posi) curve->pos = posi->getValue(), _f = true;
            else if (pControl == offs) curve->phase = offs->getValue(), _f = true;
            else if (pControl == shpr) curve->shaper3 = shpr->getValue(), _f = true;
            
            if (_f)
                curve->setDirty(true);
        }

        void UpdateIndex()
        {
            lfo1->color = lfo2->color = lfo3->color = lfo4->color = lfo5->color = { 128, 128, 128, 255 };
            if (index == 0) lfo1->color = { 200, 200, 200, 255 };
            if (index == 1) lfo2->color = { 200, 200, 200, 255 };
            if (index == 2) lfo3->color = { 200, 200, 200, 255 };
            if (index == 3) lfo4->color = { 200, 200, 200, 255 };
            if (index == 4) lfo5->color = { 200, 200, 200, 255 };

            rate->setTag(Params::LFORate1 + index);
            amnt->setTag(Params::LFOLvl1 + index);
            posi->setTag(Params::LFOPos1 + index);
            offs->setTag(Params::LFOPhase1 + index);
            shpr->setTag(Params::LFOShaper1 + index);
            sync->setTag(Params::LFOSync1 + index);

            setDirty(true);
        }

        void createControls(IControlListener* listener, MyEditor* editor)
        {
            swtc = new SwitchThing{ { 0, 0, 335, 30 } };
            swtc->setIndex = [&](int i) {
                if (i < 0 || i > 4) return;
                index = i;
                UpdateIndex();
            };

            rate = new Knob{ {   5, 130,   5 + 65, 130 + 55 }, editor };
            amnt = new Knob{ {  70, 130,  70 + 65, 130 + 55 }, editor };
            posi = new Knob{ { 135, 130, 135 + 65, 130 + 55 }, editor };
            offs = new Knob{ { 200, 130, 200 + 65, 130 + 55 }, editor };
            shpr = new Knob{ { 265, 130, 265 + 65, 130 + 55 }, editor };
            sync = new Knob{ { 282,  32, 282 + 45,  32 + 20 }, editor };
            curve = new WaveformView{ {  5,  30, 5 + 325, 30 + 95 } };

            nvd1 = new DragThing{ {   5,   5,   5 + 60,   5 + 18 } };
            nvd2 = new DragThing{ {  71,   5,  71 + 60,   5 + 18 } };
            nvd3 = new DragThing{ { 137,   5, 137 + 60,   5 + 18 } };
            nvd4 = new DragThing{ { 203,   5, 203 + 60,   5 + 18 } };
            nvd5 = new DragThing{ { 269,   5, 269 + 60,   5 + 18 } };

            nvd1->source = ModSources::LFO1;
            nvd2->source = ModSources::LFO2;
            nvd3->source = ModSources::LFO3;
            nvd4->source = ModSources::LFO4;
            nvd5->source = ModSources::LFO5;

            rate->color = MainLFO;
            amnt->color = MainLFO;
            posi->color = MainLFO;
            offs->color = MainLFO;
            shpr->color = MainLFO;
            sync->color = MainLFO;
            curve->color = MainLFO;

            lfo1 = new Label{ {   5,   5,   5 + 65,   5 + 20 } };
            lfo1->fontsize = 14;
            lfo1->center = true;
            lfo1->color = { 200, 200, 200, 255 };
            lfo1->value = "LFO 1";
            lfo2 = new Label{ {  71,   5,  71 + 65,   5 + 20 } };
            lfo2->fontsize = 14;
            lfo2->center = true;
            lfo2->color = { 128, 128, 128, 255 };
            lfo2->value = "LFO 2";
            lfo3 = new Label{ { 137,   5, 137 + 65,   5 + 20 } };
            lfo3->fontsize = 14;
            lfo3->center = true;
            lfo3->color = { 128, 128, 128, 255 };
            lfo3->value = "LFO 3";
            lfo4 = new Label{ { 203,   5, 203 + 65,   5 + 20 } };
            lfo4->fontsize = 14;
            lfo4->center = true;
            lfo4->color = { 128, 128, 128, 255 };
            lfo4->value = "LFO 4";
            lfo5 = new Label{ { 269,   5, 269 + 65,   5 + 20 } };
            lfo5->fontsize = 14;
            lfo5->center = true;
            lfo5->color = { 128, 128, 128, 255 };
            lfo5->value = "LFO 5";

            rate->setListener(listener);
            amnt->setListener(listener);
            posi->setListener(listener);
            offs->setListener(listener);
            shpr->setListener(listener);
            sync->setListener(listener);

            rate->registerControlListener(this);
            amnt->registerControlListener(this);
            posi->registerControlListener(this);
            offs->registerControlListener(this);
            shpr->registerControlListener(this);
            sync->registerControlListener(this);

            rate->setTag(Params::LFORate1 + index);
            amnt->setTag(Params::LFOLvl1 + index);
            posi->setTag(Params::LFOPos1 + index);
            offs->setTag(Params::LFOPhase1 + index);
            shpr->setTag(Params::LFOShaper1 + index);
            sync->setTag(Params::LFOSync1 + index);

            rate->name = "Rate"; amnt->name = "Amount";   posi->name = "Pos";
            rate->min = 0.1;     amnt->min = -100;        posi->min = 0;
            rate->max = 30;      amnt->max = 100;         posi->max = 100;
            rate->reset = 1;     amnt->reset = 50;        posi->reset = 0;
            rate->decimals = 1;  amnt->decimals = 1;      posi->decimals = 1;
            rate->unit = " Hz";  amnt->unit = " %";       posi->unit = " %";
            rate->type = 2;      amnt->type = 2;          posi->type = 2;

            offs->name = "Offset"; shpr->name = "Shaper"; sync->name = "Sync";
            offs->min = 0;         shpr->min = -100;      sync->min = 0;
            offs->max = 100;       shpr->max = 100;       sync->max = 1;
            offs->reset = 0;       shpr->reset = 0;       sync->reset = 0;
            offs->decimals = 1;    shpr->decimals = 1;    sync->decimals = 1;
            offs->unit = " %";     shpr->unit = " %";     sync->unit = "";
            offs->type = 2;        shpr->type = 2;        sync->type = 3;

            addView(curve);

            addView(rate);
            addView(amnt);
            addView(posi);
            addView(offs);
            addView(shpr);
            addView(sync);

            addView(lfo1);
            addView(lfo2);
            addView(lfo3);
            addView(lfo4);
            addView(lfo5);

            addView(nvd1);
            addView(nvd2);
            addView(nvd3);
            addView(nvd4);
            addView(nvd5);

            addView(swtc);
        }

        LFOView(const CRect& size, int index, IControlListener* listener, MyEditor* editor)
            : CViewContainer(size), index(index)
        {
            createControls(listener, editor);
            setBackgroundColor({ 23, 23, 23, 255 });
        }
    };

    struct LFOAttributes
    {
        static inline CPoint Size = { 335, 200 };

        static inline auto Name = "LFO";
        static inline auto BaseView = UIViewCreator::kCViewContainer;

        static inline std::tuple Attributes
        {
            Attr{ "index", &LFOView::index },
        };
    };

    class LFOViewFactory : public ViewFactoryBase<LFOView, LFOAttributes>
    {
    public:
        CView* create(const UIAttributes& attributes, const IUIDescription* description) const override
        {
            CRect _size{ CPoint{ 45, 45 }, LFOAttributes::Size };
            int _index = 0;
            attributes.getIntegerAttribute("index", _index);
            MyEditor* _editor = dynamic_cast<MyEditor*>(description->getController());
            auto* _value = new LFOView(_size, _index, description->getControlListener(""), _editor);
            apply(_value, attributes, description);
            return _value;
        }
    };

    static inline LFOViewFactory lfoViewFactory;
}