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

        Label* lfol;
        Label* modu;

        Label* lfo1;
        Label* lfo2;
        Label* lfo3;
        Label* lfo4;
        Label* lfo5;

        OptionMenu* mod[5];
        Knob* modt[5];
        Knob* moda[5];

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
            
            for (int i = 0; i < 5; i++)
                if (pControl == mod[i])
                {
                    _f = true;
                    if (mod[i]->getValue() == 0)
                        mod[i]->setFontColor({ 128, 128, 128, 255 });
                    else
                        mod[i]->setFontColor({ 200, 200, 200, 255 });
                }

            if (_f)
                curve->setDirty(true);
        }

        CMouseEventResult onMouseDown(CPoint& where, const CButtonState& buttons) override
        {
            auto _r = CViewContainer::onMouseDown(where, buttons);
            if (where.y - getViewSize().top > 0 && where.y - getViewSize().top < 30 && where.x - getViewSize().left < 335 && where.x - getViewSize().left > 0)
            {
                index = std::floor((where.x - getViewSize().left) / 65);

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
                sync->setTag(Params::LFOSync1 + index);

                for (int i = 0; i < 5; i++)
                    mod[i]->setTag(Params::LFO1M1 + index + (i * 10)), moda[i]->setTag(Params::LFO1M1A + index + (i * 10));

                setDirty(true);

                return kMouseEventHandled;
            }

            if (where.y - getViewSize().top > 125 && where.y - getViewSize().top < 150 && where.x - getViewSize().left < 285)
            {
                int _page = std::floor((where.x - getViewSize().left) / 167);

                lfol->color = modu->color = { 128, 128, 128, 255 };
                switch (_page)
                {
                case 0:
                {
                    rate->setVisible(true);
                    amnt->setVisible(true);
                    posi->setVisible(true);
                    offs->setVisible(true);
                    shpr->setVisible(true);

                    for (int i = 0; i < 5; i++)
                        mod[i]->setVisible(false), moda[i]->setVisible(false);

                    lfol->color = { 200, 200, 200, 255 };
                    break;
                }
                case 1:
                {
                    rate->setVisible(false);
                    amnt->setVisible(false);
                    posi->setVisible(false);
                    offs->setVisible(false);
                    shpr->setVisible(false);

                    for (int i = 0; i < 5; i++)
                        mod[i]->setVisible(true), moda[i]->setVisible(true);

                    modu->color = { 200, 200, 200, 255 };
                    break;
                }
                }

                setDirty(true);

                return kMouseEventHandled;
            }
            return _r;
        }

        void createControls(IControlListener* listener)
        {
            rate = new Knob{ {   5, 155,   5 + 65, 155 + 40 } };
            amnt = new Knob{ {  70, 155,  70 + 65, 155 + 40 } };
            posi = new Knob{ { 135, 155, 135 + 65, 155 + 40 } };
            offs = new Knob{ { 200, 155, 200 + 65, 155 + 40 } };
            shpr = new Knob{ { 265, 155, 265 + 65, 155 + 40 } };
            sync = new Knob{ { 285, 128, 285 + 45, 128 + 20 } };
            curve = new WaveformView{ {  5,  30, 5 + 325, 30 + 95 } };

            for (int i = 0; i < 5; i++)
            {
                modt[i] = new Knob{ { 0, 0, 1, 1 } };
                modt[i]->setListener(listener);
                modt[i]->setTag(Params::LFO1M1 + index + (i * 10));

                moda[i] = new Knob{ { 5. + i * 65, 155,   5. + i * 65 + 65, 155 + 40 } };
                moda[i]->setListener(listener);
                moda[i]->setTag(Params::LFO1M1A + index + (i * 10));
                moda[i]->name = "";
                moda[i]->min = -100;
                moda[i]->max = 100;
                moda[i]->reset = 0;
                moda[i]->decimals = 1;
                moda[i]->unit = " %";
                moda[i]->type = 2;
                moda[i]->setVisible(false);
                moda[i]->setListener(listener);

                mod[i] = new OptionMenu{ { 5. + i * 65, 155,   5. + i * 65 + 65, 155 + 20 }, listener, Params::LFO1M1 + index + (i * 10), nullptr, nullptr, OptionMenu::kCheckStyle };
                mod[i]->setNbItemsPerColumn(32);
                mod[i]->setFrameColor({ 0, 0, 0, 0 });
                mod[i]->setBackColor({ 0, 0, 0, 0 });
                mod[i]->setFontColor({ 128, 128, 128, 255 });
                mod[i]->center = false;
                mod[i]->addEntry("None");
                mod[i]->setVisible(false);
                mod[i]->registerControlListener(this);
                int _offset = 0;
                for (int j = 0; j < Params::ModCount; j++)
                {
                    int _index = ParamOrder[j + _offset];
                    if (_index == -1)
                        mod[i]->addSeparator(), ++_offset, _index = ParamOrder[j + _offset];
                    mod[i]->addEntry(ParamNames[_index].name);
                }
            }

            lfol = new Label{ {  85, 130,  85 + 80, 130 + 20 } };
            lfol->fontsize = 14;
            lfol->center = true;
            lfol->color = { 200, 200, 200, 255 };
            lfol->value = "LFO";
            modu = new Label{ { 165, 130, 165 + 80, 130 + 20 } };
            modu->fontsize = 14;
            modu->center = true;
            modu->color = { 128, 128, 128, 255 };
            modu->value = "Mod";

            lfo1 = new Label{ {   5,   5,   5 + 65,   5 + 20 } };
            lfo1->fontsize = 14;
            lfo1->center = true;
            lfo1->color = { 200, 200, 200, 255 };
            lfo1->value = "LFO 1";
            lfo2 = new Label{ {  70,   5,  70 + 65,   5 + 20 } };
            lfo2->fontsize = 14;
            lfo2->center = true;
            lfo2->color = { 128, 128, 128, 255 };
            lfo2->value = "LFO 2";
            lfo3 = new Label{ { 135,   5, 135 + 65,   5 + 20 } };
            lfo3->fontsize = 14;
            lfo3->center = true;
            lfo3->color = { 128, 128, 128, 255 };
            lfo3->value = "LFO 3";
            lfo4 = new Label{ { 200,   5, 200 + 65,   5 + 20 } };
            lfo4->fontsize = 14;
            lfo4->center = true;
            lfo4->color = { 128, 128, 128, 255 };
            lfo4->value = "LFO 4";
            lfo5 = new Label{ { 265,   5, 265 + 65,   5 + 20 } };
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

            for (int i = 0; i < 5; i++)
                mod[i]->setTag(Params::LFO1M1 + index + (i * 10)), moda[i]->setTag(Params::LFO1M1A + index + (i * 10));

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

            addView(rate);
            addView(amnt);
            addView(posi);
            addView(offs);
            addView(shpr);
            addView(sync);

            addView(curve);

            addView(lfol);
            addView(modu);

            addView(lfo1);
            addView(lfo2);
            addView(lfo3);
            addView(lfo4);
            addView(lfo5);

            for (int i = 0; i < 5; i++)
                addView(moda[i]), addView(mod[i]);
        }

        LFOView(const CRect& size, int index, IControlListener* listener)
            : CViewContainer(size), index(index)
        {
            createControls(listener);
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
            auto* _value = new LFOView(_size, _index, description->getControlListener(""));
            apply(_value, attributes, description);
            return _value;
        }
    };

    static inline LFOViewFactory lfoViewFactory;
}