#pragma once
#include "pch.hpp"
#include "myplugincids.hpp"
#include "Modules.hpp"
#include "Knob.hpp"
#include "Label.hpp"
#include "myplugincontroller.hpp"

namespace Kaixo
{
    struct Dummy : public CControl
    {
        Dummy(size_t p, IControlListener* listener) 
            : CControl{ { 0, 0, 0, 0 }, listener, p }
        {}

        void draw(CDrawContext*) override {}

        CLASS_METHODS(Dummy, CControl)
    };

    class ModeView : public CScrollView, public IControlListener
    {
    public:
        int index = 0;

        std::vector<Dummy*> params;

        Label* name = new Label{ { 0, 0, 335, 410 } };
        size_t id = 0;

        void valueChanged(CControl* pControl) override
        {
            auto s = std::to_string(pControl->getTag());
            name->value = s.c_str();

            setDirty(true);
        }

        CMouseEventResult onMouseDown(CPoint& where, const CButtonState& buttons) override
        {
            auto s = std::to_string(++id);
            name->value = s.c_str();
            setDirty(true);
            return kMouseEventHandled;
        }

        CMouseEventResult onMouseUp(CPoint& where, const CButtonState& buttons) override
        {
            setBackgroundColor({ 23, 23, 23, 255 });
            return kMouseEventHandled;
        }

        void createControls(IController* controller)
        {
            auto* listener = controller->getControlListener("");
            MyEditor* _editor = dynamic_cast<MyEditor*>(controller);
            if (!_editor) throw nullptr;
            addView(name);
            params.reserve(Params::Size);
            for (size_t i = 0; i < Params::Size; i++)
            {
                auto* _k = new Dummy{ i, listener };
                params.push_back(_k);
                addView(_k);
                _k->registerControlListener(this);
            }
        }

        ModeView(const CRect& size, int index, IController* controller)
            : CScrollView(size, size, CScrollView::kAutoHideScrollbars |
                CScrollView::kVerticalScrollbar | CScrollView::kAutoDragScrolling | 
                CScrollView::kOverlayScrollbars | CScrollView::kDontDrawFrame), index(index)
        {
            createControls(controller);
            setBackgroundColor({ 23, 23, 23, 255 });
        }
    };

    struct ModeAttributes
    {
        static inline CPoint Size = { 335, 410 };

        static inline auto Name = "Mode";
        static inline auto BaseView = UIViewCreator::kCViewContainer;

        static inline std::tuple Attributes
        {
            Attr{ "index", &ModeView::index },
        };
    };

    class ModeViewFactory : public ViewFactoryBase<ModeView, ModeAttributes>
    {
    public:
        CView* create(const UIAttributes& attributes, const IUIDescription* description) const override
        {
            CRect _size{ CPoint{ 45, 45 }, ModeAttributes::Size };
            int _index = 0;
            attributes.getIntegerAttribute("index", _index);
            auto* _value = new ModeView(_size, _index, description->getController());
            apply(_value, attributes, description);
            return _value;
        }
    };

    static inline ModeViewFactory modeViewFactory;
}