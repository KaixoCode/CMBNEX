#pragma once
#include "pch.hpp"
#include "Views/OscillatorView.hpp"
#include "Views/EnvelopeView.hpp"
#include "Views/CombineView.hpp"
#include "Views/LFOView.hpp"
#include "Views/SubOscView.hpp"
#include "Views/MidiView.hpp"
#include "Views/TopBarView.hpp"
#include "Views/ViewFactoryBase.hpp"
#include "Views/PresetView.hpp"
#include "Controller.hpp"

namespace Kaixo
{
    class MainView : public CViewContainer
    {
    public:
        PresetView* prst;
        TopBarView* tbrv;
        OscillatorPart* oscp;
        CombinePart* cmbn;
        LFOView* lfop;
        EnvelopeView* envp; 
        MidiView* midi;
        SubOscView* subo;
        BackgroundEffect* bgef;

        MainView(const CRect& size, IControlListener* listener, MyEditor* editor);
    };

    struct MainViewAttributes
    {
        static inline CPoint Size = { 70, 200 };

        static inline auto Name = "MainPanel";
        static inline auto BaseView = UIViewCreator::kCViewContainer;

        static inline std::tuple Attributes{};
    };

    class MainViewFactory : public ViewFactoryBase<MainView, MainViewAttributes>
    {
    public:
        CView* create(const UIAttributes& attributes, const IUIDescription* description) const override;
    };

    static inline MainViewFactory mainViewFactory;
}