#pragma once

#include "Core/EditorComponent.h"
#include "Math/Color.h"

#define PLAYER_TEAM 0
#define ENEMY_TEAM 1

namespace DYE::DYEditor
{
	DYE_COMPONENT("Popup UI", DYE::DYEditor::PopupUIComponent)
	struct PopupUIComponent
	{
		DYE_PROPERTY()
		DYE::String Text;
	};

	DYE_COMPONENT("Aimer Window Component", DYE::DYEditor::AimerWindowComponent)
	struct AimerWindowComponent
	{
		DYE_PROPERTY()
		DYE::Vector3 Offset = glm::vec3(0, 200, 0);
	};
}