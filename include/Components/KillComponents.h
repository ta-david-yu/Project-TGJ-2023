#pragma once

#include "Core/EditorComponent.h"
#include "Math/Color.h"

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
}