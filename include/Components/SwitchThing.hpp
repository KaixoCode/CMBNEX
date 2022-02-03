#pragma once
#include "pch.hpp"

namespace Kaixo
{
    // Switch thing for LFO/Env, switches when pressed and drag/drop hover
    class SwitchThing : public CView, public VSTGUI::IDropTarget
    {
    public:
        SwitchThing(const CRect& r)
            : CView(r)
        { setDropTarget(this); }

        std::function<void(int)> setIndex;

        CMouseEventResult onMouseDown(CPoint& where, const CButtonState& buttons) override;
        DragOperation onDragEnter(DragEventData data);
        DragOperation onDragMove(DragEventData data);
        void onDragLeave(DragEventData data) {};
        bool onDrop(DragEventData data) { return false; };
    };
}