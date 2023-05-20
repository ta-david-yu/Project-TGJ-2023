#pragma once

#include "Core/EditorComponent.h"

namespace DYE::DYEditor
{
	DYE_COMPONENT("Killable Component", DYE::DYEditor::KillableComponent)
	struct KillableComponent
	{
		DYE_PROPERTY()
		DYE::Int32 MaxHitPoint = 1;

		DYE_PROPERTY()
		DYE::Int32 CurrHitPoint = 1;
	};

	DYE_COMPONENT("Killed Component", DYE::DYEditor::KilledComponent)
	struct KilledComponent
	{
	};

	DYE_COMPONENT("Explode On Killed Component", DYE::DYEditor::ExplodeOnKilledComponent)
	struct ExplodeOnKilledComponent
	{
		DYE_PROPERTY()
		DYE::Float ExplodeRadius = 3.5f;
	};
}