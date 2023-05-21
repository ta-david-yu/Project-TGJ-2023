#include "InputEventBuffingLayer.h"

#include "Event/Event.h"
#include "Event/KeyEvent.h"

namespace DYE::DYEditor
{
	struct InputData
	{
		bool IsConfirmPressed = false;
		bool IsIncreaseDistancePressed = false;
		bool IsDecreaseDistancePressed = false;
		bool IsFirePressed = false;
		bool IsSliderPushedPressed = false;
		bool IsSliderPulledPressed = false;
		bool IsSimon1Pressed = false;
		bool IsSimon2Pressed = false;
		bool IsSimon3Pressed = false;
		bool IsShellInPressed = false;
		bool IsShellOutPressed = false;
	};

	static InputData s_Data;

	void InputEventBuffingLayer::OnEvent(Event &event)
	{
		if (event.GetEventType() != EventType::KeyDown)
		{
			return;
		}

		auto &keyEvent = (KeyEvent &) event;
		switch (keyEvent.GetKeyCode())
		{
			case KeyCode::Return:
				s_Data.IsConfirmPressed = true;
				break;
			case KeyCode::Up:
				s_Data.IsIncreaseDistancePressed = true;
				break;
			case KeyCode::Down:
				s_Data.IsDecreaseDistancePressed = true;
				break;
			case KeyCode::Space:
				s_Data.IsFirePressed = true;
				break;
			case KeyCode::NumpadPlus:
				s_Data.IsSliderPushedPressed = true;
				break;
			case KeyCode::NumpadMinus:
				s_Data.IsSliderPulledPressed = true;
				break;
			case KeyCode::Numpad1:
				s_Data.IsSimon1Pressed = true;
				break;
			case KeyCode::Numpad2:
				s_Data.IsSimon2Pressed = true;
				break;
			case KeyCode::Numpad3:
				s_Data.IsSimon3Pressed = true;
				break;
			case KeyCode::Numpad4:
				s_Data.IsShellInPressed = true;
				break;
			case KeyCode::Numpad5:
				s_Data.IsShellOutPressed = true;
				break;
		}

	}

	void InputEventBuffingLayer::OnEndOfFrame()
	{
		s_Data.IsConfirmPressed = false;
		s_Data.IsIncreaseDistancePressed = false;
		s_Data.IsDecreaseDistancePressed = false;
		s_Data.IsFirePressed = false;
		s_Data.IsSliderPushedPressed = false;
		s_Data.IsSliderPulledPressed = false;
		s_Data.IsSimon1Pressed = false;
		s_Data.IsSimon2Pressed = false;
		s_Data.IsSimon3Pressed = false;
		s_Data.IsShellInPressed = false;
		s_Data.IsShellOutPressed = false;
	}

	bool InputEventBuffingLayer::IsConfirmPressed()
	{
		return s_Data.IsConfirmPressed;
	}

	bool InputEventBuffingLayer::IsIncreaseDistancePressed()
	{
		return s_Data.IsIncreaseDistancePressed;
	}

	bool InputEventBuffingLayer::IsDecreaseDistancePressed()
	{
		return s_Data.IsDecreaseDistancePressed;
	}

	bool InputEventBuffingLayer::IsFirePressed()
	{
		return s_Data.IsFirePressed;
	}

	bool InputEventBuffingLayer::IsSliderPushedPressed()
	{
		return s_Data.IsSliderPushedPressed;
	}

	bool InputEventBuffingLayer::IsSliderPulledPressed()
	{
		return s_Data.IsSliderPulledPressed;
	}

	bool InputEventBuffingLayer::IsSimon1Pressed()
	{
		return s_Data.IsSimon1Pressed;
	}

	bool InputEventBuffingLayer::IsSimon2Pressed()
	{
		return s_Data.IsSimon2Pressed;
	}

	bool InputEventBuffingLayer::IsSimon3Pressed()
	{
		return s_Data.IsSimon3Pressed;
	}

	bool InputEventBuffingLayer::IsShellInPressed()
	{
		return s_Data.IsShellInPressed;
	}

	bool InputEventBuffingLayer::IsShellOutPressed()
	{
		return s_Data.IsShellOutPressed;
	}
}