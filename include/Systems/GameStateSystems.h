#pragma once

#include "Core/Entity.h"
#include "Core/EditorSystem.h"
#include "Core/Components.h"
#include "Components/HowitzerComponents.h"
#include "Components/KillComponents.h"
#include "Components/EnvironmentComponents.h"
#include "Components/GameStateComponents.h"
#include "Core/Time.h"
#include "Math/Math.h"
#include "Math/EasingFunctions.h"
#include "Math/PrimitiveTest.h"
#include "ImGui/ImGuiUtil.h"
#include "Graphics/RenderCommand.h"
#include "InputEventBuffingLayer.h"

#include "Graphics/DebugDraw.h"

#include <cstdlib>
#include <ctime>
#include <glm/gtc/random.hpp>

namespace DYE::DYEditor
{
	DYE_SYSTEM("Initialize Random Seed System", DYE::DYEditor::InitializeRandomSeedSystem)
	struct InitializeRandomSeedSystem final : public SystemBase
	{
		ExecutionPhase GetPhase() const override { return ExecutionPhase::Initialize; }
		void Execute(DYE::DYEditor::World &world, DYE::DYEditor::ExecuteParameters params) override
		{
			std::srand(std::time(nullptr));
		}
	};

	DYE_SYSTEM("Set Graphics On Initialize System", DYE::DYEditor::SetLineWidthToTwoOnInitialize)
	struct SetLineWidthToTwoOnInitialize final : public SystemBase
	{
		ExecutionPhase GetPhase() const override
		{
			return ExecutionPhase::Initialize;
		}

		void Execute(World &world, DYE::DYEditor::ExecuteParameters params) override
		{
			RenderCommand::GetInstance().SetLinePrimitiveWidth(2.0f);
		}
	};

	DYE_SYSTEM("Set Graphics On TearDown System", DYE::DYEditor::SetLineWidthToOneOnTearDown)
	struct SetLineWidthToOneOnTearDown final : public SystemBase
	{
		ExecutionPhase GetPhase() const override
		{
			return ExecutionPhase::TearDown;
		}

		void Execute(World &world, DYE::DYEditor::ExecuteParameters params) override
		{
			RenderCommand::GetInstance().SetLinePrimitiveWidth(1.0f);
		}
	};

	DYE_SYSTEM("Check If GameOver System", DYE::DYEditor::CheckIfGameOverSystem)
	struct CheckIfGameOverSystem final : public SystemBase
	{
		ExecutionPhase GetPhase() const override { return ExecutionPhase::Update; }
		void Execute(DYE::DYEditor::World &world, DYE::DYEditor::ExecuteParameters params) override
		{
			int playerLeft = 0;
			for (auto entity : world.GetView<PlayerComponent>())
			{
				playerLeft++;
			}

			if (playerLeft != 0)
			{
				return;
			}

			// Game Over.
			auto view = world.GetView<ShowGameOverUITimerComponent>();
			for (auto gameOverEntity : view)
			{
				// Get entities that have game over timer but has not been assigned with is game over.
				Entity wrappedEntity = world.WrapIdentifierIntoEntity(gameOverEntity);
				if (!wrappedEntity.HasComponent<IsGameOverComponent>())
				{
					wrappedEntity.AddComponent<IsGameOverComponent>();
				}
			}
		}
	};

	DYE_SYSTEM("Show GameOver UI If Timer Up System", DYE::DYEditor::ShowGameOverUIIfTimerUpSystem)
	struct ShowGameOverUIIfTimerUpSystem final : public SystemBase
	{
		ExecutionPhase GetPhase() const override { return ExecutionPhase::ImGui; }
		void Execute(DYE::DYEditor::World &world, DYE::DYEditor::ExecuteParameters params) override
		{
			auto view = world.GetView<ShowGameOverUITimerComponent, IsGameOverComponent, TeamPointsComponent>();
			for (auto gameOverEntity : view)
			{
				auto &teamPoints = view.get<TeamPointsComponent>(gameOverEntity);
				auto &timer = view.get<ShowGameOverUITimerComponent>(gameOverEntity);
				if (timer.Timer > 0.0f)
				{
					timer.Timer -= TIME.DeltaTime();
					continue;
				}

				// Draw Game Over UI.
				ImGuiIO &io = ImGui::GetIO();
				ImGuiWindowFlags windowFlags =
					ImGuiWindowFlags_NoDecoration |
					ImGuiWindowFlags_AlwaysAutoResize |
					ImGuiWindowFlags_NoSavedSettings |
					ImGuiWindowFlags_NoFocusOnAppearing |
					ImGuiWindowFlags_NoNav |
					ImGuiWindowFlags_NoMove;

				const ImGuiViewport *viewport = ImGui::GetMainViewport();
				ImVec2 workPos = viewport->WorkPos; // Use work area to avoid menu-bar/task-bar, if any!
				ImVec2 workSize = viewport->WorkSize;

				float const padding = 10;
				ImVec2 windowPos = ImVec2(workPos.x + workSize.x * 0.5f, workPos.y + workSize.y * 0.5f);
				ImVec2 windowPivot = ImVec2(0.5f, 0.5f);

				ImGui::SetNextWindowPos(windowPos, ImGuiCond_Always, windowPivot);
				ImGui::SetNextWindowBgAlpha(0.35f);

				if (ImGui::Begin("Game Over Window", nullptr, windowFlags))
				{
					ImGui::SetWindowFontScale(3.0f);
					ImGui::TextUnformatted("Game Over");
					ImGui::Text("Score - %d", teamPoints.Points);
					ImGui::Separator();
					ImGui::TextUnformatted("Press Fire Button To Restart");
				}
				ImGui::End();

				// Check input logic, if we should load the new scene.
				if (InputEventBuffingLayer::IsFirePressed())
				{
					auto &loadSceneComponent = world.CreateCommandEntity().AddComponent<LoadSceneComponent>();
					loadSceneComponent.SceneAssetPath = timer.ScenePathToLoadOnConfirm;
				}
			}
		}
	};
}