#pragma once

#include "Core/EditorComponent.h"
#include "Input/InputManager.h"

namespace DYE::DYEditor
{
	DYE_COMPONENT("Follow Parent Position Component", DYE::DYEditor::FollowParentPositionComponent)
	struct FollowParentPositionComponent
	{
		DYE_PROPERTY()
		DYE::Vector3 PositionOffset = glm::vec3(0);
	};

	DYE_COMPONENT("Howitzer Input Component", DYE::DYEditor::HowitzerInputComponent)
	struct HowitzerInputComponent
	{
		KeyCode RotateClockwiseButton = KeyCode::Right;
		KeyCode RotateAnticlockwiseButton = KeyCode::Left;

		DYE_PROPERTY()
		DYE::Float AngleStepDegreePerPress = 18;
	};

	DYE_COMPONENT("Howitzer Aiming Component", DYE::DYEditor::HowitzerAimingComponent)
	struct HowitzerAimingComponent
	{
		DYE_PROPERTY()
		DYE::Float AngleDegreeRelativeToParent = 0;
	};
}