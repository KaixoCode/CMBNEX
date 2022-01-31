#pragma once
#include "pch.hpp"
#include "Components/Parameter.hpp"
#include "Components/Label.hpp"
#include "Components/DragThing.hpp"

namespace Kaixo
{
    class MidiView : public CViewContainer
    {
    public:
        Parameter *retr, *poly, *thrd, * gain,* tnsp, * bend, * glde, * time, * clip, * mac1, * mac2, * mac3, * mac4, * mac5;
        DragThing* mcd1, * mcd2, * mcd3, * mcd4, * mcd5, * keym, * velm, *ranm, *modm;
        Label * clpl, * keyl, * vell, *ranl, *modl;

        MidiView(const CRect& size, IControlListener* listener, MyEditor* editor);
        ~MidiView();
    };
}