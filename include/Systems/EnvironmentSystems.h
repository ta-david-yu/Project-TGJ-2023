#pragma once

#include "Core/Entity.h"
#include "Core/EditorSystem.h"
#include "Core/Components.h"
#include "Components/HowitzerComponents.h"
#include "Components/KillComponents.h"
#include "Components/EnvironmentComponents.h"
#include "Core/Time.h"
#include "Math/Math.h"
#include "Math/EasingFunctions.h"
#include "Math/PrimitiveTest.h"
#include "ImGui/ImGuiUtil.h"

#include "Graphics/DebugDraw.h"

#include <glm/gtc/random.hpp>

namespace DYE::DYEditor
{
	DYE_SYSTEM("Rocket Spawner System", DYE::DYEditor::RocketSpawnerSystem)
	struct RocketSpawnerSystem final : public SystemBase
	{
		ExecutionPhase GetPhase() const override
		{
			return ExecutionPhase::Update;
		}

		void Execute(World &world, DYE::DYEditor::ExecuteParameters params) override
		{
			auto targetView = world.GetView<RocketTargetComponent, TransformComponent>();

			auto view = world.GetView<RocketSpawnerComponent, TransformComponent>();
			for (auto entity : view)
			{
				auto &rocketSpawner = view.get<RocketSpawnerComponent>(entity);
				auto &spawnerTransform = view.get<TransformComponent>(entity);

				rocketSpawner.TimeSinceStart += TIME.DeltaTime();
				rocketSpawner.SpawnTimer -= TIME.DeltaTime();
				if (rocketSpawner.SpawnTimer > 0)
				{
					continue;
				}

				float nextSpawnTime =
					rocketSpawner.InitialSpawnInterval +
					glm::linearRand(-rocketSpawner.SpawnIntervalRandomOffset, rocketSpawner.SpawnIntervalRandomOffset);
				rocketSpawner.SpawnTimer += nextSpawnTime;

				// Spawn rocket.
				for (auto targetEntity: targetView)
				{
					auto targetTransform = targetView.get<TransformComponent>(targetEntity);

					glm::vec2 const centerToTarget = targetTransform.Position - spawnerTransform.Position;
					float spawnerToTargetAngleRadian = glm::atan(centerToTarget.y, centerToTarget.x);
					if (spawnerToTargetAngleRadian < 0.0f)
					{
						// Atan ranges from 0 ~ pi and -pi ~ 0. But we want it to be 0 ~ 2pi
						// Therefore we add 2pi to anything that's below 0.
						spawnerToTargetAngleRadian += 2 * glm::pi<float>();
					}

					// We want to pick a spawn location that's on the opposite half side of the circle where the target is at.
					float const spawnerToTargetAngleDegree = glm::degrees(spawnerToTargetAngleRadian);
					float angleDegreeCapA = spawnerToTargetAngleDegree + 90;
					float angleDegreeCapB = spawnerToTargetAngleDegree - 90;
					if (angleDegreeCapA <= 0) angleDegreeCapA += 360;
					if (angleDegreeCapA >= 360) angleDegreeCapA -= 360;
					if (angleDegreeCapB <= 0) angleDegreeCapB += 360;
					if (angleDegreeCapB >= 360) angleDegreeCapB -= 360;

					if (angleDegreeCapA > angleDegreeCapB)
					{
						angleDegreeCapA -= 360;
					}

					float spawnAngleDegree = 0;
					spawnAngleDegree = glm::linearRand(angleDegreeCapA, angleDegreeCapB);
					if (angleDegreeCapA < angleDegreeCapB)
					{
						//spawnAngleDegree = glm::linearRand(angleDegreeCapA, angleDegreeCapB);
					}
					else
					{
						//spawnAngleDegree = glm::linearRand(angleDegreeCapB, angleDegreeCapA);
					}

					float const spawnAngleRadian = glm::radians(spawnAngleDegree);

					// Start position.
					glm::vec2 startPosition = spawnerTransform.Position;
					startPosition.x += rocketSpawner.SpawnStartingRadius * glm::cos(spawnAngleRadian);
					startPosition.y += rocketSpawner.SpawnStartingRadius * glm::sin(spawnAngleRadian);
					//startPosition += glm::circularRand(rocketSpawner.SpawnStartingRadius);

					// Aim at.
					auto offset = glm::diskRand(rocketSpawner.AimRadiusOffset);
					glm::vec2 aimAt = targetTransform.Position;
					aimAt += offset;

					// Orientation.
					glm::vec2 lookDir = aimAt - startPosition;
					float const angleRadian = glm::atan(lookDir.y, lookDir.x);
					glm::quat const rocketOrientation = glm::quat(glm::vec3(0, 0, angleRadian));

					// Spawn rocket.
					Entity newRocketEntity = world.CreateEntity("Rocket");
					auto &newRocketTransform = newRocketEntity.AddComponent<TransformComponent>();
					newRocketTransform.Position = {startPosition, 0};
					newRocketTransform.Rotation = rocketOrientation;
					newRocketTransform.Scale = glm::vec3 (1.2f);

					auto &collider = newRocketEntity.AddComponent<CircleColliderComponent>();
					collider.Radius = 0.75f;

					//auto &drawCollider = newRocketEntity.AddComponent<DrawCircleColliderComponent>();
					//drawCollider.Color = Color::Red;

					auto &spriteRenderer = newRocketEntity.AddComponent<SpriteRendererComponent>();
					spriteRenderer.Texture = Texture2D::Create("assets//Textures//Rocket.png");
					spriteRenderer.Color = Color::Red;

					auto &projectile = newRocketEntity.AddComponent<ProjectileMovementComponent>();
					projectile.MaxTravelDistance = 60;

					// We want to map the speed value exponentially, so it only gets harder faster in the late game.
					float speedT = glm::clamp(rocketSpawner.TimeSinceStart / rocketSpawner.TimeWhenProjectileReachMaxSpeed, 0.0f, 1.0f);
					speedT *= speedT;

					projectile.TravelSpeedPerSecond = Math::Lerp(rocketSpawner.InitialProjectileSpeed, rocketSpawner.MaxProjectileSpeed, speedT);

					newRocketEntity.AddComponent<KillRocketTargetOnOverlap>();

					auto &killableComponent = newRocketEntity.AddComponent<KillableComponent>();
					killableComponent.TeamID = ENEMY_TEAM;

					auto &multiplyPointOnKilled = newRocketEntity.AddComponent<MultiplyPointsOfTeamOnKilledComponent>();
					multiplyPointOnKilled.TeamIDToMultiplyPoints = PLAYER_TEAM;

					auto &explodeAnimation = newRocketEntity.AddComponent<PlayExplodeAnimationOnKilledComponent>();
					explodeAnimation.StartRadius = collider.Radius;
					explodeAnimation.EndRadius =  collider.Radius * 1.5f;
					explodeAnimation.StartColor = Color::Red;
					explodeAnimation.EndColor = glm::vec4(1, 0, 0, 0);
					explodeAnimation.AnimationTime = 1;
				}
			}
		}
	};

	DYE_SYSTEM("Objective Spawner System", DYE::DYEditor::ObjectiveSpawnerSystem)
	struct ObjectiveSpawnerSystem final : public SystemBase
	{
		ExecutionPhase GetPhase() const override
		{
			return ExecutionPhase::Update;
		}

		void Execute(World &world, DYE::DYEditor::ExecuteParameters params) override
		{
			auto existingObjectiveView = world.GetView<ObjectiveComponent>();
			int const numberOfObjectives = existingObjectiveView.size();

			auto view = world.GetView<ObjectiveSpawnerComponent, TransformComponent>();
			for (auto entity : view)
			{
				auto &objectiveSpawner = view.get<ObjectiveSpawnerComponent>(entity);
				auto &spawnerTransform = view.get<TransformComponent>(entity);

				if (numberOfObjectives >= objectiveSpawner.MaxObjectivesOnField)
				{
					// Reach max number of objectives. skip it.
					continue;
				}

				objectiveSpawner.SpawnTimer -= TIME.DeltaTime();
				if (objectiveSpawner.SpawnTimer > 0)
				{
					continue;
				}

				// TODO: randomize Next spawn time.
				float nextSpawnTime = objectiveSpawner.InitialSpawnInterval;
				objectiveSpawner.SpawnTimer += nextSpawnTime;

				// Start position.
				glm::vec2 startPosition = spawnerTransform.Position;
				startPosition += glm::diskRand(objectiveSpawner.SpawnStartingRadius);

				// Spawn objective.
				Entity newObjectiveEntity = world.CreateEntity("Objective");
				newObjectiveEntity.AddComponent<ObjectiveComponent>();

				auto &newObjectiveTransform = newObjectiveEntity.AddComponent<TransformComponent>();
				newObjectiveTransform.Position = {startPosition, 0};

				auto &collider = newObjectiveEntity.AddComponent<CircleColliderComponent>();
				collider.Radius = 1.40f;

				auto &drawCollider = newObjectiveEntity.AddComponent<DrawCircleColliderComponent>();
				drawCollider.Color = Color::Green;

				auto &killableComponent = newObjectiveEntity.AddComponent<KillableComponent>();
				killableComponent.TeamID = ENEMY_TEAM;

				auto &multiplyPointOnKilled = newObjectiveEntity.AddComponent<AddPointsToTeamOnKilledComponent>();
				multiplyPointOnKilled.TeamIDToAddTo = PLAYER_TEAM;

				auto &animation = newObjectiveEntity.AddComponent<PlayExplodeAnimationOnKilledComponent>();
				animation.StartRadius = collider.Radius;
				animation.EndRadius = collider.Radius * 1.5f;
				animation.StartColor = Color::Green;
				animation.EndColor = glm::vec4(0, 1, 0, 0);
				animation.AnimationTime = 1.0f;
			}
		}

	};

	DYE_SYSTEM("Check Rocket Hit System", DYE::DYEditor::CheckRocketHitSystem)
	struct CheckRocketHitSystem final : public SystemBase
	{
		ExecutionPhase GetPhase() const override
		{
			return ExecutionPhase::FixedUpdate;
		}

		void Execute(DYE::DYEditor::World &world, DYE::DYEditor::ExecuteParameters params) override
		{
			auto targetView = world.GetView<RocketTargetComponent, CircleColliderComponent, TransformComponent>();

			auto view = world.GetView<KillRocketTargetOnOverlap, CircleColliderComponent, TransformComponent>();
			for (auto entity : view)
			{
				auto& killerTransform = view.get<TransformComponent>(entity);
				auto& killerCircle = view.get<CircleColliderComponent>(entity);

				bool hitSomething = false;
				for (auto targetEntity : targetView)
				{
					auto& targetTransform = targetView.get<TransformComponent>(targetEntity);
					auto& targetCircle = targetView.get<CircleColliderComponent>(targetEntity);

					bool const overlapped = Math::CircleCircleIntersect(killerTransform.Position, killerCircle.Radius, targetTransform.Position, targetCircle.Radius);
					if (!overlapped)
					{
						continue;
					}

					// Hit something! Try to kill it.
					Entity wrappedTargetEntity = world.WrapIdentifierIntoEntity(targetEntity);

					wrappedTargetEntity.AddOrGetComponent<KilledComponent>();

					hitSomething = true;
				}

				if (hitSomething)
				{
					Entity wrappedKillerEntity = world.WrapIdentifierIntoEntity(entity);
					wrappedKillerEntity.AddOrGetComponent<KilledComponent>();
				}
			}
		}
	};
}