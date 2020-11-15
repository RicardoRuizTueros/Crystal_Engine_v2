#include "EditorLayer.h"

#include "platform/openGL/OpenGLShader.h"

#include <imgui/imgui.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <chrono>

using namespace glm;

namespace Crystal
{
	EditorLayer::EditorLayer()
		: Layer("EditorLayer"), cameraController(1920.0f / 1080.0f)
	{

	}

	void EditorLayer::OnAttach()
	{
		CRYSTAL_PROFILE_FUNCTION();

		checkerTexture = Texture2D::Create("../assets/textures/checkerboard.png");

		FrameBufferSpecification frameBufferSpecification;
		frameBufferSpecification.width = 1280;
		frameBufferSpecification.height = 720;
		frameBuffer = FrameBuffer::Create(frameBufferSpecification);

		activeScene = CreateReference<Scene>();

		auto greenSquare = activeScene->CreateEntity("Green Square");
		greenSquare.AddComponent<SpriteRendererComponent>(vec4{ 0.0f, 1.0f, 0.0f, 1.0f });

		auto redSquare = activeScene->CreateEntity("Red square");
		redSquare.AddComponent<SpriteRendererComponent>(vec4{ 1.0f, 0.0f, 0.0f, 1.0f });

		cameraA = activeScene->CreateEntity("Camera A");
		cameraA.AddComponent<CameraComponent>();
		cameraA.GetComponent<CameraComponent>().camera.SetProjectionType(SceneCamera::ProjectionType::Orthographic);

		cameraB = activeScene->CreateEntity("Camera B");
		cameraB.AddComponent<CameraComponent>().primary = false;

		// Camera controller class
		class CameraController : public ScriptableEntity
		{
		public:
			virtual void OnCreate() override
			{
				auto& translation = GetComponent<TransformComponent>().translation;
				translation.x = rand() % 10 - 5.0f;
			}
			
			virtual void OnDestroy() override
			{
			}

			virtual void OnUpdate(Timestep timestep) override
			{
				auto& translation = GetComponent<TransformComponent>().translation;
				float speed = 5.0f;

				if (Input::IsKeyPressed(Key::A))
					translation.x -= speed * timestep;
				if (Input::IsKeyPressed(Key::D))
					translation.x += speed * timestep;
				if (Input::IsKeyPressed(Key::W))
					translation.y += speed * timestep;
				if (Input::IsKeyPressed(Key::S))
					translation.y -= speed * timestep;
			}
		};

		cameraA.AddComponent<NativeScriptComponent>().Bind<CameraController>();
		cameraB.AddComponent<NativeScriptComponent>().Bind<CameraController>();

		sceneHierarchyPanel.SetContext(activeScene);
	}

	void EditorLayer::OnDetach()
	{
		CRYSTAL_PROFILE_FUNCTION();
	}

	void EditorLayer::OnUpdate(Timestep timestep)
	{
		CRYSTAL_PROFILE_FUNCTION();

		// Resize
		if (FrameBufferSpecification specification = frameBuffer->GetSpecification();
			viewportSize.x > 0.0f && viewportSize.y > 0.0f && 
			(specification.width != viewportSize.x || specification.height != viewportSize.y))
		{
			frameBuffer->Resize((uint32_t)viewportSize.x, (uint32_t)viewportSize.y);
			cameraController.OnResize(viewportSize.x, viewportSize.y);
			activeScene->OnViewportResize((uint32_t)viewportSize.x, (uint32_t)viewportSize.y);
		}

		// Update
		if (viewportFocused)
			cameraController.OnUpdate(timestep);

		Renderer2D::ResetStatistics();
		frameBuffer->Bind();
		RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
		RenderCommand::Clear();
		
		activeScene->OnUpdate(timestep);

		frameBuffer->Unbind();
	}

	void EditorLayer::OnImGuiRender()
	{
		CRYSTAL_PROFILE_FUNCTION();

		static bool dockspaceOpen = true;
		static bool opt_fullscreen_persistant = true;
		bool opt_fullscreen = opt_fullscreen_persistant;
		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

		// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
		// because it would be confusing to have two docking targets within each others.
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		if (opt_fullscreen)
		{
			ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(viewport->Pos);
			ImGui::SetNextWindowSize(viewport->Size);
			ImGui::SetNextWindowViewport(viewport->ID);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
			window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		}

		// When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background and handle the pass-thru hole, so we ask Begin() to not render a background.
		if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
			window_flags |= ImGuiWindowFlags_NoBackground;

		// Important: note that we proceed even if Begin() returns false (aka window is collapsed).
		// This is because we want to keep our DockSpace() active. If a DockSpace() is inactive, 
		// all active windows docked into it will lose their parent and become undocked.
		// We cannot preserve the docking relationship between an active window and an inactive docking, otherwise 
		// any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("DockSpace Demo", &dockspaceOpen, window_flags);
		ImGui::PopStyleVar();

		if (opt_fullscreen)
			ImGui::PopStyleVar(2);

		// DockSpace
		ImGuiIO& io = ImGui::GetIO();

		ImGuiStyle& style = ImGui::GetStyle();
		float minimumWindowSizeX = style.WindowMinSize.x;

		// Set the minimun docked window width
		style.WindowMinSize.x = 370.0f;

		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		}

		// Reset to default for non-docked windows
		style.WindowMinSize.x = minimumWindowSizeX;

		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				// Disabling fullscreen would allow the window to be moved to the front of other windows, 
				// which we can't undo at the moment without finer window depth/z control.
				//ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen_persistant);

				if (ImGui::MenuItem("Serialize"))
				{
					SceneSerializer serializer(activeScene);
					serializer.Serialize("../assets/scenes/Example.scene");
				}

				if (ImGui::MenuItem("Deserialize"))
				{
					SceneSerializer serializer(activeScene);
					serializer.Deserialize("../assets/scenes/Example.scene");
				}

				if (ImGui::MenuItem("Exit")) Crystal::Application::Get().Close();
				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}

		sceneHierarchyPanel.OnImGuiRender();

		ImGui::Begin("Statistics");
		auto statistics = Crystal::Renderer2D::GetStatistics();
		ImGui::Text("Renderer2D Statistics:");
		ImGui::Text("Draw Calls: %d", statistics.drawCalls);
		ImGui::Text("Quads: %d", statistics.quadCount);
		ImGui::Text("Vertices: %d", statistics.GetVertexCount());
		ImGui::Text("Indices: %d", statistics.GetIndexCount());

		ImGui::End();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0.0f, 0.0f });
		ImGui::Begin("Viewport");

		viewportFocused = ImGui::IsWindowFocused();
		viewportHovered = ImGui::IsWindowHovered();
		Application::Get().GetImGuiLayer()->BlockEvents(!viewportFocused || !viewportHovered);

		ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
		viewportSize = { viewportPanelSize.x, viewportPanelSize.y };

		uint64_t textureID = frameBuffer->GetColorAttachmentRendererID();
		ImGui::Image(reinterpret_cast<void*>(textureID), ImVec2{ viewportSize.x, viewportSize.y }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
		ImGui::End();
		ImGui::PopStyleVar();

		ImGui::End();
	}

	void EditorLayer::OnEvent(Event& event)
	{
		cameraController.OnEvent(event);
	}
}
