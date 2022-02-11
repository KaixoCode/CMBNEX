#include "Views/LFOView.hpp"

namespace Kaixo
{
    void LFOView::onIdle()
    {
        if (!wtloaded)
        {
            curve->setDirty(true);

            if (Wavetables::basicLoaded())
                wtloaded = true;
        }

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

    CMouseEventResult LFOView::onMouseDown(CPoint& where, const CButtonState& buttons)
    {
        pwhere = where;
        auto _r = CViewContainer::onMouseDown(where, buttons);
        pressed = true;
        return kMouseEventHandled;
    }

    CMouseEventResult LFOView::onMouseMoved(CPoint& where, const CButtonState& buttons)
    {
        auto _r = CViewContainer::onMouseMoved(where, buttons);

        CPoint where2(where);
        where2.offset(-getViewSize().left, -getViewSize().top);
        getTransform().inverse().transform(where2);

        if (pwhere != where && pressed)
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

    void LFOView::valueChanged(CControl* pControl)
    {
        modulateChange = true;
        onIdle();
        if (pControl == sync)
        {
            if (sync->getValue()) rate->settings.unit = "time";
            else rate->settings.unit = " Hz";
            rate->setDirty(true);
        }
    }

    void LFOView::UpdateIndex()
    {
        lfo1->settings.enabled = lfo2->settings.enabled = lfo3->settings.enabled = lfo4->settings.enabled = lfo5->settings.enabled = false;
        if (index == 0) lfo1->settings.enabled = true;
        if (index == 1) lfo2->settings.enabled = true;
        if (index == 2) lfo3->settings.enabled = true;
        if (index == 3) lfo4->settings.enabled = true;
        if (index == 4) lfo5->settings.enabled = true;

        rate->setTag(Params::LFORate1 + index);
        amnt->setTag(Params::LFOLvl1 + index);
        posi->setTag(Params::LFOPos1 + index);
        offs->setTag(Params::LFOPhase1 + index);
        shpr->setTag(Params::LFOShaper1 + index);
        sync->setTag(Params::LFOSync1 + index);
        retr->setTag(Params::LFORetr1 + index);

        setDirty(true);
    }

    LFOView::LFOView(const CRect& size, IControlListener* listener, MyEditor* editor)
        : CViewContainer(size)
    {
        setBackgroundColor({ 0, 0, 0, 0 });
        addView(new BackgroundEffect{ {.size = { 0, 0, getWidth(), getHeight() } } });
        setWantsIdle(true);

        swtc = new SwitchThing{ { 0, 0, 335, 30 } };
        swtc->setIndex = [&](int i) {
            if (i < 0 || i > 4) return;
            index = i;
            UpdateIndex();
        };

        rate = new Parameter{ {
            .tag = Params::LFORate1 + index,
            .editor = editor, .listener = this,
            .size = {   5, 130,   5 + 65, 130 + 55 },
            .type = Parameter::NUMBER, .name = "Rate",
            .min = 0.1, .max = 30, .reset = 1, .decimals = 1,
            .unit = " Hz",
        } };

        amnt = new Parameter{ {
            .tag = Params::LFOLvl1 + index,
            .editor = editor, .listener = this,
            .size = {  70, 130,  70 + 65, 130 + 55 },
            .type = Parameter::NUMBER, .name = "Amount",
            .min = -100, .max = 100, .reset = 50, .decimals = 1,
            .unit = " %",
        } };

        posi = new Parameter{ {
            .tag = Params::LFOPos1 + index,
            .editor = editor, .listener = this,
            .size = { 135, 130, 135 + 65, 130 + 55 },
            .type = Parameter::NUMBER, .name = "Pos",
            .min = 0, .max = 100, .reset = 0, .decimals = 1,
            .unit = " %",
        } };

        offs = new Parameter{ {
            .tag = Params::LFOPhase1 + index,
            .editor = editor, .listener = this,
            .size = { 200, 130, 200 + 65, 130 + 55 },
            .type = Parameter::NUMBER ,.name = "Offset",
            .min = 0, .max = 100, .reset = 0, .decimals = 1,
            .unit = " %",
        } };

        shpr = new Parameter{ {
            .tag = Params::LFOShaper1 + index,
            .editor = editor, .listener = this,
            .size = { 265, 130, 265 + 65, 130 + 55 },
            .type = Parameter::NUMBER, .name = "Shaper",
            .min = -100, .max = 100, .reset = 0, .decimals = 1,
            .unit = " %",
        } };

        sync = new Parameter{ {
            .tag = Params::LFOSync1 + index,
            .editor = editor, .listener = this,
            .size = { 282,  32, 282 + 45,  32 + 20 },
            .type = Parameter::BUTTON, .name = "Sync"
        } };

        retr = new Parameter{ {
            .tag = Params::LFORetr1 + index,
            .editor = editor, .listener = this,
            .size = { 257,  32, 257 + 20,  32 + 20 },
            .type = Parameter::BUTTON, .name = "R"
        } };

        curve = new WaveformView{ {  5,  30, 5 + 325, 30 + 95 } };
        curve->LFO = true;

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

        lfo1 = new Label{ {
            .size = {   5,   5,   5 + 65,   5 + 20 },
            .value = "LFO 1", .center = true, .fontsize = 14, .enabled = true,
        } };

        lfo2 = new Label{ {
            .size = {  71,   5,  71 + 65,   5 + 20 },
            .value = "LFO 2", .center = true, .fontsize = 14, .enabled = false,
        } };

        lfo3 = new Label{ {
            .size = { 137,   5, 137 + 65,   5 + 20 },
            .value = "LFO 3", .center = true, .fontsize = 14, .enabled = false,
        } };

        lfo4 = new Label{ {
            .size = { 203,   5, 203 + 65,   5 + 20 },
            .value = "LFO 4", .center = true, .fontsize = 14, .enabled = false,
        } };

        lfo5 = new Label{ {
            .size = { 269,   5, 269 + 65,   5 + 20 },
            .value = "LFO 5", .center = true, .fontsize = 14, .enabled = false,
        } };

        addView(curve); addView(rate); addView(amnt); addView(posi); addView(offs); 
        addView(shpr); addView(sync); addView(retr); addView(lfo1); addView(lfo2); 
        addView(lfo3); addView(lfo4); addView(lfo5); addView(nvd1); addView(nvd2); 
        addView(nvd3); addView(nvd4); addView(nvd5); addView(swtc);

        editor->controller->wakeupCalls.emplace_back(Params::LFORate1 + index, rate->modulation, modulateChange);
        editor->controller->wakeupCalls.emplace_back(Params::LFOLvl1 + index, amnt->modulation, modulateChange);
        editor->controller->wakeupCalls.emplace_back(Params::LFOPos1 + index, posi->modulation, modulateChange);
        editor->controller->wakeupCalls.emplace_back(Params::LFOPhase1 + index, offs->modulation, modulateChange);
        editor->controller->wakeupCalls.emplace_back(Params::LFOShaper1 + index, shpr->modulation, modulateChange);
    }

    LFOView::~LFOView()
    {
        rate->forget(); amnt->forget(); posi->forget(); offs->forget(); 
        shpr->forget(); sync->forget();
    }
}