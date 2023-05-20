#include "InputEventBuffingLayer.h"

#include "Event/Event.h"
#include "Event/KeyEvent.h"

namespace DYE::DYEditor
{
	struct InputData
	{
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
		if (event.GetEventType() != EventType::KeyDown && event.GetEventType() != EventType::KeyUp)
		{
			return;
		}

		bool isPressed = event.GetEventType() == EventType::KeyDown;
		auto &keyEvent = (KeyEvent &) event;

		//DYE_LOG("%s", keyEvent.ToString().c_str());

		switch (keyEvent.GetKeyCode())
		{
			case KeyCode::Up:
				s_Data.IsIncreaseDistancePressed = isPressed;
				break;
			case KeyCode::Down:
				s_Data.IsDecreaseDistancePressed = isPressed;
				break;
			case KeyCode::Space:
				s_Data.IsFirePressed = isPressed;
				break;
			case KeyCode::NumpadPlus:
				s_Data.IsSliderPushedPressed = isPressed;
				break;
			case KeyCode::NumpadMinus:
				s_Data.IsSliderPulledPressed = isPressed;
				break;
			case KeyCode::Numpad1:
				s_Data.IsSimon1Pressed = isPressed;
				break;
			case KeyCode::Numpad2:
				s_Data.IsSimon2Pressed = isPressed;
				break;
			case KeyCode::Numpad3:
				s_Data.IsSimon3Pressed = isPressed;
				break;
			case KeyCode::Numpad4:
				s_Data.IsShellInPressed = isPressed;
				break;
			case KeyCode::Numpad5:
				s_Data.IsShellOutPressed = isPressed;
				break;
		}

	}

	void InputEventBuffingLayer::OnEndOfFrame()
	{
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