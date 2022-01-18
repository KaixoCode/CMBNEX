#pragma once
#include "pch.hpp"
#include "myplugincids.hpp"
#include "Modules.hpp"
#include "Knob.hpp"
#include "Label.hpp"

namespace Kaixo
{
    class TopBarView : public CViewContainer
    {
    public:
        int index = 0;
        Knob* gain;

        void createControls(IControlListener* listener, MyEditor* editor)
        {
            constexpr CColor main{ 0, 179, 98, 255 };
            constexpr CColor text{ 200, 200, 200, 255 };
            constexpr CColor back{ 40, 40, 40, 255 };
            constexpr CColor brdr{ 30, 30, 30, 255 };
            constexpr CColor offt{ 128, 128, 128, 255 };

            gain = new Knob{ {   5,  5,   5 + 65,  5 + 100 }, editor };

            gain->setListener(listener);

            gain->setTag(Params::GainZ);


            gain->name = "L,H,B";
            gain->min = 4;
            gain->max = 3;
            gain->reset = 1;
            gain->decimals = 0;
            gain->unit = "";
            gain->type = 4;

            addView(gain);
        }

        TopBarView(const CRect& size, int index, IControlListener* listener, MyEditor* editor)
            : CViewContainer(size), index(index)
        {
            createControls(listener, editor);
            setBackgroundColor({ 23, 23, 23, 255 });
        }
    };

    struct TopBarAttributes
    {
        static inline CPoint Size = { 140, 110 };

        static inline auto Name = "TopBar";
        static inline auto BaseView = UIViewCreator::kCViewContainer;

        static inline std::tuple Attributes
        {
            Attr{ "index", &TopBarView::index },
        };
    };

    class TopBarViewFactory : public ViewFactoryBase<TopBarView, TopBarAttributes>
    {
    public:
        CView* create(const UIAttributes& attributes, const IUIDescription* description) const override
        {
            CRect _size{ CPoint{ 45, 45 }, TopBarAttributes::Size };
            int _index = 0;
            attributes.getIntegerAttribute("index", _index);
            MyEditor* _editor = dynamic_cast<MyEditor*>(description->getController());
            auto* _value = new TopBarView(_size, _index, description->getControlListener(""), _editor);
            apply(_value, attributes, description);
            return _value;
        }
    };

    static inline TopBarViewFactory topBarViewFactory; 
}
