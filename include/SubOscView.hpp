#pragma once
#include "pch.hpp"
#include "myplugincids.hpp"
#include "Modules.hpp"
#include "Knob.hpp"
#include "Label.hpp"
#include "WaveformView.hpp"

namespace Kaixo
{
    class SubOscView : public CViewContainer
    {
    public:
        Knob* gain; // Tune
        Knob* octv; // Pos
        Knob* ovrt; // Pos
        Label* titl;
        int index = 0;

        void createControls(IControlListener* listener, MyEditor* editor)
        {
            gain = new Knob{ {  10,  35,  10 + 50,  30 + 80 }, editor };
            octv = new Knob{ {   5, 110,   6 + 60, 110 + 40 }, editor };
            ovrt = new Knob{ {   5, 155,   5 + 60, 155 + 40 }, editor };
            
            titl = new Label{ { 0,   3, 0 + 70,   3 + 30 } };
            titl->fontsize = 24;
            titl->center = true;
            titl->color = { 200, 200, 200, 255 };
            titl->value = "S U B";

            gain->setListener(listener);
            octv->setListener(listener);
            ovrt->setListener(listener);

            gain->setTag(Params::SubGain);
            octv->setTag(Params::SubOct);
            ovrt->setTag(Params::SubOvertone);

            gain->name = "Gain"; octv->name = "Octave"; ovrt->name = "Overtone";
            gain->min = 0;       octv->min = -2;        ovrt->min = 0;     
            gain->max = 1;       octv->max = 2;         ovrt->max = 100;      
            gain->reset = 0;     octv->reset = 0;       ovrt->reset = 0;     
            gain->decimals = 1;  octv->decimals = 0;    ovrt->decimals = 1;  
            gain->unit = " dB";  octv->unit = "";       ovrt->unit = " %";  
            gain->type = 0;      octv->type = 2;        ovrt->type = 2;

            addView(gain);
            addView(octv);
            addView(ovrt);
            addView(titl);
        }

        SubOscView(const CRect& size, int index, IControlListener* listener, MyEditor* editor)
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

    struct SubOscAttributes
    {
        static inline CPoint Size = { 70, 200 };

        static inline auto Name = "SubOsc";
        static inline auto BaseView = UIViewCreator::kCViewContainer;

        static inline std::tuple Attributes
        {
            Attr{ "index", &SubOscView::index },
        };
    };

    class SubOscViewFactory : public ViewFactoryBase<SubOscView, SubOscAttributes>
    {
    public:
        CView* create(const UIAttributes& attributes, const IUIDescription* description) const override
        {
            CRect _size{ CPoint{ 45, 45 }, SubOscAttributes::Size };
            int _index = 0;
            attributes.getIntegerAttribute("index", _index);
            MyEditor* _editor = dynamic_cast<MyEditor*>(description->getController());
            auto* _value = new SubOscView(_size, _index, description->getControlListener(""), _editor);
            apply(_value, attributes, description);
            return _value;
        }
    };

    static inline SubOscViewFactory subOscViewFactory;
}