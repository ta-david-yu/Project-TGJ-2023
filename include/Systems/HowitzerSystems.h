#pragma once

#include "Core/Entity.h"
#include "Core/EditorSystem.h"
#include "Core/Components.h"
#include "Components/HowitzerComponents.h"
#include "Components/KillComponents.h"
#include "Core/Time.h"
#include "Math/Math.h"
#include "Math/EasingFunctions.h"
#include "ImGui/ImGuiUtil.h"

#include "Graphics/DebugDraw.h"

namespace DYE::DYEditor
{
	DYE_SYSTEM("Follow Parent Position System", DYE::DYEditor::FollowParentPositionSystem)
	struct FollowParentPositionSystem final : public SystemBase
	{
		ExecutionPhase GetPhase() const final { return ExecutionPhase::FixedUpdate; }
		void Execute(DYE::DYEditor::World &world, DYE::DYEditor::ExecuteParameters params) final
		{
			auto view = world.GetView<FollowParentPositionComponent, ParentComponent, TransformComponent>();
			for (auto entity : view)
			{
				auto followParent = view.get<FollowParentPositionComponent>(entity);
				auto &childTransform = view.get<TransformComponent>(entity);
				auto parentComp = view.get<ParentComponent>(entity);
				Entity parentEntity = world.TryGetEntityWithGUID(parentComp.ParentGUID).value();

				TransformComponent parentTransform = parentEntity.GetComponent<TransformComponent>();
				childTransform.Position = parentTransform.Position + followParent.PositionOffset;
			}
		}
	};

	DYE_SYSTEM("Howitzer Input System", DYE::DYEditor::HowitzerInputSystem)
	struct HowitzerInputSystem final : public SystemBase
	{
		bool UseDebugKeyboardInput = false;

		ExecutionPhase GetPhase() const final { return ExecutionPhase::Update; }
		void Execute(DYE::DYEditor::World &world, DYE::DYEditor::ExecuteParameters params) final
		{
			{
				auto view = world.GetView<HowitzerInputComponent, HowitzerAimingComponent, TransformComponent>();
				for (auto entity: view)
				{
					auto howitzerInput = view.get<HowitzerInputComponent>(entity);
					auto &howitzerAiming = view.get<HowitzerAimingComponent>(entity);
					auto transform = view.get<TransformComponent>(entity);

					if (UseDebugKeyboardInput)
					{
						if (INPUT.GetKeyDown(howitzerInput.RotateClockwiseButton))
						{
							howitzerAiming.AngleDegreeRelativeToParent -= howitzerInput.AngleStepDegreePerPress;
						}

						if (INPUT.GetKeyDown(howitzerInput.RotateAnticlockwiseButton))
						{
							howitzerAiming.AngleDegreeRelativeToParent += howitzerInput.AngleStepDegreePerPress;
						}

						if (INPUT.GetKey(howitzerInput.IncreaseDistanceButton))
						{
							howitzerAiming.CurrDistance += howitzerInput.DistanceChangePerPress;
							if (howitzerAiming.CurrDistance > howitzerAiming.MaxDistance)
							{
								howitzerAiming.CurrDistance = howitzerAiming.MaxDistance;
							}
						}

						if (INPUT.GetKey(howitzerInput.DecreaseDistanceButton))
						{
							howitzerAiming.CurrDistance -= howitzerInput.DistanceChangePerPress;
							if (howitzerAiming.CurrDistance < howitzerAiming.MinDistance)
							{
								howitzerAiming.CurrDistance = howitzerAiming.MinDistance;
							}
						}
					}
					else
					{
						// -1 ~ 1: the orientation of howitzer
						float const x = INPUT.GetGamepadAxis(howitzerInput.ControllerID, GamepadAxis::LeftTrigger);
						float const y = INPUT.GetGamepadAxis(howitzerInput.ControllerID, GamepadAxis::RightTrigger);

						glm::vec2 const axis = {x * 2 - 1, y * 2 - 1};
						float const length = glm::length(axis);
						if (length > 0.0001f)
						{
							float const angleRadian = glm::atan(axis.x, axis.y);
							howitzerAiming.AngleDegreeRelativeToParent = glm::degrees(angleRadian);
						}

						/*
						// The distance of the howitzer
						float distanceT = length;
						float const minimumLength = 0.1f;
						if (distanceT < minimumLength)
						{
							distanceT = minimumLength;
						}
						if (distanceT > 1.0f)
						{
							distanceT = 1.0f;
						}
						distanceT -= minimumLength;
						distanceT /= (1.0f - minimumLength);
						howitzerAiming.CurrDistance = Math::Lerp(howitzerAiming.MinDistance, howitzerAiming.MaxDistance, distanceT);*/

						if (INPUT.GetKey(howitzerInput.IncreaseDistanceButton))
						{
							howitzerAiming.CurrDistance += howitzerInput.DistanceChangePerPress;
							if (howitzerAiming.CurrDistance > howitzerAiming.MaxDistance)
							{
								howitzerAiming.CurrDistance = howitzerAiming.MaxDistance;
							}
						}

						if (INPUT.GetKey(howitzerInput.DecreaseDistanceButton))
						{
							howitzerAiming.CurrDistance -= howitzerInput.DistanceChangePerPress;
							if (howitzerAiming.CurrDistance < howitzerAiming.MinDistance)
							{
								howitzerAiming.CurrDistance = howitzerAiming.MinDistance;
							}
						}
					}

					if (INPUT.GetKeyDown(howitzerInput.FireButton) ||
						INPUT.GetGamepadButton(howitzerInput.ControllerID, howitzerInput.FireGamepadButton))
					{
						Entity firedProjectile = world.CreateEntity("Player Projectile");
						auto &projectileTransform = firedProjectile.AddComponent<TransformComponent>();
						projectileTransform.Position = transform.Position;
						projectileTransform.Rotation = transform.Rotation;

						// TODO: adjust parameters here.
						auto &projectileMovement = firedProjectile.AddComponent<ProjectileMovementComponent>();
						projectileMovement.MaxTravelDistance = howitzerAiming.CurrDistance;

						auto &explodeOnKilled = firedProjectile.AddComponent<ExplodeOnKilledComponent>();
						explodeOnKilled.TeamIDToKill = ENEMY_TEAM;
						explodeOnKilled.ExplodeRadius = 3.5f;

						auto &projectileSphere = firedProjectile.AddComponent<DebugDrawSphereComponent>();
					}
				}
			}
		}

		void DrawInspector(DYE::DYEditor::World &world) override
		{
			ImGuiUtil::DrawBoolControl("Debug Keyboard Input", UseDebugKeyboardInput);
		}
	};

	DYE_SYSTEM("Rotate Howitzer System", DYE::DYEditor::RotateHowitzerSystem)
	struct RotateHowitzerSystem final : public SystemBase
	{
		ExecutionPhase GetPhase() const final { return ExecutionPhase::FixedUpdate; }
		void Execute(DYE::DYEditor::World &world, DYE::DYEditor::ExecuteParameters params) final
		{
			auto view = world.GetView<HowitzerAimingComponent, ParentComponent, TransformComponent>();
			for (auto entity : view)
			{
				auto howitzerAiming = view.get<HowitzerAimingComponent>(entity);
				auto &childTransform = view.get<TransformComponent>(entity);
				auto parentComp = view.get<ParentComponent>(entity);
				Entity parentEntity = world.TryGetEntityWithGUID(parentComp.ParentGUID).value();

				TransformComponent parentTransform = parentEntity.GetComponent<TransformComponent>();
				childTransform.Rotation =
					parentTransform.Rotation *
					glm::quat(glm::vec3(0, 0, glm::radians(howitzerAiming.AngleDegreeRelativeToParent)));
			}
		}
	};

	DYE_SYSTEM("Render Howitzer Aim System", DYE::DYEditor::RenderHowitzerAimSystem)
	struct RenderHowitzerAimSystem final : public SystemBase
	{
		ExecutionPhase GetPhase() const final { return ExecutionPhase::Render; }
		void Execute(DYE::DYEditor::World &world, DYE::DYEditor::ExecuteParameters params) final
		{
			auto view = world.GetView<HowitzerInputComponent, HowitzerAimingComponent, TransformComponent>();
			for (auto entity : view)
			{
				auto howitzerInput = view.get<HowitzerInputComponent>(entity);
				auto howitzerAiming = view.get<HowitzerAimingComponent>(entity);
				auto &transform = view.get<TransformComponent>(entity);

				// Aim distance range.
				auto const minAimPosition = transform.Position + transform.GetRight() * howitzerAiming.MinDistance;
				auto const maxAimPosition = transform.Position + transform.GetRight() * howitzerAiming.MaxDistance;
				//DebugDraw::Circle(minAimPosition, 0.2f, glm::vec3(0, 0, 1), Color::Yellow);
				//DebugDraw::Circle(maxAimPosition, 0.2f, glm::vec3(0, 0, 1), Color::Yellow);
				DebugDraw::Line(transform.Position, maxAimPosition, Color::Yellow);

				// Ruler.
				for (int distance = howitzerAiming.MinDistance; distance <= howitzerAiming.MaxDistance; distance += howitzerInput.DistanceChangePerPress)
				{
					glm::vec3 const center = transform.Position + transform.GetRight() * (float) distance;
					glm::vec3 const left = center + transform.GetForward() * 0.15f;
					glm::vec3 const right = center + transform.GetForward() * -0.15f;
					DebugDraw::Line(left, right, Color::Yellow);
				}

				// Current distance.
				auto const aimPosition = transform.Position + transform.GetRight() * howitzerAiming.CurrDistance;
				float const hitRadius = 3.5f; // FIXME: put this in another component.
				DebugDraw::Circle(aimPosition, hitRadius, glm::vec3(0, 0, 1), Color::Red);
				DebugDraw::Circle(aimPosition, 0.25f, glm::vec3(0, 0, 1), Color::Yellow);
			}
		}
	};

	DYE_SYSTEM("Projectile Travel System", DYE::DYEditor::ProjectileTravelSystem)
	struct ProjectileTravelSystem final : public SystemBase
	{
		ExecutionPhase GetPhase() const override { return ExecutionPhase::FixedUpdate; }
		void Execute(DYE::DYEditor::World &world, DYE::DYEditor::ExecuteParameters params) override
		{
			auto view = world.GetView<ProjectileMovementComponent, TransformComponent>();
			for (auto entity : view)
			{
				Entity wrappedEntity = world.WrapIdentifierIntoEntity(entity);

				auto &projectTileMovement = view.get<ProjectileMovementComponent>(entity);
				auto &transform = view.get<TransformComponent>(entity);

				// Calculate new travel location.
				float const travelDistance = (float) TIME.FixedDeltaTime() * projectTileMovement.TravelSpeedPerSecond;
				transform.Position += transform.GetRight() * travelDistance;
				projectTileMovement.TravelledDistance += travelDistance;

				if (projectTileMovement.TravelledDistance > projectTileMovement.MaxTravelDistance)
				{
					// Reached max distance, kill the projectile entity.
					wrappedEntity.AddComponent<KilledComponent>();
				}
			}
		}
	};

	DYE_SYSTEM("Render Debug Sphere System", DYE::DYEditor::RenderDebugSphereSystem)
	struct RenderDebugSphereSystem final : public SystemBase
	{
		ExecutionPhase GetPhase() const override { return ExecutionPhase::Render; }
		void Execute(DYE::DYEditor::World &world, DYE::DYEditor::ExecuteParameters params) override
		{
			auto view = world.GetView<DebugDrawSphereComponent, TransformComponent>();
			for (auto entity : view)
			{
				auto &sphere = view.get<DebugDrawSphereComponent>(entity);
				auto &transform = view.get<TransformComponent>(entity);

				DebugDraw::Circle(transform.Position, sphere.Radius, glm::vec3 (0, 0, 1), sphere.Color);
			}
		}
	};
}