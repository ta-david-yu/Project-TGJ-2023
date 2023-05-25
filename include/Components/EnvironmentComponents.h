#pragma once

#include "Core/EditorComponent.h"
#include "Math/Color.h"

#define PLAYER_TEAM 0
#define ENEMY_TEAM 1

namespace DYE::DYEditor
{
	DYE_COMPONENT("Border", DYE::DYEditor::BorderComponent)
	struct BorderComponent
	{
	};

	DYE_COMPONENT("Rocket Spawner", DYE::DYEditor::RocketSpawnerComponent)
	struct RocketSpawnerComponent
	{
		DYE_PROPERTY()
		DYE::Float InitialSpawnInterval = 20.0f;

		DYE_PROPERTY()
		DYE::Float SpawnIntervalRandomOffset = 3;

		DYE_PROPERTY()
		DYE::Int32 SpawnsPerTarget = 1;

		DYE_PROPERTY()
		DYE::Float SpawnStartingRadius = 25;

		DYE_PROPERTY()
		DYE::Float AimRadiusOffset = 2.5f;

		DYE_PROPERTY()
		DYE::Float InitialProjectileSpeed = 3.0f;

		DYE_PROPERTY()
		DYE::Float MaxProjectileSpeed = 7.5f;

		DYE_PROPERTY()
		DYE::Float TimeWhenProjectileReachMaxSpeed = 120;

		// Game State: Read Only
		DYE_PROPERTY()
		DYE::Float SpawnTimer = 20.0f;

		DYE_PROPERTY()
		DYE::Float TimeSinceStart = 0.0f;
	};

	DYE_COMPONENT("Kill Rocket Target On Overlap", DYE::DYEditor::KillRocketTargetOnOverlap)
	struct KillRocketTargetOnOverlap
	{
	};

	DYE_COMPONENT("Rocket Target", DYE::DYEditor::RocketTargetComponent)
	struct RocketTargetComponent
	{
	};

	DYE_COMPONENT("Objective Spawner", DYE::DYEditor::ObjectiveSpawnerComponent)
	struct ObjectiveSpawnerComponent
	{
		DYE_PROPERTY()
		DYE::Float InitialSpawnInterval = 20.0f;

		DYE_PROPERTY()
		DYE::Float SpawnIntervalRandomOffset = 3;

		DYE_PROPERTY()
		DYE::Int32 MaxObjectivesOnField = 3;

		DYE_PROPERTY()
		DYE::Float SpawnStartingRadius = 20;

		DYE_PROPERTY()
		DYE::Float SpawnTimer = 20.0f;
	};

	DYE_COMPONENT("Objective", DYE::DYEditor::ObjectiveComponent)
	struct ObjectiveComponent
	{
	};
}