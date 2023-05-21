#pragma once

#include "Core/Entity.h"
#include "Core/EditorSystem.h"
#include "Core/Components.h"
#include "Components/HowitzerComponents.h"
#include "Components/KillComponents.h"
#include "Components/GameStateComponents.h"
#include "Components/MiscUIComponents.h"
#include "Core/Time.h"
#include "Math/Math.h"
#include "Math/EasingFunctions.h"
#include "Math/PrimitiveTest.h"

#include "Graphics/DebugDraw.h"

namespace DYE::DYEditor
{
	DYE_SYSTEM("Countdown Kill Timer System", DYE::DYEditor::CountdownKillTimerSystem)
	struct CountdownKillTimerSystem final : public SystemBase
	{
		ExecutionPhase GetPhase() const final { return ExecutionPhase::Update; }
		void Execute(DYE::DYEditor::World &world, DYE::DYEditor::ExecuteParameters params) final
		{
			{
				auto view = world.GetView<KillTimerComponent>();
				for (auto entity: view)
				{
					auto &killTimer = view.get<KillTimerComponent>(entity);
					killTimer.Timer -= TIME.DeltaTime();

					if (killTimer.Timer > 0)
					{
						continue;
					}

					// The time is up, assign killed component to the entity.
					Entity wrappedEntity = world.WrapIdentifierIntoEntity(entity);
					wrappedEntity.AddOrGetComponent<KilledComponent>();
					wrappedEntity.RemoveComponent<KillTimerComponent>();
				}
			}
		}
	};

	DYE_SYSTEM("Game Effect On Killed System", DYE::DYEditor::GameEffectOnKilledSystem)
	struct GameEffectOnKilledSystem final : public SystemBase
	{
		ExecutionPhase GetPhase() const final { return ExecutionPhase::Update; }
		void Execute(DYE::DYEditor::World &world, DYE::DYEditor::ExecuteParameters params) final
		{
			// Cause explosion on killed.
			{
				auto killableTargetView = world.GetView<KillableComponent, TransformComponent, CircleColliderComponent>();

				auto view = world.GetView<KilledComponent, ExplodeOnKilledComponent, TransformComponent>();
				for (auto entity: view)
				{
					auto &explodeOnKilled = view.get<ExplodeOnKilledComponent>(entity);
					auto explodedTransform = view.get<TransformComponent>(entity);
					float const explodeRadius = explodeOnKilled.ExplodeRadius;
					int const teamToKill = explodeOnKilled.TeamIDToKill;

					// Check if the explosion kills any enemies.
					for (auto [killableEntity, killable, transform, circleCollider] : killableTargetView.each())
					{
						bool const hit = Math::CircleCircleIntersect(explodedTransform.Position, explodeRadius, transform.Position, circleCollider.Radius);
						bool const isTargetTeam = teamToKill == killable.TeamID;
						if (!hit || !isTargetTeam)
						{
							continue;
						}

						Entity wrappedKillableEntity = world.WrapIdentifierIntoEntity(killableEntity);

						if (!wrappedKillableEntity.HasComponent<KilledComponent>())
						{
							wrappedKillableEntity.AddComponent<KilledComponent>();
							wrappedKillableEntity.AddComponent<DeathContributePointsComponent>();
						}
					}

					// Play explosion animation effect.
					Entity explodeEntity = world.CreateEntity("Explode Effect");
					explodeEntity.AddComponent<TransformComponent>().Position = explodedTransform.Position;

					auto &explodeAnimation = explodeEntity.AddComponent<ExplodeAnimationComponent>();
					explodeAnimation.StartRadius = explodeRadius * 0.3f;
					explodeAnimation.EndRadius = explodeRadius + 0.75f;
					explodeAnimation.AnimationTime = 1.0f;
				}
			}

			// Play explosion animation on killed.
			{
				auto view = world.GetView<KilledComponent, PlayExplodeAnimationOnKilledComponent, TransformComponent>();
				for (auto entity: view)
				{
					auto &explodeAnimationOnKilled = view.get<PlayExplodeAnimationOnKilledComponent>(entity);
					auto killedTransform = view.get<TransformComponent>(entity);

					// Play explosion animation effect.
					Entity animationEntity = world.CreateEntity("Explode Effect");
					animationEntity.AddComponent<TransformComponent>().Position = killedTransform.Position;

					auto &explodeAnimation = animationEntity.AddComponent<ExplodeAnimationComponent>();
					explodeAnimation.StartRadius = explodeAnimationOnKilled.StartRadius;
					explodeAnimation.EndRadius = explodeAnimationOnKilled.EndRadius;
					explodeAnimation.StartColor = explodeAnimationOnKilled.StartColor;
					explodeAnimation.EndColor = explodeAnimationOnKilled.EndColor;
					explodeAnimation.AnimationTime = explodeAnimationOnKilled.AnimationTime;
				}
			}

			// Add points on killed.
			{
				auto teamPointView = world.GetView<TeamPointsComponent>();
				auto view = world.GetView<KilledComponent, AddPointsToTeamOnKilledComponent, DeathContributePointsComponent>();
				for (auto entity: view)
				{
					auto &addPointsToTeam = view.get<AddPointsToTeamOnKilledComponent>(entity);

					// Check if there is any team with the given id
					for (auto [teamEntity, teamPoints] : teamPointView.each())
					{
						if (teamPoints.TeamID == addPointsToTeam.TeamIDToAddTo)
						{
							teamPoints.Points += addPointsToTeam.Points;

							char textBuff[256] = "";
							std::sprintf(textBuff, "+ %d = %f", addPointsToTeam.Points, teamPoints.Points);
							Entity popupEntity = world.CreateEntity("Popup - Add Points");
							popupEntity.AddComponent<KillTimerComponent>().Timer = 3.0f;
							popupEntity.AddComponent<PopupUIComponent>().Text = textBuff;
						}
					}
				}
			}

			// Multiply points on killed.
			{
				auto teamPointView = world.GetView<TeamPointsComponent>();
				auto view = world.GetView<KilledComponent, MultiplyPointsOfTeamOnKilledComponent, DeathContributePointsComponent>();
				for (auto entity: view)
				{
					auto &multiplyPoints = view.get<MultiplyPointsOfTeamOnKilledComponent>(entity);

					// Check if there is any team with the given id
					for (auto [teamEntity, teamPoints] : teamPointView.each())
					{
						if (teamPoints.TeamID == multiplyPoints.TeamIDToMultiplyPoints)
						{
							teamPoints.Points *= multiplyPoints.Multiplier;

							char textBuff[256] = "";
							std::sprintf(textBuff, "x %f = %f", multiplyPoints.Multiplier, teamPoints.Points);
							Entity popupEntity = world.CreateEntity("Popup - Multiply Points");
							popupEntity.AddComponent<KillTimerComponent>().Timer = 2.0f;
							popupEntity.AddComponent<PopupUIComponent>().Text = textBuff;
						}
					}
				}
			}
		}
	};

	DYE_SYSTEM("Destroy Entity On Killed System", DYE::DYEditor::DestroyEntityOnKilledSystem)
	struct DestroyEntityOnKilledSystem final : public SystemBase
	{
		ExecutionPhase GetPhase() const final { return ExecutionPhase::Cleanup; }
		void Execute(DYE::DYEditor::World &world, DYE::DYEditor::ExecuteParameters params) final
		{
			{
				auto view = world.GetView<KilledComponent>();
				for (auto entity: view)
				{
					Entity wrappedEntity = world.WrapIdentifierIntoEntity(entity);

					if (wrappedEntity.HasComponent<InvincibleComponent>())
					{
						wrappedEntity.RemoveComponent<KilledComponent>();
					}
					else
					{
						world.DestroyEntityAndChildren(wrappedEntity);
					}
				}
			}
		}
	};

	DYE_SYSTEM("Explode Animation System", DYE::DYEditor::ExplodeAnimationSystem)
	struct ExplodeAnimationSystem final : public SystemBase
	{
		ExecutionPhase GetPhase() const override { return ExecutionPhase::LateUpdate; }
		void Execute(DYE::DYEditor::World &world, DYE::DYEditor::ExecuteParameters params) override
		{
			auto view = world.GetView<ExplodeAnimationComponent, TransformComponent>();
			for (auto entity : view)
			{
				auto &explodeAnimation = view.get<ExplodeAnimationComponent>(entity);
				auto &transform = view.get<TransformComponent>(entity);

				explodeAnimation.Timer += TIME.DeltaTime();
				bool isKilled = false;
				if (explodeAnimation.Timer > explodeAnimation.AnimationTime)
				{
					explodeAnimation.Timer = explodeAnimation.AnimationTime;
					isKilled = true;
				}

				float const progress = glm::clamp(explodeAnimation.Timer / explodeAnimation.AnimationTime, 0.0f, 1.0f);
				float const t = GetEasingFunction(Ease::EaseOutCubic)(progress);

				glm::vec4 const color = Math::Lerp(explodeAnimation.StartColor, explodeAnimation.EndColor, t);
				float const radius = Math::Lerp(explodeAnimation.StartRadius, explodeAnimation.EndRadius, t);
				DebugDraw::Circle(transform.Position, radius, glm::vec3(0, 0, 1), color);

				if (isKilled)
				{
					world.DestroyEntityAndChildren(world.WrapIdentifierIntoEntity(entity));
				}
			}
		}
	};

	DYE_SYSTEM("Draw Circle Collider Component", DYE::DYEditor::DrawCircleColliderSystem)
	struct DrawCircleColliderSystem final : public SystemBase
	{
		ExecutionPhase GetPhase() const override { return ExecutionPhase::Render; }
		void Execute(DYE::DYEditor::World &world, DYE::DYEditor::ExecuteParameters params) override
		{
			auto view = world.GetView<TransformComponent, CircleColliderComponent, DrawCircleColliderComponent>();
			for (auto entity : view)
			{
				auto &circle = view.get<CircleColliderComponent>(entity);
				auto &drawCircle = view.get<DrawCircleColliderComponent>(entity);
				auto &transform = view.get<TransformComponent>(entity);
				DebugDraw::Circle(transform.Position, circle.Radius, glm::vec3(0, 0, 1), drawCircle.Color);
			}
		}
	};
}