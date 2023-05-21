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
}