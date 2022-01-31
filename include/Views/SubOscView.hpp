#pragma once
#include "pch.hpp"
#include "Components/Parameter.hpp"
#include "Components/Label.hpp"

namespace Kaixo
{
    class SubOscView : public CViewContainer
    {
    public:
        Parameter* gain, * octv, * ovrt;

        SubOscView(const CRect& size, IControlListener* listener, MyEditor* editor);
        ~SubOscView();
    };
}