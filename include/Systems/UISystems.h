#pragma once

#include "Core/Entity.h"
#include "Core/EditorSystem.h"
#include "Core/Components.h"
#include "Components/HowitzerComponents.h"
#include "Components/KillComponents.h"
#include "Components/GameStateComponents.h"
#include "Components/MiscUIComponents.h"
#include "Core/Time.h"
#include "Math/Math.h"
#include "Math/EasingFunctions.h"
#include "Math/PrimitiveTest.h"
#include "ImGui/ImGuiUtil.h"
#include "InputEventBuffingLayer.h"

#include "Graphics/DebugDraw.h"

namespace DYE::DYEditor
{
	DYE_SYSTEM("Team Points UI System", DYE::DYEditor::TeamPointsUISystem)
	struct TeamPointsUISystem final : public SystemBase
	{
		ExecutionPhase GetPhase() const final { return ExecutionPhase::ImGui; }
		void Execute(DYE::DYEditor::World &world, DYE::DYEditor::ExecuteParameters params) final
		{
			auto view = world.GetView<TeamPointsComponent, TeamPointsUIComponent>();

			for (auto entity : view)
			{
				auto &teamPoints = view.get<TeamPointsComponent>(entity);

				ImGuiIO& io = ImGui::GetIO();
				ImGuiWindowFlags windowFlags =
					ImGuiWindowFlags_NoDecoration |
					ImGuiWindowFlags_AlwaysAutoResize |
					ImGuiWindowFlags_NoSavedSettings |
					ImGuiWindowFlags_NoFocusOnAppearing |
					ImGuiWindowFlags_NoNav |
					ImGuiWindowFlags_NoMove;

				const ImGuiViewport* viewport = ImGui::GetMainViewport();
				ImVec2 workPos = viewport->WorkPos; // Use work area to avoid menu-bar/task-bar, if any!
				ImVec2 workSize = viewport->WorkSize;

				float const padding = 10;
				ImVec2 windowPos = ImVec2(workPos.x + padding, workPos.y + workSize.y - padding);
				ImVec2 windowPivot = ImVec2(0, 1);

				ImGui::SetNextWindowSize(ImVec2(200, 0), ImGuiCond_Always);
				ImGui::SetNextWindowPos(windowPos, ImGuiCond_Always, windowPivot);
				ImGui::SetNextWindowBgAlpha(0.35f);

				if (ImGui::Begin("Score", nullptr, windowFlags))
				{
					ImGui::SetWindowFontScale(2.0f);
					ImGui::TextUnformatted("Score");
					ImGui::Separator();
					ImGui::Text("%d", (int) teamPoints.Points);
				}
				ImGui::End();

				//Entity wrappedEntity = world.WrapIdentifierIntoEntity(entity);
			}
		}
	};

	DYE_SYSTEM("Draw Popup UI System", DYE::DYEditor::DrawPopupUISystem)
	struct DrawPopupUISystem final : public SystemBase
	{
		ExecutionPhase GetPhase() const override { return ExecutionPhase::ImGui; }
		void Execute(DYE::DYEditor::World &world, DYE::DYEditor::ExecuteParameters params) override
		{
			auto popupView = world.GetView<PopupUIComponent>();
			int popupCounter = 0;
			float const popupOffsetY = 35;

			ImGui::PushID("Popup UI");
			for (auto popupEntity : popupView)
			{
				auto &popupUIComponent = popupView.get<PopupUIComponent>(popupEntity);

				ImGuiIO& io = ImGui::GetIO();
				ImGuiWindowFlags windowFlags =
					ImGuiWindowFlags_NoDecoration |
					ImGuiWindowFlags_AlwaysAutoResize |
					ImGuiWindowFlags_NoSavedSettings |
					ImGuiWindowFlags_NoFocusOnAppearing |
					ImGuiWindowFlags_NoNav |
					ImGuiWindowFlags_NoMove;

				const ImGuiViewport* viewport = ImGui::GetMainViewport();
				ImVec2 workPos = viewport->WorkPos; // Use work area to avoid menu-bar/task-bar, if any!
				ImVec2 workSize = viewport->WorkSize;

				float const paddingX = 10;
				float const paddingY = 90;
				ImVec2 windowPos = ImVec2(workPos.x + paddingX, workPos.y + workSize.y - paddingY - popupOffsetY * (popupCounter));
				ImVec2 windowPivot = ImVec2(0, 1);

				ImGui::SetNextWindowPos(windowPos, ImGuiCond_Always, windowPivot);
				ImGui::SetNextWindowBgAlpha(0.35f);

				std::string id = std::to_string(popupCounter);
				if (ImGui::Begin(id.c_str(), nullptr, windowFlags))
				{
					ImGui::TextUnformatted(popupUIComponent.Text.c_str());
				}
				ImGui::End();

				popupCounter++;
			}
			ImGui::PopID();
		}
	};

	DYE_SYSTEM("Howitzer Window System", DYE::DYEditor::HowitzerWindowSystem)
	struct HowitzerWindowSystem final : public SystemBase
	{
		ExecutionPhase GetPhase() const override { return ExecutionPhase::ImGui; }
		void Execute(DYE::DYEditor::World &world, DYE::DYEditor::ExecuteParameters params) override
		{

		}
	};


	DYE_SYSTEM("Title Tutorial UI System", DYE::DYEditor::TitleTutorialUISystem)
	struct TitleTutorialUISystem final : public SystemBase
	{
		ExecutionPhase GetPhase() const final { return ExecutionPhase::ImGui; }
		void Execute(DYE::DYEditor::World &world, DYE::DYEditor::ExecuteParameters params) final
		{
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

			ImVec2 windowPos = ImVec2(workPos.x + workSize.x * 0.5f, workPos.y + workSize.y * 0.5f);
			ImVec2 windowPivot = ImVec2(0.5f, 0.5f);

			ImGui::SetNextWindowPos(windowPos, ImGuiCond_Always, windowPivot);
			ImGui::SetNextWindowBgAlpha(0.35f);

			if (ImGui::Begin("Title Tutorial UI", nullptr, windowFlags))
			{
				ImGui::Separator();

				ImGui::SeparatorText("Commander");
				ImGui::TextWrapped("Guide your crew to dodge red rockets & hit green circle objectives.");

				ImGui::SeparatorText("Aimer");
				ImGui::TextWrapped("Follow the commander's order to adjust the aim.");

				ImGui::SeparatorText("Reloader");
				ImGui::TextWrapped("Fire & reload the ammo on commander's order.");

				ImGui::SeparatorText("Turtle");
				ImGui::TextWrapped("Follow the commander's order to move around. Rotate to turn & lean forward/backward to move.");

				ImGui::Separator();
				ImGui::TextUnformatted("Press Fire Button To Start");
			}
			ImGui::End();

			// Check input logic, if we should load the new scene.
			if (InputEventBuffingLayer::IsFirePressed())
			{
				auto &loadSceneComponent = world.CreateCommandEntity().AddComponent<LoadSceneComponent>();
				loadSceneComponent.SceneAssetPath = "assets//Scenes//GameScene.tscene";
			}
		}
	};
}