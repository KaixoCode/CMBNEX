#include "Components/SwitchThing.hpp"

namespace Kaixo
{
    CMouseEventResult SwitchThing::onMouseDown(CPoint& where, const CButtonState& buttons)
    {
        if (where.y - getViewSize().top > 0 && where.y - getViewSize().top < 30 && where.x - getViewSize().left < 335 && where.x - getViewSize().left > 0)
        {
            setIndex(std::floor((where.x - getViewSize().left) / 65));
        }

        return kMouseEventNotHandled;
    }

    DragOperation SwitchThing::onDragEnter(DragEventData data)
    {
        if (data.pos.y - getViewSize().top > 0 && data.pos.y - getViewSize().top < 30
            && data.pos.x - getViewSize().left < 335 && data.pos.x - getViewSize().left > 0)
        {
            setIndex(std::floor((data.pos.x - getViewSize().left) / 65));
        }
        return DragOperation::None;
    };

    DragOperation SwitchThing::onDragMove(DragEventData data)
    {
        if (data.pos.y - getViewSize().top > 0 && data.pos.y - getViewSize().top < 30
            && data.pos.x - getViewSize().left < 335 && data.pos.x - getViewSize().left > 0)
        {
            setIndex(std::floor((data.pos.x - getViewSize().left) / 65));
        }
        return DragOperation::None;
    };
}