#pragma once
#include "pch.hpp"
#include "Components/Parameter.hpp"
#include "Components/Label.hpp"
#include "Components/BackgroundEffect.hpp"
#include "Components/DragThing.hpp"
#include "Components/SwitchThing.hpp"
#include "Processing/Modules.hpp"

namespace Kaixo
{
	class PresetView : public CViewContainer
	{
	public:
		struct Overlay : public CView
		{
			using CView::CView;
			void draw(CDrawContext* pContext)
			{
				setVisible(false);
			}
		};

		bool press = false;

		CMouseEventResult onMouseDown(CPoint& where, const CButtonState& buttons) override
		{
			press = true;
			overlay->setVisible(true);

			CViewContainer::onMouseDown(where, buttons);
			auto a = getViewSize();
			a.inset({ 390, 200 });
			if (!a.pointInside(where))
			{
				editor->saveColor();
				setVisible(false);
			}
			return kMouseEventHandled;
		}

		CMouseEventResult onMouseUp(CPoint& where, const CButtonState& buttons) override
		{
			press = false;
			CViewContainer::onMouseUp(where, buttons);
			overlay->setVisible(true);
			return kMouseEventHandled;
		}

		CMouseEventResult onMouseMoved(CPoint& where, const CButtonState& buttons) override
		{
			CViewContainer::onMouseMoved(where, buttons);
			if (press) overlay->setVisible(true);
			return kMouseEventHandled;
		}

		struct MySlider : public CView
		{
			MySlider(CRect size, MyEditor* editor)
				: CView(size), editor(editor)
			{}

			double value1 = rgb2hsv(Colors::MainGreen).h / 360.;
			double value2 = rgb2hsv(Colors::MainGreen).s;
			double value3 = rgb2hsv(Colors::MainGreen).v / 255.;
			int pressed = 0;

			void UpdateColor()
			{
				Color _color = Colors::MainGreen;
				Color _hsv = rgb2hsv(_color);
				_hsv.h = value1 * 360.;
				_hsv.s = value2;
				_hsv.v = value3 * 255;
				Colors::MainGreen = hsv2rgb(_hsv);
				Colors::SelectText = _hsv.v < 127 ? CColor{ 200, 200, 200, 255 } : CColor{ 15, 15, 15, 255 };
			}

			CMouseEventResult onMouseDown(CPoint& where, const CButtonState& buttons) override
			{
				if (where.y < getViewSize().top + getHeight() / 3)
				{
					pressed = 1;
					if (buttons.isDoubleClick())
						value1 = 153. / 360.;
				}
				else if (where.y < getViewSize().top + 2 * getHeight() / 3)
				{
					pressed = 2;
					if (buttons.isDoubleClick())
						value2 = 100. / 100.;
				}
				else
				{
					pressed = 3;
					if (buttons.isDoubleClick())
						value3 =  70. / 100.;
				}
				UpdateColor();
				setDirty(true);
				return kMouseEventHandled;
			}

			CMouseEventResult onMouseUp(CPoint& where, const CButtonState& buttons) override
			{
				pressed = 0;
				return kMouseEventHandled;
			}

			CMouseEventResult onMouseMoved(CPoint& where, const CButtonState& buttons) override
			{
				if (pressed)
				{
					if (pressed == 1)
						value1 = constrain((where.x - getViewSize().left) / getViewSize().getWidth(), 0, 1);
					else if (pressed == 2)
						value2 = constrain((where.x - getViewSize().left) / getViewSize().getWidth(), 0, 1);
					else
						value3 = constrain((where.x - getViewSize().left) / getViewSize().getWidth(), 0, 1);

					UpdateColor();
					setDirty(true);
				}
				return kMouseEventHandled;
			}

			MyEditor* editor;

			void draw(CDrawContext* pContext)
			{
				auto a = getViewSize();
				double top = a.top;
				double h = getHeight() / 3;
				double p = 5;
				a.top = top;
				a.bottom = top + h - p;
				pContext->setFillColor(Colors::KnobBack);
				pContext->drawRect(a, kDrawFilled);

				pContext->setFillColor(Colors::MainGreen);
				double r = value1 * getWidth();
				pContext->drawRect({ a.left, a.top, a.left + r, a.bottom }, kDrawFilled);

				a.top = top + h;
				a.bottom = top + 2 * h - p;
				pContext->setFillColor(Colors::KnobBack);
				pContext->drawRect(a, kDrawFilled);

				pContext->setFillColor(Colors::MainGreen);
				r = value2 * getWidth();
				pContext->drawRect({ a.left, a.top, a.left + r, a.bottom }, kDrawFilled);

				a.top = top + 2 * h;
				a.bottom = top + 3 * h - p;
				pContext->setFillColor(Colors::KnobBack);
				pContext->drawRect(a, kDrawFilled);

				pContext->setFillColor(Colors::MainGreen);
				r = value3 * getWidth();
				pContext->drawRect({ a.left, a.top, a.left + r, a.bottom }, kDrawFilled);
			}
		};

		struct MyButton : public CView
		{
			CMouseEventResult onMouseDown(CPoint& where, const CButtonState& buttons) override
			{
				Colors::lightMode ^= true;
				editor->UpdateTheme(Colors::lightMode);
				editor->saveTheme();
				setDirty(true);
				return kMouseEventHandled;
			}

			MyEditor* editor;

			MyButton(CRect size, MyEditor* editor)
				: CView(size), editor(editor)
			{}

			void draw(CDrawContext* pContext)
			{
				auto a = getViewSize();
				pContext->setFillColor(Colors::lightMode ? Colors::MainGreen : Colors::KnobBack);
				pContext->drawRect(a, kDrawFilled);
			}
		};

		MyEditor* editor;
		Overlay* overlay;

		PresetView(const CRect& size, IControlListener* listener, MyEditor* editor)
			: CViewContainer(size), editor(editor)
		{
			editor->loadSettings();
			setBackgroundColor({ 0, 0, 0, 128 });
			auto a = getViewSize();
			addView(new BackgroundEffect{ {.size{ a.inset(390, 200) } } });
			double y = 5;
			addView(new Label{ {.size = { a.left, a.top + y, a.right, a.top + y + 20 }, .value = "CMBNEX", .center = true}});
			y += 28;
			// CMBNEX Is a 4 oscillator synth with 
			// a unique tuneable 2 dimensional waveshaper
			// and 3 combiners that use several algorithms 
			// to combine incoming signals. 
			addView(new Label{ {.size = { a.left, a.top + y, a.right, a.top + y + 20 }, .value = "CMBNEX Is a 4 oscillator synth with", .center = true, .fontsize = 14 } });
			y += 20;
			addView(new Label{ {.size = { a.left, a.top + y, a.right, a.top + y + 20 }, .value = "a unique tuneable 2 dimensional waveshaper", .center = true, .fontsize = 14 } });
			y += 20;
			addView(new Label{ {.size = { a.left, a.top + y, a.right, a.top + y + 20 }, .value = "and 3 combiners that use several algorithms", .center = true, .fontsize = 14 } });
			y += 20;
			addView(new Label{ {.size = { a.left, a.top + y, a.right, a.top + y + 20 }, .value = "to combine incoming signals. ", .center = true, .fontsize = 14 } });
			y += 30;
			addView(new Label{ {.size = { a.left, a.top + y, a.right, a.top + y + 20 }, .value = "For more information, and the manual, visit", .center = true, .fontsize = 14 }});
			y += 20;
			addView(new Label{ {.size = { a.left, a.top + y, a.right, a.top + y + 20 }, .value = "https://github.com/KaixoCode/CMBNEX", .center = true, .fontsize = 14 }});
			y += 40;
			addView(new Label{ {.size = { a.left, a.top + y, a.right, a.top + y + 20 }, .value = "Change Color (HSV)", .center = true, .fontsize = 14 }});
			y += 20;
			addView(new MySlider{ { a.left + 50, a.top + y, a.right - 50, a.top + y + 70 }, editor });
			y += 70;
			addView(new MyButton{ { a.left + 50, a.top + y, a.left + 70, a.top + y + 20 }, editor });
			y += 1;
			addView(new Label{ {.size = { a.left + 80, a.top + y, a.right, a.top + y + 20 }, .value = "Light mode", .center = false, .fontsize = 14 } });
			addView(overlay = new Overlay{ getViewSize() }); overlay->setMouseEnabled(false);
		}

		~PresetView(){}
	};
}
