#pragma once

#include "Core/LayerBase.h"

namespace DYE::DYEditor
{
	class InputEventBuffingLayer final : public DYE::LayerBase
	{
	public:
		void OnEvent(DYE::Event &event) override;
		void OnEndOfFrame() override;

		static bool IsConfirmPressed();
		static bool IsIncreaseDistancePressed();
		static bool IsDecreaseDistancePressed();
		static bool IsFirePressed();
		static bool IsSliderPushedPressed();
		static bool IsSliderPulledPressed();
		static bool IsSimon1Pressed();
		static bool IsSimon2Pressed();
		static bool IsSimon3Pressed();
		static bool IsShellInPressed();
		static bool IsShellOutPressed();
	};
}