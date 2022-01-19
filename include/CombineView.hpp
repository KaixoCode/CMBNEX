#pragma once
#include "pch.hpp"
#include "myplugincids.hpp"
#include "Modules.hpp"
#include "Knob.hpp"
#include "Label.hpp"
#include "OptionMenu.hpp"

namespace Kaixo
{
    class CombineToggle : public CViewContainer
    {
    public:
        Label* ftrl = nullptr; // Filter title
        Knob* enbr = nullptr; // Enable Filter
        Knob* fltr = nullptr; // Filter type
        Knob* freq = nullptr; // Filter Frequency  
        Knob* reso = nullptr; // Filter Resonance  

        Knob* addm = nullptr;
        Knob* minm = nullptr;
        Knob* mulm = nullptr;
        Knob* ponm = nullptr;
        Knob* maxm = nullptr;
        Knob* modm = nullptr;
        Knob* andm = nullptr;
        Knob* inlm = nullptr;
        Knob* orrm = nullptr;
        Knob* xorm = nullptr;

        Label* fldl = nullptr;
        Knob* enbf = nullptr;
        Knob* fold = nullptr;
        Knob* bias = nullptr;

        Label* drvl = nullptr;
        Knob* enbd = nullptr;
        Knob* driv = nullptr;
        Knob* dgai = nullptr;

        Knob* dest = nullptr;

        Knob* dcof = nullptr;

        Label* dcol = nullptr;
        Label* outl = nullptr;

        int index = 0;

        CombineToggle(const CRect& size, int index, IControlListener* listener, MyEditor* editor)
            : CViewContainer(size), index(index)
        {
            setBackgroundColor({ 23, 23, 23, 0 });

            auto* v = new CViewContainer{ { 5, 0, getWidth() - 5, getHeight() } };
            v->setBackgroundColor({ 23, 23, 23, 255 });
            addView(v);

            CColor brdclr{ 70, 70, 70, 255 };
            if (index == 0)
            {
                auto* v = new CViewContainer{ { 0,   0, 5,   0 + 67 } };
                v->setBackgroundColor(brdclr);
                addView(v);


                outl = new Label{ { 380,  60, 380 + 30,  60 + 20 } };
                outl->fontsize = 14, outl->center = true, outl->value = "Out";
                dest = new Knob{ { 385,  83, 385 + 20,  83 + 97 + 24 }, editor };
                dest->name = "III,IV,V,VI,O";    
                dest->min = 4;                   
                dest->max = 5;                   
                dest->reset = 0;                 
                dest->decimals = 0;              
                dest->unit = "";                 
                dest->type = 6;

                dest->setListener(listener); dest->setTag(Params::DestX);
            }
            else if (index == 1)
            {
                auto* v = new CViewContainer{ { 0,  72, 5,  72 + 67 } };
                v->setBackgroundColor(brdclr);
                addView(v);


                outl = new Label{ { 380, 108, 380 + 30, 108 + 20 } };
                outl->fontsize = 14, outl->center = true, outl->value = "Out";
                dest = new Knob{ { 385, 131, 385 + 20, 131 + 49 + 24 }, editor };
                dest->name = "V,VI,O";
                dest->min = 4;
                dest->max = 3;
                dest->reset = 0;
                dest->decimals = 0;
                dest->unit = "";
                dest->type = 6;

                dest->setListener(listener); dest->setTag(Params::DestY);
            }
            else if (index == 2)
            {
                auto* v = new CViewContainer{ { 0, 144, 5, 144 + 67 } };
                v->setBackgroundColor(brdclr);
                addView(v);

                outl = nullptr;
                dest = nullptr;
            }

            CColor dkrclr{ 17, 17, 17, 255 };

            auto* v1 = new CViewContainer{ {  10, 5,  10 + 140, 5 + 201 } };
            v1->setBackgroundColor(dkrclr);
            addView(v1);

            auto* v2 = new CViewContainer{ { 155, 5, 155 + 70, 5 + 201 } };
            v2->setBackgroundColor(dkrclr);
            addView(v2);

            auto* v3 = new CViewContainer{ { 230, 5, 230 + 70, 5 + 201 } };
            v3->setBackgroundColor(dkrclr);
            addView(v3);

            auto* v4 = new CViewContainer{ { 305, 5, 305 + 70, 5 + 201 } };
            v4->setBackgroundColor(dkrclr);
            addView(v4);

            auto* v5 = new CViewContainer{ { 380, 5, 380 +  30, 5 + 201 } };
            v5->setBackgroundColor(dkrclr);
            addView(v5);

            dcol = new Label{ { 380,  10, 380 + 30,  10 + 20 } };
            dcol->fontsize = 14, dcol->center = true, dcol->value = "DC";
            dcof = new Knob{ { 385,  30, 385 + 20,  30 + 20 }, editor }; dcof->setListener(listener); dcof->setTag(Params::DCX + index);

            ftrl = new Label{ { 325,  8, 325 + 40,   8 + 18 } };
            ftrl->fontsize = 14, ftrl->center = false, ftrl->value = "Filter";
            enbr = new Knob{ { 309,   9, 309 + 14,   9 + 14 }, editor }; enbr->setListener(listener); enbr->setTag(Params::ENBFilterX + index);
            fltr = new Knob{ { 309,  27, 309 + 63,  27 + 17 }, editor }; fltr->setListener(listener); fltr->setTag(Params::FilterX + index);
            freq = new Knob{ { 310,  47, 310 + 60,  47 + 95 }, editor }; freq->setListener(listener); freq->setTag(Params::FreqX + index);
            reso = new Knob{ { 310, 148, 310 + 65, 148 + 53 }, editor }; reso->setListener(listener); reso->setTag(Params::ResoX + index);

            drvl = new Label{ { 250,  8, 250 + 40,   8 + 18 } };
            drvl->fontsize = 14, drvl->center = false, drvl->value = "Drive";
            enbd = new Knob{ { 234,   9, 234 + 14,   9 + 14 }, editor }; enbd->setListener(listener); enbd->setTag(Params::ENBDriveX + index);
            dgai = new Knob{ { 235,  47, 235 + 60,  47 + 95 }, editor }; dgai->setListener(listener); dgai->setTag(Params::DriveGainX + index);
            driv = new Knob{ { 235, 148, 235 + 65, 148 + 53 }, editor }; driv->setListener(listener); driv->setTag(Params::DriveAmtX + index);

            fldl = new Label{ { 175,  8, 175 + 40,   8 + 18 } };
            fldl->fontsize = 14, fldl->center = false, fldl->value = "Fold";
            enbf = new Knob{ { 159,   9, 159 + 14,   9 + 14 }, editor }; enbf->setListener(listener); enbf->setTag(Params::ENBFoldX + index);
            fold = new Knob{ { 160,  47, 160 + 60,  47 + 95 }, editor }; fold->setListener(listener); fold->setTag(Params::FoldX + index);
            bias = new Knob{ { 160, 148, 160 + 65, 148 + 53 }, editor }; bias->setListener(listener); bias->setTag(Params::BiasX + index);

            minm = new Knob{ { 15,  10 - 0 * 1, 15 + 65,  10 + 35 - 0 * 1 }, editor }; minm->setListener(listener); minm->setTag(Params::MinMixX + index);
            mulm = new Knob{ { 15,  50 - 1 * 1, 15 + 65,  50 + 35 - 1 * 1 }, editor }; mulm->setListener(listener); mulm->setTag(Params::MultMixX + index);
            ponm = new Knob{ { 15,  90 - 2 * 1, 15 + 65,  90 + 35 - 2 * 1 }, editor }; ponm->setListener(listener); ponm->setTag(Params::PongMixX + index); 
            maxm = new Knob{ { 15, 130 - 3 * 1, 15 + 65, 130 + 35 - 3 * 1 }, editor }; maxm->setListener(listener); maxm->setTag(Params::MaxMixX + index); 
            modm = new Knob{ { 15, 170 - 4 * 1, 15 + 65, 170 + 35 - 4 * 1 }, editor }; modm->setListener(listener); modm->setTag(Params::ModMixX + index); 
            andm = new Knob{ { 80,  10 - 0 * 1, 80 + 65,  10 + 35 - 0 * 1 }, editor }; andm->setListener(listener); andm->setTag(Params::AndMixX + index); 
            inlm = new Knob{ { 80,  50 - 1 * 1, 80 + 65,  50 + 35 - 1 * 1 }, editor }; inlm->setListener(listener); inlm->setTag(Params::InlvMixX + index); 
            orrm = new Knob{ { 80,  90 - 2 * 1, 80 + 65,  90 + 35 - 2 * 1 }, editor }; orrm->setListener(listener); orrm->setTag(Params::OrMixX + index); 
            xorm = new Knob{ { 80, 130 - 3 * 1, 80 + 65, 130 + 35 - 3 * 1 }, editor }; xorm->setListener(listener); xorm->setTag(Params::XOrMixX + index);
            addm = new Knob{ { 80, 170 - 4 * 1, 80 + 65, 170 + 35 - 4 * 1 }, editor }; addm->setListener(listener); addm->setTag(Params::AddMixX + index);

            enbr->name = "";    fltr->name = "L,H,B"; freq->name = "Freq"; reso->name = "Reso";
            enbr->min = 0;      fltr->min = 4;        freq->min = 20;      reso->min = 0;
            enbr->max = 1;      fltr->max = 3;        freq->max = 22000;   reso->max = 125;
            enbr->reset = 0;    fltr->reset = 1;      freq->reset = 0;     reso->reset = 0;
            enbr->decimals = 1; fltr->decimals = 0;   freq->decimals = 1;  reso->decimals = 1;
            enbr->unit = "";    fltr->unit = "";      freq->unit = " Hz";  reso->unit = " %";
            enbr->type = 3;     fltr->type = 4;       freq->type = 0;      reso->type = 2;
                        
            enbf->name = "";    fold->name = "Gain"; bias->name = "Bias";
            enbf->min = 0;      fold->min = 1;       bias->min = -1;
            enbf->max = 1;      fold->max = 15.9;    bias->max = 1;
            enbf->reset = 0;    fold->reset = 1;     bias->reset = 0;
            enbf->decimals = 1; fold->decimals = 1;  bias->decimals = 2;
            enbf->unit = "";    fold->unit = " dB";  bias->unit = "";
            enbf->type = 3;     fold->type = 0;      bias->type = 2;
                        
            enbd->name = "";    dgai->name = "Gain"; driv->name = "Drive";
            enbd->min = 0;      dgai->min = 1;       driv->min = 0;
            enbd->max = 1;      dgai->max = 4;     driv->max = 100;
            enbd->reset = 0;    dgai->reset = 1;     driv->reset = 0;
            enbd->decimals = 1; dgai->decimals = 1;  driv->decimals = 1;
            enbd->unit = "";    dgai->unit = " dB";  driv->unit = " %";
            enbd->type = 3;     dgai->type = 0;      driv->type = 2;

            minm->name = "Min"; mulm->name = "Mult"; ponm->name = "Pong"; maxm->name = "Max"; modm->name = "Mod";
            minm->min = 0;      mulm->min = 0;       ponm->min = 0;       maxm->min = 0;      modm->min = 0;
            minm->max = 1;      mulm->max = 1;       ponm->max = 1;       maxm->max = 1;      modm->max = 1;
            minm->reset = 0;    mulm->reset = 0;     ponm->reset = 0;     maxm->reset = 0;    modm->reset = 0;
            minm->decimals = 1; mulm->decimals = 1;  ponm->decimals = 1;  maxm->decimals = 1; modm->decimals = 1;
            minm->unit = "";    mulm->unit = "";     ponm->unit = "";     maxm->unit = "";    modm->unit = "";
            minm->type = 7;     mulm->type = 7;      ponm->type = 7;      maxm->type = 7;     modm->type = 7;

            andm->name = "And"; inlm->name = "Inlv"; orrm->name = "Or";  xorm->name = "XOr"; addm->name = "Add";
            andm->min = 0;      inlm->min = 0;       orrm->min = 0;      xorm->min = 0;      addm->min = 0;
            andm->max = 1;      inlm->max = 1;       orrm->max = 1;      xorm->max = 1;      addm->max = 1;
            andm->reset = 0;    inlm->reset = 0;     orrm->reset = 0;    xorm->reset = 0;    addm->reset = 0;
            andm->decimals = 1; inlm->decimals = 1;  orrm->decimals = 1; xorm->decimals = 1; addm->decimals = 1;
            andm->unit = "";    inlm->unit = "";     orrm->unit = "";    xorm->unit = "";    addm->unit = "";
            andm->type = 7;     inlm->type = 7;      orrm->type = 7;     xorm->type = 7;     addm->type = 7;

            dcof->name = "";
            dcof->min = 0;
            dcof->max = 1;
            dcof->reset = 0;
            dcof->decimals = 1;
            dcof->unit = "";
            dcof->type = 3;

            addView(ftrl);
            addView(enbr);
            addView(fltr);
            addView(freq);
            addView(reso);

            addView(minm);
            addView(mulm);
            addView(ponm);
            addView(maxm);
            addView(modm);
            addView(andm);
            addView(inlm);
            addView(orrm);
            addView(xorm);
            addView(addm);

            addView(drvl);
            addView(enbd);
            addView(dgai);
            addView(driv);
            addView(fldl);
            addView(enbf);
            addView(fold);
            addView(bias);

            if (dest) addView(dest);
            if (outl) addView(outl);

            addView(dcof);
            addView(dcol);
        }
    };

    class CombineView : public CViewContainer
    {
    public:
        int index = 0;
        Knob* gmix;
        Knob* gain;
        Knob* pgai;
        Label* titl;
        Label* inp1;
        Label* inp2;

        CombineToggle* toggle;

        bool selected = false;

        void Select()
        {
            if (!selected)
            {
                toggle->setVisible(true);
                //ovrl->color = { 17, 17, 17, 255 };
                //ovrl->setDirty(true);
            }
            selected = true;
        }

        void Unselect()
        {
            if (selected)
            {
                toggle->setVisible(false);
                //ovrl->color = { 128, 128, 128, 0 };
                //ovrl->setDirty(true);
            }
            selected = false;
        }

        CombineView(CombineToggle* toggle, const CRect& size, int index, IControlListener* listener, MyEditor* editor)
            : CViewContainer(size), toggle(toggle), index(index)
        {
            setBackgroundColor({ 23, 23, 23, 255 });

            constexpr CColor main{ 0, 179, 98, 255 };
            constexpr CColor text{ 200, 200, 200, 255 };
            constexpr CColor back{ 40, 40, 40, 255 };
            constexpr CColor brdr{ 30, 30, 30, 255 };
            constexpr CColor offt{ 128, 128, 128, 255 };

            pgai = new Knob{ {  35,  7,  35 + 65,  7 + 53 }, editor };
            gmix = new Knob{ { 100,  7, 100 + 65,  7 + 53 }, editor };
            gain = new Knob{ { 165,  7, 165 + 65,  7 + 53 }, editor };

            inp1 = new Label{ { 5, 1, 5 + 20, 1 + 14 } };
            inp1->fontsize = 14;
            inp1->center = true;
            inp1->color = { 128, 128, 128, 255 };
            inp1->value = index == 0 ? "I" : index == 1 ? "III" : "V";

            inp2 = new Label{ { 5, 48, 5 + 20, 48 + 14 } };
            inp2->fontsize = 14;
            inp2->center = true;
            inp2->color = { 128, 128, 128, 255 };
            inp2->value = index == 0 ? "II" : index == 1 ? "IV" : "VI";

            titl = new Label{ { 5, 19, 5 + 20, 19 + 24 } };
            titl->fontsize = 24;
            titl->center = true;
            titl->color = { 200, 200, 200, 255 };
            titl->value = index == 0 ? "X" : index == 1 ? "Y" : "Z";

            gain->setListener(listener);
            pgai->setListener(listener);
            gmix->setListener(listener);

            gain->setTag(Params::GainX + index);
            pgai->setTag(Params::PreGainX + index);
            gmix->setTag(Params::MixX + index);

            gain->name = "Gain"; pgai->name = "Pre-Gain"; gmix->name = "Mix"; 
            gain->min = 0;       pgai->min = 0;           gmix->min = 0;       
            gain->max = 1;       pgai->max = 1;           gmix->max = 100;       
            gain->reset = 1;     pgai->reset = 1;         gmix->reset = 100;     
            gain->decimals = 1;  pgai->decimals = 1;      gmix->decimals = 1;  
            gain->unit = " dB";  pgai->unit = " dB";      gmix->unit = " %";  
            gain->type = 2;      pgai->type = 2;          gmix->type = 2;      

            addView(gain);
            addView(pgai);
            addView(gmix);

            addView(titl);
            addView(inp1);
            addView(inp2);
        }
    };

    class CombinePart : public CViewContainer
    {
    public:
        CombineView* views[3];
        CombineToggle* toggles[3];

        CMouseEventResult onMouseDown(CPoint& where, const CButtonState& buttons) override
        {
            auto _r = CViewContainer::onMouseDown(where, buttons);
            CPoint where2(where);
            where2.offset(-getViewSize().left, -getViewSize().top);
            getTransform().inverse().transform(where2);
            void* selected = nullptr;
            for (auto& o : views)
            {
                if (o->getViewSize().pointInside(where2))
                {
                    o->Select();
                    selected = o;
                    break;
                }
            }

            if (selected)
            {
                for (auto& o : views)
                    if (o != selected) o->Unselect();
            }

            return _r;
        }

        CombinePart(const CRect& size, IControlListener* listener, MyEditor* editor)
            : CViewContainer(size)
        {
            setBackgroundColor({ 0, 0, 0, 0 });

            toggles[0] = new CombineToggle{ { 230, 0, 230 + 420, 0 + 211 }, 0, listener, editor };
            toggles[1] = new CombineToggle{ { 230, 0, 230 + 420, 0 + 211 }, 1, listener, editor };
            toggles[2] = new CombineToggle{ { 230, 0, 230 + 420, 0 + 211 }, 2, listener, editor };

            toggles[0]->setVisible(false);
            toggles[1]->setVisible(false);
            toggles[2]->setVisible(false);

            views[0] = new CombineView{ toggles[0], { 0,   0, 0 + 230,   0 + 67 }, 0, listener, editor };
            views[1] = new CombineView{ toggles[1], { 0,  72, 0 + 230,  72 + 67 }, 1, listener, editor };
            views[2] = new CombineView{ toggles[2], { 0, 144, 0 + 230, 144 + 67 }, 2, listener, editor };

            for (auto& i : views)
            {
                //i->toggle->phse->registerControlListener(i);
                //i->toggle->wtps->registerControlListener(i);
                //i->toggle->sync->registerControlListener(i);
                //i->toggle->bend->registerControlListener(i);
                //i->toggle->shpr->registerControlListener(i);
                //i->toggle->shpm->registerControlListener(i);
                //i->toggle->shp2->registerControlListener(i);
                //i->toggle->sh2m->registerControlListener(i);
                //i->toggle->plsw->registerControlListener(i);
                //i->toggle->enbs->registerControlListener(i);
                //i->toggle->enbd->registerControlListener(i);
                //i->toggle->enbf->registerControlListener(i);
                //i->toggle->enbn->registerControlListener(i);
                //i->toggle->enbr->registerControlListener(i);

                //editor->controller->wakeupCalls.emplace_back(Params::WTPos1 + i->index, i->toggle->wtps->modulation, i->modulateChange);
                //editor->controller->wakeupCalls.emplace_back(Params::Sync1 + i->index, i->toggle->sync->modulation, i->modulateChange);
                //editor->controller->wakeupCalls.emplace_back(Params::Phase1 + i->index, i->toggle->phse->modulation, i->modulateChange);
                //editor->controller->wakeupCalls.emplace_back(Params::PulseW1 + i->index, i->toggle->plsw->modulation, i->modulateChange);
                //editor->controller->wakeupCalls.emplace_back(Params::Bend1 + i->index, i->toggle->bend->modulation, i->modulateChange);
                //editor->controller->wakeupCalls.emplace_back(Params::Shaper1 + i->index, i->toggle->shpr->modulation, i->modulateChange);
                //editor->controller->wakeupCalls.emplace_back(Params::ShaperMix1 + i->index, i->toggle->shpm->modulation, i->modulateChange);
                //editor->controller->wakeupCalls.emplace_back(Params::Shaper21 + i->index, i->toggle->shp2->modulation, i->modulateChange);
                //editor->controller->wakeupCalls.emplace_back(Params::Shaper2Mix1 + i->index, i->toggle->sh2m->modulation, i->modulateChange);
            }

            views[0]->Select();

            addView(views[0]);
            addView(views[1]);
            addView(views[2]);

            addView(toggles[0]);
            addView(toggles[1]);
            addView(toggles[2]);
        }
    };

    struct CombineAttributes
    {
        static inline CPoint Size = { 430, 220 };

        static inline auto Name = "Combine";
        static inline auto BaseView = UIViewCreator::kCViewContainer;

        static inline std::tuple Attributes{};
    };

    class CombinePartFactory : public ViewFactoryBase<CombinePart, CombineAttributes>
    {
    public:
        CView* create(const UIAttributes& attributes, const IUIDescription* description) const override
        {
            CRect _size{ CPoint{ 45, 45 }, CombineAttributes::Size };
            MyEditor* _editor = dynamic_cast<MyEditor*>(description->getController());
            auto* _value = new CombinePart(_size, description->getControlListener(""), _editor);
            apply(_value, attributes, description);
            return _value;
        }
    };

    static inline CombinePartFactory combineViewFactory;

    class CombineModeView : public CViewContainer
    {
    public:
        int index = 0;
        Knob* destA;
        Knob* destB;
        Knob* destC;
        Knob* destD;
        Knob* destX;
        Knob* destY;

        Label* lablA;
        Label* lablB;
        Label* lablC;
        Label* lablD;
        Label* lablX;
        Label* lablY;

        Label* titl;

        void createControls(IControlListener* listener, MyEditor* editor)
        {
            destA = new Knob{ { 0 * 24 + 5,  35, 0 * 24 + 5 + 20,  35 + 145 + 24 }, editor };
            destB = new Knob{ { 1 * 24 + 5,  35, 1 * 24 + 5 + 20,  35 + 145 + 24 }, editor };
            destC = new Knob{ { 2 * 24 + 5,  35, 2 * 24 + 5 + 20,  35 + 145 + 24 }, editor };
            destD = new Knob{ { 3 * 24 + 5,  35, 3 * 24 + 5 + 20,  35 + 145 + 24 }, editor };
            destX = new Knob{ { 4 * 24 + 5,  83, 4 * 24 + 5 + 20,  83 +  97 + 24 }, editor };
            destY = new Knob{ { 5 * 24 + 5, 131, 5 * 24 + 5 + 20, 131 +  49 + 24 }, editor };

            lablA = new Label{ { 0 * 24 + 5, 178 + 24, 0 * 24 + 5 + 20, 178 + 25 + 24 } };
            lablB = new Label{ { 1 * 24 + 5, 178 + 24, 1 * 24 + 5 + 20, 178 + 25 + 24 } };
            lablC = new Label{ { 2 * 24 + 5, 178 + 24, 2 * 24 + 5 + 20, 178 + 25 + 24 } };
            lablD = new Label{ { 3 * 24 + 5, 178 + 24, 3 * 24 + 5 + 20, 178 + 25 + 24 } };
            lablX = new Label{ { 4 * 24 + 5, 178 + 24, 4 * 24 + 5 + 20, 178 + 25 + 24 } };
            lablY = new Label{ { 5 * 24 + 5, 178 + 24, 5 * 24 + 5 + 20, 178 + 25 + 24 } };

            titl = new Label{ { 5, 3, 5 + 140, 3 + 30 } };
            titl->fontsize = 24;
            titl->center = true;
            titl->color = { 200, 200, 200, 255 };
            titl->value = "M A T R I X";

            lablA->value = "A";
            lablA->fontsize = 14;
            lablA->center = true;
            lablB->value = "B";
            lablB->fontsize = 14;
            lablB->center = true;
            lablC->value = "C";
            lablC->fontsize = 14;
            lablC->center = true;
            lablD->value = "D";
            lablD->fontsize = 14;
            lablD->center = true;
            lablX->value = "X";
            lablX->fontsize = 14;
            lablX->center = true;
            lablY->value = "Y";
            lablY->fontsize = 14;
            lablY->center = true;

            destA->setListener(listener);
            destB->setListener(listener);
            destC->setListener(listener);
            destD->setListener(listener);
            destX->setListener(listener);
            destY->setListener(listener);

            destA->setTag(Params::DestA);
            destB->setTag(Params::DestB);
            destC->setTag(Params::DestC);
            destD->setTag(Params::DestD);
            destX->setTag(Params::DestX);
            destY->setTag(Params::DestY);

            destA->name = "I,II,III,IV,V,VI,O";      destB->name = "I,II,III,IV,V,VI,O";      destC->name = "I,II,III,IV,V,VI,O";
            destA->min = 4;                   destB->min = 4;                   destC->min = 4;
            destA->max = 7;                   destB->max = 7;                   destC->max = 7;
            destA->reset = 0;                 destB->reset = 0;                 destC->reset = 0;
            destA->decimals = 0;              destB->decimals = 0;              destC->decimals = 0;
            destA->unit = "";                 destB->unit = "";                 destC->unit = "";
            destA->type = 6;                  destB->type = 6;                  destC->type = 6;
                                                                                
            destD->name = "I,II,III,IV,V,VI,O"; destX->name = "III,IV,V,VI,O";      destY->name = "V,VI,O";
            destD->min = 4;                     destX->min = 4;                   destY->min = 4;
            destD->max = 7;                     destX->max = 5;                   destY->max = 3;
            destD->reset = 0;                   destX->reset = 0;                 destY->reset = 0;
            destD->decimals = 0;                destX->decimals = 0;              destY->decimals = 0;
            destD->unit = "";                   destX->unit = "";                 destY->unit = "";
            destD->type = 6;                    destX->type = 6;                  destY->type = 6;

            addView(destA);
            addView(destB);
            addView(destC);
            addView(destD);
            addView(destX);
            addView(destY);

            addView(lablA);
            addView(lablB);
            addView(lablC);
            addView(lablD);
            addView(lablX);
            addView(lablY);

            addView(titl);
        }

        CombineModeView(const CRect& size, int index, IControlListener* listener, MyEditor* editor)
            : CViewContainer(size), index(index)
        {
            createControls(listener, editor);
            setBackgroundColor({ 23, 23, 23, 255 });
        }
    };

    struct CombineModeAttributes
    {
        static inline CPoint Size = { 150, 200 };

        static inline auto Name = "CombineMode";
        static inline auto BaseView = UIViewCreator::kCViewContainer;

        static inline std::tuple Attributes
        {
            Attr{ "index", &CombineModeView::index },
        };
    };

    class CombineModeViewFactory : public ViewFactoryBase<CombineModeView, CombineModeAttributes>
    {
    public:
        CView* create(const UIAttributes& attributes, const IUIDescription* description) const override
        {
            CRect _size{ CPoint{ 45, 45 }, CombineModeAttributes::Size };
            int _index = 0;
            attributes.getIntegerAttribute("index", _index);
            MyEditor* _editor = dynamic_cast<MyEditor*>(description->getController());
            auto* _value = new CombineModeView(_size, _index, description->getControlListener(""), _editor);
            apply(_value, attributes, description);
            return _value;
        }
    };

    static inline CombineModeViewFactory combineModeViewFactory;
}