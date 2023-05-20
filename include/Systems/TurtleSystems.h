#pragma once

#include "Core/EditorSystem.h"

#include "Core/Components.h"
#include "Components/TurtleComponents.h"
#include "Input/InputManager.h"
#include "Math/Math.h"
#include "Core/Time.h"

namespace DYE::DYEditor
{
	DYE_SYSTEM("Turtle Input System", DYE::DYEditor::TurtleInputSystem)
	struct TurtleInputSystem final : public SystemBase
	{
		ExecutionPhase GetPhase() const final { return ExecutionPhase::Update; }
		void Execute(DYE::DYEditor::World &world, DYE::DYEditor::ExecuteParameters params) final
		{
			auto view = world.GetView<TurtleInputComponent, TankRotationComponent, TankMovementComponent>();

			for (auto entity : view)
			{
				auto turtleInput = view.get<TurtleInputComponent>(entity);
				auto &targetRotation = view.get<TankRotationComponent>(entity);
				auto &tankMovement = view.get<TankMovementComponent>(entity);

				if (!INPUT.IsGamepadConnected(turtleInput.ControllerID))
				{
					continue;
				}

				// Rotation
				float const horizontal = INPUT.GetGamepadAxis(turtleInput.ControllerID, GamepadAxis::LeftStickHorizontal);
				float const vertical = INPUT.GetGamepadAxis(turtleInput.ControllerID, GamepadAxis::LeftStickVertical);

				glm::vec2 const axis = {horizontal, vertical};
				if (glm::length2(axis) > 0.5f * 0.5f)
				{
					float const angleRadian = glm::atan(axis.y, axis.x);
					targetRotation.AngleInRadian = angleRadian;
				}

				// Movement
				float const moveVertical = INPUT.GetGamepadAxis(turtleInput.ControllerID, GamepadAxis::RightStickVertical);
				if (glm::abs(moveVertical) > 0.1f)
				{
					tankMovement.InputBuffer = moveVertical;
				}

				//float const angleDegree = glm::degrees(angleRadian);
				//transform.Rotation = glm::quat {glm::vec3 {0, 0, angleRadian}};
			}
		}
	};

	DYE_SYSTEM("Turtle Movement System", DYE::DYEditor::TurtleMovementSystem)
	struct TurtleMovementSystem final : public SystemBase
	{
		ExecutionPhase GetPhase() const final { return ExecutionPhase::FixedUpdate; }
		void Execute(DYE::DYEditor::World &world, DYE::DYEditor::ExecuteParameters params) final
		{
			auto view = world.GetView<TankRotationComponent, TankMovementComponent, TransformComponent>();

			for (auto entity : view)
			{
				auto targetRotation = view.get<TankRotationComponent>(entity);
				auto &tankMovement = view.get<TankMovementComponent>(entity);
				auto &transform = view.get<TransformComponent>(entity);

				transform.Rotation = glm::quat(glm::vec3(0, 0, targetRotation.AngleInRadian));

				if (glm::abs(tankMovement.InputBuffer) > 0.0f)
				{
					auto movingDir = transform.GetRight();
					transform.Position += movingDir * (tankMovement.InputBuffer * tankMovement.FullSpeedPerSecond *
													   (float) TIME.FixedDeltaTime());
					tankMovement.InputBuffer = 0;
				}
			}
		}
	};
}