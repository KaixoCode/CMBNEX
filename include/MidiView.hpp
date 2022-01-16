#pragma once
#include "pch.hpp"
#include "myplugincids.hpp"
#include "Modules.hpp"
#include "Knob.hpp"
#include "Label.hpp"
#include "WaveformView.hpp"

namespace Kaixo
{
    class MidiView : public CViewContainer
    {
    public:
        /*
         * Midi:
         *  - TPose
         *  - Bend
         *  - Glide
         *  - Rtg
         *  - Oversampling
         *  - Osc<Vel
         *  - Freq<Key
         *  - Freq<Vel
         */

        Knob* tnsp = nullptr;
        Knob* bend = nullptr;
        Knob* glde = nullptr;
        Knob* retr = nullptr;
        Knob* ovsm = nullptr;
        Knob* pann = nullptr;
        Knob* time = nullptr;
        Knob* clip = nullptr;

        Knob* mac1 = nullptr;
        Knob* mac2 = nullptr;
        Knob* mac3 = nullptr;
        Knob* mac4 = nullptr;
        Knob* mac5 = nullptr;

        DragThing* mcd1;
        DragThing* mcd2;
        DragThing* mcd3;
        DragThing* mcd4;
        DragThing* mcd5;

        DragThing* keym;
        DragThing* velm;
    
        Label* ovsl;
        Label* rtrl;
        Label* clpl;

        Label* keyl;
        Label* vell;

        int index = 0;

        void createControls(IControlListener* listener, MyEditor* editor)
        {
            tnsp = new Knob{ { 135,  5, 135 +  65,  5 + 50 }, editor };
            bend = new Knob{ { 200,  5, 200 +  65,  5 + 50 }, editor };
            pann = new Knob{ { 265,  5, 265 +  65,  5 + 50 }, editor };
            glde = new Knob{ {  70,  5,  70 +  65,  5 + 50 }, editor };
            time = new Knob{ {   5,  5,   5 +  70,  5 + 50 }, editor };
            clip = new Knob{ {  70, 79 + 85,  70 + 128, 79 + 20 + 85}, editor };
            retr = new Knob{ { 200, 79 + 85, 200 +  20, 79 + 20 + 85}, editor };
            ovsm = new Knob{ { 225, 79 + 85, 225 + 108, 79 + 20 + 85}, editor };

            mac1 = new Knob{ {   5, 60,   5 + 65, 60 + 88 }, editor };
            mac2 = new Knob{ {  70, 60,  70 + 65, 60 + 88 }, editor };
            mac3 = new Knob{ { 135, 60, 135 + 65, 60 + 88 }, editor };
            mac4 = new Knob{ { 200, 60, 200 + 65, 60 + 88 }, editor };
            mac5 = new Knob{ { 265, 60, 265 + 65, 60 + 88 }, editor };

            mcd1 = new DragThing{ {   5, 61,   5 + 60, 61 + 15 } };
            mcd2 = new DragThing{ {  70, 61,  70 + 60, 61 + 15 } };
            mcd3 = new DragThing{ { 135, 61, 135 + 60, 61 + 15 } };
            mcd4 = new DragThing{ { 200, 61, 200 + 60, 61 + 15 } };
            mcd5 = new DragThing{ { 265, 61, 265 + 60, 61 + 15 } };

            keym = new DragThing{ {  5, 62 + 85,   5 + 60, 62 + 15 + 85 } };
            velm = new DragThing{ {  5, 83 + 85,   5 + 60, 83 + 15 + 85 } };

            keym->source = ModSources::Key;
            velm->source = ModSources::Vel;

            mcd1->source = ModSources::Mac1;
            mcd2->source = ModSources::Mac2;
            mcd3->source = ModSources::Mac3;
            mcd4->source = ModSources::Mac4;
            mcd5->source = ModSources::Mac5;

            keyl = new Label{ { 11, 61 + 85,  11 + 60, 61 + 20 + 85 } };
            keyl->fontsize = 14;
            keyl->center = false;
            keyl->value = "Key";

            vell = new Label{ { 11, 82 + 85,  11 + 60, 82 + 20 + 85 } };
            vell->fontsize = 14;
            vell->center = false;
            vell->value = "Velocity";

            ovsl = new Label{ { 226, 59 + 85, 226 + 90, 59 + 20 + 85 } };
            ovsl->fontsize = 14;
            ovsl->center = false;
            ovsl->value = "Oversample";

            clpl = new Label{ {  71, 59 + 85,  71 + 65, 59 + 20 + 85 } };
            clpl->fontsize = 14;
            clpl->center = false;
            clpl->value = "Clipping";

            rtrl = new Label{ { 201, 59 + 85, 201 + 20, 59 + 20 + 85 } };
            rtrl->fontsize = 14;
            rtrl->center = false;
            rtrl->value = "Rtr";

            tnsp->setListener(listener);
            bend->setListener(listener);
            glde->setListener(listener);
            retr->setListener(listener);
            ovsm->setListener(listener);
            pann->setListener(listener);
            time->setListener(listener);
            clip->setListener(listener);
            mac1->setListener(listener);
            mac2->setListener(listener);
            mac3->setListener(listener);
            mac4->setListener(listener);
            mac5->setListener(listener);

            mac1->modable = false;
            mac2->modable = false;
            mac3->modable = false;
            mac4->modable = false;
            mac5->modable = false;

            tnsp->setTag(Params::Transpose);
            bend->setTag(Params::Bend);
            glde->setTag(Params::Glide);
            retr->setTag(Params::Retrigger);
            ovsm->setTag(Params::Oversample);
            pann->setTag(Params::Panning);
            time->setTag(Params::Time);
            clip->setTag(Params::Clipping);
            mac1->setTag(Params::Macro1);
            mac2->setTag(Params::Macro2);
            mac3->setTag(Params::Macro3);
            mac4->setTag(Params::Macro4);
            mac5->setTag(Params::Macro5);

            tnsp->name = "Transp";    bend->name = "Bend";
            tnsp->min = -48;          bend->min = 0;
            tnsp->max = 48;           bend->max = 24;
            tnsp->reset = 0;          bend->reset = 2;
            tnsp->decimals = 1;       bend->decimals = 1;
            tnsp->unit = " st";       bend->unit = " st";
            tnsp->type = 2;           bend->type = 2;
            
            glde->name = "Glide";
            glde->min = 0;
            glde->max = 10000;
            glde->reset = 0;
            glde->decimals = 1;
            glde->unit = " s";
            glde->type = 2;

            retr->name = "R";        ovsm->name = "Off,2x,4x,8x";
            retr->min = 0;           ovsm->min = 4;
            retr->max = 100;         ovsm->max = 4;
            retr->reset = 0;         ovsm->reset = 1;
            retr->decimals = 1;      ovsm->decimals = 1;
            retr->unit = "";         ovsm->unit = "";
            retr->type = 3;          ovsm->type = 4;

            pann->name = "Pan";      time->name = "Time";
            pann->min = -50;         time->min = -100;
            pann->max = 50;          time->max = 100;
            pann->reset = 0;         time->reset = 0;
            pann->decimals = 1;      time->decimals = 1;
            pann->unit = "pan";      time->unit = " %";
            pann->type = 2;          time->type = 2;

            clip->name = "Warm,Hard,Clip";
            clip->min = 4;
            clip->max = 3;
            clip->reset = 1;
            clip->decimals = 1;
            clip->unit = "";
            clip->type = 4;

            mac1->name = "Macro P"; mac2->name = "Macro Q"; mac3->name = "Macro R"; 
            mac1->min = 0;          mac2->min = 0;          mac3->min = 0;          
            mac1->max = 100;        mac2->max = 100;        mac3->max = 100;        
            mac1->reset = 0;        mac2->reset = 0;        mac3->reset = 0;       
            mac1->decimals = 1;     mac2->decimals = 1;     mac3->decimals = 1;     
            mac1->unit = "%";       mac2->unit = "%";       mac3->unit = "%";       
            mac1->type = 0;         mac2->type = 0;         mac3->type = 0;         

            mac4->name = "Macro S"; mac5->name = "Macro T";
            mac4->min = 0;          mac5->min = 0;         
            mac4->max = 100;        mac5->max = 100;       
            mac4->reset = 0;        mac5->reset = 0;      
            mac4->decimals = 1;     mac5->decimals = 1;    
            mac4->unit = "%";       mac5->unit = "%";      
            mac4->type = 0;         mac5->type = 0;        

            addView(tnsp);
            addView(bend);
            addView(glde);
            addView(retr);
            addView(ovsm);
            addView(pann);
            addView(time);
            addView(clip);
            addView(mac1);
            addView(mac2);
            addView(mac3);
            addView(mac4);
            addView(mac5);

            addView(mcd1);
            addView(mcd2);
            addView(mcd3);
            addView(mcd4);
            addView(mcd5);

            addView(ovsl);
            addView(rtrl);
            addView(clpl);

            addView(keyl);
            addView(vell);

            addView(keym);
            addView(velm);
        }

        MidiView(const CRect& size, int index, IControlListener* listener, MyEditor* editor)
            : CViewContainer(size), index(index)
        {
            createControls(listener, editor);
            setBackgroundColor({ 23, 23, 23, 255 });
        }

        void draw(CDrawContext* pContext) override
        {
            CViewContainer::draw(pContext);
        }
    };

    struct MidiAttributes
    {
        static inline CPoint Size = { 335, 100 };

        static inline auto Name = "Midi";
        static inline auto BaseView = UIViewCreator::kCViewContainer;

        static inline std::tuple Attributes
        {
            Attr{ "index", &MidiView::index },
        };
    };

    class MidiViewFactory : public ViewFactoryBase<MidiView, MidiAttributes>
    {
    public:
        CView* create(const UIAttributes& attributes, const IUIDescription* description) const override
        {
            CRect _size{ CPoint{ 45, 45 }, MidiAttributes::Size }; 
            int _index = 0;
            attributes.getIntegerAttribute("index", _index);
            MyEditor* _editor = dynamic_cast<MyEditor*>(description->getController());
            auto* _value = new MidiView(_size, _index, description->getControlListener(""), _editor);
            apply(_value, attributes, description);
            return _value;
        }
    };

    static inline MidiViewFactory midiViewFactory;
}