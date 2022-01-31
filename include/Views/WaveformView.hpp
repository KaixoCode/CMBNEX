#pragma once
#include "pch.hpp"
#include "Processing/Modules.hpp"
#include "Components/BackgroundEffect.hpp"
#include "Components/Parameter.hpp"
#include "Components/Label.hpp"

namespace Kaixo
{
    // Display waveform! uses actual Oscillator instance for accuracy.
    class WaveformView : public CView
    {
    public:
        using CView::CView;

        double repeat = 1;

        bool enabled = true;

        double offset = 0.5;
        double level = 1;
        double phase = 0;
        bool enableFold = false;
        double fold = 0;
        double bias = 0;
        bool enableDrive = false;
        double drive = 0;
        double drivegain = 0;
        double noise = 0;

        Oscillator osc;

        double ValueAt(double p, double step);

        void draw(CDrawContext* pContext) override;
    };
}