#pragma once

#include "Core/Entity.h"
#include "Core/EditorSystem.h"
#include "Core/Components.h"
#include "Components/HowitzerComponents.h"
#include "Components/KillComponents.h"
#include "Components/GameStateComponents.h"
#include "Components/MiscUIComponents.h"
#include "Components/EnvironmentComponents.h"
#include "Core/Time.h"
#include "Math/Math.h"
#include "Math/EasingFunctions.h"
#include "Math/PrimitiveTest.h"
#include "ImGui/ImGuiUtil.h"
#include "InputEventBuffingLayer.h"
#include "Graphics/RenderCommand.h"

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

	DYE_SYSTEM("Howitzer HUD System", DYE::DYEditor::HowitzerHUDSystem)
	struct HowitzerHUDSystem final : public SystemBase
	{
		ExecutionPhase GetPhase() const override { return ExecutionPhase::ImGui; }
		void Execute(DYE::DYEditor::World &world, DYE::DYEditor::ExecuteParameters params) override
		{
			auto view = world.GetView<HowitzerAimingComponent>();
			for (auto entity : view)
			{
				Entity wrappedEntity = world.WrapIdentifierIntoEntity(entity);
				HowitzerAimingComponent &howitzerAiming = view.get<HowitzerAimingComponent>(entity);

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
				float const paddingY = 10;
				ImVec2 windowPos = ImVec2(workPos.x + workSize.x - paddingX, workPos.y + workSize.y - paddingY);
				ImVec2 windowPivot = ImVec2(1, 1);

				ImGui::SetNextWindowPos(windowPos, ImGuiCond_Always, windowPivot);
				ImGui::SetNextWindowBgAlpha(0.35f);

				if (ImGui::Begin("Howitzer Loaded State", nullptr, windowFlags))
				{
					ImGui::SetWindowFontScale(2.0f);
					ImVec4 codeColor(1, 1, 1, 1);
					auto tryGetShell = wrappedEntity.TryGetComponent<ShellComponent>();
					if (!tryGetShell.has_value())
					{
						codeColor = ImVec4(1.0f, 0.5f, 0.5f, 1.0f);
						ImGui::TextColored(codeColor, "Please load a new shell.");
					}
					else
					{
						ShellComponent &shell = tryGetShell.value();
						if (!shell.HasAmmo)
						{
							codeColor = ImVec4(1.0f, 0.5f, 0.5f, 1.0f);
							ImGui::TextColored(codeColor, "The shell is empty.");
							ImGui::TextColored(codeColor, "Please unload the shell.");
						}
						else if (!shell.IsOperated())
						{
							codeColor = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
							ImGui::TextColored(codeColor, "Press all the buttons to initialize the shell.");
						}
						else if (shell.IsOperated() && !shell.ActivationSucceed)
						{
							codeColor = ImVec4(1.0f, 0.5f, 0.5f, 1.0f);
							ImGui::TextColored(codeColor, "The shell failed to initialize.");
							ImGui::TextColored(codeColor, "Please unload the shell.");
						}
						else if (shell.IsOperated() && shell.ActivationSucceed)
						{
							codeColor = ImVec4(0.0f, 1.0f, 0.0f, 1.0f);
							ImGui::TextColored(codeColor, "The shell is loaded.");
							ImGui::TextColored(codeColor, "Press button to fire.");
						}
					}
				}
				ImGui::End();
			}
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
			ImGui::SetNextWindowViewport(viewport->ID);

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

	DYE_SYSTEM("Reloader Window System", DYE::DYEditor::ReloaderWindowSystem)
	struct ReloaderWindowSystem final : public SystemBase
	{
		ExecutionPhase GetPhase() const override { return ExecutionPhase::ImGui; }
		void Execute(DYE::DYEditor::World &world, DYE::DYEditor::ExecuteParameters params) override
		{
			auto view = world.GetView<HowitzerAimingComponent>();
			for (auto entity : view)
			{
				Entity wrappedEntity = world.WrapIdentifierIntoEntity(entity);
				HowitzerAimingComponent &howitzerAiming = view.get<HowitzerAimingComponent>(entity);

				ImGuiIO& io = ImGui::GetIO();
				ImGuiWindowFlags windowFlags =
					ImGuiWindowFlags_NoResize |
					ImGuiWindowFlags_NoCollapse |
					ImGuiWindowFlags_NoFocusOnAppearing;

				const ImGuiViewport* viewport = ImGui::GetMainViewport();
				ImVec2 workPos = viewport->WorkPos; // Use work area to avoid menu-bar/task-bar, if any!
				ImVec2 workSize = viewport->WorkSize;

				ImGui::SetNextWindowSize(ImVec2(450, 800), ImGuiCond_Always);
				ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 25);
				if (ImGui::Begin("Loader Interface", nullptr, windowFlags))
				{
					glm::vec2 const topLeftPos = {ImGui::GetCursorScreenPos().x, ImGui::GetCursorScreenPos().y};

					ImGui::SetWindowFontScale(1.5f);

					bool highlightTube = false;
					bool highlightButtonBox = false;
					bool highlightFireButton = false;
					bool highlightOperationButtons = false;

					bool button1 = false;
					bool button2 = false;
					bool button3 = false;


					ImDrawList *pDrawList = ImGui::GetWindowDrawList();

					ImU32 highlightColor = ImGui::GetColorU32(ImVec4(0, 1, 0, 1));
					ImU32 disabledColor = ImGui::GetColorU32(ImVec4(1, 1, 1, 0.8f));

					ImVec4 codeColor(1, 1, 1, 1);
					auto tryGetShell = wrappedEntity.TryGetComponent<ShellComponent>();
					if (!tryGetShell.has_value())
					{
						codeColor = ImVec4(1.0f, 0.5f, 0.5f, 1.0f);
						ImGui::TextColored(codeColor, "Please load a new shell.");
						highlightTube = true;
					}
					else
					{
						ShellComponent &shell = tryGetShell.value();
						button1 = shell.IsButton1Operated;
						button2 = shell.IsButton2Operated;
						button3 = shell.IsButton3Operated;

						if (!shell.HasAmmo)
						{
							codeColor = ImVec4(1.0f, 0.5f, 0.5f, 1.0f);
							ImGui::TextColored(codeColor, "The shell is empty.");
							ImGui::TextColored(codeColor, "Please unload the shell.");
							highlightTube = true;
						}
						else if (!shell.IsOperated())
						{
							codeColor = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
							ImGui::TextColored(codeColor, "Press all the buttons");
							ImGui::TextColored(codeColor, "to initialize the shell.");
							//highlightButtonBox = true;
							highlightOperationButtons = true;
						}
						else if (shell.IsOperated() && !shell.ActivationSucceed)
						{
							codeColor = ImVec4(1.0f, 0.5f, 0.5f, 1.0f);
							ImGui::TextColored(codeColor, "The shell failed to initialize.");
							ImGui::TextColored(codeColor, "Please unload the shell.");
							highlightTube = true;
						}
						else if (shell.IsOperated() && shell.ActivationSucceed)
						{
							codeColor = ImVec4(0.0f, 1.0f, 0.0f, 1.0f);
							ImGui::TextColored(codeColor, "The shell is loaded.");
							ImGui::TextColored(codeColor, "Press button to fire.");
							//highlightButtonBox = true;
							highlightFireButton = true;
						}

						// Draw shell
						glm::vec2 tip = topLeftPos + glm::vec2(310, 500);
						glm::vec2 shellTopLeft = topLeftPos + glm::vec2(280, 550);
						glm::vec2 shellTopRight = topLeftPos + glm::vec2(340, 550);
						glm::vec2 shellBotLeft = topLeftPos + glm::vec2(280, 690);
						glm::vec2 shellBotRight = topLeftPos + glm::vec2(340, 690);

						auto shellColor = highlightTube? highlightColor : disabledColor;
						if (shell.HasAmmo)
						{
							pDrawList->AddTriangleFilled
							(
								ImVec2(tip.x, tip.y),
								ImVec2(shellTopLeft.x, shellTopLeft.y),
								ImVec2(shellTopRight.x, shellTopRight.y),
								shellColor
							);

							pDrawList->AddRectFilled
							(
								ImVec2(shellTopLeft.x, shellTopLeft.y),
								ImVec2(shellBotRight.x, shellBotRight.y),
								shellColor
							);
						}
						else
						{
							pDrawList->AddTriangle
							(
								ImVec2(tip.x, tip.y),
								ImVec2(shellTopLeft.x, shellTopLeft.y),
								ImVec2(shellTopRight.x, shellTopRight.y),
								shellColor
							);

							pDrawList->AddRect
							(
								ImVec2(shellTopLeft.x, shellTopLeft.y),
								ImVec2(shellBotRight.x, shellBotRight.y),
								shellColor
							);
						}
					}

					// Draw button box
					glm::vec2 buttonBoxMin = topLeftPos + glm::vec2(100, 500);
					glm::vec2 buttonBoxMax = topLeftPos + glm::vec2(250, 700);
					pDrawList->AddRect(ImVec2(buttonBoxMin.x, buttonBoxMin.y), ImVec2(buttonBoxMax.x, buttonBoxMax.y),
									   highlightButtonBox? highlightColor : disabledColor);

					// Draw tube
					glm::vec2 tubeMin = topLeftPos + glm::vec2(270, 100);
					glm::vec2 tubeMax = topLeftPos + glm::vec2(350, 700);
					pDrawList->AddRect(ImVec2(tubeMin.x, tubeMin.y), ImVec2(tubeMax.x, tubeMax.y),
									   highlightTube? highlightColor : disabledColor);

					// Draw fire button
					glm::vec2 fireButtonPos = (buttonBoxMin + buttonBoxMax) * 0.5f;
					float const radius = 20;
					pDrawList->AddCircle(ImVec2(fireButtonPos.x, fireButtonPos.y), radius,
										 highlightFireButton? highlightColor : disabledColor);

					// Draw sequence buttons
					glm::vec2 button1Pos = buttonBoxMin + glm::vec2(-30, 50);
					glm::vec2 button2Pos = buttonBoxMin + glm::vec2(-30, 150);
					glm::vec2 button3Pos = fireButtonPos + glm::vec2(0, 120);
					float const smallBtnRadius = 10;
					pDrawList->AddCircle(ImVec2(button1Pos.x, button1Pos.y), smallBtnRadius,
										 highlightOperationButtons && !button1? highlightColor : disabledColor);
					pDrawList->AddCircle(ImVec2(button2Pos.x, button2Pos.y), smallBtnRadius,
										 highlightOperationButtons && !button2? highlightColor : disabledColor);
					pDrawList->AddCircle(ImVec2(button3Pos.x, button3Pos.y), smallBtnRadius,
										 highlightOperationButtons && !button3? highlightColor : disabledColor);
				}
				ImGui::End();
				ImGui::PopStyleVar();
			}
		}
	};

	DYE_SYSTEM("Turtle Status Window System", DYE::DYEditor::TurtleStatusWindowSystem)
	struct TurtleStatusWindowSystem final : public SystemBase
	{
		ExecutionPhase GetPhase() const override { return ExecutionPhase::ImGui; }
		void Execute(DYE::DYEditor::World &world, DYE::DYEditor::ExecuteParameters params) override
		{
			auto view = world.GetView<PlayerComponent, InvincibleComponent>();
			for (auto entity : view)
			{
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

				float const paddingY = 10;
				ImVec2 windowPos = ImVec2(workPos.x + workSize.x * 0.5f, workPos.y + paddingY);
				ImVec2 windowPivot = ImVec2(0.5f, 0);

				ImGui::SetNextWindowPos(windowPos, ImGuiCond_Always, windowPivot);
				ImGui::SetNextWindowBgAlpha(0.0f);
				if (ImGui::Begin("SUPER TURTLE MODE", nullptr, windowFlags))
				{
					ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "SUPER TURTLE MODE");
				}
				ImGui::End();
			}
		}

	};

	DYE_SYSTEM("Aimer Window System", DYE::DYEditor::AimerWindowSystem)
	struct AimerWindowSystem final : public SystemBase
	{
		ExecutionPhase GetPhase() const override { return ExecutionPhase::Render; }

		void Execute(DYE::DYEditor::World &world, DYE::DYEditor::ExecuteParameters params) override
		{
			// Copy turtle position, Howizter angle & distance & world border.

			auto view = world.GetView<AimerWindowComponent>();
			for (auto aimerWindowEntity : view)
			{
				auto offset = view.get<AimerWindowComponent>(aimerWindowEntity).Offset;

				// Player turtle
				{
					auto playerView = world.GetView<PlayerComponent, TransformComponent, CircleColliderComponent>();
					for (auto playerEntity: playerView)
					{
						auto &transform = playerView.get<TransformComponent>(playerEntity);
						auto &circleCollider = playerView.get<CircleColliderComponent>(playerEntity);

						auto position = transform.Position + offset;
						DebugDraw::Circle(position, circleCollider.Radius, glm::vec3(0, 0, 1), Color::White);
						DebugDraw::Line(position, position + transform.GetRight() * circleCollider.Radius,
										Color::White);
					}
				}

				// Howitzer aiming
				{
					auto howitzerView = world.GetView<HowitzerAimingComponent, HowitzerInputComponent, TransformComponent>();
					for (auto howitzerEntity: howitzerView)
					{
						auto howitzerInput = howitzerView.get<HowitzerInputComponent>(howitzerEntity);
						auto howitzerAiming = howitzerView.get<HowitzerAimingComponent>(howitzerEntity);
						auto &transform = howitzerView.get<TransformComponent>(howitzerEntity);

						// Aim distance range.
						auto const minAimPosition = transform.Position + transform.GetRight() * howitzerAiming.MinDistance;
						auto const maxAimPosition = transform.Position + transform.GetRight() * howitzerAiming.MaxDistance;
						//DebugDraw::Circle(minAimPosition, 0.2f, glm::vec3(0, 0, 1), Color::Yellow);
						//DebugDraw::Circle(maxAimPosition, 0.2f, glm::vec3(0, 0, 1), Color::Yellow);
						DebugDraw::Line(transform.Position + offset, maxAimPosition + offset, Color::Yellow);

						// Ruler.
						for (int distance = howitzerAiming.MinDistance; distance <= howitzerAiming.MaxDistance; distance += howitzerInput.DistanceChangePerPress)
						{
							glm::vec3 const center = transform.Position + transform.GetRight() * (float) distance;
							glm::vec3 const left = center + transform.GetForward() * 0.15f;
							glm::vec3 const right = center + transform.GetForward() * -0.15f;
							DebugDraw::Line(left + offset, right + offset, Color::Yellow);
						}

						// Current distance.
						auto const aimPosition = transform.Position + transform.GetRight() * howitzerAiming.CurrDistance;
						float const hitRadius = 3.5f; // FIXME: put this in another component.
						DebugDraw::Circle(aimPosition + offset, hitRadius, glm::vec3(0, 0, 1), Color::White);
						DebugDraw::Circle(aimPosition + offset, 0.25f, glm::vec3(0, 0, 1), Color::Yellow);
					}
				}

				// World border
				{
					auto borderView = world.GetView<BorderComponent, CircleColliderComponent, DrawCircleColliderComponent, TransformComponent>();
					for (auto borderEntity: borderView)
					{
						auto &circle = borderView.get<CircleColliderComponent>(borderEntity);
						auto &drawCircle = borderView.get<DrawCircleColliderComponent>(borderEntity);
						auto &transform = borderView.get<TransformComponent>(borderEntity);

						DebugDraw::Circle(transform.Position + offset, circle.Radius, glm::vec3(0, 0, 1), drawCircle.Color);
					}
				}

				// Player projectiles
				{
					auto playerProjectileView = world.GetView<RenderedOnAimerWindowComponent, ProjectileMovementComponent, TransformComponent>();
					for (auto projectileEntity: playerProjectileView)
					{
						auto &transform = playerProjectileView.get<TransformComponent>(projectileEntity);
						auto const position = transform.Position + offset;
						float const radius = 0.4f;
						DebugDraw::Circle(position, radius, glm::vec3(0, 0, 1), Color::White);
						//DebugDraw::Line(position, position + transform.GetRight() * radius, Color::White);
					}
				}

				// Explosion animations
				{
					auto explosionView = world.GetView<ExplodeAnimationComponent, TransformComponent>();
					for (auto explosionEntity : explosionView)
					{
						auto &explodeAnimation = explosionView.get<ExplodeAnimationComponent>(explosionEntity);
						auto &transform = explosionView.get<TransformComponent>(explosionEntity);

						float const progress = glm::clamp(explodeAnimation.Timer / explodeAnimation.AnimationTime, 0.0f, 1.0f);
						float const t = GetEasingFunction(Ease::EaseOutCubic)(progress);

						glm::vec4 const color = Math::Lerp(explodeAnimation.StartColor, explodeAnimation.EndColor, t);
						float const radius = Math::Lerp(explodeAnimation.StartRadius, explodeAnimation.EndRadius, t);

						auto const position = transform.Position + offset;
						DebugDraw::Circle(position, radius, glm::vec3(0, 0, 1), color);
					}
				}
			}
		}
	};
}