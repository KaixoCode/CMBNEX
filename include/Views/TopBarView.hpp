#pragma once
#include "pch.hpp"
#include "Components/Parameter.hpp"
#include "Components/Label.hpp"
#include "Views/PresetView.hpp"

namespace Kaixo
{
    class TopBarView : public CViewContainer
    {
    public:

        // Simple button class
        class Button : public CView
        {
        public:
            using CView::CView;

            BackgroundEffect bgef{ {.size = getViewSize() } };
            std::function<void(void)> press;
            bool p = false;
            bool really = false;
            String text;

            std::chrono::steady_clock::time_point pressed;

            CMouseEventResult onMouseExited(CPoint& where, const CButtonState& buttons) override;
            CMouseEventResult onMouseDown(CPoint& where, const CButtonState& buttons) override;
            CMouseEventResult onMouseUp(CPoint& where, const CButtonState& buttons) override;

            void draw(CDrawContext* pContext) override;
        };

        // Simple class to display the name of a preset.
        class NameDisplay : public CView
        {
        public:
            NameDisplay(const CRect& c, String& name)
                : CView(c), name(name)
            {}

            std::function<void(void)> press;
            String& name;

            CMouseEventResult onMouseDown(CPoint& where, const CButtonState& buttons) override;

            void draw(CDrawContext* pContext) override;
        };

        // Overlay for the entire top-bar
        class Overlay : public CView
        {
        public:
            using CView::CView;

            void draw(CDrawContext* pContext) override;
        };

        CNewFileSelector* selector;
        CNewFileSelector* loader;

        NameDisplay* nameDisplay;
        Overlay* ovrl;
        Button* bSave;
        Button* bInit;

        MyEditor* editor;
        
        CMessageResult notify(CBaseObject* sender, IdStringPtr message) override;

        PresetView* prst;
        TopBarView(const CRect& size, IControlListener* listener, MyEditor* editor, PresetView* prst);
    };
}
