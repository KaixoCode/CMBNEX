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
		CMouseEventResult onMouseDown(CPoint& where, const CButtonState& buttons) override
		{
			setVisible(false);
			return kMouseEventHandled;
		}

		PresetView(const CRect& size, IControlListener* listener, MyEditor* editor)
			: CViewContainer(size)
		{
			setBackgroundColor({ 0, 0, 0, 80 });
			auto a = getViewSize();
			addView(new BackgroundEffect{ {.size{ a.inset(100, 100) } } });
		}

		~PresetView(){}
	};
}
