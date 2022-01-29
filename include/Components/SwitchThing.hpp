#pragma once
#include "pch.hpp"

namespace Kaixo
{
    class SwitchThing : public CView, public IDropTarget
    {
    public:
        SwitchThing(const CRect& r)
            : CView(r)
        {
            setDropTarget(this);
        }

        CMouseEventResult onMouseDown(CPoint& where, const CButtonState& buttons) override
        {
            if (where.y - getViewSize().top > 0 && where.y - getViewSize().top < 30 && where.x - getViewSize().left < 335 && where.x - getViewSize().left > 0)
            {
                setIndex(std::floor((where.x - getViewSize().left) / 65));
            }

            return kMouseEventNotHandled;
        }

        std::function<void(int)> setIndex;

        DragOperation onDragEnter(DragEventData data)
        {
            if (data.pos.y - getViewSize().top > 0 && data.pos.y - getViewSize().top < 30
                && data.pos.x - getViewSize().left < 335 && data.pos.x - getViewSize().left > 0)
            {
                setIndex(std::floor((data.pos.x - getViewSize().left) / 65));
            }
            return DragOperation::None;
        };

        DragOperation onDragMove(DragEventData data)
        {
            if (data.pos.y - getViewSize().top > 0 && data.pos.y - getViewSize().top < 30
                && data.pos.x - getViewSize().left < 335 && data.pos.x - getViewSize().left > 0)
            {
                setIndex(std::floor((data.pos.x - getViewSize().left) / 65));
            }
            return DragOperation::None;
        };

        void onDragLeave(DragEventData data) {};

        bool onDrop(DragEventData data)
        {
            return false;
        };
    };
}