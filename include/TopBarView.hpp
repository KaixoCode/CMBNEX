#pragma once
#include "pch.hpp"
#include "myplugincids.hpp"
#include "Modules.hpp"
#include "Knob.hpp"
#include "Label.hpp"

namespace Kaixo
{
    class TopBarView : public CViewContainer
    {
    public:
        Knob* gain;

        CNewFileSelector* selector;
        CNewFileSelector* loader;

        CMessageResult notify(CBaseObject* sender, IdStringPtr message) override
        {
            if (selector == sender)
            {
                if (auto file = selector->getSelectedFile(0))
                {
                    editor->savePreset(file);
                }
                return kMessageUnknown;
            } else if (loader == sender)
            {
                if (auto file = loader->getSelectedFile(0))
                {
                    editor->loadPreset(file);
                }
                return kMessageUnknown;
            }
            else
                return CViewContainer::notify(sender, message);
        }

        class Button : public CView
        {
        public:
            using CView::CView;

            std::function<void(void)> press;

            CMouseEventResult onMouseDown(CPoint& where, const CButtonState& buttons) override
            {
                press();
                return kMouseEventHandled;
            }

            void draw(CDrawContext* pContext) override
            {
                auto a = getViewSize();
                pContext->setFillColor({ 255, 255, 255, 255 });
                pContext->drawRect(a, kDrawFilled);
            }
        };

        Button* bSave;
        Button* bLoad;

        MyEditor* editor;

        TopBarView(const CRect& size, IControlListener* listener, MyEditor* editor)
            : CViewContainer(size), editor(editor)
        {
            setBackgroundColor({ 0, 0, 0, 0 });
            addView(new BackgroundEffect{ { 0, 0, getWidth(), getHeight() } });
            bSave = new Button{ { 0, 0, 100, 100 } };
            bSave->press = [this]() {
                selector = CNewFileSelector::create(getFrame(), CNewFileSelector::kSelectSaveFile);
                selector->addFileExtension(CFileExtension{ "CMBNEX Preset", "cmbnex" });
                selector->setDefaultExtension(CFileExtension{ "CMBNEX Preset", "cmbnex" });
                selector->setDefaultSaveName("Preset.cmbnex");
                selector->setTitle("Choose An Audio File");
                selector->run(this);
                selector->forget();
            };          
            
            bLoad= new Button{ { 105, 0, 105 + 100, 100 } };
            bLoad->press = [this]() {
                loader = CNewFileSelector::create(getFrame(), CNewFileSelector::kSelectFile);
                loader->addFileExtension(CFileExtension{ "CMBNEX Preset", "cmbnex" });
                loader->setDefaultExtension(CFileExtension{ "CMBNEX Preset", "cmbnex" });
                loader->setDefaultSaveName("Preset.cmbnex");
                loader->setTitle("Choose An Audio File");
                loader->run(this);
                loader->forget();
            };
            addView(bSave);
            addView(bLoad);

            gain = new Knob{ {   5,  5,   5 + 65,  5 + 100 }, editor };

            gain->setListener(listener);

            gain->setTag(Params::GainZ);

            gain->name = "Gain";
            gain->min = 0;
            gain->max = 100;
            gain->reset = 1;
            gain->decimals = 1;
            gain->unit = " %";
            gain->type = 0;
            gain->modable = false;

            //addView(gain);

        }
    };
}
