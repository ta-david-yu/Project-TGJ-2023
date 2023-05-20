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

		KeyCode IncreaseDistanceButton = KeyCode::Up;
		KeyCode DecreaseDistanceButton = KeyCode::Down;

		KeyCode FireButton = KeyCode::Space;

		GamepadButton FireGamepadButton = GamepadButton::A;

		DYE_PROPERTY()
		DYE::Int32 ControllerID = 0;

		DYE_PROPERTY()
		DYE::Float AngleStepDegreePerPress = 18;

		DYE_PROPERTY()
		DYE::Float DistanceChangePerPress = 1;
	};

	DYE_COMPONENT("Howitzer Aiming Component", DYE::DYEditor::HowitzerAimingComponent)
	struct HowitzerAimingComponent
	{
		DYE_PROPERTY()
		DYE::Float AngleDegreeRelativeToParent = 0;

		DYE_PROPERTY()
		DYE::Float MinDistance = 5;

		DYE_PROPERTY()
		DYE::Float MaxDistance = 15;

		DYE_PROPERTY()
		DYE::Float CurrDistance = 5;
	};

	DYE_COMPONENT("Projectile Movement Component", DYE::DYEditor::ProjectileMovementComponent)
	struct ProjectileMovementComponent
	{
		DYE_PROPERTY()
		DYE::Float TravelSpeedPerSecond = 10.0f;

		DYE_PROPERTY()
		DYE::Float MaxTravelDistance = 50.0f;

		DYE_PROPERTY()
		DYE::Float TravelledDistance = 0.0f;
	};

	DYE_COMPONENT("Debug Draw Sphere Component", DYE::DYEditor::DebugDrawSphereComponent)
	struct DebugDrawSphereComponent
	{
		DYE_PROPERTY()
		DYE::Color4 Color = glm::vec4(1, 1, 1, 1);

		DYE_PROPERTY()
		DYE::Float Radius = 0.5f;
	};
}