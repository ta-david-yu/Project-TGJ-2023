#pragma once

#include "Core/Entity.h"
#include "Core/EditorSystem.h"
#include "Core/Components.h"
#include "Components/HowitzerComponents.h"
#include "Components/KillComponents.h"
#include "Core/Time.h"
#include "Math/Math.h"
#include "Math/EasingFunctions.h"
#include "ImGui/ImGuiUtil.h"

#include "InputEventBuffingLayer.h"

#include "Graphics/DebugDraw.h"

namespace DYE::DYEditor
{
	DYE_SYSTEM("Follow Parent Position System", DYE::DYEditor::FollowParentPositionSystem)
	struct FollowParentPositionSystem final : public SystemBase
	{
		ExecutionPhase GetPhase() const final { return ExecutionPhase::FixedUpdate; }
		void Execute(DYE::DYEditor::World &world, DYE::DYEditor::ExecuteParameters params) final
		{
			auto view = world.GetView<FollowParentPositionComponent, ParentComponent, TransformComponent>();
			for (auto entity : view)
			{
				auto followParent = view.get<FollowParentPositionComponent>(entity);
				auto &childTransform = view.get<TransformComponent>(entity);
				auto parentComp = view.get<ParentComponent>(entity);
				Entity parentEntity = world.TryGetEntityWithGUID(parentComp.ParentGUID).value();

				TransformComponent parentTransform = parentEntity.GetComponent<TransformComponent>();
				childTransform.Position = parentTransform.Position + followParent.PositionOffset;
			}
		}
	};

	DYE_SYSTEM("Howitzer Input System", DYE::DYEditor::HowitzerInputSystem)
	struct HowitzerInputSystem final : public SystemBase
	{
		bool UseDebugKeyboardInput = false;
		bool IgnoreAmmoCount = false;
		float AimRotationDirection = 1.0f;
		AudioSource FireSoundSource;
		AudioSource ShellInSoundSource;
		AudioSource ShellOutSoundSource;
		AudioSource ButtonCheckSoundSource;
		AudioSource ReadyToFireSoundSource;

		AudioSource RotateClickSoundSource;

		void InitializeLoad(DYE::DYEditor::World &world, DYE::DYEditor::InitializeLoadParameters) override
		{
			FireSoundSource.SetClip(AudioClip::Create(
				"assets//Audio//shot-00.wav",
				AudioClipProperties
				{
					.LoadType = AudioLoadType::DecompressOnLoad
				}));

			ShellInSoundSource.SetClip(AudioClip::Create(
				"assets//Audio//shell_in-00.wav",
				AudioClipProperties
					{
						.LoadType = AudioLoadType::DecompressOnLoad
					}));

			ShellOutSoundSource.SetClip(AudioClip::Create(
				"assets//Audio//shell_out-00.wav",
				AudioClipProperties
					{
						.LoadType = AudioLoadType::DecompressOnLoad
					}));

			ButtonCheckSoundSource.SetClip(AudioClip::Create(
				"assets//Audio//button_check-00.wav",
				AudioClipProperties
					{
						.LoadType = AudioLoadType::DecompressOnLoad
					}));

			ReadyToFireSoundSource.SetClip(AudioClip::Create(
				"assets//Audio//ready_to_fire-00.wav",
				AudioClipProperties
					{
						.LoadType = AudioLoadType::DecompressOnLoad
					}));

			RotateClickSoundSource.SetClip(AudioClip::Create(
				"assets//Audio//rotate_click-00.wav",
				AudioClipProperties
					{
						.LoadType = AudioLoadType::DecompressOnLoad
					}));
		}

		ExecutionPhase GetPhase() const final { return ExecutionPhase::Update; }
		void Execute(DYE::DYEditor::World &world, DYE::DYEditor::ExecuteParameters params) final
		{
			{
				if (INPUT.GetKeyDown(KeyCode::F5))
				{
					UseDebugKeyboardInput = !UseDebugKeyboardInput;
				}

				if (INPUT.GetKeyDown(KeyCode::F6))
				{
					AimRotationDirection = -AimRotationDirection;
				}

				auto view = world.GetView<HowitzerInputComponent, HowitzerAimingComponent, TransformComponent>();
				for (auto entity: view)
				{
					Entity wrappedEntity = world.WrapIdentifierIntoEntity(entity);

					auto howitzerInput = view.get<HowitzerInputComponent>(entity);
					auto &howitzerAiming = view.get<HowitzerAimingComponent>(entity);
					auto transform = view.get<TransformComponent>(entity);

					if (UseDebugKeyboardInput)
					{
						if (INPUT.GetKeyDown(howitzerInput.RotateClockwiseButton))
						{
							howitzerAiming.AngleDegreeRelativeToParent -= howitzerInput.AngleStepDegreePerPress;
							RotateClickSoundSource.Play();
						}

						if (INPUT.GetKeyDown(howitzerInput.RotateAnticlockwiseButton))
						{
							howitzerAiming.AngleDegreeRelativeToParent += howitzerInput.AngleStepDegreePerPress;
							RotateClickSoundSource.Play();
						}

						if (INPUT.GetKeyDown(howitzerInput.IncreaseDistanceButton))
						{
							howitzerAiming.CurrDistance += howitzerInput.DistanceChangePerPress;
							if (howitzerAiming.CurrDistance > howitzerAiming.MaxDistance)
							{
								howitzerAiming.CurrDistance = howitzerAiming.MaxDistance;
							}
							RotateClickSoundSource.Play();
						}

						if (INPUT.GetKeyDown(howitzerInput.DecreaseDistanceButton))
						{
							howitzerAiming.CurrDistance -= howitzerInput.DistanceChangePerPress;
							if (howitzerAiming.CurrDistance < howitzerAiming.MinDistance)
							{
								howitzerAiming.CurrDistance = howitzerAiming.MinDistance;
							}
							RotateClickSoundSource.Play();
						}
					}
					else
					{
						if (INPUT.IsGamepadConnected(howitzerInput.ControllerID))
						{
							// -1 ~ 1: the orientation of howitzer
							float const x = INPUT.GetGamepadAxis(howitzerInput.ControllerID, GamepadAxis::LeftTrigger);
							float const y = INPUT.GetGamepadAxis(howitzerInput.ControllerID, GamepadAxis::RightTrigger);

							glm::vec2 const axis = {x * 2 - 1, y * 2 - 1};
							float const length = glm::length(axis);
							if (length > 0.0001f)
							{
								float const angleRadian = glm::atan(axis.y, axis.x);
								howitzerAiming.AngleDegreeRelativeToParent = AimRotationDirection * glm::degrees(angleRadian);
							}

							if (InputEventBuffingLayer::IsIncreaseDistancePressed())
							{
								howitzerAiming.CurrDistance += howitzerInput.DistanceChangePerPress;
								if (howitzerAiming.CurrDistance > howitzerAiming.MaxDistance)
								{
									howitzerAiming.CurrDistance = howitzerAiming.MaxDistance;
								}
								RotateClickSoundSource.Play();
							}

							if (InputEventBuffingLayer::IsDecreaseDistancePressed())
							{
								howitzerAiming.CurrDistance -= howitzerInput.DistanceChangePerPress;
								if (howitzerAiming.CurrDistance < howitzerAiming.MinDistance)
								{
									howitzerAiming.CurrDistance = howitzerAiming.MinDistance;
								}
								RotateClickSoundSource.Play();
							}
						}
					}

					if (wrappedEntity.HasComponent<ShellComponent>())
					{
						ShellComponent &shell = wrappedEntity.GetComponent<ShellComponent>();

						bool fulfillFireCheck = false;
						if (InputEventBuffingLayer::IsSimon1Pressed() && !shell.IsButton1Operated)
						{
							shell.IsButton1Operated = true;
							shell.SequenceNumber *= 2;
							ButtonCheckSoundSource.Play();
							fulfillFireCheck |= shell.CheckSequenceAndUpdate();
						}

						if (InputEventBuffingLayer::IsSimon2Pressed() && !shell.IsButton2Operated)
						{
							shell.IsButton2Operated = true;
							shell.SequenceNumber += 1;
							ButtonCheckSoundSource.Play();
							fulfillFireCheck |= shell.CheckSequenceAndUpdate();
						}

						if (InputEventBuffingLayer::IsSimon3Pressed() && !shell.IsButton3Operated)
						{
							shell.IsButton3Operated = true;
							shell.SequenceNumber = shell.SequenceNumber * shell.SequenceNumber;
							ButtonCheckSoundSource.Play();
							fulfillFireCheck |= shell.CheckSequenceAndUpdate();
						}

						if (fulfillFireCheck)
						{
							ReadyToFireSoundSource.Play();
						}

						if (InputEventBuffingLayer::IsFirePressed() ||
							INPUT.GetKeyDown(howitzerInput.FireButton) ||
							INPUT.GetGamepadButton(howitzerInput.ControllerID, howitzerInput.FireGamepadButton))
						{
							bool const hasInfiniteAmmo = wrappedEntity.HasComponent<InfiniteAmmoComponent>();

							bool const superMode = hasInfiniteAmmo | IgnoreAmmoCount;
							bool const canFire = superMode || (shell.HasAmmo & shell.ActivationSucceed);
							if (!superMode && !canFire)
							{
								// Can't fire the ammo.
								continue;
							}

							Entity firedProjectile = world.CreateEntity("Player Projectile");
							auto &projectileTransform = firedProjectile.AddComponent<TransformComponent>();
							projectileTransform.Position = transform.Position;
							projectileTransform.Rotation = transform.Rotation;

							auto &projectileMovement = firedProjectile.AddComponent<ProjectileMovementComponent>();
							projectileMovement.MaxTravelDistance = howitzerAiming.CurrDistance;

							firedProjectile.AddComponent<RenderedOnAimerWindowComponent>();

							auto &explodeOnKilled = firedProjectile.AddComponent<ExplodeOnKilledComponent>();
							explodeOnKilled.TeamIDToKill = ENEMY_TEAM;
							explodeOnKilled.ExplodeRadius = 3.5f;

							auto &spriteRenderer = firedProjectile.AddComponent<SpriteRendererComponent>();
							spriteRenderer.Texture = Texture2D::Create("assets//Textures//Bullet.png");

							// Play sound effect. (NASTY CODE I KNOW)
							FireSoundSource.Play();

							// Used up the ammo.
							shell.HasAmmo = false;
						}
					}

					if (InputEventBuffingLayer::IsShellInPressed() && !wrappedEntity.HasComponent<ShellComponent>())
					{
						static int targetSequences[] = { 441, 401, 484, 242, 201, 202 };

						int rand = std::rand() % 6;
						//wrappedEntity.AddComponent<ShellComponent>().TargetSequenceNumber = targetSequences[rand];
						wrappedEntity.AddComponent<ShellComponent>().TargetSequenceNumber = targetSequences[0];

						ShellInSoundSource.Play();
					}

					if (InputEventBuffingLayer::IsShellOutPressed())
					{
						wrappedEntity.RemoveComponent<ShellComponent>();

						ShellOutSoundSource.Play();
					}

					if (INPUT.GetKeyDown(KeyCode::F7))
					{
						if (wrappedEntity.HasComponent<InfiniteAmmoComponent>())
						{
							wrappedEntity.RemoveComponent<InfiniteAmmoComponent>();
						}
						else
						{
							wrappedEntity.AddComponent<InfiniteAmmoComponent>();
						}
					}
				}
			}
		}

		void DrawInspector(DYE::DYEditor::World &world) override
		{
			ImGuiUtil::DrawBoolControl("Debug Keyboard Input", UseDebugKeyboardInput);
			ImGuiUtil::DrawBoolControl("Ignore Ammo Load", IgnoreAmmoCount);
		}
	};

	DYE_SYSTEM("Rotate Howitzer System", DYE::DYEditor::RotateHowitzerSystem)
	struct RotateHowitzerSystem final : public SystemBase
	{
		ExecutionPhase GetPhase() const final { return ExecutionPhase::FixedUpdate; }
		void Execute(DYE::DYEditor::World &world, DYE::DYEditor::ExecuteParameters params) final
		{
			auto view = world.GetView<HowitzerAimingComponent, ParentComponent, TransformComponent>();
			for (auto entity : view)
			{
				auto howitzerAiming = view.get<HowitzerAimingComponent>(entity);
				auto &childTransform = view.get<TransformComponent>(entity);
				auto parentComp = view.get<ParentComponent>(entity);
				Entity parentEntity = world.TryGetEntityWithGUID(parentComp.ParentGUID).value();

				TransformComponent parentTransform = parentEntity.GetComponent<TransformComponent>();
				childTransform.Rotation =
					parentTransform.Rotation *
					glm::quat(glm::vec3(0, 0, glm::radians(howitzerAiming.AngleDegreeRelativeToParent)));
			}
		}
	};

	DYE_SYSTEM("Render Howitzer Aim System", DYE::DYEditor::RenderHowitzerAimSystem)
	struct RenderHowitzerAimSystem final : public SystemBase
	{
		ExecutionPhase GetPhase() const final { return ExecutionPhase::Render; }
		void Execute(DYE::DYEditor::World &world, DYE::DYEditor::ExecuteParameters params) final
		{
			auto view = world.GetView<HowitzerInputComponent, HowitzerAimingComponent, TransformComponent>();
			for (auto entity : view)
			{
				auto howitzerInput = view.get<HowitzerInputComponent>(entity);
				auto howitzerAiming = view.get<HowitzerAimingComponent>(entity);
				auto &transform = view.get<TransformComponent>(entity);

				// Aim distance range.
				auto const minAimPosition = transform.Position + transform.GetRight() * howitzerAiming.MinDistance;
				auto const maxAimPosition = transform.Position + transform.GetRight() * howitzerAiming.MaxDistance;
				//DebugDraw::Circle(minAimPosition, 0.2f, glm::vec3(0, 0, 1), Color::Yellow);
				//DebugDraw::Circle(maxAimPosition, 0.2f, glm::vec3(0, 0, 1), Color::Yellow);
				DebugDraw::Line(transform.Position, maxAimPosition, Color::Yellow);

				// Ruler.
				for (int distance = howitzerAiming.MinDistance; distance <= howitzerAiming.MaxDistance; distance += howitzerInput.DistanceChangePerPress)
				{
					glm::vec3 const center = transform.Position + transform.GetRight() * (float) distance;
					glm::vec3 const left = center + transform.GetForward() * 0.15f;
					glm::vec3 const right = center + transform.GetForward() * -0.15f;
					DebugDraw::Line(left, right, Color::Yellow);
				}

				// Current distance.
				auto const aimPosition = transform.Position + transform.GetRight() * howitzerAiming.CurrDistance;
				float const hitRadius = 3.5f; // FIXME: put this in another component.
				DebugDraw::Circle(aimPosition, hitRadius, glm::vec3(0, 0, 1), Color::White);
				DebugDraw::Circle(aimPosition, 0.25f, glm::vec3(0, 0, 1), Color::Yellow);
			}
		}
	};

	DYE_SYSTEM("Projectile Travel System", DYE::DYEditor::ProjectileTravelSystem)
	struct ProjectileTravelSystem final : public SystemBase
	{
		ExecutionPhase GetPhase() const override { return ExecutionPhase::FixedUpdate; }
		void Execute(DYE::DYEditor::World &world, DYE::DYEditor::ExecuteParameters params) override
		{
			auto view = world.GetView<ProjectileMovementComponent, TransformComponent>();
			for (auto entity : view)
			{
				Entity wrappedEntity = world.WrapIdentifierIntoEntity(entity);

				auto &projectTileMovement = view.get<ProjectileMovementComponent>(entity);
				auto &transform = view.get<TransformComponent>(entity);

				// Calculate new travel location.
				bool reachEnd = false;

				float travelDistance = (float) TIME.FixedDeltaTime() * projectTileMovement.TravelSpeedPerSecond;
				float newTravelledDistance = projectTileMovement.TravelledDistance + travelDistance;
				if (newTravelledDistance > projectTileMovement.MaxTravelDistance)
				{
					newTravelledDistance = projectTileMovement.MaxTravelDistance;
					reachEnd = true;
				}
				travelDistance = newTravelledDistance - projectTileMovement.TravelledDistance;

				transform.Position += transform.GetRight() * travelDistance;
				projectTileMovement.TravelledDistance += travelDistance;

				if (reachEnd)
				{
					// Reached max distance, kill the projectile entity.
					wrappedEntity.AddOrGetComponent<KilledComponent>();
				}
			}
		}
	};

	DYE_SYSTEM("Render Debug Sphere System", DYE::DYEditor::RenderDebugSphereSystem)
	struct RenderDebugSphereSystem final : public SystemBase
	{
		ExecutionPhase GetPhase() const override { return ExecutionPhase::Render; }
		void Execute(DYE::DYEditor::World &world, DYE::DYEditor::ExecuteParameters params) override
		{
			auto view = world.GetView<DebugDrawSphereComponent, TransformComponent>();
			for (auto entity : view)
			{
				auto &sphere = view.get<DebugDrawSphereComponent>(entity);
				auto &transform = view.get<TransformComponent>(entity);

				DebugDraw::Circle(transform.Position, sphere.Radius, glm::vec3 (0, 0, 1), sphere.Color);
			}
		}
	};

	DYE_SYSTEM("Render Triangle System", DYE::DYEditor::RenderTriangleSystem)
	struct RenderTriangleSystem final : public SystemBase
	{
		ExecutionPhase GetPhase() const override { return ExecutionPhase::Render; }
		void Execute(DYE::DYEditor::World &world, DYE::DYEditor::ExecuteParameters params) override
		{
			auto view = world.GetView<DrawTriangleOnTransformComponent, TransformComponent>();
			for (auto entity : view)
			{
				auto &triangle = view.get<DrawTriangleOnTransformComponent>(entity);
				auto &transform = view.get<TransformComponent>(entity);

				auto const forward = transform.Position + transform.GetRight() * 0.4f;
				auto const right = transform.Position + transform.GetRight() * -0.4f + transform.GetForward() * 0.2f;
				auto const left = transform.Position + transform.GetRight() * -0.4f + transform.GetForward() * -0.2f;

				DebugDraw::Line(forward, right, triangle.Color);
				DebugDraw::Line(right, left, triangle.Color);
				DebugDraw::Line(left, forward, triangle.Color);
			}
		}
	};
}