#pragma once

#include "Core/Entity.h"
#include "Core/EditorSystem.h"
#include "Core/Components.h"
#include "Components/HowitzerComponents.h"
#include "Components/KillComponents.h"
#include "Core/Time.h"

#include "Graphics/DebugDraw.h"

namespace DYE::DYEditor
{
	DYE_SYSTEM("Destroy Entity On Killed System", DYE::DYEditor::DestroyEntityOnKilledSystem)
	struct DestroyEntityOnKilledSystem final : public SystemBase
	{
		ExecutionPhase GetPhase() const final { return ExecutionPhase::Update; }
		void Execute(DYE::DYEditor::World &world, DYE::DYEditor::ExecuteParameters params) final
		{
			auto view = world.GetView<KilledComponent>();
			for (auto entity : view)
			{
				Entity wrappedEntity = world.WrapIdentifierIntoEntity(entity);
				world.DestroyEntityAndChildren(wrappedEntity);
			}
		}
	};
}