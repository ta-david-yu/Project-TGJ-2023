#pragma once

#include "Core/EditorComponent.h"
#include "Math/Color.h"

#define PLAYER_TEAM 0
#define ENEMY_TEAM 1

namespace DYE::DYEditor
{
	DYE_COMPONENT("Killable Component", DYE::DYEditor::KillableComponent)
	struct KillableComponent
	{
		DYE_PROPERTY()
		DYE::Int32 TeamID = PLAYER_TEAM;

		DYE_PROPERTY()
		DYE::Int32 MaxHitPoint = 1;

		DYE_PROPERTY()
		DYE::Int32 CurrHitPoint = 1;
	};

	DYE_COMPONENT("Circle Collider Component", DYE::DYEditor::CircleColliderComponent)
	struct CircleColliderComponent
	{
		DYE_PROPERTY()
		DYE::Float Radius = 1;
	};

	DYE_COMPONENT("Draw Circle Collider Component", DYE::DYEditor::DrawCircleColliderComponent)
	struct DrawCircleColliderComponent
	{
		DYE_PROPERTY()
		DYE::Color4 Color = Color::Green;
	};

	DYE_COMPONENT("Killed Component", DYE::DYEditor::KilledComponent)
	struct KilledComponent
	{
	};

	DYE_COMPONENT("Explode On Killed Component", DYE::DYEditor::ExplodeOnKilledComponent)
	struct ExplodeOnKilledComponent
	{
		DYE_PROPERTY()
		DYE::Int32 TeamIDToKill = ENEMY_TEAM;

		DYE_PROPERTY()
		DYE::Float ExplodeRadius = 3.5f;
	};

	DYE_COMPONENT("Play Explode Animation On Killed", DYE::DYEditor::PlayExplodeAnimationOnKilledComponent)
	struct PlayExplodeAnimationOnKilledComponent
	{
		DYE_PROPERTY()
		DYE::Float StartRadius = 3.5f;

		DYE_PROPERTY()
		DYE::Float EndRadius = 5.5f;

		DYE_PROPERTY()
		DYE::Color4 StartColor = Color::White;

		DYE_PROPERTY()
		DYE::Color4 EndColor = glm::vec4(1, 1, 1, 0);

		DYE_PROPERTY()
		DYE::Float AnimationTime = 0.5f;
	};

	DYE_COMPONENT("Add Points To Team On Killed Component", DYE::DYEditor::AddPointsToTeamOnKilledComponent)
	struct AddPointsToTeamOnKilledComponent
	{
		DYE_PROPERTY()
		DYE::Int32 TeamIDToAddTo = PLAYER_TEAM;

		DYE_PROPERTY()
		DYE::Int32 Points = 100;
	};

	DYE_COMPONENT("Multiply Points Of Team On Killed Component", DYE::DYEditor::MultiplyPointsOfTeamOnKilledComponent)
	struct MultiplyPointsOfTeamOnKilledComponent
	{
		DYE_PROPERTY()
		DYE::Int32 TeamIDToMultiplyPoints = PLAYER_TEAM;

		DYE_PROPERTY()
		DYE::Float Multiplier = 2;
	};

	DYE_COMPONENT("Kill Timer Component", DYE::DYEditor::KillTimerComponent)
	struct KillTimerComponent
	{
		DYE_PROPERTY()
		DYE::Float Timer = 1.5f;
	};

	DYE_COMPONENT("Explode Animation Component", DYE::DYEditor::ExplodeAnimationComponent)
	struct ExplodeAnimationComponent
	{
		DYE_PROPERTY()
		DYE::Float StartRadius = 3.5f;

		DYE_PROPERTY()
		DYE::Float EndRadius = 4.0f;

		DYE_PROPERTY()
		DYE::Color4 StartColor = Color::White;

		DYE_PROPERTY()
		DYE::Color4 EndColor = glm::vec4(1, 1, 1, 0);

		DYE_PROPERTY()
		DYE::Float AnimationTime = 0.5f;

		DYE::Float Timer = 0.0f;
	};

	DYE_COMPONENT("Player Killed Sound", DYE::DYEditor::PlayerKilledSoundComponent)
	struct PlayerKilledSoundComponent
	{
	};

	DYE_COMPONENT("Invincible Component", DYE::DYEditor::InvincibleComponent)
	struct InvincibleComponent
	{
	};
}