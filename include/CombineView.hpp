#pragma once
#include "pch.hpp"
#include "myplugincids.hpp"
#include "Modules.hpp"
#include "Knob.hpp"
#include "Label.hpp"
#include "OptionMenu.hpp"

namespace Kaixo
{
    class CombineView : public CViewContainer
    {
    public:
        int index = 0;
        Knob* dcfr;
        Knob* gain;
        Knob* mode;
        Knob* fltr; // Filter type
        Knob* freq; // Filter Frequency
        Knob* reso; // Filter Resonance
        Label* titl;
        Label* inp1;
        Label* inp2;
        Label* ftrl; // Filter title

        void createControls(IControlListener* listener)
        {
            constexpr CColor main{ 0, 179, 98, 255 };
            constexpr CColor text{ 200, 200, 200, 255 };
            constexpr CColor back{ 40, 40, 40, 255 };
            constexpr CColor brdr{ 30, 30, 30, 255 };
            constexpr CColor offt{ 128, 128, 128, 255 };

            gain = new Knob{ {   5,  35,   5 +  65,  35 + 90 } };
            dcfr = new Knob{ { 105, 130, 105 +  30, 130 + 20 } };
            mode = new Knob{ {   5, 130,   5 +  95, 130 + 20 } };
            fltr = new Knob{ {  69, 175,  69 +  70, 175 + 20 } };
            freq = new Knob{ {  70,  35,  70 +  65,  35 + 90 } };
            reso = new Knob{ {   5, 155,   5 +  65, 155 + 40 } };

            inp1 = new Label{ { 5, 8, 5 + 50, 8 + 30 } };
            inp1->fontsize = 16;
            inp1->center = true;
            inp1->color = { 128, 128, 128, 255 };
            inp1->value = index == 0 ? "I" : index == 1 ? "III" : "V";

            inp2 = new Label{ { 85, 8, 85 + 50, 8 + 30 } };
            inp2->fontsize = 16;
            inp2->center = true;
            inp2->color = { 128, 128, 128, 255 };
            inp2->value = index == 0 ? "II" : index == 1 ? "IV" : "VI";

            titl = new Label{ { 5, 3, 5 + 130, 3 + 30 } };
            titl->fontsize = 24;
            titl->center = true;
            titl->color = { 200, 200, 200, 255 };
            titl->value = index == 0 ? "X" : index == 1 ? "Y" : "Z";

            ftrl = new Label{ { 70, 155, 70 + 65, 155 + 20 } };
            ftrl->fontsize = 14;
            ftrl->center = false;
            ftrl->color = { 200, 200, 200, 255 };
            ftrl->value = "Filter";

            gain->setListener(listener);
            dcfr->setListener(listener);
            mode->setListener(listener);
            fltr->setListener(listener);
            freq->setListener(listener);
            reso->setListener(listener);

            gain->setTag(Params::GainX + index);
            dcfr->setTag(Params::DCX + index);
            mode->setTag(Params::ModeX + index);
            fltr->setTag(Params::FilterX + index);
            freq->setTag(Params::FreqX + index);
            reso->setTag(Params::ResoX + index);

            gain->name = "Gain";     dcfr->name = "DC";
            gain->min = 0;           dcfr->min = 0;
            gain->max = 1;           dcfr->max = 1;
            gain->reset = 1;         dcfr->reset = 0;
            gain->decimals = 1;      dcfr->decimals = 0;
            gain->unit = " dB";      dcfr->unit = "";
            gain->type = 0;          dcfr->type = 3;

            fltr->name = "L,H,B";    freq->name = "Freq";     reso->name = "Reso";
            fltr->min = 4;           freq->min = 20;          reso->min = 0;
            fltr->max = 3;           freq->max = 22000;       reso->max = 125;
            fltr->reset = 1;         freq->reset = 22000;     reso->reset = 0;
            fltr->decimals = 0;      freq->decimals = 1;      reso->decimals = 1;
            fltr->unit = "";         freq->unit = " Hz";      reso->unit = " %";
            fltr->type = 4;          freq->type = 0;          reso->type = 2;

            mode->parts = { "Add", "Min", "Mult", "Pong", "Max", "Mod", "And", "Inlv", "Or", "Xor" };
            mode->min = 0;
            mode->max = 1;
            mode->reset = 0;
            mode->decimals = 1;
            mode->unit = "";
            mode->type = 5;

            addView(gain);
            addView(fltr);
            addView(freq);
            addView(reso);
            addView(dcfr);
            addView(mode);
            addView(titl);
            addView(ftrl);

            addView(inp1);
            addView(inp2);
        }

        CombineView(const CRect& size, int index, IControlListener* listener)
            : CViewContainer(size), index(index)
        {
            createControls(listener);
            setBackgroundColor({ 23, 23, 23, 255 });
        }
    };

    struct CombineAttributes
    {
        static inline CPoint Size = { 140, 200 };

        static inline auto Name = "Combine";
        static inline auto BaseView = UIViewCreator::kCViewContainer;

        static inline std::tuple Attributes
        {
            Attr{ "index", &CombineView::index },
        };
    };

    class CombineViewFactory : public ViewFactoryBase<CombineView, CombineAttributes>
    {
    public:
        CView* create(const UIAttributes& attributes, const IUIDescription* description) const override
        {
            CRect _size{ CPoint{ 45, 45 }, CombineAttributes::Size };
            int _index = 0;
            attributes.getIntegerAttribute("index", _index);
            auto* _value = new CombineView(_size, _index, description->getControlListener(""));
            apply(_value, attributes, description);
            return _value;
        }
    };

    static inline CombineViewFactory combineViewFactory;

    class CombineModeView : public CViewContainer
    {
    public:
        int index = 0;
        Knob* destA;
        Knob* destB;
        Knob* destC;
        Knob* destD;
        Knob* destX;
        Knob* destY;

        Label* lablA;
        Label* lablB;
        Label* lablC;
        Label* lablD;
        Label* lablX;
        Label* lablY;

        Label* titl;

        void createControls(IControlListener* listener)
        {
            destA = new Knob{ { 0 * 24 + 5,  35, 0 * 24 + 5 + 20,  35 + 145 } };
            destB = new Knob{ { 1 * 24 + 5,  35, 1 * 24 + 5 + 20,  35 + 145 } };
            destC = new Knob{ { 2 * 24 + 5,  35, 2 * 24 + 5 + 20,  35 + 145 } };
            destD = new Knob{ { 3 * 24 + 5,  35, 3 * 24 + 5 + 20,  35 + 145 } };
            destX = new Knob{ { 4 * 24 + 5,  83, 4 * 24 + 5 + 20,  83 +  97 } };
            destY = new Knob{ { 5 * 24 + 5, 131, 5 * 24 + 5 + 20, 131 +  49 } };

            lablA = new Label{ { 0 * 24 + 5, 178, 0 * 24 + 5 + 20, 178 + 25 } };
            lablB = new Label{ { 1 * 24 + 5, 178, 1 * 24 + 5 + 20, 178 + 25 } };
            lablC = new Label{ { 2 * 24 + 5, 178, 2 * 24 + 5 + 20, 178 + 25 } };
            lablD = new Label{ { 3 * 24 + 5, 178, 3 * 24 + 5 + 20, 178 + 25 } };
            lablX = new Label{ { 4 * 24 + 5, 178, 4 * 24 + 5 + 20, 178 + 25 } };
            lablY = new Label{ { 5 * 24 + 5, 178, 5 * 24 + 5 + 20, 178 + 25 } };

            titl = new Label{ { 5, 3, 5 + 140, 3 + 30 } };
            titl->fontsize = 24;
            titl->center = true;
            titl->color = { 200, 200, 200, 255 };
            titl->value = "M A T R I X";

            lablA->value = "A";
            lablA->fontsize = 14;
            lablA->center = true;
            lablB->value = "B";
            lablB->fontsize = 14;
            lablB->center = true;
            lablC->value = "C";
            lablC->fontsize = 14;
            lablC->center = true;
            lablD->value = "D";
            lablD->fontsize = 14;
            lablD->center = true;
            lablX->value = "X";
            lablX->fontsize = 14;
            lablX->center = true;
            lablY->value = "Y";
            lablY->fontsize = 14;
            lablY->center = true;

            destA->setListener(listener);
            destB->setListener(listener);
            destC->setListener(listener);
            destD->setListener(listener);
            destX->setListener(listener);
            destY->setListener(listener);

            destA->setTag(Params::DestA);
            destB->setTag(Params::DestB);
            destC->setTag(Params::DestC);
            destD->setTag(Params::DestD);
            destX->setTag(Params::DestX);
            destY->setTag(Params::DestY);

            destA->name = "I,II,III,IV,V,VI";      destB->name = "I,II,III,IV,V,VI";      destC->name = "I,II,III,IV,V,VI";
            destA->min = 4;                   destB->min = 4;                   destC->min = 4;
            destA->max = 6;                   destB->max = 6;                   destC->max = 6;
            destA->reset = 0;                 destB->reset = 0;                 destC->reset = 0;
            destA->decimals = 0;              destB->decimals = 0;              destC->decimals = 0;
            destA->unit = "";                 destB->unit = "";                 destC->unit = "";
            destA->type = 6;                  destB->type = 6;                  destC->type = 6;
                                                                                
            destD->name = "I,II,III,IV,V,VI"; destX->name = "III,IV,V,VI";      destY->name = "V,VI";
            destD->min = 4;                   destX->min = 4;                   destY->min = 4;
            destD->max = 6;                   destX->max = 4;                   destY->max = 2;
            destD->reset = 0;                 destX->reset = 0;                 destY->reset = 0;
            destD->decimals = 0;              destX->decimals = 0;              destY->decimals = 0;
            destD->unit = "";                 destX->unit = "";                 destY->unit = "";
            destD->type = 6;                  destX->type = 6;                  destY->type = 6;

            addView(destA);
            addView(destB);
            addView(destC);
            addView(destD);
            addView(destX);
            addView(destY);

            addView(lablA);
            addView(lablB);
            addView(lablC);
            addView(lablD);
            addView(lablX);
            addView(lablY);

            addView(titl);
        }

        CombineModeView(const CRect& size, int index, IControlListener* listener)
            : CViewContainer(size), index(index)
        {
            createControls(listener);
            setBackgroundColor({ 23, 23, 23, 255 });
        }
    };

    struct CombineModeAttributes
    {
        static inline CPoint Size = { 150, 200 };

        static inline auto Name = "CombineMode";
        static inline auto BaseView = UIViewCreator::kCViewContainer;

        static inline std::tuple Attributes
        {
            Attr{ "index", &CombineModeView::index },
        };
    };

    class CombineModeViewFactory : public ViewFactoryBase<CombineModeView, CombineModeAttributes>
    {
    public:
        CView* create(const UIAttributes& attributes, const IUIDescription* description) const override
        {
            CRect _size{ CPoint{ 45, 45 }, CombineModeAttributes::Size };
            int _index = 0;
            attributes.getIntegerAttribute("index", _index);
            auto* _value = new CombineModeView(_size, _index, description->getControlListener(""));
            apply(_value, attributes, description);
            return _value;
        }
    };

    static inline CombineModeViewFactory combineModeViewFactory;
}