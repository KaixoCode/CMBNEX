#include "Components/DragThing.hpp"

namespace Kaixo
{
    CMouseEventResult DragThing::onMouseDown(CPoint& where, const CButtonState& buttons)
    {
        return kMouseEventHandled;
    }

    CMouseEventResult DragThing::onMouseMoved(CPoint& where, const CButtonState& buttons)
    {
        if (!dragging && getViewSize().pointInside(where))
        {
            dragging = true;
            int* _data = new int[1];
            _data[0] = (int)source;
            doDrag(DragDescription{ CDropSource::create((void*)_data, sizeof(int) * 1, IDataPackage::Type::kBinary) }, this);
            return kMouseEventHandled;
        }

        return kMouseEventNotHandled;
    }

    void DragThing::draw(CDrawContext* pContext)
    {
        auto a = getViewSize();
        pContext->setLineWidth(1);
        a.top += 1;
        a.left += 1;
        pContext->setFillColor({ 255, 255, 255, 16 });
        pContext->drawRect(a, kDrawFilled);
        a.right = a.left + 3;
        pContext->setFillColor(Colors::MainGreen);
        pContext->drawRect(a, kDrawFilled);
    }
}