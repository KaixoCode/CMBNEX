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
        CNewFileSelector* selector;
        CNewFileSelector* loader;

        CMessageResult notify(CBaseObject* sender, IdStringPtr message) override
        {
            if (selector == sender)
            {
                if (auto file = selector->getSelectedFile(0))
                {
                    editor->savePreset(file);
                    nameDisplay->setDirty(true);
                }
                return kMessageUnknown;
            } else if (loader == sender)
            {
                if (auto file = loader->getSelectedFile(0))
                {
                    editor->loadPreset(file);
                    nameDisplay->setDirty(true);
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

            BackgroundEffect bgef{ getViewSize() };
            bool p = false;
            String text;

            CMouseEventResult onMouseDown(CPoint& where, const CButtonState& buttons) override
            {
                if (p)
                {
                    bgef.pressed = true;
                    setDirty(true);
                }
                press();
                return kMouseEventHandled;
            }

            CMouseEventResult onMouseUp(CPoint& where, const CButtonState& buttons) override
            {
                bgef.pressed = false;
                setDirty(true);
                return kMouseEventHandled;
            }

            void draw(CDrawContext* pContext) override
            {
                auto a = getViewSize();
                bgef.draw(pContext);
                pContext->setFont(pContext->getFont(), 14);
                pContext->setFontColor(MainText);
                double off = bgef.pressed ? 1 : 0;
                pContext->drawString(text, { a.left + 8 + off, a.getCenter().y + 3 + off });
            }
        };

        class NameDisplay : public CView
        {
        public:
            NameDisplay(const CRect& c, String& name)
                : CView(c), name(name)
            {}

            std::function<void(void)> press;
            String& name;

            BackgroundEffect bgef{ getViewSize() };

            CMouseEventResult onMouseDown(CPoint& where, const CButtonState& buttons) override
            {
                press();
                return kMouseEventHandled;
            }

            void draw(CDrawContext* pContext) override
            {
                bgef.dark = true;
                bgef.draw(pContext);
                auto a = getViewSize();
                pContext->setFont(pContext->getFont(), 16);
                pContext->setFontColor(MainText);
                pContext->drawString(name, { a.left + 5, a.getCenter().y + 4 });
            }
        };

        class Overlay : public CView
        {
        public:
            using CView::CView;

            void draw(CDrawContext* pContext) override
            {
                auto a = getViewSize();

                pContext->setFrameColor(BorderHover);

                for (int i = 0; i < 3; i++)
                {
                    double d = 5;
                    double y = 17 + i * d;
                    double x = 75;
                    double w = 50;

                    pContext->drawLine({ x, y }, { x + w, y });
                    pContext->drawLine({ getWidth() - x, y }, { getWidth() - x - w, y });
                }

            }
        };

        Overlay* ovrl;

        Button* bSave;
        Button* bInit;

        NameDisplay* nameDisplay;

        MyEditor* editor;

        TopBarView(const CRect& size, IControlListener* listener, MyEditor* editor)
            : CViewContainer(size), editor(editor)
        {
            setBackgroundColor({ 0, 0, 0, 0 });
            addView(new BackgroundEffect{ { 0, 0, getWidth(), getHeight() } });
            bInit = new Button{ { 492, 10, 492 + 37, 10 + 25 } };
            bInit->text = "Init";
            bInit->press = [this]() {
                this->editor->controller->Init();
                nameDisplay->setDirty(true);
            };                  
            bInit->p = true;
            
            bSave = new Button{ { 437, 10, 437 + 50, 10 + 25 } };
            bSave->text = "Save";
            bSave->press = [this]() {
                selector = CNewFileSelector::create(getFrame(), CNewFileSelector::kSelectSaveFile);
                selector->addFileExtension(CFileExtension{ "CMBNEX Preset", "cmbnex" });
                selector->setDefaultExtension(CFileExtension{ "CMBNEX Preset", "cmbnex" });
                selector->setDefaultSaveName("Preset.cmbnex");
                selector->setTitle("Choose An Audio File");
                selector->run(this);
                selector->forget();
            };          
            
            nameDisplay = new NameDisplay{ { 200, 5, 200 + 335, 5 + 35 }, editor->controller->preset };
            nameDisplay->press = [this]() {
                loader = CNewFileSelector::create(getFrame(), CNewFileSelector::kSelectFile);
                loader->addFileExtension(CFileExtension{ "CMBNEX Preset", "cmbnex" });
                loader->setDefaultExtension(CFileExtension{ "CMBNEX Preset", "cmbnex" });
                loader->setDefaultSaveName("Preset.cmbnex");
                loader->setTitle("Choose An Audio File");
                loader->run(this);
                loader->forget();
            };
            
            ovrl = new Overlay{ { 0, 0, getWidth(), getHeight() } };
            ovrl->setMouseEnabled(false);

            addView(nameDisplay);
            addView(bSave);
            addView(bInit);
            addView(ovrl);
        }
    };
}
