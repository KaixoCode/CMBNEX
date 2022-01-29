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
#include "Controller.hpp"

namespace Kaixo
{
    class MainView : public CViewContainer
    {
    public:
        TopBarView* tbrv;
        OscillatorPart* oscp;
        CombinePart* cmbn;
        LFOView* lfop;
        EnvelopeView* envp; 
        MidiView* midi;
        SubOscView* subo;
        BackgroundEffect* bgef;

        MainView(const CRect& size, IControlListener* listener, MyEditor* editor)
            : CViewContainer { size }
        {
            setBackgroundColor(Background);
            bgef = new BackgroundEffect{ { 745, 5, 745 + 335, 5 + 90 } };
            tbrv = new TopBarView{ { 5, 5, 740, 50 }, listener, editor };
            oscp = new OscillatorPart{ { 0, 0, 740, 505 }, listener, editor };
            cmbn = new CombinePart{ { 5, 510, 5 + 650, 510 + 215 }, listener, editor };
            lfop = new LFOView{ { 745, 315, 745 + 335, 315 + 190 }, listener, editor };
            envp = new EnvelopeView{ { 745, 100, 745 + 335, 100 + 210 }, listener, editor };
            midi = new MidiView{ { 745, 510, 745 + 335, 510 + 211 }, listener, editor };
            subo = new SubOscView{ { 655, 510, 655 + 85, 510 + 211 }, listener, editor };

            auto* _v = new CViewContainer{ { 0, 0, 1085, 727 } };
            _v->setBackgroundColor(Background);
            addView(_v);

            addView(bgef); addView(oscp); addView(cmbn); addView(lfop); addView(envp); 
            addView(midi); addView(subo); addView(tbrv);
        }
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
        CView* create(const UIAttributes& attributes, const IUIDescription* description) const override
        {
            CRect _size{ CPoint{ 45, 45 }, MainViewAttributes::Size };
            MyEditor* _editor = dynamic_cast<MyEditor*>(description->getController());
            auto* _value = new MainView(_size, description->getControlListener(""), _editor);
            apply(_value, attributes, description);
            return _value;
        }
    };

    static inline MainViewFactory mainViewFactory;
}