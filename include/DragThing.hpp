#pragma once
#include "pch.hpp"
#include "myplugincids.hpp"
#include "Modules.hpp"
#include "ViewFactoryBase.hpp"

namespace Kaixo
{
    class DragThing : public CView, public IDragCallback
    {
    public:
        using CView::CView;

        ModSources source;

        bool dragging = false;
        void dragWillBegin(IDraggingSession* session, CPoint pos) {};
        void dragMoved(IDraggingSession* session, CPoint pos) {};
        void dragEnded(IDraggingSession* session, CPoint pos, DragOperation result) { dragging = false; };


        CMouseEventResult onMouseDown(CPoint& where, const CButtonState& buttons) override
        {
            return kMouseEventHandled;
        }

        CMouseEventResult onMouseMoved(CPoint& where, const CButtonState& buttons) override
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

        void draw(CDrawContext* pContext) override
        {
            constexpr CColor back = KnobBack;
            constexpr CColor brdr = Border;
            auto a = getViewSize();
            pContext->setLineWidth(1);
            a.top += 1;
            a.left += 1;
            pContext->setFillColor({ 255, 255, 255, 16 });
            pContext->drawRect(a, kDrawFilled);
            a.right = a.left + 3;
            pContext->setFillColor(MainGreen);
            pContext->drawRect(a, kDrawFilled);
        }
    };
}