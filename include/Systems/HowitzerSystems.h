#pragma once

#include "Core/Entity.h"
#include "Core/EditorSystem.h"
#include "Core/Components.h"
#include "Components/HowitzerComponents.h"

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
		ExecutionPhase GetPhase() const final { return ExecutionPhase::Update; }
		void Execute(DYE::DYEditor::World &world, DYE::DYEditor::ExecuteParameters params) final
		{
			{
				auto view = world.GetView<HowitzerInputComponent, HowitzerAimingComponent>();
				for (auto entity: view)
				{
					auto howitzerInput = view.get<HowitzerInputComponent>(entity);
					auto &howitzerAiming = view.get<HowitzerAimingComponent>(entity);
					//auto &childTransform = view.get<TransformComponent>(entity);

					if (INPUT.GetKeyDown(howitzerInput.RotateClockwiseButton))
					{
						howitzerAiming.AngleDegreeRelativeToParent -= howitzerInput.AngleStepDegreePerPress;
					}

					if (INPUT.GetKeyDown(howitzerInput.RotateAnticlockwiseButton))
					{
						howitzerAiming.AngleDegreeRelativeToParent += howitzerInput.AngleStepDegreePerPress;
					}

					if (INPUT.GetKeyDown(howitzerInput.IncreaseDistanceButton))
					{
						howitzerAiming.CurrDistance += howitzerInput.DistanceChangePerPress;
						if (howitzerAiming.CurrDistance > howitzerAiming.MaxDistance)
						{
							howitzerAiming.CurrDistance = howitzerAiming.MaxDistance;
						}
					}

					if (INPUT.GetKeyDown(howitzerInput.DecreaseDistanceButton))
					{
						howitzerAiming.CurrDistance -= howitzerInput.DistanceChangePerPress;
						if (howitzerAiming.CurrDistance < howitzerAiming.MinDistance)
						{
							howitzerAiming.CurrDistance = howitzerAiming.MinDistance;
						}
					}
				}
			}
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
}