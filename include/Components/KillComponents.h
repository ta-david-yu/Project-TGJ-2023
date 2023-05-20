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
		DYE::Float Radius;
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

	DYE_COMPONENT("Add Points To Team On Killed Component", DYE::DYEditor::AddPointsToTeamOnKilledComponent)
	struct AddPointsToTeamOnKilledComponent
	{
		DYE_PROPERTY()
		DYE::Int32 TeamIDToAddTo = PLAYER_TEAM;

		DYE_PROPERTY()
		DYE::Int32 Points = 100.0f;
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

	DYE_COMPONENT("Team Points Component", DYE::DYEditor::TeamPointsComponent)
	struct TeamPointsComponent
	{
		DYE_PROPERTY()
		DYE::Int32 TeamID = PLAYER_TEAM;

		DYE_PROPERTY()
		DYE::Int32 Points = 0;
	};

	DYE_COMPONENT("Team Points UI", DYE::DYEditor::TeamPointsUIComponent)
	struct TeamPointsUIComponent
	{
	};

}