#pragma once
#include "pch.hpp"

namespace Kaixo
{
    // Simple drag source for a modulation source.
    class DragThing : public CView, public IDragCallback
    {
    public:
        using CView::CView;

        ModSources source; // Mod source of this drag thing

        bool dragging = false; // Currently dragging, don't edit manually!
        void dragWillBegin(IDraggingSession* session, CPoint pos) {};
        void dragMoved(IDraggingSession* session, CPoint pos) {};
        void dragEnded(IDraggingSession* session, CPoint pos, DragOperation result) { dragging = false; };

        CMouseEventResult onMouseDown(CPoint& where, const CButtonState& buttons) override;
        CMouseEventResult onMouseMoved(CPoint& where, const CButtonState& buttons) override;

        void draw(CDrawContext* pContext) override;
    };
}