#pragma once

#include "Core/EditorComponent.h"
#include "KillComponents.h"
#include "Math/Color.h"

namespace DYE::DYEditor
{
	DYE_COMPONENT("Player", DYE::DYEditor::PlayerComponent)
	struct PlayerComponent
	{
	};

	DYE_COMPONENT("Is Game Over", DYE::DYEditor::IsGameOverComponent)
	struct IsGameOverComponent
	{
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

	DYE_COMPONENT("Show Game Over UI Timer", DYE::DYEditor::ShowGameOverUITimerComponent)
	struct ShowGameOverUITimerComponent
	{
		DYE_PROPERTY()
		DYE::Float Timer = 3.0f;

		DYE_PROPERTY()
		DYE::String ScenePathToLoadOnConfirm;
	};
}