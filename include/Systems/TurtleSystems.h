#pragma once

#include "Core/EditorSystem.h"

#include "Core/Entity.h"
#include "Core/Components.h"
#include "Components/TurtleComponents.h"
#include "Components/EnvironmentComponents.h"
#include "Components/KillComponents.h"
#include "Input/InputManager.h"
#include "Math/PrimitiveTest.h"
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

				if (INPUT.GetKeyDown(KeyCode::I))
				{
					Entity wrappedEntity = world.WrapIdentifierIntoEntity(entity);
					if (wrappedEntity.HasComponent<InvincibleComponent>())
					{
						wrappedEntity.RemoveComponent<InvincibleComponent>();
					}
					else
					{
						wrappedEntity.AddComponent<InvincibleComponent>();
					}
				}

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
			auto worldBorderView = world.GetView<BorderComponent, CircleColliderComponent, TransformComponent>();
			std::optional<Entity> worldBorderEntity = {};
			for (auto entity : worldBorderView)
			{
				worldBorderEntity = world.WrapIdentifierIntoEntity(entity);
				break;
			}

			auto view = world.GetView<TankRotationComponent, TankMovementComponent, TransformComponent, CircleColliderComponent>();

			for (auto entity : view)
			{
				auto targetRotation = view.get<TankRotationComponent>(entity);
				auto &tankMovement = view.get<TankMovementComponent>(entity);
				auto &transform = view.get<TransformComponent>(entity);
				auto &circleCollider = view.get<CircleColliderComponent>(entity);

				transform.Rotation = glm::quat(glm::vec3(0, 0, targetRotation.AngleInRadian));

				if (glm::abs(tankMovement.InputBuffer) > 0.0f)
				{
					auto movingDir = transform.GetRight();
					auto offset = movingDir * (tankMovement.InputBuffer * tankMovement.FullSpeedPerSecond *
											   (float) TIME.FixedDeltaTime());
					auto newPosition = transform.Position + offset;

					if (!worldBorderEntity.has_value())
					{
						// No need to do position check cuz no border.
						transform.Position = newPosition;
						tankMovement.InputBuffer = 0;
						continue;
					}

					// Do border collision check.
					auto &worldTransform = worldBorderEntity->GetComponent<TransformComponent>();
					auto &worldCircleCollider = worldBorderEntity->GetComponent<CircleColliderComponent>();

					int testCounter = 3;
					bool isInsideWorldBorder = Math::IsCircleInsideCircle(
						newPosition, circleCollider.Radius,
						worldTransform.Position, worldCircleCollider.Radius);

					while (!isInsideWorldBorder)
					{
						// Pick the center point between the new position & old position.
						newPosition = (newPosition + transform.Position) * 0.5f;

						isInsideWorldBorder = Math::IsCircleInsideCircle(
							newPosition, circleCollider.Radius,
							worldTransform.Position, worldCircleCollider.Radius);

						testCounter--;

						if (testCounter == 0)
						{
							newPosition = transform.Position;
							break;
						}
					}

					transform.Position = newPosition;
					tankMovement.InputBuffer = 0;
				}
			}
		}
	};
}