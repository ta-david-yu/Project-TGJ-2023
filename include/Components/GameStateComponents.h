#pragma once

#include "Core/EditorComponent.h"
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

	DYE_COMPONENT("Show Game Over UI Timer", DYE::DYEditor::ShowGameOverUITimerComponent)
	struct ShowGameOverUITimerComponent
	{
		DYE_PROPERTY()
		DYE::Float Timer = 3.0f;

		DYE_PROPERTY()
		DYE::String ScenePathToLoadOnConfirm;
	};
}