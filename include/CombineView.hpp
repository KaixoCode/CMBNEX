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
            setBackgroundColor({ 0, 0, 0, 0 });

            double x = 2;

            auto* v = new BackgroundEffect{ { 5 + x, 0, getWidth() - 5 - x, getHeight() } };
            addView(v);

            CColor brdclr = BorderHover;
            if (index == 0)
            {
                auto* v = new BackgroundEffect{ { 0,   0, 8,   0 + 67 } };
                v->sides = 1;
                addView(v);

                outl = new Label{ { 379 + x,  60, 379 + 30 + x,  60 + 20 } };
                outl->fontsize = 14, outl->center = true, outl->value = "Out";
                dest = new Knob{ { 384 + x,  83, 384 + 20 + x,  83 + 97 + 24 }, editor };
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
                auto* v = new BackgroundEffect{ { 0,  72, 8,  72 + 67 } };
                v->sides = 1;
                addView(v);

                outl = new Label{ { 379 + x, 108, 379 + 30 + x, 108 + 20 } };
                outl->fontsize = 14, outl->center = true, outl->value = "Out";
                dest = new Knob{ { 384 + x, 131, 384 + 20 + x, 131 + 49 + 24 }, editor };
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
                auto* v = new BackgroundEffect{ { 0, 144, 8, 144 + 67 } };
                v->sides = 1;
                addView(v);

                outl = nullptr;
                dest = nullptr;
            }

            auto* v1 = new BackgroundEffect{ {  10 + x, 5,  10 + 140 + x, 5 + 201 }, true };
            addView(v1);

            auto* v2 = new BackgroundEffect{ { 155 + x, 5, 155 + 70 + x, 5 + 201 }, true };
            addView(v2);

            auto* v3 = new BackgroundEffect{ { 230 + x, 5, 230 + 70 + x, 5 + 201 }, true };
            addView(v3);

            auto* v4 = new BackgroundEffect{ { 305 + x, 5, 305 + 70 + x, 5 + 201 }, true };
            addView(v4);

            auto* v5 = new BackgroundEffect{ { 380 + x, 5, 380 +  30 + x, 5 + 201 }, true };
            addView(v5);

            dcol = new Label{ { 379 + x,  10, 379 + 30 + x,  10 + 20 } };
            dcol->fontsize = 14, dcol->center = true, dcol->value = "DC";
            dcof = new Knob{ { 384 + x,  30, 384 + 20 + x,  30 + 20 }, editor }; dcof->setListener(listener); dcof->setTag(Params::DCX + index);

            ftrl = new Label{ { 325 + x,  8, 325 + 40 + x,   8 + 18 } };
            ftrl->fontsize = 14, ftrl->center = false, ftrl->value = "Filter";
            enbr = new Knob{ { 309 + x,   9, 309 + 14 + x,   9 + 14 }, editor }; enbr->setListener(listener); enbr->setTag(Params::ENBFilterX + index);
            fltr = new Knob{ { 309 + x,  27, 309 + 63 + x,  27 + 17 }, editor }; fltr->setListener(listener); fltr->setTag(Params::FilterX + index);
            freq = new Knob{ { 310 + x,  47, 310 + 60 + x,  47 + 95 }, editor }; freq->setListener(listener); freq->setTag(Params::FreqX + index);
            reso = new Knob{ { 310 + x, 148, 310 + 65 + x, 148 + 53 }, editor }; reso->setListener(listener); reso->setTag(Params::ResoX + index);

            drvl = new Label{ { 250 + x,  8, 250 + 40 + x,   8 + 18 } };
            drvl->fontsize = 14, drvl->center = false, drvl->value = "Drive";
            enbd = new Knob{ { 234 + x,   9, 234 + 14 + x,   9 + 14 }, editor }; enbd->setListener(listener); enbd->setTag(Params::ENBDriveX + index);
            dgai = new Knob{ { 235 + x,  47, 235 + 60 + x,  47 + 95 }, editor }; dgai->setListener(listener); dgai->setTag(Params::DriveGainX + index);
            driv = new Knob{ { 235 + x, 148, 235 + 65 + x, 148 + 53 }, editor }; driv->setListener(listener); driv->setTag(Params::DriveAmtX + index);

            fldl = new Label{ { 175 + x,  8, 175 + 40 + x,   8 + 18 } };
            fldl->fontsize = 14, fldl->center = false, fldl->value = "Fold";
            enbf = new Knob{ { 159 + x,   9, 159 + 14 + x,   9 + 14 }, editor }; enbf->setListener(listener); enbf->setTag(Params::ENBFoldX + index);
            fold = new Knob{ { 160 + x,  47, 160 + 60 + x,  47 + 95 }, editor }; fold->setListener(listener); fold->setTag(Params::FoldX + index);
            bias = new Knob{ { 160 + x, 148, 160 + 65 + x, 148 + 53 }, editor }; bias->setListener(listener); bias->setTag(Params::BiasX + index);

            minm = new Knob{ { 15 + x,  10 - 0 * 1, 15 + 65 + x,  10 + 35 - 0 * 1 }, editor }; minm->setListener(listener); minm->setTag(Params::MinMixX + index);
            mulm = new Knob{ { 15 + x,  50 - 1 * 1, 15 + 65 + x,  50 + 35 - 1 * 1 }, editor }; mulm->setListener(listener); mulm->setTag(Params::MultMixX + index);
            ponm = new Knob{ { 15 + x,  90 - 2 * 1, 15 + 65 + x,  90 + 35 - 2 * 1 }, editor }; ponm->setListener(listener); ponm->setTag(Params::PongMixX + index); 
            maxm = new Knob{ { 15 + x, 130 - 3 * 1, 15 + 65 + x, 130 + 35 - 3 * 1 }, editor }; maxm->setListener(listener); maxm->setTag(Params::MaxMixX + index); 
            modm = new Knob{ { 15 + x, 170 - 4 * 1, 15 + 65 + x, 170 + 35 - 4 * 1 }, editor }; modm->setListener(listener); modm->setTag(Params::ModMixX + index); 
            andm = new Knob{ { 80 + x,  10 - 0 * 1, 80 + 65 + x,  10 + 35 - 0 * 1 }, editor }; andm->setListener(listener); andm->setTag(Params::AndMixX + index); 
            inlm = new Knob{ { 80 + x,  50 - 1 * 1, 80 + 65 + x,  50 + 35 - 1 * 1 }, editor }; inlm->setListener(listener); inlm->setTag(Params::InlvMixX + index); 
            orrm = new Knob{ { 80 + x,  90 - 2 * 1, 80 + 65 + x,  90 + 35 - 2 * 1 }, editor }; orrm->setListener(listener); orrm->setTag(Params::OrMixX + index); 
            xorm = new Knob{ { 80 + x, 130 - 3 * 1, 80 + 65 + x, 130 + 35 - 3 * 1 }, editor }; xorm->setListener(listener); xorm->setTag(Params::XOrMixX + index);
            addm = new Knob{ { 80 + x, 170 - 4 * 1, 80 + 65 + x, 170 + 35 - 4 * 1 }, editor }; addm->setListener(listener); addm->setTag(Params::AddMixX + index);

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

        ~CombineToggle()
        {
            enbr->forget();
            fltr->forget();
            freq->forget();
            reso->forget();
            minm->forget();
            mulm->forget();
            ponm->forget();
            maxm->forget();
            modm->forget();
            andm->forget();
            inlm->forget();
            orrm->forget();
            xorm->forget();
            addm->forget();
            enbd->forget();
            dgai->forget();
            driv->forget();
            enbf->forget();
            fold->forget();
            bias->forget();
            if (dest) dest->forget();
            dcof->forget();
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
                //bgef->pressed = true;
                //bgef->setDirty(true);
                //gmix->setViewSize(((CRect)gmix->getViewSize()).offset({ bgef->edge, bgef->edge }));
                //gain->setViewSize(((CRect)gain->getViewSize()).offset({ bgef->edge, bgef->edge }));
                //pgai->setViewSize(((CRect)pgai->getViewSize()).offset({ bgef->edge, bgef->edge }));
                //titl->setViewSize(((CRect)titl->getViewSize()).offset({ bgef->edge, bgef->edge }));
                //inp1->setViewSize(((CRect)inp1->getViewSize()).offset({ bgef->edge, bgef->edge }));
                //inp2->setViewSize(((CRect)inp2->getViewSize()).offset({ bgef->edge, bgef->edge }));
            }
            selected = true;
        }

        void Unselect()
        {
            if (selected)
            {
                toggle->setVisible(false);
                //bgef->pressed = false;
                //bgef->setDirty(true);
                //gmix->setViewSize(((CRect)gmix->getViewSize()).offset({ -bgef->edge, -bgef->edge }));
                //gain->setViewSize(((CRect)gain->getViewSize()).offset({ -bgef->edge, -bgef->edge }));
                //pgai->setViewSize(((CRect)pgai->getViewSize()).offset({ -bgef->edge, -bgef->edge }));
                //titl->setViewSize(((CRect)titl->getViewSize()).offset({ -bgef->edge, -bgef->edge }));
                //inp1->setViewSize(((CRect)inp1->getViewSize()).offset({ -bgef->edge, -bgef->edge }));
                //inp2->setViewSize(((CRect)inp2->getViewSize()).offset({ -bgef->edge, -bgef->edge }));
            }
            selected = false;
        }

        BackgroundEffect* bgef;

        CombineView(CombineToggle* toggle, const CRect& size, int index, IControlListener* listener, MyEditor* editor)
            : CViewContainer(size), toggle(toggle), index(index)
        {
            setBackgroundColor({ 0, 0, 0, 0 });
            bgef = new BackgroundEffect{ { 0, 0, getWidth(), getHeight() } };
            addView(bgef);

            constexpr CColor main = MainGreen;
            constexpr CColor text = MainText;
            constexpr CColor back = Border;
            constexpr CColor brdr = MainBack;
            constexpr CColor offt = OffText;

            pgai = new Knob{ {  35,  7,  35 + 65,  7 + 53 }, editor };
            gmix = new Knob{ { 100,  7, 100 + 65,  7 + 53 }, editor };
            gain = new Knob{ { 165,  7, 165 + 65,  7 + 53 }, editor };

            inp1 = new Label{ { 5, 1, 5 + 20, 1 + 14 } };
            inp1->fontsize = 14;
            inp1->center = true;
            inp1->enabled = false;
            inp1->value = index == 0 ? "I" : index == 1 ? "III" : "V";

            inp2 = new Label{ { 5, 48, 5 + 20, 48 + 14 } };
            inp2->fontsize = 14;
            inp2->center = true;
            inp2->enabled = false;
            inp2->value = index == 0 ? "II" : index == 1 ? "IV" : "VI";

            titl = new Label{ { 5, 19, 5 + 20, 19 + 24 } };
            titl->fontsize = 24;
            titl->center = true;
            titl->enabled = true;
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

        ~CombineView()
        {
            gain->forget();
            pgai->forget();
            gmix->forget();
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

            toggles[0] = new CombineToggle{ { 228, 0, 228 + 424, 0 + 211 }, 0, listener, editor };
            toggles[1] = new CombineToggle{ { 228, 0, 228 + 424, 0 + 211 }, 1, listener, editor };
            toggles[2] = new CombineToggle{ { 228, 0, 228 + 424, 0 + 211 }, 2, listener, editor };

            toggles[0]->setVisible(false);
            toggles[1]->setVisible(false);
            toggles[2]->setVisible(false);

            views[0] = new CombineView{ toggles[0], { 0,   0, 0 + 230,   0 + 67 }, 0, listener, editor };
            views[1] = new CombineView{ toggles[1], { 0,  72, 0 + 230,  72 + 67 }, 1, listener, editor };
            views[2] = new CombineView{ toggles[2], { 0, 144, 0 + 230, 144 + 67 }, 2, listener, editor };

            views[0]->Select();

            addView(views[0]);
            addView(views[1]);
            addView(views[2]);

            addView(toggles[0]);
            addView(toggles[1]);
            addView(toggles[2]);
        }
    };
}