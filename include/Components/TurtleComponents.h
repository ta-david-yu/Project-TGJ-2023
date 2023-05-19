#pragma once

#include "Core/EditorComponent.h"

namespace DYE::DYEditor
{
	DYE_COMPONENT("Turtle Input Component", DYE::DYEditor::TurtleInputComponent)
	struct TurtleInputComponent
	{
		DYE_PROPERTY()
		DYE::Int32 ControllerID = 0;
	};

	DYE_COMPONENT("Target Rotation Component", DYE::DYEditor::TargetRotationComponent)
	struct TargetRotationComponent
	{
		DYE_PROPERTY()
		DYE::Float AngleInRadian = 0;
	};

	DYE_COMPONENT("Tank Movement Component", DYE::DYEditor::TankMovementComponent)
	struct TankMovementComponent
	{
		DYE_PROPERTY()
		DYE::Float FullSpeedPerSecond = 1;

		DYE_PROPERTY()
		DYE::Float InputBuffer = 0;
	};
}