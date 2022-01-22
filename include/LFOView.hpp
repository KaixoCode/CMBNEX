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
        bool modulateChange = false;

        void onIdle()
        {
            if (modulateChange)
            {
                modulateChange = false;
                if (sync->getValue())
                {
                    size_t _index = std::floor(rate->getValue() * (TimesAmount - 1));
                    curve->repeat = 1 + _index;
                }
                else
                {
                    curve->repeat = 1 + rate->getValue() * rate->getValue() * 20;
                }
                curve->level = amnt->getValue() * 2 - 1;
                curve->osc.settings.wtpos = posi->getValue();
                curve->phase = offs->getValue();
                curve->osc.settings.shaper3 = shpr->getValue();

                curve->setDirty(true);
            }
        }

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
            modulateChange = true;
            onIdle();
            if (pControl == sync)
            {
                if (sync->getValue()) rate->unit = "time";
                else rate->unit = " Hz";
                rate->setDirty(true);
            }
        }

        void UpdateIndex()
        {
            lfo1->enabled = lfo2->enabled = lfo3->enabled = lfo4->enabled = lfo5->enabled = false;
            if (index == 0) lfo1->enabled = true;
            if (index == 1) lfo2->enabled = true;
            if (index == 2) lfo3->enabled = true;
            if (index == 3) lfo4->enabled = true;
            if (index == 4) lfo5->enabled = true;

            rate->setTag(Params::LFORate1 + index);
            amnt->setTag(Params::LFOLvl1 + index);
            posi->setTag(Params::LFOPos1 + index);
            offs->setTag(Params::LFOPhase1 + index);
            shpr->setTag(Params::LFOShaper1 + index);
            sync->setTag(Params::LFOSync1 + index);

            setDirty(true);
        }

        LFOView(const CRect& size, IControlListener* listener, MyEditor* editor)
            : CViewContainer(size)
        {
            setBackgroundColor({ 0, 0, 0, 0 });
            addView(new BackgroundEffect{ { 0, 0, getWidth(), getHeight() } });
            setWantsIdle(true);

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

            lfo1 = new Label{ {   5,   5,   5 + 65,   5 + 20 } };
            lfo1->fontsize = 14;
            lfo1->center = true;
            lfo1->enabled = true;
            lfo1->value = "LFO 1";
            lfo2 = new Label{ {  71,   5,  71 + 65,   5 + 20 } };
            lfo2->fontsize = 14;
            lfo2->center = true;
            lfo2->enabled = false;
            lfo2->value = "LFO 2";
            lfo3 = new Label{ { 137,   5, 137 + 65,   5 + 20 } };
            lfo3->fontsize = 14;
            lfo3->center = true;
            lfo3->enabled = false;
            lfo3->value = "LFO 3";
            lfo4 = new Label{ { 203,   5, 203 + 65,   5 + 20 } };
            lfo4->fontsize = 14;
            lfo4->center = true;
            lfo4->enabled = false;
            lfo4->value = "LFO 4";
            lfo5 = new Label{ { 269,   5, 269 + 65,   5 + 20 } };
            lfo5->fontsize = 14;
            lfo5->center = true;
            lfo5->enabled = false;
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

            editor->controller->wakeupCalls.emplace_back(Params::LFORate1 + index, rate->modulation, modulateChange);
            editor->controller->wakeupCalls.emplace_back(Params::LFOLvl1 + index, amnt->modulation, modulateChange);
            editor->controller->wakeupCalls.emplace_back(Params::LFOPos1 + index, posi->modulation, modulateChange);
            editor->controller->wakeupCalls.emplace_back(Params::LFOPhase1 + index, offs->modulation, modulateChange);
            editor->controller->wakeupCalls.emplace_back(Params::LFOShaper1 + index, shpr->modulation, modulateChange);
        }

        ~LFOView()
        {
            rate->forget();
            amnt->forget();
            posi->forget();
            offs->forget();
            shpr->forget();
            sync->forget();
        }
    };
}